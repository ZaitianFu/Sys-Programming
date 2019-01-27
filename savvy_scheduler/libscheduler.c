/**
* Savvy_scheduler Lab
* CS 241 - Fall 2018
*/
//
#include "libpriqueue/libpriqueue.h"
#include "libscheduler.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "print_functions.h"
// partner: xinyey2
typedef struct _job_info {
    int id;

   int priority;
	double arrival_time;
	double burst_time;
	double start_time;
	double 	end_time;
	double remain_time;
    /* Add whatever other bookkeeping you need into this struct. */
} job_info;

priqueue_t pqueue;
scheme_t pqueue_scheme;
comparer_t comparision_func;
//
int job_num=0;
double waiting_time=0;
double turnarround_time=0;
double response_time=0;

void scheduler_start_up(scheme_t s) {
    switch (s) {
    case FCFS:
        comparision_func = comparer_fcfs;
        break;
    case PRI:
        comparision_func = comparer_pri;
        break;
    case PPRI:
        comparision_func = comparer_ppri;
        break;
    case PSRTF:
        comparision_func = comparer_psrtf;
        break;
    case RR:
        comparision_func = comparer_rr;
        break;
    case SJF:
        comparision_func = comparer_sjf;
        break;
    default:
        printf("Did not recognize scheme\n");
        exit(1);
    }
    priqueue_init(&pqueue, comparision_func);
    pqueue_scheme = s;
    // Put any set up code you may need here
}

static int break_tie(const void *a, const void *b) {
    return comparer_fcfs(a, b);
}

int comparer_fcfs(const void *a, const void *b) {
	job_info* ainfo=(job_info*)(((job*)a)->metadata);
	job_info* binfo=(job_info*)(((job*)b)->metadata);
	if (ainfo->arrival_time < binfo->arrival_time) return -1;
	else if (ainfo->arrival_time > binfo->arrival_time) return 1;
	else return 0;

}

int comparer_ppri(const void *a, const void *b) {
    // Complete as is
    return comparer_pri(a, b);
}

int comparer_pri(const void *a, const void *b) {
	job_info* ainfo=(job_info*)(((job*)a)->metadata);
        job_info* binfo=(job_info*)(((job*)b)->metadata);
        if (ainfo->priority < binfo->priority) return -1;
        else if (ainfo->priority > binfo->priority) return 1;
        else return comparer_fcfs(a,b);

}

int comparer_psrtf(const void *a, const void *b) {
 	job_info* ainfo=(job_info*)(((job*)a)->metadata);
        job_info* binfo=(job_info*)(((job*)b)->metadata);
        if (ainfo->remain_time < binfo->remain_time) return -1;
        else if (ainfo->remain_time > binfo->remain_time) return 1;
        else return comparer_fcfs(a,b);

}

int comparer_rr(const void *a, const void *b) {
    return comparer_pri(a,b);
}

int comparer_sjf(const void *a, const void *b) {
        job_info* ainfo=(job_info*)(((job*)a)->metadata);
        job_info* binfo=(job_info*)(((job*)b)->metadata);
        if (ainfo->burst_time < binfo->burst_time) return -1;
        else if (ainfo->burst_time > binfo->burst_time) return 1;
        else return comparer_fcfs(a,b);

}

// Do not allocate stack space or initialize ctx. These will be overwritten by
// gtgo
void scheduler_new_job(job *newjob, int job_number, double time,
                       scheduler_info *sched_data) {
    // TODO complete me!
job_info* ret=(job_info*) malloc(sizeof(job_info));
ret->id = job_number;
  ret->priority = sched_data->priority;
  ret->arrival_time = time;
  ret->burst_time = sched_data->running_time;
 ret->start_time = -1;
 ret->end_time = -1;
 ret->remain_time = sched_data->running_time;
 newjob->metadata = ret;

  priqueue_offer(&pqueue, newjob);

}

job *scheduler_quantum_expired(job *job_evicted, double time) {
    // TODO complete me!
	if (priqueue_size( &pqueue)==0){
 		if (job_evicted ==NULL)
			return NULL;
        	else return job_evicted;

	}
	else if(job_evicted==NULL){
		job * first=priqueue_peek(&pqueue);
		job_info* first_info=(job_info*)first->metadata;
    		if(first_info->remain_time == 0){
      			priqueue_poll(&pqueue);
      			job *ret = priqueue_peek(&pqueue);
      			return ret;
    		}		
    		first_info->start_time = time;
    		return first;
  	}
	job_info *evicted_info = (job_info*)job_evicted->metadata;
  	if(evicted_info->start_time == -1) evicted_info->start_time = time;
	if(!(pqueue_scheme == PPRI || pqueue_scheme == PRI || pqueue_scheme == PSRTF || pqueue_scheme == RR) && job_evicted != NULL){
		if(evicted_info->start_time == -1) 
			evicted_info->start_time = time;
		return job_evicted;}
	else{
		evicted_info->remain_time=time-evicted_info->start_time;
  		job *head = priqueue_poll(&pqueue);
    		job_info *head_info = (job_info*)head->metadata;
    		if(pqueue_scheme == RR){
        		head_info->priority += 10;
		response_time+= head_info->start_time-head_info->arrival_time;
    		}		
  
  		if(head_info->remain_time > 0) priqueue_offer(&pqueue, head);
  			return priqueue_peek(&pqueue);
 	}		
   	 return NULL;
}
   

void scheduler_job_finished(job *job_done, double time) {
    // TODO complete me!
	job_num++;
	job_info* done_info=(job_info*)job_done->metadata;
	done_info->remain_time=0;
	done_info->end_time=time;
	waiting_time += (done_info->end_time - done_info->arrival_time) - done_info->burst_time;
  	response_time += done_info->start_time - done_info->arrival_time;
  	turnarround_time += done_info->end_time - done_info->arrival_time;
  	free(done_info);
 	done_info = NULL;

}

static void print_stats() {
    fprintf(stderr, "turnaround     %f\n", scheduler_average_turnaround_time());
    fprintf(stderr, "total_waiting  %f\n", scheduler_average_waiting_time());
    fprintf(stderr, "total_response %f\n", scheduler_average_response_time());
}

double scheduler_average_waiting_time() {
    // TODO complete me!

    return waiting_time/job_num;
}

double scheduler_average_turnaround_time() {
    // TODO complete me!
    return turnarround_time/job_num;
}

double scheduler_average_response_time() {
    // TODO complete me!
    return response_time/job_num;
}

void scheduler_show_queue() {
    // Implement this if you need it!i

}

void scheduler_clean_up() {
    priqueue_destroy(&pqueue);
    print_stats();
}
