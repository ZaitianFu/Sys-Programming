/**
* Networking Lab
* CS 241 - Fall 2018
*/

#include "common.h"
#include "format.h"
#include "vector.h"
#include "format.h"
#include <ctype.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/epoll.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char **parse_args(int argc, char **argv);
verb check_args(char **args);
//chatroom
ssize_t readfd(int socket, char *buffer, size_t num)
{

    size_t b = 0;
    ssize_t result=0;
    while(num>b)
    {
        result = read(socket,buffer+b,num-b);
        if(-1==result)
        {
            if((errno==EINTR || errno == EAGAIN || errno == EWOULDBLOCK) && result==-1)
                continue;

            return result;
        }
        if(result==0)
            return b;

        b+=result;
    }
    return b;
}

ssize_t writefd(int socket, char *buffer, size_t num) {

    size_t b = 0;
    ssize_t result;
    while(b < num)
    {
        result = write(socket,buffer+b,num-b);
        if(-1==result )
        {
            if(result ==-1 && (errno==EINTR || errno == EWOULDBLOCK || errno == EAGAIN))
                continue;

            return result;
        }
        if(0==result)
            return b;


        b+=result;
    }
    return b;
}


int main(int argc, char **argv) {
    char response[1000];

    if(2>=argc )
      return 0;

    char** arguments = parse_args(argc, argv);
    verb option = check_args(arguments);

    if (! arguments) return 0;

    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct addrinfo hints, *result;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int s = getaddrinfo(arguments[0], arguments[1], &hints, &result);
    if (connect(sock_fd, result->ai_addr, result->ai_addrlen) == -1)
    {
        perror("connect fail");
          freeaddrinfo(result);
       return 0;
    }
    if (s != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        freeaddrinfo(result);
        return 0;
    }


        if(option == PUT)
        {
          int local_file = open(arguments[4], O_RDONLY);

          if(local_file==-1)
            return 1;

          struct stat st;
          stat(arguments[4], &st);
          size_t size = st.st_size;

          dprintf(sock_fd, "PUT %s\n", arguments[3]);
          writefd(sock_fd, (char*)&size, 8);

          char buffer[1024];
          int total_read = 0;
          while((int)size > total_read  )
          {
            memset(buffer, 0, 1024);
            int bytes_read = read(local_file, buffer, 1024);
            if(0==bytes_read)
            {
              total_read+=bytes_read;
              write(sock_fd, buffer, bytes_read);
              print_connection_closed();
              return -1;
            }
            else if(-1==bytes_read )
              return -1;
            else
            {
              total_read+=bytes_read;
              write(sock_fd, buffer, bytes_read);
            }
          }
          shutdown(sock_fd, SHUT_WR);
          memset(response, 0, 1000);
          readfd(sock_fd, response, 999);

          if(response==strstr(response, "OK") )
              print_success();

          else if(strstr(response, "ERROR") == response)
          {
            char* emmmmmmmm = strtok(response, "\n");
            emmmmmmmm = strtok(NULL, "\n");
            print_error_message(emmmmmmmm);
          }
          else
            print_invalid_response();

        }

        if(option == GET)
        {
          int local_file = open(arguments[4], O_TRUNC|O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR);
          dprintf(sock_fd, "GET %s\n", arguments[3]);
          shutdown(sock_fd, SHUT_WR);

          memset(response, 0, 999);
          readfd(sock_fd, response, 3);
          printf("%s",response);

          if(strstr(response, "OK") == response)
          {
            size_t size = 0;
            readfd(sock_fd, (char*)&size, 8);

            char buffer[1024];
            size_t total_read = 0;
            while(total_read < size)
            {
              memset(buffer, 0, 1024);
              int bytes_read = read(sock_fd, buffer, 1024);
              if(bytes_read == 0)
              {
                total_read+=bytes_read;
                write(local_file, buffer, bytes_read);
                print_connection_closed();
                return -1;
              }
              else if(-1==bytes_read )
                return -1;
              else
              {
                total_read+=bytes_read;
                write(local_file, buffer, bytes_read);
              }
            }
            total_read += readfd(sock_fd, buffer, 1);
            if(total_read<size)
            {
              print_too_little_data();
              close(local_file);
            }
            else if(total_read>size)
            {
              print_received_too_much_data();
              close(local_file);
            }
            else if (total_read == size)
            {
              close(local_file);
            }
          }
          else if(response==strstr(response, "ERROR") )
          {
            memset(response, 0, 999);
            readfd(sock_fd, response, 999);
            char* emmmmmmmm = strtok(response, "\n");
            emmmmmmmm = strtok(NULL, "\n");
            print_error_message(emmmmmmmm);
		print_invalid_response();
          }
          else
            print_invalid_response();

        }

        if(option == LIST)
        {
          dprintf(sock_fd, "LIST\n");
          shutdown(sock_fd, SHUT_WR);

          char larger_response[100000];
          memset(larger_response, 0, 100000);

          readfd(sock_fd, larger_response, 3);

          if(larger_response==strstr(larger_response, "OK") )
          {
            size_t size = 0;

            readfd(sock_fd, (char*)&size, 8);

            char buffer_data[size+2];
            memset(buffer_data, 0, size+1);
            size_t total_read = readfd(sock_fd, buffer_data, size+1);

            if(total_read<size)
            {
              print_too_little_data();
            }
            else if(total_read>size)
            {
              print_received_too_much_data();
            }
            else if (total_read==size)
            {
              fprintf(stdout, "%s", buffer_data);
            }
          }
          else if(strstr(larger_response, "ERROR") == larger_response)
          {
            memset(response, 0, 999);
            readfd(sock_fd, response, 999);
            char* emmmmmmmm = strtok(larger_response, "\n");
            emmmmmmmm = strtok(NULL, "\n");
            print_error_message(emmmmmmmm);
          }
          else
            print_invalid_response();

        }


    if(option == DELETE)
    {
        dprintf(sock_fd, "DELETE %s\n", argv[3]);
        shutdown(sock_fd, SHUT_WR);
        memset(response,0,1000);
        readfd(sock_fd, response, 999);

        if(response==strstr(response, "ERROR") )
        {
          char* emmmmmmmm = strtok(response, "\n");
          emmmmmmmm = strtok(NULL, "\n");
          print_error_message(emmmmmmmm);
        }
        else if(response==strstr(response, "OK") )
            print_success();
        else
          print_invalid_response();

    }

    free(arguments);
    shutdown(sock_fd, SHUT_RD);
    exit(0);
}

