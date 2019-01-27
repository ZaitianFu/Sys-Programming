/**
* Text Editor Lab
* CS 241 - Fall 2018
*/

#include "document.h"
#include "editor.h"
#include "format.h"
#include "sstring.h"

#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *get_filename(int argc, char *argv[]) {
    // TODO implement get_filename
    // take a look at editor_main.c to see what this is used for
	if(argc>=2)
		return argv[argc-1];
	else
		return NULL;    
}

sstring *handle_create_string() {
    // TODO create empty string
	char* a="\0";
	return cstr_to_sstring(a);
}
document *handle_create_document(const char *path_to_file) {
    // TODO create the document
	return document_create_from_file(path_to_file);
    
}

void handle_cleanup(editor *editor) {
    // TODO destroy the document
	document_destroy(editor->document);
}

void handle_display_command(editor *editor, size_t start_line,
                            ssize_t max_lines, size_t start_col_index,
                            ssize_t max_cols) {
    // TODO implement handle_display_command
	ssize_t realmaxline=document_size(editor->document);
	if(!(editor->document) || (realmaxline ==0) ) {
	     print_document_empty_error();
	     return;
    }
	if (max_lines==(ssize_t)-1 || max_lines>realmaxline)
		max_lines=realmaxline;
	for (ssize_t r=start_line; r<=max_lines && max_lines>0; r++){
		 print_line(editor->document, r, start_col_index,max_cols);
		max_lines--;
	}
		
}

void handle_insert_command(editor *editor, location loc, const char *line) {
    // TODO implement handle_insert_command
/*	assert(line);
	document_insert_line(editor->document, loc.line_no, line);
    if (loc.line_no> document_size(editor->document)){
      loc.idx = 0;
      document_insert_line(editor->document,loc.line_no,line);
    }
   
    char* getline;
	getline=(char*)document_get_line(editor->document,loc.line_no);
    for (size_t i=0; i<loc.idx; i++){
      if (*(getline+i)=='\n')
      return;
    }

    char* half = malloc(strlen(getline)+strlen(line)+1);
    strncpy(half,getline, loc.idx);
    strcat(half,line);
    strcat(half, getline+loc.idx);

    document_set_line(editor->document,loc.line_no,half);
    free(half);
*/
      assert(loc.line_no >0);
      assert(loc.idx >= 0);

      size_t lnum = loc.line_no;
      size_t index = loc.idx;
     if (document_size(editor->document)>=lnum){
         
       const char*ori = document_get_line(editor->document,lnum);
       char* result = malloc(sizeof(char)*(strlen(ori)+strlen(line))+1);
       strncpy (result,ori,index);
       result[(int)index]=0;

       strcat (result,line);
       strcat (result,ori+index);

       document_set_line(editor->document,lnum,result);
       free(result);
       return;
     }
     document_insert_line(editor->document,lnum,line);
}

void handle_append_command(editor *editor, size_t line_no, const char *line) {
    // TODO implement handle_append_command
	 if (line_no > document_size(editor->document)) {
        document_insert_line(editor->document, line_no, line);
    } else {
        const char *docline = document_get_line(editor->document, line_no);
        /*
        if (docline) {
            char *newdocline = malloc((strlen(docline) + strlen(line) + 1) * sizeof(char));
            strcpy(newdocline, docline);
            strcat(newdocline, line);
            document_set_line(editor->document, line_no, newdocline);
            free(newdocline);
        } else {
            document_insert_line(editor->document, line_no, line);
        }
}*/

    char * result = malloc (sizeof(char)*(strlen(docline)+strlen(line)+1));
    
    result = strcpy(result,docline);
    char*  appends = malloc (sizeof(char)*strlen(line)+1);
    appends = strcpy(appends,line);
    char* temp = appends;
    int flag = 1;
    while(1){
      if(flag){ 
	flag--;
	--temp;
      }
      char * b = strchr (temp+1,'\\');
      if(!b){
        if (appends!=temp) appends = strcat(appends,temp);
        result = strcat(result,appends);
        document_set_line(editor->document,line_no,result);    break;
      }
      else if (*(b+1)== 0){
        *b = 0;
        if (temp !=appends) appends = strcat(appends,temp);
        result = strcat(result,appends);
        document_set_line(editor->document,line_no,result); break;
      }
      else if (*(b+1)=='n'){
        *b = 0;
        if (temp !=appends) appends = strcat(appends,temp);
        result = strcat(result,appends);
        document_set_line(editor->document,line_no,result);
        document_insert_line(editor->document,line_no+1,"");
        handle_write_command(editor,line_no+1,b+2); break;
      }
      else {
        *b = 0;
        if (temp!=appends) 
		appends = strcat(appends,temp);
        b++;
        temp = b;
      }
    }
    free(appends);
    free(result);
   }
}

