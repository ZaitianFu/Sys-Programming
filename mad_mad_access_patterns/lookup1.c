/**
* Mad Mad Access Patterns Lab
* CS 241 - Fall 2018
*/

#include "tree.h"
#include "utils.h"
 #include <string.h>
 #include <stdlib.h>

// partner: xinyey2
/*
  Look up a few nodes in the tree and print the info they contain.
  This version uses fseek() and fread() to access the data.

  ./lookup1 <data_file> <word> [<word> ...]
*/

void recurse (FILE* file, char* word, uint32_t offset){
  BinaryTreeNode node;
  if (offset<1){
     printNotFound(word);
     return;
  }
  if (fseek(file,offset,SEEK_SET)==-1){
    exit(1);
  }

char nodeword[20];

fread(&node,sizeof(BinaryTreeNode),1,file);
fseek(file,sizeof(BinaryTreeNode)+offset,SEEK_SET);
fgets(nodeword,20,file);
  int res = strcmp(word,nodeword);
  if (res==0){
    printFound(word, node.count,node.price);
    return;
  }
  else if(res>0){
    recurse(file,word,node.right_child);
  }
  else{
    recurse(file,word,node.left_child);
  }

  return;
}


int main(int argc, char **argv) {
  if (argc<3){printArgumentUsage();
	exit(1);
}
  char* c = argv[1];
  FILE* file =fopen(c,"r");
 
	if (file == NULL){
	openFail(c);
	exit(1);}
  for (int i=2; i<argc; i++){
	char* word =argv[i];
 
    recurse(file,word,4);
  }
  fclose(file);
    return 0;
}

