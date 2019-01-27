/**
* Mad Mad Access Patterns Lab
* CS 241 - Fall 2018
*/

#include "tree.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
//partner: xinyey2         
static void* file;
/*
  Look up a few nodes in the tree and print the info they contain.
  This version uses mmap to access the data.

  ./lookup2 <data_file> <word> [<word> ...]
*/

void recurse (int fd, uint32_t offset, char* word){
  if (offset<1){
     printNotFound(word);
     return;
  }
  BinaryTreeNode *node = file+offset;
  
  int res = strcmp(word, node->word);
  if (res==0){
    printFound(word,node->count, node->price);
    return;
  }
  else if (res>0){
    recurse(fd, node->right_child,word);
  }
  else {
    recurse(fd,node->left_child,word);
  }
  }

int main(int argc, char **argv) {

  if (argc<3){printArgumentUsage();  exit(1);}
  char* c = argv[1];
  
  int fd = open(c,O_RDONLY);
if (fd ==-1){
	openFail(c);
	exit(1);	
}

  struct stat detail;
if ( stat(c,&detail)==-1){
  openFail(c);
}

  file = mmap(NULL,detail.st_size,PROT_READ,MAP_SHARED,fd,0);

  if (file == MAP_FAILED) {
         mmapFail(c);
         exit(3);
     }
    close(fd);
  for (int i=2; i<argc; i++){
    recurse(fd,4,argv[i]);
  }


    return 0;
}