void handle_write_command(editor *editor, size_t line_no, const char *line) {
    // TODO implement handle_write_command 
  
    if (line_no > document_size(editor->document)){
      document_insert_line (editor->document,line_no,line); 
	return ;
    }
    char *start = strdup(line);
    char * it = start;
    int flag = 1;
    while(1){
      if(flag){
        --it; 
	flag--;
      }
      char * sub = strchr (it+1,'\\');
      if(!sub){
        if (start!=it) start = strcat(start,it);
        document_set_line(editor->document,line_no,start);   
	 break;
      }
      else if (*(sub+1)=='n'){
        *sub = 0;
        if (start!=it) start = strcat(start,it);
        document_set_line(editor->document,line_no,start);
        document_insert_line(editor->document,line_no+1,"");
        handle_write_command(editor,line_no+1 ,sub+2); 
	break;
      }
      else if (*(sub+1)=='\0'){
        *sub = 0;
        if (start!=it) start = strcat(start,it);
          document_set_line(editor->document,line_no,start);    
		break;
      }
      else {
        *sub = 0;
        if (it !=start) 
		start = strcat(start,it);
        it = sub+1;
      }
    }
    free(start);
}

void handle_delete_command(editor *editor, location loc, size_t num_chars) {
    // TODO implement handle_delete_command
	 size_t size = document_size(editor->document);
        if (loc.line_no > size) {
		for(size_t i=1; i<size+1;i++)
          		 document_delete_line(editor->document, i);
    	
		
	} 
	else{
	char* ret=strdup(document_get_line(editor->document,loc.line_no));
	*(ret+loc.idx)=0;
	if(strlen(document_get_line(editor->document,loc.line_no))-loc.idx>num_chars){
		char* t=ret+loc.idx+num_chars;
		strcat(ret,t);
 	}

	document_set_line(editor->document,loc.line_no, ret);
	free(ret);
	}
}

void handle_delete_line(editor *editor, size_t line_no) {
    // TODO implement handle_delete_line
	document_delete_line(editor->document, line_no);
}

location handle_search_command(editor *editor, location loc,
                               const char *search_str) {
    // TODO implement handle_search_command
	assert(search_str);
    if (!strcmp(search_str,"")){
      return (location){0,0};
    }
    size_t sizel = document_size(editor->document);
    
    for (size_t i = 0 ;i < sizel+1 ; i++){
      size_t curr;
      if (sizel != loc.line_no + i){
        
	curr = (loc.line_no + i)%sizel;
      }
      else {
        curr=sizel;
      }
      if (!curr){
        curr = sizel;
      }

      const char* line = document_get_line(editor->document,curr);
      char* result = NULL;
      if (curr!=loc.line_no){
        result = strstr(line,search_str);
      }
      else {
        result = strstr(line+loc.idx-1,search_str);
        if (line +loc.idx -1== result){
          result = strstr(line+loc.idx,search_str);
        }
      }
      if (result){
        return (location){(int)curr,(int)(result-line)};
      }

	}
return (location){0,0};
}
void handle_merge_line(editor *editor, size_t line_no) {
    // TODO implement handle_merge_line
	//size_t size = document_size(editor->document);
	//assert(size>line_no);

	handle_append_command(editor, line_no, document_get_line(editor->document, line_no+1));
	document_delete_line(editor->document, line_no+1); 
	
}

void handle_split_line(editor *editor, location loc) {
    // TODO implement handle_split_line
	const char * line = document_get_line(editor->document,loc.line_no);
    
    char * splited = calloc (1,strlen(line)+1);
    splited = strcpy(splited,line + loc.idx);
	
	document_insert_line(editor->document,loc.line_no+1,splited);
    handle_delete_command(editor,loc,strlen(line));
/*	
	const char* line=document_get_line(editor->document, loc.line_no)+loc.idx;
	document_insert_line(editor->document, loc.line_no+1, line);
	handle_delete_command(editor, loc, strlen(document_get_line(editor->document, loc.line_no)));
*/
}

void handle_save_command(editor *editor) {
    // TODO implement handle_save_command
	document_write_to_file(editor->document,editor-> filename);
}
