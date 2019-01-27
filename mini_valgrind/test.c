/**
* Mini Valgrind Lab
* CS 241 - Fall 2018
*/

#include <stdio.h>
#include <stdlib.h>

int main() {
    // Your tests here using malloc and free
char* yxynb = calloc(1,1000);
free(yxynb);
char* yxyznb=NULL;
yxyznb=realloc(yxyznb,100);
char* yxytnb=malloc(10);
free(yxytnb);
free(yxynb);
  return 0;
}
