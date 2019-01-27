/**
* Networking Lab
* CS 241 - Fall 2018
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include "common.h"
#include "format.h"
#include "vector.h"
#include "dictionary.h"
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <ctype.h>
#include <signal.h>


static char tmpbuff[1024];
static char morebuffer[4096];
static int close_status = 0;
static vector* file_vector;
static dictionary* status_dic;
static size_t file_len = -1;
static char * directory;
static char* op;


int determinecommand(char* buffer,char* command){

  size_t size = strlen(command);
  for(size_t i= 0;i< size;i++){
    if(buffer[i]!= *(command+i)){
      return 0;
    }
  }
  return 1;
}

void string_concat(char* buffer,char* first, char* second, char* third){
  memset(buffer,0,1024);
  buffer = strcpy(buffer,first);
  strcat(buffer,second);
  strcat(buffer,third);
}


verb get_command_option(char *buffer){

  if(determinecommand(buffer, "PUT")){
    return PUT;
  }else if(determinecommand(buffer, "LIST")){
    return LIST;
  }else if(determinecommand(buffer,"DELETE")){
    return DELETE;
	}else if(determinecommand(buffer, "GET")){
    return GET;
  }
  else return V_UNKNOWN;
}

void initializer(struct epoll_event *event){
  size_t status_size = sizeof(metadata);
  metadata* thismeta = malloc (status_size);
  memset(thismeta, 0, status_size);

  thismeta->hc = calloc (1,512);
  thismeta->file_name = calloc (1,512);
	thismeta->size_offfsets = 0;
  thismeta->buffer = calloc (1,4096);
  thismeta->command = V_UNKNOWN;
  thismeta->cfdcpy=event->data.fd;
  thismeta->header = 0;
	thismeta->file_size = 0;
	thismeta->offfsets = 0;
  thismeta->client_fd = event->data.fd;
  thismeta->argcnum = 2;

  dictionary_set(status_dic,&event->data.fd,thismeta);
}


void cleanupvecc(){
  size_t i = 0;
  while ( i < vector_size(file_vector) ){

  memset (morebuffer,0,4096);
  char* temp = vector_get(file_vector,i);
  string_concat(morebuffer,directory,"/",temp);

  if (!unlink(morebuffer)){
    fprintf(stderr, "%s removed\n", morebuffer);
  }
  i++;
}
}

int judge_command(char* buffer,char* command ){

  size_t size = strlen(command);
  for(size_t i= 0;i< size;i++){
    if(*(command+i)!=buffer[i]){
      return 0;
    }
  }
  return 1;
}

void cleandirectt(){
  vector * values = dictionary_values(status_dic);

  VECTOR_FOR_EACH(values,temp,{
    if (((metadata*)temp)->buffer) free(((metadata*)temp)->buffer);
    if (((metadata*)temp)->hc) free(((metadata*)temp)->hc);
    if (((metadata*)temp)->file_name) free(((metadata*)temp)->file_name);
    free (temp);
  });

    vector_destroy(values);
}
void final_cleanup(){

    cleandirectt();
    cleanupvecc();

    if (!rmdir(directory))
      fprintf(stderr, "removed directory\n");

    close_status = 1;
		exit(1);
}


int ppcommand(	char* hc, metadata* connection){

  char* name_start = strchr(hc, ' ');
  name_start ++;
size_t temp_size = strchr(hc,'\n')-name_start;
strncat(connection->file_name, name_start, temp_size);
  return 0;
}

int get_command(metadata* connection){
	connection->header = 1;
	char* hc = connection->hc;
  verb option = get_command_option(hc);
  if (option==0) op="GET";
  if (option==1) op="PUT";
  if (option==2) op="DELETE";
  if (option==3) op="LIST";
  if (option==4) op="V_UNKNOWN";
  fprintf(stderr, "Option is %s\n",op);

  switch (option) {
		case PUT:{
  		connection->command = PUT;

      ppcommand(hc,connection);
  		return 0;
  	}break;
    case DELETE:{

  		connection->command = DELETE;

      ppcommand(hc,connection);

  		return 0;
  	}break;

    case GET:{
  		connection->command = GET;
      ppcommand(hc,connection);
  		return 0;
  	}break;
    case LIST:{
  		connection->command = LIST;
  		return 0;
  	}break;
    case V_UNKNOWN:{

	connection->command=V_UNKNOWN;
      return -1;}break;
    default:{
	connection->command=V_UNKNOWN;
      return -1;}

  }

}
int fileexitornot(char* file_name){
  VECTOR_FOR_EACH(file_vector,temp,{
    if(!strcmp(temp,file_name)){
      return 1;
    }
  });
  return 0;
}

void getinserver(metadata* connection){

	int has_file = fileexitornot(connection->file_name);
  fprintf(stderr, "%d\n",has_file);

		if (has_file){
			memset(tmpbuff,0,1024);
      string_concat(tmpbuff,directory,"/",connection->file_name);

      int fd = open(tmpbuff, O_RDONLY);
      dprintf(connection->client_fd,"OK\n");

			size_t size = lseek(fd,0,SEEK_END);
			write_all_to_socket (connection->client_fd, (char*)&size, sizeof(size_t));

      lseek(fd, 0,SEEK_SET);
      memset(morebuffer,0,4096);
			int read_byte = -1;
			while(0<(read_byte= read_all_from_socket(fd,morebuffer,4096))){

				write_all_to_socket(connection->client_fd, morebuffer , read_byte);
        fprintf(stderr, "%d bytes sent\n", read_byte );
			}
		}
		else {

      dprintf(connection->client_fd,"%s/n%s","ERROR",err_no_such_file);

		}
		return;

	/*
		{

			fseek(newfile, 0, SEEK_END);
			size_t file_size = ftell(newfile);
			fseek(newfile, 0, SEEK_SET);

			//connection->newfile = newfile;
			dprintf(connection->client_fd,"OK\n");
			connection->file_size= file_size;
			write_all_to_socket(connection->client_fd, (char*)&file_size, sizeof(size_t));
	//	}
		ssize_t s =  write_message(socket_info->newfile, client_socket, 4096,socket_info);
		socket_info->size_left -= s;
		if (socket_info->size_left <= 0 || s == -1)
		{
			fclose(socket_info->newfile);
			socket_info->size_left = -1;
		}*/
}

