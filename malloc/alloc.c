/*
*Malloc Lab
* CS 241 - Fall 2018
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define SIZE (2L * 1024L * 1024L * 1024L)

/**
 * Allocate space for array in memory
 *
 * Allocates a block of memory for an array of num elements, each of them size
 * bytes long, and initializes all its bits to zero. The effective result is
 * the allocation of an zero-initialized memory block of (num * size) bytes.
 *
 * @param num
 *    Number of elements to be allocated.
 * @param size
 *    Size of elements.
 *
 * @return
 *    A pointer to the memory block allocated by the function.
 *
 *    The type of this pointer is always void*, which can be cast to the
 *    desired type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory, a
 *    NULL pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/calloc/
 */
void *calloc(size_t num, size_t size) {
    // implement calloc!
  size_t msize = num * size;
    void *ret = malloc(msize);
    if (ret)
        memset(ret, 0, msize);
    return ret;
   
}

/**
 * Allocate memory block
 *
 * Allocates a block of size bytes of memory, returning a pointer to the
 * beginning of the block.  The content of the newly allocated block of
 * memory is not initialized, remaining with indeterminate values.
 *
 * @param size
 *    Size of the memory block, in bytes.
 *
 * @return
 *    On success, a pointer to the memory block allocated by the function.
 *i
 *    The type of this pointer is always void*, which can be cast to the
 *    desired type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory,
 *    a null pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/malloc/
 */
static char *start = NULL;

typedef struct metadata {
	size_t size; 
	size_t freed;
	struct metadata *next; 
	void* ptr;
} metadata;

static metadata *head = NULL;
/*
void split(metadata* start, size_t size){
  size_t block = sizeof(metadata);
  size_t total = start->size;
  char* ftemp = (char*)start->ptr+ size;
  metadata* first_end = (metadata*) ftemp;
  // modify first block size && freed
  start->size  = size;
  start->freed = 0;


   first_end->size = size;
   first_end->freed = 0;

   first_end->ptr = start->ptr;

  metadata* second_begin = first_end+1;
  size_t rest  = total - size - 2*block;

  second_begin->size = rest;
  second_begin->ptr  =  second_begin+1; // may wrong
  second_begin->freed = 1;
  //second_begin -> next  = start->next;

  char* s_end = (char*) start+total+block;
  metadata* second_end = (metadata*) s_end;
  second_end->size = rest;
  second_end->ptr = second_begin->ptr;
  second_end->freed = 1;
  //second_end->next = start->next;

    start->next = NULL;
    first_end->next = NULL;

second_end->next =head;
second_begin->next = head;
head = second_begin;


  return;
}
*/
void split(metadata* start, size_t size){
  size_t total = start->size;
 
  char* ftemp = (char*)start->ptr+ size;
  metadata* first_end = (metadata*) ftemp;
 
  start->size  = size;
  start->freed = 0;

   first_end->size = size;
   first_end->ptr = start->ptr;
   first_end->freed=0;

  metadata* second_begin =(metadata*)first_end+1;
  second_begin->size = total - size - 2*sizeof(metadata);
  second_begin->ptr  =  second_begin+1;
  second_begin->freed = 1;

  char* s_end = (char*) start+total+sizeof(metadata);
  metadata* second_end = (metadata*) s_end;
  second_end->size = total - size - 2*sizeof(metadata);
  second_end->ptr = second_begin->ptr;
  second_end->freed = 1;

    start->next = NULL;
    first_end->next = NULL;

    second_begin->next = head;
    head = second_begin;
  return;
}

void merge(){
/*
  size_t asize = a->size;
  size_t bsize = b->size;
  size_t newsize = asize+bsize+2*sizeof(metadata);
  metadata* bend = (metadata*)((char*)b->ptr+bsize);
  bend->size = newsize;
  b = a;
  a->size = newsize;
*/
	//a->size =a->size+ b->size + 2*sizeof(metadata);
	//a->next=b->next;		
	//free(b);	
	//b->next=a->next->next;

	metadata*curr; metadata* prev;
 curr=head;
 while(curr){
	if (curr->next){
   		curr->size+=(curr->next->size)+2*sizeof(metadata);
   		curr->next=curr->next->next;
  	}
  	prev=curr;
  	curr=curr->next;
 }
}


