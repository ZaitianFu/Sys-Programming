/**
 * Networking Lab
 * CS 241 - Fall 2018
 */
#include "common.h"
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "format.h"
#include "dictionary.h"



ssize_t read_all_from_socket(int socket, char *buffer, size_t expect) {
    size_t count = 0 ;
    while (count < expect){
      ssize_t read_byte = read(socket,buffer+count,expect-count);
      if (!read_byte){
        return count;
      }
      if (errno == EINTR &&read_byte == -1 ){

        continue;
      }
      if ( errno != EINTR && read_byte == -1 ){

        return -1;
      }
      if (read_byte == -1 && errno == EAGAIN){

        return count;
      }
      count +=read_byte;
    }

    return count;
}

ssize_t write_all_to_socket(int socket, const char *buffer, size_t expect) {
    ssize_t length = 0;
    while((ssize_t)expect>length){
      size_t write_len = expect-length;
      ssize_t n = write(socket, buffer + length,write_len);
      if(n == 0){
        return 0;
      }else if(n < 0){
        if(errno == EINTR){

          continue;
        }else{
          return -1;
        }
      }

      length += n;
    }
    return length;
}
