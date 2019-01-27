/**
* Text Editor Lab
* CS 241 - Fall 2018
*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "document.h"
#include "vector.h"
#include <fcntl.h>
struct document {
    vector *vector;
};

document *document_create() {
    document *this = (document *)malloc(sizeof(document));
    assert(this);
    this->vector = vector_create(string_copy_constructor, string_destructor,
                                 string_default_constructor);
    return this;
}

void document_write_to_file(document *this, const char *path_to_file) {
    assert(this);
    assert(path_to_file);
    // see the comment in the header file for a description of how to do this!
    // TODO: your code here!
/*	FILE* newf=fopen(path_to_file, "w");
	if( access( path_to_file, F_OK ) != -1 ) {
    	// file exists
	} 
	else {
    	// file doesn't exist
		}
	char* a;
	 size_t size=vector_size(this->vector);
         for(size_t i=0;i<size;i++){
		a=vector_get(this->vector,i);
		if(a) 
		 	fprintf(newf, "%s\n",a);
		else
			fprintf(newf, "\n");
	}
	 fclose(newf);

*/


    mode_t m = S_IROTH | S_IWOTH| S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;
    int fd = open(path_to_file, O_CREAT | O_TRUNC | O_RDWR, m);
    
    for(int i = 1; i <= (int)document_size(this); i++) {
      dprintf(fd, "%s\n", document_get_line(this, i));
    }
    close(fd);


}

document *document_create_from_file(const char *path_to_file) {
	assert(path_to_file);
	// this function will read a file which is created by document_write_to_file
	// TODO: your code here!
	char* c=NULL;
	size_t s=0;
	size_t size;
	document* ret=document_create();
	FILE *file;
	//size_t line=1;
	file = fopen(path_to_file, "r");
	if (!file) 
		return ret;
	else{
    		while ((size=(getline(&c,&s,file))) != (size_t)-1){
			 if ( c[size-1] == '\n') {
            			c[size-1] = '\0';
            			vector_push_back(ret->vector, c);
        		 }/*
			if (size>(size_t)-1)
            			document_insert_line(ret, line, c);
        			line++;*/
		}	
    		fclose(file);
	}
	free(c);
   	 return ret;
}
void document_destroy(document *this) {
    assert(this);
    vector_destroy(this->vector);
    free(this);
}

size_t document_size(document *this) {
    assert(this);
    return vector_size(this->vector);
}

void document_set_line(document *this, size_t line_number, const char *str) {
    assert(this);
    assert(str);
    size_t index = line_number - 1;
    vector_set(this->vector, index, (void *)str);
}

const char *document_get_line(document *this, size_t line_number) {
    assert(this);
    assert(line_number > 0);
    size_t index = line_number - 1;
    return (const char *)vector_get(this->vector, index);
}

void document_insert_line(document *this, size_t line_number, const char *str) {
    assert(this);
    assert(str);
    // TODO: your code here!
    // How are you going to handle the case when the user wants to
    // insert a line past the end of the document?
   
// assert(line_number > 0);
	
	size_t i=line_number-1;
    if (! str) return;
    else if (vector_size(this->vector)>i){
    	vector_insert(this->vector, i, (void*) str);
//	free(temp);	
	}
    else{
	//size_t size = vector_size(this->vector);
      vector_resize(this->vector,line_number);
      vector_set(this->vector,i,(void*)str);
       return;
	}
}

void document_delete_line(document *this, size_t line_number) {
    assert(this);
    assert(line_number > 0);
    size_t index = line_number - 1;
    vector_erase(this->vector, index);
}