void *malloc(size_t size) {
    // implement malloc!
    if(!start) 
	{	
		start = (char *) sbrk(0);
		char *return_ptr = (char *) sbrk(size+2*sizeof(metadata));
		metadata *data = (metadata *) return_ptr;
		data->next = NULL;
		data->size = size;
		return_ptr += sizeof(metadata);
		data->ptr = return_ptr;
    		data->freed = 0;

    		char* top =  (char*)data->ptr + size;
    		metadata* end = (metadata*) top;
    		end->size = size;
    		end->ptr = data->ptr;
    		end->freed = 0;
    		end->next =NULL;

    		return data->ptr;
	}

	metadata *curr =head; 
	metadata *prev = NULL; 
	while(curr)
	{	
		if(curr->size >= size)
		{
			if(prev)
				prev->next = curr->next;	
			if(curr == head)
				head = curr->next;
		
			if ((size-SIZE/2+100)<1000&&((curr->size - size)>1/3*SIZE))
                                split(curr,size);
			curr->next = NULL;
			curr->freed=0;
			//return curr->ptr;
			return curr->ptr;
		}
		prev = curr;
		curr = curr->next;
	}

	metadata* p2 = (metadata*) sbrk(size+2*sizeof(metadata));
 	 if (p2 == (void*)-1) 
		return NULL;
  	p2->size = size;
  	p2->freed = 0;
  	p2->ptr = p2+1;

	char* temp =  (char*)p2->ptr + size;
  	((metadata*)temp)->size = size;
  	((metadata*)temp)->ptr = p2->ptr;
  	((metadata*)temp)->freed = 0;

	return p2->ptr;
}

/**
 * Deallocate space in memory
 *
 * A block of memory previously allocated using a call to malloc(),
 * calloc() or realloc() is deallocated, making it available again for
 * further allocations.
 *
 * Notice that this function leaves the value of ptr unchanged, hence
 * it still points to the same (now invalid) location, and not to the
 * null pointer.
 *
 * @param ptr
 *    Pointer to a memory block previously allocated with malloc(),
 *    calloc() or realloc() to be deallocated.  If a null pointer is
 *    passed as argument, no action occurs.
 */
void free(void *ptr) {
    // implement free!
	if (!ptr)
		return;

	metadata *freed = (metadata *) ( ((char *) ptr) - sizeof(metadata));
	metadata* left = freed-1;
	metadata* lb = (metadata*)((char*)left-left->size-sizeof(metadata));
	if (freed->freed) return;
	if(!head)
	{	
		 head = freed;
	         freed->freed=1;

		//return;
	}
	else
	{
		if (lb->freed){                     
			lb->size = lb->size+freed->size+2*sizeof(metadata);
                         return;
                 }

		freed->next=head;
		head = freed;
		freed->freed=1;
		return;
	}

/*
 metadata* before = (metadata*)((char*)freed-sizeof(metadata));
 metadata* after = (metadata*)((char*)freed+sizeof(metadata));
 if (before && before->freed==1)
    merge(before,freed); 
*/
// if (head->next)
 //  merge(head, head->next);
//	 merge();
}

/**
 * Reallocate memory block
 *
 * The size of the memory block pointed to by the ptr parameter is changed
 * to the size bytes, expanding or reducing the amount of memory available
 * in the block.
 *
 * The function may move the memory block to a new location, in which case
 * the new location is returned. The content of the memory block is preserved
 * up to the lesser of the new and old sizes, even if the block is moved. If
 * the new size is larger, the value of the newly allocated portion is
 * indeterminate.
 *
 * In case that ptr is NULL, the function behaves exactly as malloc, assigning
 * a new block of size bytes and returning a pointer to the beginning of it.
 *
 * In case that the size is 0, the memory previously allocated in ptr is
 * deallocated as if a call to free was made, and a NULL pointer is returned.
 *
 * @param ptr
 *    Pointer to a memory block previously allocated with malloc(), calloc()
 *    or realloc() to be reallocated.
 *
 *    If this is NULL, a new block is allocated and a pointer to it is
 *    returned by the function.
 *
 * @param size
 *    New size for the memory block, in bytes.
 *
 *    If it is 0 and ptr points to an existing block of memory, the memory
 *    block pointed by ptr is deallocated and a NULL pointer is returned.
 *
 * @return
 *    A pointer to the reallocated memory block, which may be either the
 *    same as the ptr argument or a new location.
 *
 *    The type of this pointer is void*, which can be cast to the desired
 *    type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory,
 *    a NULL pointer is returned, and the memory block pointed to by
 *    argument ptr is left unchanged.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/realloc/
 */
void *realloc(void *ptr, size_t size) {
// implement realloc!
	if (!ptr)
		return malloc(size);

	if (!size)
	{
		free(ptr);
		return NULL;
	}
	metadata *data = (metadata *) ((char *) ptr - sizeof(metadata));
	size_t old_size = data->size;
	
	if(size <= old_size)
		return ptr;
	
	void* ret = malloc(size);
	if (!ret) return NULL;
	size_t min;
	if (old_size<size)
		min=old_size;
	else
		min=size;
	memmove(ret, ptr, min);
	free(ptr);
	return ret;
}
