/*Parallel Make Lab
* CS 241 - Fall 2018
*/

#include "graph.h"
#include "format.h"
#include "parmake.h"
#include "parser.h"
#include "rule.h"
#include "queue.h"
#include "set.h"
#include <unistd.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pthread.h>

graph * g;
queue * q;
set * s;
int run_rule(char * t);
int iscycle(char* s);
int checkdep(char * target);
int run(char * target);
int ifcommand(char * target) ;
void* sth(void* ptr);

pthread_mutex_t m;
pthread_cond_t cv;

int iscycle(char * target) {

    rule_t * r = (rule_t *) graph_get_vertex_value(g, target);

    if (r->state){
        if (strcmp(r->data, "visited")==0)
		return 1;
	else 
		return 0;
	}

    r->state = 1;
    r->data = "visited";

    vector * neighbors = graph_neighbors(g, target);
    size_t size = vector_size(neighbors);
  //size_t j=0; 
    for (size_t i=0;i < size; i++) {
        char * nb = vector_get(neighbors, i);
	//if (i==0) j=1;
	//printf("%s\n",nb);

        if (iscycle(nb)) {
		//it=i;
            if (strcmp(target, "")){              
	//	print_cycle_failure(target);
	
	
		}

              vector_destroy(neighbors);
            return 1;
         }
    }

    r->data = "unvisited";
    queue_push(q, target);

    vector_destroy(neighbors);
    return 0;
}

int checkdep(char * target) {
    vector * neighbors = graph_neighbors(g, target);
    size_t size = vector_size(neighbors);
    
    for (size_t i=0; i < size; i++) {
        char * nb = vector_get(neighbors, i);
        rule_t * value = (rule_t *) graph_get_vertex_value(g, nb);

        if (!strcmp(value->data, "visited")) {
            vector_destroy(neighbors);
            return 1;
        }

    }
    vector_destroy(neighbors);
    return 0;
}

int run(char * target) {
    vector * neighbors = graph_neighbors(g, target);
    int ret = 0;
    size_t size = vector_size(neighbors);

    for (size_t i=0;i < size;i++) {
        char * nb = vector_get(neighbors, i);
        rule_t * value = (rule_t *) graph_get_vertex_value(g, nb);

        if (!strcmp(value->data, "unvisited")) {
		 pthread_mutex_lock(&m);
                while (strcmp((char*)value->data, "unvisited") == 0) {
			 pthread_cond_wait(&cv, &m);
                }

		  pthread_mutex_unlock(&m);
                if (strcmp(value->data, "visited")==0)
                  ret=1;
                else
                  ret=0;
            }
        }
    
    vector_destroy(neighbors);
    return ret;
}

int ifcommand(char * target) {
    if (!(access(target, F_OK) != -1))
        return 1;

    struct stat tmp1;
    stat(target, &tmp1);

    vector * neighbors = graph_neighbors(g, target);
    int ret = 0;

    for (size_t i=0; i < vector_size(neighbors);i++) {
        if (! (access(vector_get(neighbors, i), F_OK) != -1)) {
            vector_destroy(neighbors);
            return 1;
        }

        struct stat tmp2;
        stat(vector_get(neighbors, i), &tmp2);

//?
        if (difftime(tmp2.st_mtime, tmp1.st_mtime) > 0)
            ret = 1;
      
    }

    vector_destroy(neighbors);
    return ret;
}

int run_rule(char * target) {
    rule_t * rule = (rule_t *) graph_get_vertex_value(g, target);
    
//.............................................
    if (rule->state && !strcmp(rule->data, "visited")) {
         pthread_mutex_lock(&m);
	pthread_cond_broadcast(&cv);
        pthread_mutex_unlock(&m);
	return 1;
    }
	


    if (checkdep(target)) {
         pthread_mutex_lock(&m);
	rule->data = "visited";
	pthread_cond_broadcast(&cv);
        pthread_mutex_unlock(&m);
          return 1;
    }

    if (run(target)) {
	 pthread_mutex_lock(&m);
        rule->data = "visited";
	 pthread_cond_broadcast(&cv);
        pthread_mutex_unlock(&m);

        return 1;
    }


    if (!ifcommand(target)) {
         pthread_mutex_lock(&m);
	rule->data = "done";
	pthread_cond_broadcast(&cv);
        pthread_mutex_unlock(&m);
        return 0;
    }

    vector * neighbors = graph_neighbors(g, target);

    for (size_t i=0;i< vector_size(rule->commands); i++) {
        if (system(vector_get(rule->commands, i))) {
	   pthread_mutex_lock(&m);
            rule->data = "visited";
            vector_destroy(neighbors);
	    pthread_cond_broadcast(&cv);
            pthread_mutex_unlock(&m);
            return 1;
        }
    }
   
    pthread_mutex_lock(&m);
    rule->data = "done";
    pthread_cond_broadcast(&cv);
    pthread_mutex_unlock(&m);
    vector_destroy(neighbors);
    return 0;
}

void* sth(void* ptr){
char * target = queue_pull(q);

       
    while (target != NULL) {
        if (!set_contains(s, target)) {
            set_add(s, target);
            run_rule(target);
        }
        target = queue_pull(q);

    }
	return NULL;
}


int parmake(char *makefile, size_t num_threads, char **targets) {
    // good luck!
    pthread_cond_init(&cv, NULL);
    pthread_mutex_init(&m, NULL);

    g = parser_parse_makefile(makefile, targets);
    q = queue_create(-1);
	vector* v=graph_vertices(g);
    if(iscycle("")==1){
	if (targets[0]!=NULL)
		print_cycle_failure(targets[0]);
	else
		print_cycle_failure(vector_get(v,1));

	}

    for (size_t i=0;i < num_threads; i++)
        queue_push(q, NULL);

 s = string_set_create();

/*
    char * target = queue_pull(q);

	s = string_set_create();
    while (target != NULL) {
        if (!set_contains(s, target)) {
            set_add(s, target);
            run_rule(target);
        }
        target = queue_pull(q);

    }*/
	pthread_t pth[num_threads];
 
    for (size_t i=0;i < num_threads; i++) {
        pthread_create(&pth[i], NULL, sth, NULL);
        
    }
 
    
    void * result;
 
    for (size_t i=0;i < num_threads;i++) {
        pthread_join(pth[i], &result);
        
    }

    pthread_cond_destroy(&cv);
    pthread_mutex_destroy(&m);
    queue_destroy(q);
    set_destroy(s);
    graph_destroy(g);
    return 0;
}

