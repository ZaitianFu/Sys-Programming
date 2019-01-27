/**
* Password Cracker Lab
* CS 241 - Fall 2018
*/

/**
 * Pthread_create hook module that records pthread_create calls.
 *
 * Use by calling init_cyrpt_hook() before pthread_create. Needs
 * the -ldl and -pthread library flag.
 *
 * Compile:
 *      gcc -pthread -ldl -c cyrpt_hook.c
 *      gcc -pthread -ldl pthread_create_hook.o main.c -o main
 */
#ifndef _PTHREAD_CREATE_HOOK_H_
#define _PTHREAD_CREATE_HOOK_H_

#include <pthread.h>
#include <stdbool.h>

/**
 * Initializes the pthread_create hook.
 *
 * Must be called before calling pthread_create() to work properly.
 *
 * @param verbose
 *      print out calls and errors
 */
void init_pthread_create_hook(bool verbose);

int get_pthread_create_calls();

int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                   void *(*start_routine)(void *), void *arg);

#endif /* _PTHREAD_CREATE_HOOK_H_ */
