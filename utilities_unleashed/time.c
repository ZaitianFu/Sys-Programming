/**
* Utilities Unleashed Lab
* CS 241 - Fall 2018
*/

#include <time.h>
#include<stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "format.h"
int main(int argc, char *argv[]) {
 if (argc < 2) {
        print_time_usage();
        return 0;
}

char **child_argv = malloc(argc * sizeof(char *));
    for (int i = 0; i < argc - 1; i++) {
        child_argv[i] = argv[i + 1];
    }
    child_argv[argc - 1] = NULL;

    pid_t child = fork();

    if (child < 0) {
        print_fork_failed();
    }

    else if (child == 0) {
        int status = execvp(child_argv[0], child_argv);
        if (status) {
            print_exec_failed();
        }
    }
    else {
       struct timespec start, end;
        int finish;

        clock_gettime(CLOCK_MONOTONIC, &start); 
        waitpid(child, &finish, WUNTRACED);
        clock_gettime(CLOCK_MONOTONIC, &end);

        double time = end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec)/1000000000.0;

        if (!finish)
            display_results(argv, time);
    }

    return 0;
}
