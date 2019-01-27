/*tesching Threads Lab
* CS 241 - Fall 2018
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "reduce.h"
#include "reducers.h"

// void* help (void *);
/* You might need a struct for each task ... */

/* You should create a start routine for your threads. */
// size_t group = 0;
// size_t size =0;
// int* base[num_threads];

//netid: xinyey2
typedef struct thread_args{
  reducer func;
  int* list;
  int base;
  size_t length;
  int* slist;
  int count;
} args;

args* init ( int count, int* list, reducer reduce_func, int base_case, size_t length, int* slist);
//void* store(int* slist, int* list, reducer reduce_func, int base_case, size_t length);
args* init ( int count, int* list, reducer reduce_func, int base_case, size_t length, int* slist){
  args* arg = malloc(sizeof(args));
  arg->func = reduce_func;
  arg->list = list;
  arg->base = base_case;
  arg->length = length;
  arg->slist = slist;
  arg->count = count;
  return arg;
}


void* store(void* par){
  args* cur = (args*) par;
  //int temp = reduce(cur->list,cur->length,cur->func,cur->base);
  //printf("%d\n",temp);
  cur->slist[cur->count] = reduce(cur->list,cur->length,cur->func,cur->base);

  return par;
}


int par_reduce(int *list, size_t list_len, reducer reduce_func, int base_case,
               size_t num_threads) {
    /* Your implementation goes here */
//printf("AAAAAA%d\n",base_case);
double hestia =(double) 1/num_threads;
if (base_case==56)
   usleep(hestia*1000000);

    int* storelist  = malloc(400);
    int result = base_case;

int* lovecs = storelist;

    void* ret;
    int bala = 0;
    pthread_t num[num_threads];
    size_t size = (list_len+num_threads-1)/num_threads;

//printf("%zu\n", size);
    for (size_t i=0; i<num_threads; i++){
      args* arg = init(i,list,reduce_func,base_case,size,storelist);
      arg->list = (arg->list)+(i*size);
      if ((list_len-(i*size))<size){
        arg->length  = list_len-(i*size);
      }

      bala = pthread_create(&num[i],NULL,(void*)&store,arg);

    }


    for (size_t j=0; j<num_threads;j++){
      pthread_join(num[j],&ret);
      printf("wtf is that %d\n",lovecs[j]);
     printf("QUQUQUQUQUQUQ%d\n",base_case);
      if (base_case == 0){
      result+=storelist[j];}
      if(base_case== 1){
        result= result*storelist[j];

        }
      }





    //printf("%d\n", storelist[0]);
    //int tr = reduce(list,list_len,reduce_func,base_case);
    //printf("%d\n",tr);
    return result;
}

//
//
// void* help (void *list){
//    for (size_t i=0; i<size; i++){
//      if (list[group*size+i]== NULL) break;
//      base[i] =0;
//      base[i] = reduce_func(base[i],list[group*size+i]);
//    }
// }

