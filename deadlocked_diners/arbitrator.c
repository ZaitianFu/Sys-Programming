/**
* Deadlocked Diners Lab
* CS 241 - Fall 2018
*/

#include "company.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern pthread_mutex_t arbitrator;
/**
 * This solution uses 'arbitrator' which is a global mutex
 * that the companies fight for before grabing interns.
 * Imagine that the companies have to ask a 3rd party for an intern
 * before interns. This eliminates hold and wait,
 * since a company will always get both interns or neither.
 */
void *work_interns(void *p) {
    Company *company = (Company *)p;
    pthread_mutex_t *left_intern, *right_intern;

    while (running) {
        left_intern = Company_get_left_intern(company);
        right_intern = Company_get_right_intern(company);

        // Ask for the arbitrator mutex first
        pthread_mutex_lock(&arbitrator);
        // Then do the normal intern grabbing
        pthread_mutex_lock(left_intern);
        pthread_mutex_lock(right_intern);
        // Then hire the interns.
        Company_hire_interns(company);
        pthread_mutex_unlock(right_intern);
        pthread_mutex_unlock(left_intern);
        // Let go of the arbitrator mutex
        pthread_mutex_unlock(&arbitrator);

        // Have your board meeting.
        Company_have_board_meeting(company);
    }
    return NULL;
}
