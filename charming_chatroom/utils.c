/**
* Chatroom Lab
* CS 241 - Fall 2018
*/
//xinyey2 bow2
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include <errno.h>
#include <unistd.h>
static const size_t MESSAGE_SIZE_DIGITS = 4;

char *create_message(char *name, char *message) {
    int name_len = strlen(name);
    int msg_len = strlen(message);
    char *msg = calloc(1, msg_len + name_len + 4);
    sprintf(msg, "%s: %s", name, message);

    return msg;
}

ssize_t get_message_size(int socket) {
    int32_t size;
    ssize_t read_bytes =
        read_all_from_socket(socket, (char *)&size, MESSAGE_SIZE_DIGITS);
    if (read_bytes == 0 || read_bytes == -1)
        return read_bytes;

    return (ssize_t)ntohl(size);
}

// You may assume size won't be larger than a 4 byte integer
ssize_t write_message_size(size_t size, int socket) {
    // Your code here
	 ssize_t write_size = htonl(size);
    ssize_t write_bytes =
        write_all_to_socket(socket, (char *)&write_size, MESSAGE_SIZE_DIGITS);
    if (write_bytes == 0 || write_bytes == -1)
        return write_bytes;
    return write_size;

}

ssize_t read_all_from_socket(int socket, char *buffer, size_t count) {
    // Your Code Here
    ssize_t len = 0;
    while(len < (ssize_t)count){
      ssize_t n = read(socket, buffer + len, count - len);
      if(n == 0){
       
        return 0;
      }
      if(n < 0){
        if(errno == EINTR) continue;
        else return -1;
      }
      len += n;
    }
    return len;

}

ssize_t write_all_to_socket(int socket, const char *buffer, size_t count) {
    // Your Code Here
     ssize_t len = 0;
    while(len < (ssize_t)count){
      ssize_t n = write(socket, buffer + len, count - len);
      if(n == 0)
        return 0;
      
      if(n < 0){
        if(errno == EINTR) continue;
        else return -1;
      }
      len += n;
    }
    return len;

}