int judge_data_size(metadata* connection){
  int size_offfsets = connection->size_offfsets;
  int file_size = connection->file_size;
  int retval = (size_offfsets==file_size)?0:((size_offfsets<file_size)?-1:1);
  fprintf(stderr, "retval = %d\n",retval );

	if(retval<0){
    print_too_little_data();
  }
	else if(retval>0){
    print_received_too_much_data();
  }
  if(retval){
    dprintf(connection->client_fd,"%s/n%s","ERROR",err_no_such_file);

  }
  return retval;
}

void putinserver(metadata* connection, char*data){
  fprintf(stderr, "%s\n","putting " );
	memset(tmpbuff,0,1024);
  string_concat(tmpbuff,directory,"/",connection->file_name);

  int fd = open(tmpbuff, O_CREAT|O_RDWR | O_TRUNC,0777);
  connection->local_fd = fd;
  connection->cfdcpy=fd;

  if (connection->cfdcpy <0 ){
    print_error_message("open file error");
    return;
  }else{
    fprintf(stderr, "%s\n","input" );
//
	judge_data_size(connection);
//
    if ((data + 8)-(connection->buffer+connection->offfsets) <0){
        connection->file_size = *(size_t*)(data+1);

        size_t have_readed = data + 4+ 8 -connection->buffer+1-4;
        size_t remaining = connection->offfsets - have_readed;


        if (0<remaining ){

          size_t count = write_all_to_socket(fd, data+sizeof(size_t)+1,remaining);

          connection->size_offfsets += count;
        }
        connection->offfsets = 0;
    }
  }

}

void listinserver(metadata * connection){

  dprintf(connection->client_fd,"OK\n");
  connection->cfdcpy=0;

  if ( (size_t)(-1)==file_len ){
    size_t zero = 0 ;
    write(connection->client_fd, &zero, sizeof(size_t));
  }else {
    write(connection->client_fd, &file_len,sizeof(size_t));
  }
	for (size_t i = 0 ; vector_size(file_vector) >i; i++){
		char* temp = vector_get(file_vector,i);
		write_all_to_socket (connection->client_fd, temp, strlen(temp));
		if (vector_size(file_vector)-1 ==i){
			break;
		}
		write_all_to_socket(connection->client_fd,"\n",1);
	}
}

