/**
* Password Cracker Lab
* CS 241 - Fall 2018
*/

/*#define _GNU_SOURCE*/
#include <dlfcn.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "pthread_create_hook.h"

typedef int (*pthread_create_fn)(pthread_t *thread, const pthread_attr_t *attr,
                                 void *(*start_routine)(void *), void *arg);
static pthread_create_fn orig_pthread_create;

bool p_verbose = false;

static int pthread_create_calls = 0;

static const char *DLSYM_PTHREAD_CREATE_NULL_MESG =
    "Unable to get original pthread_create()\n";
static const char *ORIG_PTHREAD_CREATE_NULL_MESG =
    "Original pthread_create() was null\n";
static const char *PTHREAD_CREATE_MESG = "pthread_create() was called\n";

void init_pthread_create_hook(bool verbose) {
    p_verbose = verbose;

#ifdef SANITIZE_THREADS
    orig_pthread_create = dlsym(RTLD_DEFAULT, "__interceptor_pthread_create");
#else
    orig_pthread_create = dlsym(RTLD_NEXT, "pthread_create");
#endif

    if (!orig_pthread_create) {
        int res = write(STDERR_FILENO, DLSYM_PTHREAD_CREATE_NULL_MESG,
                        strlen(DLSYM_PTHREAD_CREATE_NULL_MESG));
        (void)res;
        exit(EXIT_FAILURE);
    }
}

int get_pthread_create_calls() {
    return pthread_create_calls;
}

int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                   void *(*start_routine)(void *), void *arg) {
    pthread_create_calls++;
    if (p_verbose) {
        int res = write(STDERR_FILENO, PTHREAD_CREATE_MESG,
                        strlen(PTHREAD_CREATE_MESG));
        (void)res;
    }

    if (orig_pthread_create) {
        return orig_pthread_create(thread, attr, start_routine, arg);
    } else {
        int res = write(STDERR_FILENO, ORIG_PTHREAD_CREATE_NULL_MESG,
                        strlen(ORIG_PTHREAD_CREATE_NULL_MESG));
        (void)res;
        exit(EXIT_FAILURE);
    }
}
