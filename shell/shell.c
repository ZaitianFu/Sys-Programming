/**
* Shell Lab
* CS 241 - Fall 2018
*/

#include "format.h"
#include "shell.h"
#include "vector.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <ctype.h>
#include <sys/types.h>

typedef struct process {
    char *command;
    char *status;
    pid_t pid;
} process;

void check_command(char*);
void showhistory(char*);
void nth_command(char*);
void cd_command(char*);
void excute(char*);
void sigheandler(int);
void newkill(int,char*);
void stop(int,char*);
void cont(int,char*);

static char *command_file_name=NULL;
static char *history_file_name=NULL;
static char* history_file_path=NULL;
static vector* history=NULL;
//vector_create(char_copy_constructor,char_destructor, char_default_constructor);
static vector* commands=NULL;
//commands=vector_create(char_copy_constructor,char_destructor,char_default_constructor);


void showhistory(char *command) {
    if (strcmp(command, "!history")==0||strcmp(command, "! history")==0 ){
	size_t len = vector_size(history);
    	const char *c;
    	for (size_t i = 0; i < len; i++) {
        	c = vector_get(history, i);
        	print_history_line(i, c);
    	}
   }
    else{
         size_t len = vector_size(history);
    	 char* c=command;
	 c++;
    	 size_t iter;
    	 char *comman;
    	 for (size_t i = 1; i <= len; i++) {
        	iter = len - i;
        	comman = vector_get(history, iter);
        	if (strstr(comman,c)!=NULL) {
            		print_command(comman);
            		check_command(comman);
            		return;
        }
    }

    print_no_history_match();
    }
}

void psing(char*command){/*
  int i = 0;
  for(i = 0; i < 128; i++){
    if(process_collection[i]){
      // printf("index %d\n",i );
      print_process_info(
        process_collection[i]->status,
        process_collection[i]->pid,
        process_collection[i]->command);
    }
  }*/
  print_process_info(
    STATUS_STOPPED,
    getpid(),
    command);
}

void nth_command(char *command) {
    char* c =command;
     c++;
    size_t len = strlen(c);

    if (!len) {
        print_invalid_index();
        return;
    }
    for (size_t i = 0; i < len; i++) {
        if (!isdigit(c[i])) {
            print_invalid_index();
            return;
        }
    }
    size_t num = atoi(c);
    size_t length = vector_size(history);
    if (num < length) {
        char *comman = vector_get(history,num);
        print_command(comman);
        check_command(comman);
        return;
    }
    else
    	print_invalid_index();
	return;
}


void cd_command(char* command){
   size_t len = strlen(command);
    if (len < 3) {
        print_no_directory("");

        return;
    }
	char* c=command;
     c+= 3;
    if (chdir(get_full_path(c))) {
        vector_push_back(history, command);
        print_no_directory(command);

    }
}

void execute(char *command) {
    vector_push_back(history,command);

pid_t pid = fork();
if (pid<0){
  print_fork_failed();
  return;
}
if (pid==0){
  char* copy = malloc(200);
strcpy(copy,command);
if (copy[strlen(command)-1] == '&') {
            copy[strlen(command)-1] = '\0';
            if (strlen(command) > 1 && copy[strlen(command)-2] == ' ')
                copy[strlen(command)-1] = '\0';
        }

  size_t num;

  char** argv = malloc(200);//ssplit
  size_t i = 0;
  char* p = strtok(copy," \n");
  while(p!=NULL){
    *(argv+i) = p;
    p = strtok(NULL," \n");
    i++;
  }
num = i;

  if(num==0||argv==NULL){
    print_exec_failed(command);
	free(argv);
	free(copy);
    exit(1);
  }
  print_command_executed(getpid());
  execvp(argv[0],argv);

  print_exec_failed(command);
	print_invalid_command(command);
	free(argv);
	free(copy);
  exit(1);

}

if (command[strlen(command)-1]=='&')
  return;

int w;
if (waitpid(pid,&w,0)<0)
  print_wait_failed();
}



void sighandler(int pid){
  if (!pid){
    print_no_process_found(pid);
  }
  else
	return;
}


void newkill(int pid, char* c){
  if (!pid){
    print_no_process_found(pid);
  }
  int stat = kill(pid,SIGTERM);
	printf("%s","killed successed");
	vector_push_back(history,c);
  if (stat==-1){
    print_no_process_found(pid);
  }
  if (stat==0){
    print_killed_process(pid,c);
  }
}

void stop(int pid, char* c){
  if (!pid){
    print_no_process_found(pid);
  }
  int stat = kill(pid,SIGTSTP);
	 vector_push_back(history,c);
  if (stat==-1){
    print_no_process_found(pid);
  }
  if (stat==0){
    print_stopped_process(pid,c);
  }
}