void deleteinserver(metadata* connection){
fprintf(stderr, "%s\n", "DELETE");
int file_exist = 0;
size_t i = 0 ;
	while ( i < (size_t)vector_size(file_vector)){
		char * single_file = vector_get(file_vector,i);
			if(!strcmp(single_file,connection->file_name)){
				file_exist = 1;
        vector_erase(file_vector,(int)i);
				break;
			}
      i++;
	}
		if (file_exist){
			file_len = file_len -  strlen(connection->file_name)-1;
			memset(tmpbuff,0,1024);
      string_concat(tmpbuff,directory,"/",connection->file_name);
      int exist = (remove(tmpbuff));
      if (!exist){
        dprintf(connection->client_fd,"OK\n");

			}
		}else {
      dprintf(connection->client_fd,"%s/n%s","ERROR",err_no_such_file);

		}
		return;
}

void accept_connections(struct epoll_event *e,int epoll_fd){

		struct sockaddr_in new_addr;
socklen_t new_len = sizeof(new_addr);
		int new_fd = accept(e->data.fd, (struct sockaddr*) &new_addr, &new_len);

		if(-1==new_fd ){
			if( errno == EWOULDBLOCK || errno == EAGAIN ){
        fprintf(stderr, "%s\n","EAGAIN or EWOULDBLOCK" );
        return;
      }else{
				perror("accept_connections");
				exit(1);
			}
		}

		char *IPad= inet_ntoa(new_addr.sin_addr);
		int port = ntohs(new_addr.sin_port);
        printf("accepted %s on port %d\n", IPad, port);

        int fcntl_result = fcntl(new_fd, F_GETFL, 0);
        fcntl(new_fd, F_SETFL, fcntl_result | O_NONBLOCK);
        struct epoll_event event;
        event.data.fd = new_fd;
        event.events = EPOLLIN;
        if(epoll_ctl (epoll_fd, EPOLL_CTL_ADD, new_fd, &event) == -1){
        	perror("accept epoll_ctl");
        	exit(1);
        }

}

void hd_parse_error(struct epoll_event *evt){
  print_error_message((char*)err_bad_request);
  dprintf(evt->data.fd,"%s/n%s","ERROR",err_no_such_file);
  close (evt->data.fd);
  dictionary_remove(status_dic,&evt->data.fd);
}
/*
int judge_data_size(metadata* connection){
  int size_offfsets = connection->size_offfsets;
  int file_size = connection->file_size;
  int retval = (size_offfsets==file_size)?0:((size_offfsets<file_size)?-1:1);
  fprintf(stderr, "retval = %d\n",retval );

	if(retval<0){
    print_too_little_data();
  }
	else if(retval>0){
    print_received_too_much_data();
  }
  if(retval){
    dprintf(connection->client_fd,"%s/n%s","ERROR",err_no_such_file);

  }
  return retval;
}
*/

void hd_command(struct epoll_event *e,metadata* connection,char* data){
  if (GET==connection->command ){
    getinserver(connection);
    close(e->data.fd);
    dictionary_remove(status_dic,&e->data.fd);
    return;
  }
  if (connection->command == PUT){
    putinserver(connection,data);

    return;

  }
  if (LIST==connection->command ){
    listinserver(connection);
    close(e->data.fd);
    dictionary_remove(status_dic, &e->data.fd);
    return;
  }
  if (DELETE== connection->command ){
    deleteinserver(connection);
    close(e->data.fd);
    dictionary_remove(status_dic, &e->data.fd);
    return;
  }
else{
  print_invalid_response();
		dprintf(e->data.fd, "ERROR\n%s\n", err_bad_request);
}
}

