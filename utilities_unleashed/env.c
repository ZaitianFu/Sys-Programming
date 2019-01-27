/**
* Utilities Unleashed Lab
* CS 241 - Fall 2018
*/
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include "format.h"
extern char** env;
int main(int argc, char *argv[]) {
	if (argc == 1) {
		int i = 0;
		char* s=env[i];
		while (s){
			puts(s);
			i++;
			s=env[i];	
		}
		return 0;
	}

	if (argc == 2) {
		print_env_usage();
		return 0;
	}

	pid_t child = fork();
	if (child == -1) {
		print_fork_failed();
		return 0;
	}

	if (child == 0) {
        execvp(argv[2], &argv[2]);
        print_exec_failed();
        exit(1111);}
	
   
    

  
    return 0;
}