/**
 * Given commandline argc and argv, parses argv.
 *
 * argc argc from main()
 * argv argv from main()
 *
 * Returns char* array in form of {host, port, method, remote, local, NULL}
 * where `method` is ALL CAPS
 */
char **parse_args(int argc, char **argv) {
    if (argc < 3) {
        return NULL;
    }

    char *host = strtok(argv[1], ":");
    char *port = strtok(NULL, ":");
    if (port == NULL) {
        return NULL;
    }

    char **args = calloc(1, 6 * sizeof(char *));
    args[0] = host;
    args[1] = port;
    args[2] = argv[2];
    char *temp = args[2];
    while (*temp) {
        *temp = toupper((unsigned char)*temp);
        temp++;
    }
    if (argc > 3) {
        args[3] = argv[3];
    }
    if (argc > 4) {
        args[4] = argv[4];
    }

    return args;
}

/**
 * Validates args to program.  If `args` are not valid, help information for the
 * program is printed.
 *
 * args     arguments to parse
 *
 * Returns a verb which corresponds to the request method
 */
verb check_args(char **args) {
    if (args == NULL) {
        print_client_usage();
        exit(1);
    }

    char *command = args[2];

    if (strcmp(command, "LIST") == 0) {
        return LIST;
    }

    if (strcmp(command, "GET") == 0) {
        if (args[3] != NULL && args[4] != NULL) {
            return GET;
        }
        print_client_help();
        exit(1);
    }

    if (strcmp(command, "DELETE") == 0) {
        if (args[3] != NULL) {
            return DELETE;
        }
        print_client_help();
        exit(1);
    }

    if (strcmp(command, "PUT") == 0) {
        if (args[3] == NULL || args[4] == NULL) {
            print_client_help();
            exit(1);
        }
        return PUT;
    }

    // Not a valid Method
    print_client_help();
    exit(1);
}
