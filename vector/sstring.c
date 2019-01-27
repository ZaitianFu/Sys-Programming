/**
* Vector Lab
* CS 241 - Fall 2018
*/

#include "sstring.h"
#include "vector.h"

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <assert.h>
#include <string.h>

struct sstring {
	char* str;
};

sstring *cstr_to_sstring(char *input) {
    // your code goes here
	sstring* ret=(sstring*)malloc(sizeof(sstring));
	ret->str=(char*)malloc(sizeof(char)*(strlen(input)+1));
	strcpy(ret->str, input);

    return ret;
}

char *sstring_to_cstr(sstring *input) {
    // your code goes here
	char* ret=(char*)malloc(sizeof(char)*(strlen(input->str)+1));
	strcpy(ret,input->str);
    return ret;
}

int sstring_append(sstring *this, sstring *addition) {
    // your code goes here
	
      strcat(this->str,addition->str);

    return strlen(this->str);
}

vector *sstring_split(sstring *this, char delimiter) {
    // your code goes here
	vector* ret=string_vector_create();
  char * pch;
int i =0;
  pch = strtok (this->str,&delimiter);
  while (pch != NULL)
  { vector_insert(ret,i, pch);
    pch = strtok (NULL, &delimiter);
   i++;
  }

    return ret;
}

int sstring_substitute(sstring *this, size_t offset, char *target,
                       char *substitution) {
    // your code goes here
	if (strstr(this->str, target)==NULL)
		return -1;
	else{
		int i;
		char* result = (char *)malloc(strlen(this->str) +(strlen(substitution) - strlen(target)) + 1);
	for (i=0; i<=(int)offset; i++){
		result[i++] = *(this->str++);
	}
	int count=0;
	 for (int j=1;*(this->str+(int)offset+j);j++)
    {
        if (strstr(this->str, target) == this->str  && count==0)
        {
            strcpy(&result[i], substitution);
            i += strlen(substitution);
            this->str += strlen(target);
		count++;
        }
        else
            result[i++] = *(this->str++);
    }
 
    result[i] = '\0';
	free(this->str); this->str=NULL;

	this->str=result;
	return 0;
	}
}


char *sstring_slice(sstring *this, int start, int end) {
    // your code goes here
	char* ret= (char*)malloc(end-start+1);
	for (int i=0; i<end-start; i++){
		*(ret+i)=this->str[start+i];
	}
	*(ret+end-start)='\0';
    return ret;
}

void sstring_destroy(sstring *this) {
    // your code goes here
	free(this->str);
	free(this);
	
}
