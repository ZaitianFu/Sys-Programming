/**
* Extreme Edge Cases Lab
* CS 241 - Fall 2018
*/

#include "camelCaser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
//void delete(char* input,int index);

char **camel_caser(const char *input_str) {

if (input_str==NULL)
	return NULL;

    char c;
    int count= 0;
	int it=0;
	c=input_str[it];
    while ((c)) {
        if (ispunct(c)) {
            count++;
        }    
	it++;
	c=input_str[it];
    }

    char **output = malloc(sizeof(char*)*(count+1));
    int countchar = 0;
    it  = 0;
    c=input_str[it];
int its=0;
    while (c) {
        if (ispunct(c)) {
            output[its] = malloc((countchar + 1) * sizeof(char));
            output[its][countchar] = '\0';
            countchar = 0;
		its++;}
        else if(isspace(c)){
            countchar++;
        }
	it++;
	c=input_str[it];
    }
    it   = 0;
    char a;
countchar=0;
    int cap  = 0;
    int fc  = 1;
    its    = 0;
c=input_str[it];
int spacechar=0;
    while (its<count) {
        if (ispunct(c)) {
            cap = 0;
		countchar=0;
            fc = 1;
            its++;
        } else if (isspace(c)) {
            cap = 1;
        } else {
                if(isalpha(c)&&cap==1 &&fc==0){
			a=toupper(c);
			spacechar=0;
			cap=0;}
		else if (isalpha(c)&&(cap==0|| fc==1)){
			a=tolower(c);
				cap=0;}
		else
			a=c;
           
            fc = 0;
            output[its][countchar] = a;
            countchar++;
        }
	it++;
	c=input_str[it];
    }

    return output;
}

	
					

void destroy(char **result) {
if (result==NULL){
    return;
}
char** temp = result;
while(*result!=NULL){
  free(*result);
  result++;
}

free(temp);
}


//void delete(char* input, int index){

  //  int length = strlen(input);

   // char *word = malloc(strlen(input + 1));
   // strcpy(word, input);
//memmove(input+index, input+index+1, (strlen(input) - index));

 //   for (; index < length-1; index++) {
   //     char *letter = input + index;
     //   char temp = *(letter + 1);
       // *letter = temp;
//	input[index]=input[index+1];
//    }
//	input=realloc(input,sizeof(char)*strlen(word)+1);
//	strcpy(input, word);
//	free(word);
//	input[index]='\0';
  //  return;
//}

