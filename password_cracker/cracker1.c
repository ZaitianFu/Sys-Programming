/**password Cracker Lab
** CS 241 - Fall 2018
*/
	
	#include "cracker1.h"
	#include "format.h"
	#include "utils.h"
	#include "queue.h"
	#include <pthread.h>
	#include <stdio.h>
	#include <string.h>
	#include <unistd.h>
	#include <crypt.h>
	
	typedef struct jobline {
	    char *name;
	    char *hash;
	    char *pass;
	} jobline;
	

	typedef struct io {
	    size_t id;
	    size_t numRecovered;
	    size_t numFailed;
	} io;
	

	jobline *parse_line();
	void eachthread(void*);
	char *init_password(char*);
	
	//
	queue*	jobpool;
	pthread_t*	thread_pool;
	io*	thread_io_pool;
	
	int start(size_t thread_count) {
	    // TODO your code here, make sure to use thread_count!
	    // Remember to ONLY crack passwords in other threads
	    jobpool        = queue_create(32);
	    thread_pool     = malloc(sizeof(pthread_t)    * thread_count);
	    thread_io_pool  = malloc(sizeof(io) * thread_count);

		              jobline *job= parse_line();
              while (job) {
                  queue_push(jobpool, job);
                  job=parse_line();
              }

              for (int i=thread_count; i>0; i--)
                  queue_push(jobpool, NULL);

		
            for (size_t i = 0; i < thread_count; i++) {
 
                  thread_io_pool[i].id           = i + 1;
		  thread_io_pool[i].numFailed    = 0;
                  thread_io_pool[i].numRecovered = 0;
                 
                  pthread_create(thread_pool + i, NULL, (void *) &eachthread, (void *) (thread_io_pool + i));
              }

	    int numre = 0;
	    int numf    = 0;

	    for (int i = 0; i < (int)thread_count; i++) {
	        pthread_join(thread_pool[i], NULL);
	        numre += thread_io_pool[i].numRecovered;
	        numf    += thread_io_pool[i].numFailed;
	    }
	
	    v1_print_summary(numre, numf);
	
	    queue_destroy(jobpool);

	    free(thread_pool);
	    free(thread_io_pool); 
	// DO NOT change the return code since AG uses it to check if your
	// program exited normally
		return 0;
	}
			
	jobline *parse_line() {
/*
	    char* name=malloc(sizeof(char)*16);
	    char* hash=malloc(sizeof(char)*32);
	    char* pass=malloc(sizeof(char)*32);

	int a=scanf("%s%s%s",name,hash,pass);
	if (a !=EOF){
	jobline *job = malloc(sizeof(jobline));
                memset(job->name , name;
                memset(job->hash , hash;
                memset(job->pass , pass;
		free(name);free(hash);free(pass);
                return job;
	}
	else{
	    free(name);free(hash);free(pass);
	    return NULL;
		}
*/
	char   *line = NULL;
	    size_t  len = 0;
	    char *name;
	    char *hash;
	    char *pass;
	   
	   int readnum=getline(&line, &len, stdin);
	    if (readnum != -1) 
	{
	        line[readnum - 1] = '\0';
	        name    = line;	

	        hash    = strchr(name, ' ');
	        hash[0] = '\0';
	        hash   += 1;
	

	        pass    = strchr(hash, ' ');
	        pass[0] = '\0';
	        pass   += 1;
	

	        jobline *job = malloc(sizeof(jobline));
	        job->name = name;
	        job->hash = hash;
	        job->pass = pass;
	        return job;
	    }
		free(line);
		return NULL;
	}
	
	// WORKER ROUTINE FOR A SINGLE THREAD
	void eachthread(void *ptr) {
	    io *thread_io = ptr;
	
	    int end;
	    int result;
	    int hashCount;
	    double timeElapsed;
	

	    struct crypt_data cdata;
	    cdata.initialized = 0;
	    const char *hash;

	    jobline *job;
	    while ( (job = queue_pull(jobpool)) ) {
	        end       = 0;
	        result    = 0;
	        hashCount = 0;
	        timeElapsed = getThreadCPUTime();
	
	        v1_print_thread_start(thread_io->id, job->name);
	        char *last = init_password(job->pass);

	        do {
	            hash = crypt_r(job->pass, "xx", &cdata);
	            hashCount++;
	            result = !strcmp(hash, job->hash);
	            end = !strcmp(last, job->pass);
	        } while ( !end && !result && incrementString(job->pass));	

	        if (result)
	            thread_io->numRecovered++;
	        else
	            thread_io->numFailed++;	

	        v1_print_thread_result(thread_io->id, job->name, job->pass, hashCount, getThreadCPUTime() - timeElapsed, !result);	

	        free(last);
		free(job->name);
		//free(job->hash);		
		//free(job->pass);
	        free(job);
	    }
	}
	
	char *init_password(char *password) {
	    char *last = strdup(password);
	    char *it = last;
	    while (*password) {
	        if (*password == '.') {
	            *password = 'a';
	            *it = 'z';
	        }
	        password++;
	        it++;
	    }
	    return last;
	}