void cont(int pid, char* c){
  if (!pid){
    print_no_process_found(pid);
  }
  int stat = kill(pid,SIGCONT);
 vector_push_back(history,c);
  if (stat==-1){
    print_no_process_found(pid);
  }

}




void check_command( char *command) {

if (strstr(command,";")!=NULL){
	char*token = strtok(command, ";&");
	char*a=NULL;
	 while( token != NULL ) {
		a=token;
		check_command(a);
		token = strtok(NULL, ";&");
	}
  return;
}

if (strstr(command,"|")!=NULL){
	char*token = strtok(command, "|");
  token = strtok(NULL, "|");
	char*a=token;
		check_command(a);
    return;
}

if (strstr(command,"&&")!=NULL){
	char*token = strtok(command, "&&");
  char*a=NULL;
   while( token != NULL ) {
    a=token;
    check_command(a);
    token = strtok(NULL, "&&");
  }
  return;
}
    size_t len = strlen(command);

    if (len == 0)
        return;
    if (command[0] == '!') {
        showhistory(command);

        return;
    }
    if (command[0] == '#') {
        nth_command(command);

        return;
    }
    if (len > 1) {
        if (command[0] == 'c' && command[1] == 'd'){
            cd_command(command);

            return; }
        else if(strcmp(command,"exit") ==0) {
      	   vector_destroy(history);

           exit(0);
           return;
         }

         else if(command[0] == 'p' && command[1] == 's'){
             psing(command);

                return;
           }

	else if (strstr(command,"kill")!=NULL){
		newkill(atoi(command+5),command);

		return;}
	else if (strstr(command,"stop")!=NULL){
		stop(atoi(command+5),command);

		return;}
	else if (strstr(command,"cont")!=NULL){
                cont(atoi(command+5),command);

		return;}
	}
	execute(command);


	return;
}

void writetfile(char* file, vector* vector){
 FILE* fp = fopen(file,"a");

 for (size_t i=0; i<vector_size(vector); i++){
   fprintf(fp,"%s\n",vector_get(vector,i));
 }
 fclose(fp);
}



int shell(int argc, char *argv[]) {
    // TODO: This is the entry point for your shell.
 signal(SIGINT, sighandler);

 if (!(argc == 1 || argc == 3 || argc == 5))
        print_usage();

int opt;
  while ((opt = getopt (argc, argv, "h:f:")) != -1)
  {
    switch(opt){
	case 'h':
    {
		   if (optarg &&!history_file_name)
                	history_file_name = strdup(optarg);
	 	    else
                	print_usage();
		break;
	}

case 'f':
	{

              if(optarg && !history_file_name)
                  command_file_name = strdup(optarg);
              else
              	  print_usage();
	      break;

    }
default:
            print_usage();

	}
}
if(history_file_name){
	if( access( optarg, F_OK ) != -1 ) {
	//file exist
	history=string_vector_create();
	FILE* file = fopen(history_file_name, "r+");
	char line[256];

	 while (fgets(line, sizeof(line), file)){
		vector_push_back(history,line);
    writetfile(history_file_name,history);
  }

         history_file_path = get_full_path(history_file_name);
	fclose(file);
  	}
	else{
	//file not exist


	

	 history = string_vector_create();
          writetfile(history_file_name,history);


	}
 }
else
	 history = string_vector_create();



  pid_t pid = getpid();
//command

if (command_file_name) {
        if(access(command_file_name, F_OK) != -1) {
	  FILE* file = fopen(command_file_name, "r+");
          char line[256];
	commands=string_vector_create();
           while (fgets(line, sizeof(line), file))
                  vector_push_back(commands,line);
          fclose(file);
             char *command;
            char cwd[1024];
            for (size_t i = 0; i < vector_size(commands); i++) {
                command = vector_get(commands, i);
                if (getcwd(cwd, sizeof(cwd)) != NULL && command) {
                    print_prompt(cwd, pid);
                    printf("%s\n", command);
                    check_command(command);
                }
            }

        }
	else
            print_script_file_error();
    }


 char cwd[512];
    char l[512];
    char *p;

    while (1) {
        if (getcwd(cwd, sizeof(cwd)) != NULL)
            print_prompt(cwd, pid);

         if (!fgets(l, 512, stdin)) {
            return 0;
        }

        if (*l == EOF) {
            return 0;
        }
        if ((p = strchr(l, '\n')) != NULL)
            *p = '\0';
        check_command(l);
    }

return 0;

}

