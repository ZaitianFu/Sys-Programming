/**
* Networking Lab
* CS 241 - Fall 2018
*/

#pragma once
#include <stddef.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dictionary.h"
#include <unistd.h>
#include <errno.h>


typedef enum { GET, PUT, DELETE, LIST, V_UNKNOWN } verb;

typedef struct metadata {
	verb command;
	char* buffer;
	char* hc;
	char* file_name;
	size_t offfsets;
	size_t file_size;
	size_t size_offfsets;
	int argcnum;
	int header;
	int client_fd;
	int local_fd;
	int cfdcpy;

}metadata;

ssize_t read_all_from_socket(int socket, char *buffer, size_t expect);
ssize_t write_all_to_socket(int socket, const char *buffer, size_t expect);
