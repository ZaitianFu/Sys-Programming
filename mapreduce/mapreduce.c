/*Lab
* CS 241 - Fall 2018
*/
//xinyey2
#include "utils.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#define SPLIT "splitter"
#define MAX(a, b) (((a) > (b)) ? (a) : (b));
static size_t mapper_count;
static int mapper_pipe[100][2];
static int reducer_pipe[2];

void setup_fds(int, int );
void close_and_exec(char *, char *const *);
pid_t start_reducer(char *, int , char *);
pid_t *read_input_chunked(char *filename, int *fds_to_write_to,
                          int num_mappers);
pid_t *start_mappers(char *mapper_exec, int num_mappers, int *read_mapper,
                     int write_reducer);
size_t count_lines(const char *);

void wait_finish(char* string_to_print,pid_t* process,size_t process_count){
  size_t i = 0;
  for(i = 0; i < process_count; i++){
   int status = 0;
   pid_t temp_pid = waitpid(process[i], &status, 0);
   if(temp_pid != -1 && WIFEXITED(status)){
    int low8bits = WEXITSTATUS(status);
    if(low8bits){
      print_nonzero_exit_status(string_to_print, low8bits);
    }
    }
  }
}
void dup2_redirection(size_t i){
  dup2(mapper_pipe[i][0], 0);
  dup2(reducer_pipe[1], 1);
}

void close_mapper_pipe(){
  size_t i = 0;
  for(i = 0; i < mapper_count; i++){
    close(mapper_pipe[i][0]);
    close(mapper_pipe[i][1]);
  }
}

int main(int argc, char **argv) {
  if(argc != 6) {
    print_usage();
    return 1;
  }
  //initialize
   mapper_count = atol(argv[5]);
   char *input_file = argv[1];
   char *output_file = argv[2];
   size_t i = 0;

   //Create an input pipe for each mapper.
   for(size_t i = 0; i < mapper_count; i++){
     pipe2(mapper_pipe[i], O_CLOEXEC);
   }
   // Create one input pipe for the reducer.
   pipe2(reducer_pipe, O_CLOEXEC);

   // Open the output file.
   int fd = open(output_file, O_CREAT | O_TRUNC | O_RDWR, S_IWUSR | S_IRUSR);

   pid_t splitter_process[mapper_count];
   pid_t mapper_process[mapper_count];
   pid_t reducer_process;

   // Start a splitter process for each mapper.
  for(i = 0; i < mapper_count; i++){
     splitter_process[i] = fork();

    if(splitter_process[i] == 0){
      char temp_string[128];
      sprintf(temp_string, "%zu", i);
      close(mapper_pipe[i][0]);
      dup2(mapper_pipe[i][1], 1);

      execl(SPLIT, SPLIT, input_file, argv[5], temp_string, NULL);
      exit(1);
  }

 }
 // Start all the mapper processes.
for(i = 0; i < mapper_count; i++){

     mapper_process[i] = fork();//fork mapper

    if(mapper_process[i] == 0){
     close(mapper_pipe[i][1]);
     dup2_redirection(i);
     execl(argv[3], argv[3], NULL);
     exit(1);
   }

 }
 // Start the reducer process.
 reducer_process = fork();

if(reducer_process == 0){
     // child
     dup2(reducer_pipe[0], 0);
     dup2(fd, 1);
     execl(argv[4], argv[4], NULL);
     exit(1);
}
 close(reducer_pipe[0]);
 close(reducer_pipe[1]);

  //close mapper
close_mapper_pipe();

  // Wait for the reducer to finish.
  wait_finish(SPLIT,splitter_process,mapper_count);
  wait_finish(argv[3],mapper_process,mapper_count);
  wait_finish(argv[4],&reducer_process,1);
  // Print nonzero subprocess exit codes.
  print_num_lines(output_file);
  close(fd);
// Count the number of lines in the output file.
  return 0;
}

