/**
* Password Cracker Lab
* CS 241 - Fall 2018
*/

/**
 * Crypt hook module that records crypt calls.
 *
 * Use by calling init_cyrpt_hook() before crypt. Needs
 * the -ldl and -pthread library flag.
 *
 * Compile:
 *      gcc -pthread -ldl -c cyrpt_hook.c
 *      gcc -pthread -ldl crypt_hook.o main.c -o main
 */
#ifndef _CRYPT_HOOK_H_
#define _CRYPT_HOOK_H_

#include <pthread.h>
#include <stdbool.h>

/**
 * Initializes the crypt hook.
 *
 * Must be called before calling crypt() to work properly.
 *
 * @param verbose
 *      print out calls and errors
 */
void init_crypt_hook(bool verbose);

char *crypt(const char *key, const char *salt);

#endif /* _CRYPT_HOOK_H_ */