void hd_data(struct epoll_event *e){
  int diction_contain = dictionary_contains(status_dic,(void*)&e->data.fd);
  if (!diction_contain){
    initializer((struct epoll_event *)e);
  }
  metadata* connection = dictionary_get(status_dic,(void*)&e->data.fd);
	ssize_t count = read(e->data.fd,connection->buffer+connection->offfsets,4096-connection->offfsets);
	connection->offfsets+=count;

		if ((!connection->header) && count){
				char* data = strchr(connection->buffer, '\n');
				if (data){
					strncat(connection->hc, connection->buffer, data - connection->buffer+1);
					int parse = get_command(connection);
					if (-1==parse ){
            hd_parse_error(e);
						return;
					}else{
            hd_command(e,connection,data);
          }

				}

		}else if ((connection->header) && count){
			char* data = strchr(connection->buffer, '\n');

      string_concat(tmpbuff,directory,"/",connection->file_name);

			if (connection->file_size!=0){
				int temp_count = write_all_to_socket(connection->local_fd,connection->buffer,count);
				connection->offfsets = 0;
				connection->size_offfsets+=temp_count;

			}else {
        char* have_readed = (connection->buffer+connection->offfsets);
				if ((data+8)-have_readed <0){
						connection->file_size = *((size_t*)(data+1));
						size_t remaining = connection->offfsets - (data+8-connection->buffer+1);
						if (remaining>0){
							size_t temp_count = write_all_to_socket(connection->local_fd, data+9,remaining);
							connection->size_offfsets += temp_count;
						}
						connection->offfsets = 0;
				}
			}
		}

	if(!count) {
		if (connection->command == PUT){
      int too_big_small = judge_data_size(connection);

			if(!too_big_small){
				int file_exists = 0;
        VECTOR_FOR_EACH(file_vector,temp,{
          if(!strcmp(temp,connection->file_name)){
            file_exists = 1;
          }
        });
					file_len += strlen(connection->file_name)+1;

					if (!file_exists){
            vector_push_back(file_vector,connection->file_name);
          }
          dprintf(connection->client_fd,"OK\n");


			}
			close(connection->local_fd);
			close(e->data.fd);
			dictionary_remove(status_dic,&e->data.fd);
		}
	}
}

int main(int argc, char** argv){

    signal(SIGINT,final_cleanup);
    signal(SIGPIPE, SIG_IGN);

    file_vector = vector_create(string_copy_constructor,string_destructor,string_default_constructor);
    status_dic = int_to_shallow_dictionary_create();

		char dnameeeee [] = "XXXXXX";
		directory = mkdtemp (dnameeeee);
		print_temp_directory(directory);

	  struct epoll_event *events;
    int s=argc;

    events = calloc(sizeof(struct epoll_event),32);
    int sock_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    int flags = fcntl(sock_fd, F_GETFL, 0);
    fcntl(sock_fd, F_SETFL, flags | O_NONBLOCK);
    struct addrinfo hints, *result = NULL;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    s = getaddrinfo(NULL, argv[1], &hints, &result);
    if (s != 0) {
        print_error_message("error in getaddrinfo");
        exit(1);
    }

    int optval = 1;
    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    if ( bind(sock_fd, result->ai_addr, result->ai_addrlen) != 0 ){
        perror("error in bind");
        exit(1);
    }
    if ( listen(sock_fd, 128) != 0 ){
        perror("error in listen");
        exit(1);
    }


		freeaddrinfo(result);
    struct sockaddr_in sin;
    socklen_t socklen = sizeof(sin);
    if (getsockname(sock_fd, (struct sockaddr *)&sin, &socklen) != -1)
      printf("Listening port  %d\n", ntohs(sin.sin_port));
    else{
      perror("getsockname");
    }



	int epoll_fd = epoll_create(1);
	if(epoll_fd == -1){
        perror("epoll_create()");
        exit(1);
  }

	struct epoll_event event;
	event.data.fd = sock_fd;
	event.events = EPOLLIN;

	if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock_fd, &event)){
        perror("epoll_ctl()");
        exit(1);
	}

	while(!close_status) {

    size_t n = epoll_wait (epoll_fd,events,32,-1);

  for (size_t i = 0 ; i < n ;i ++){
    if(sock_fd == events[i].data.fd){
          printf("connecting\n");
      		accept_connections( &events[i], epoll_fd);
        }
      	else
      		hd_data(&events[i]);
    }
	}

  free(events);
	dictionary_destroy(status_dic);
  fprintf(stderr, "sever shutdown\n");
	vector_destroy(file_vector);
  return 0;
}
