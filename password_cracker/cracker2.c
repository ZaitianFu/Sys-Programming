
/*password Cracker Lab
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

static pthread_mutex_t m=PTHREAD_MUTEX_INITIALIZER; 
pthread_barrier_t barrier;

typedef struct info{
	char name[32];
	char hash[32];
	char pass[32];
	char cpy[32];
	int hash_count;
	char *password;
	int eof;
	int i_prefix;
	int found;
	int num_threads;
	double cpu_sum;
} info;

typedef struct io{
	pthread_t id;
	int number;
	int rv;
} io;

info * someinfo;

void* eachthread(void*);

int start(size_t thread_count) {

	someinfo = malloc(sizeof(info));
	someinfo->hash_count = 0;
	someinfo->cpu_sum = 0;
	
	someinfo->password = malloc(sizeof(char)*32);

	pthread_barrier_init (&barrier, NULL, thread_count+2-1);
	pthread_mutex_init(&m, NULL);

	someinfo->num_threads = thread_count;
	someinfo->found = -1;

	double initial_time;
	double final_time;
	double initial_cpu;
	double final_cpu;

	io ** breaker = malloc(sizeof(io*)*thread_count);
	for(int i = 0; i < (int)thread_count; i++){
		breaker[i] = malloc(sizeof(io));
		breaker[i]->number = i+1;
	}

	for(int i = 0; i < (int)thread_count; i++){
		pthread_create(&(breaker[i]->id), NULL, eachthread, (void*)breaker[i]);
	}

	
	char name[32];
	char hash[32];
	char pass[32];
	int prefix = 0;
	int a;

	while(true){
		initial_cpu = getThreadCPUTime();
 		initial_time = getTime();
		a = scanf("%s%s%s", name, hash, pass);

			someinfo->eof = a;

		someinfo->found = 1;
		 someinfo->hash_count = 0;
                 someinfo->cpu_sum = 0;


		pthread_barrier_wait(&barrier);

		if(a == EOF) break;

		v2_print_start_user(name);
			
			strcpy((someinfo->name), name);
			strcpy((someinfo->hash), hash);
			strcpy((someinfo->pass), pass);
			
			pthread_mutex_lock(&m);
			prefix = (getPrefixLength(someinfo->pass));
			someinfo->i_prefix = prefix;
			pthread_mutex_unlock(&m);

		pthread_barrier_wait(&barrier);
		pthread_barrier_wait(&barrier);

		final_time = getTime();
		final_cpu = getThreadCPUTime();

		someinfo->cpu_sum += final_cpu - initial_cpu;

		v2_print_summary(name, someinfo->password, someinfo->hash_count,
                      final_time-initial_time, someinfo->cpu_sum, someinfo->found);
	
		//a = scanf("%s %s %s", name, hash, partial);
		}

	void * result;
	for(int i = 0; i < (int)thread_count; i++){
		pthread_join(breaker[i]->id, &result);
	}
	
	strcpy(someinfo->cpy,pass);
	free(someinfo->password);
	free(someinfo);

	for(size_t i = 0; i < thread_count; i++){
		free(breaker[i]);
	}
	free(breaker);

	pthread_mutex_destroy(&m);
	pthread_barrier_destroy(&barrier);

	return 0;
}


void * eachthread(void * ptr){

	io * in = (io*)ptr;
	
	char * buffer = malloc(sizeof(char)*32);
	char * gethash;
	char * cmp = malloc(sizeof(char)*32);
	
	long start_index, count_limit;
	int num_hashes = 0;
	int thread_status;
	int other;
	double initial_cpu, final_cpu;

	struct crypt_data cdata;
	cdata.initialized = 0;

	while(true){

		thread_status = 2;
		in->rv=thread_status-num_hashes;

		pthread_barrier_wait(&barrier);
		if(someinfo->eof == EOF) break;

		pthread_barrier_wait(&barrier);

		getSubrange(strlen(someinfo->pass) - someinfo->i_prefix, someinfo->num_threads, in->number, &start_index, &count_limit);

		strcpy(buffer, someinfo->pass);
		setStringPosition(buffer+someinfo->i_prefix, start_index);

		v2_print_thread_start(in->number, someinfo->name, start_index,buffer);

		
		initial_cpu = getThreadCPUTime();
		num_hashes=0;
		for(int i = 0; i < (int)count_limit; i++){
			
			pthread_mutex_lock(&m);
				other = someinfo->found;
			pthread_mutex_unlock(&m);

			if(other == 0){	
				thread_status = 1;
				break;
			}


			num_hashes++;

			gethash = crypt_r(buffer, "xx", &cdata);
			strcpy(cmp, gethash);


			if(strcmp(someinfo->hash, cmp) == 0){

				strcpy(someinfo->password, buffer);

				pthread_mutex_lock(&m);
				someinfo->found = 0;
				pthread_mutex_unlock(&m);

				thread_status = 0;

				break;
			}

			incrementString(buffer + someinfo->i_prefix);

		}

		final_cpu = getThreadCPUTime();

		pthread_mutex_lock(&m);
		someinfo->cpu_sum += (final_cpu - initial_cpu);
		pthread_mutex_unlock(&m);

		v2_print_thread_result(in->number, num_hashes, thread_status);

		pthread_mutex_lock(&m);
		someinfo->hash_count += num_hashes;
		pthread_mutex_unlock(&m);

		pthread_barrier_wait(&barrier);
	}

	free(buffer);
	free(cmp);
	return NULL;
}


