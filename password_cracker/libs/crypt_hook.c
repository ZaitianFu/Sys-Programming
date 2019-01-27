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

#include "crypt_hook.h"

typedef char *(*crypt_fn)(const char *key, const char *salt);
static crypt_fn orig_crypt;

bool c_verbose = false;

static const char *DLSYM_CRYPT_NULL_MESG = "Unable to get original crypt()\n";
static const char *ORIG_CRYPT_NULL_MESG = "Original crypt() was null\n";
static const char *CRYPT_MESG = "crypt() was called\n";

void init_crypt_hook(bool verbose) {
    c_verbose = verbose;

    orig_crypt = dlsym(RTLD_NEXT, "crypt");

    if (!orig_crypt) {
        int res = write(STDERR_FILENO, DLSYM_CRYPT_NULL_MESG,
                        strlen(DLSYM_CRYPT_NULL_MESG));
        (void)res;
        exit(EXIT_FAILURE);
    }
}

char *crypt(const char *key, const char *salt) {
    if (c_verbose) {
        int res = write(STDERR_FILENO, CRYPT_MESG, strlen(CRYPT_MESG));
        (void)res;
        c_verbose = false;
    }

    if (orig_crypt) {
        return orig_crypt(key, salt);
    } else {
        int res = write(STDERR_FILENO, ORIG_CRYPT_NULL_MESG,
                        strlen(ORIG_CRYPT_NULL_MESG));
        (void)res;
        exit(EXIT_FAILURE);
    }
}
