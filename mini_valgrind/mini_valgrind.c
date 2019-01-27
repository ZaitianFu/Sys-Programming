/**
* Mini Valgrind Lab
* CS 241 - Fall 2018
*/
//partner NetID: xinyey2 zhao108 kmle2
#include "mini_valgrind.h"
#include <stdio.h>
#include <string.h>
meta_data *head;
size_t total_memory_requested;
size_t total_memory_freed;
size_t invalid_addresses;

void *mini_malloc(size_t request_size, const char *filename,
                  void *instruction) {
    // your code here
	if (request_size==0)
    		return NULL;
	meta_data* data =(meta_data*)malloc(sizeof(meta_data)+request_size);
	if (data==NULL) return NULL;
	data->request_size=request_size;
	data->filename=filename;
	data->instruction=instruction;
	data->next=head;
	head=data;
	total_memory_requested+=request_size;
	void* ret=data+1;
	return ret;
}

void *mini_calloc(size_t num_elements, size_t element_size,
                  const char *filename, void *instruction) {
    // your code here
	if (num_elements==0 || element_size==0)
    		return NULL;
	size_t reqsize=num_elements* element_size;
	
    meta_data* callocd  = calloc(reqsize+sizeof(meta_data),1);
    if (callocd==NULL){
      return NULL;
    }
    callocd-> filename = filename;
    callocd-> instruction = instruction;
    callocd->request_size  = reqsize;
    callocd->next = head;
    head = callocd;
    total_memory_requested += reqsize;
    return head+1;
}

void *mini_realloc(void *payload, size_t request_size, const char *filename,
                   void *instruction) {
    // your code here
/*	if (! payload)
		return mini_malloc(request_size,filename,instruction);
	else if (request_size==0)
		mini_free(payload);
*/if (payload ==NULL){
      if (request_size==0){
        return NULL;
      }
      void* ret = mini_malloc(request_size,filename,instruction);
      return ret;
    }
    else if (request_size ==0){
      mini_free(payload);
      return NULL;
    }
    _Bool inside =0;

   meta_data*  louis = payload-sizeof(meta_data);
    meta_data * temp = head;
    while (temp){
      if(louis == temp)
      inside = 1;
      temp = temp->next;
    }
    if (!inside){
      invalid_addresses++;
      return NULL;
    }
    if (louis==NULL){
      invalid_addresses++;
      return NULL;
    }

    meta_data* start = louis;
    if (start->request_size>request_size){
      total_memory_freed+= (start->request_size-request_size);
    }else{
      total_memory_requested += (request_size-start->request_size);
        }
    size_t need = request_size+sizeof(meta_data);
       
    start = realloc(start,need);
    start->request_size = request_size;
    if (start ==NULL){
      return NULL;
    }


    return start+1;

}

void mini_free(void *payload) {
    // your code here
/*	if(!payload)
		return;
	
	meta_data* prev=NULL;
	meta_data* newhead=head;
	meta_data* newnext;
	while(newhead){
		newnext = newhead->next;
        	void *memo = ((void *)newhead )+sizeof(meta_data);
       		if (payload == memo) {
            		if (prev) {
                		prev->next = newnext;
            		}		 
			else {
                		head = newnext;
            		}
            		total_memory_freed += newhead->request_size;
            		free(newhead);
			newhead=NULL;
            		break;
        	}
        
        	prev = newhead;
        	newhead = newnext;
    	}
	invalid_addresses++;
	return;*/
 if (payload==NULL){
      return;
    }
    meta_data* data  = payload-sizeof(meta_data);

    _Bool inside =0;
    meta_data * temp = head;
    meta_data * findprev;
    findprev = head;

    while (temp){
      if(data == temp){
      inside = 1;
      break;
    }
      temp = temp->next;
    }
    if (!inside){
      invalid_addresses++;
      return;
    }
    if (data==NULL){
      invalid_addresses++;
      return;
    }

      size_t needtofree = data->request_size;
      total_memory_freed+= needtofree;

      while (findprev){
        if (findprev->next ==NULL){
          break;
        }
        if(data == findprev->next){
        findprev->next = data->next;
        break;
      }
        findprev = findprev->next;
      }

      if (head==data)
        head = data->next;

      // meta_data* start = caito;


      //((meta_data*)(payload-sizeof(meta_data))) = ((meta_data*)(payload-sizeof(meta_data)))->next;
      meta_data* eliminated = data;
      free(eliminated);

      eliminated = NULL;


}
