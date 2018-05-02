/**
 * A proxy server that connects with a web server and browser
 *
 * Juan Zamudio - jzamudio
 * Cleo Foreman - cforman
 *
 * Proxy Port #: 61206
 * Tiny Server Port #: 61207
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <resolv.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>
#include "csapp.h"

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400
#define PROXY_PORT 61206
#define SERVER_PORT 61207
#define DEFAULT_PORT 80
#define PROXY_HTTP " HTTP/1.0\r\n"
#define EXPECTED_HTTP "HTTP/1.1"
#define GET "GET"

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr = "\r\nUser-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3";
static const char *host_hdr = "Host: ";
static const char *connection_hdr = "\r\nConnection: close";
static const char *proxy_connection_hdr = "\r\nProxy-Connection: close\r\n\r\n";

/*
 * serve_static - copy a file back to the client
 */
 /*
  * get_filetype - derive file type from file name
  */
 // void get_filetype(char *filename, char *filetype)
 // {
 //   if (strstr(filename, ".html"))
 //      strcpy(filetype, "text/html");
 //   else if (strstr(filename, ".gif"))
 //      strcpy(filetype, "image/gif");
 //   else if (strstr(filename, ".png"))
 //      strcpy(filetype, "image/png");
 //   else if (strstr(filename, ".jpg"))
 //      strcpy(filetype, "image/jpeg");
 //   else
 //      strcpy(filetype, "text/plain");
 // }

/* $begin serve_static */
// void serve_static(int fd, char *filename, int filesize)
// {
//     char filetype[MAXLINE], buf[MAXBUF];
//
//     /* Send response headers to client */
//     get_filetype(filename, filetype);       //line:netp:servestatic:getfiletype
//     // sprintf(buf, "HTTP/1.0 200 OK\r\n");    //line:netp:servestatic:beginserve
//     // sprintf(buf, "%sServer: Tiny Web Server\r\n", buf);
//     // sprintf(buf, "%sConnection: close\r\n", buf);
//     // sprintf(buf, "%sContent-length: %d\r\n", buf, filesize);
//     sprintf(buf, "Content-type: %s\r\n\r\n", buf, filetype);
//     Rio_writen(fd, buf, strlen(buf));       //line:netp:servestatic:endserve
//     printf("Response headers:\n");
//     printf("%s", buf);
//
//     /* Send response body to client */
//     // srcfd = Open(filename, O_RDONLY, 0);    //line:netp:servestatic:open
//     // srcp = Mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0);//line:netp:servestatic:mmap
//     // Close(srcfd);                           //line:netp:servestatic:close
//     // Rio_writen(fd, srcp, filesize);         //line:netp:servestatic:write
//     // Munmap(srcp, filesize);                 //line:netp:servestatic:munmap
// }
/* $end serve_static */

void server(int num_args, char* parsed_input[], char* headers, int connfd) {
  size_t n;
  int clientfd;
  char *host, *port, *query, buf[MAXLINE];
  char get_request[MAXLINE];
  rio_t rio;

  host = parsed_input[0];
  port = parsed_input[1];
  query = parsed_input[2];

  strcpy(get_request, "GET ");
  strcat(get_request, query);
  strcat(get_request, PROXY_HTTP);
  strcat(get_request, headers);

  printf("%s", get_request);

  clientfd = Open_clientfd(host, port); //HTTP Request Port
  Rio_readinitb(&rio, clientfd);

  printf("%s", get_request);
  Rio_writen(clientfd, get_request, strlen(get_request));
  // serve_static(connfd, query, strlen(query));

  while((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {
    printf("Web server received %d bytes\n", (int)n);

    Rio_writen(connfd, buf, n);
  }
  Close(clientfd);
}

void parseURL(char* url, int connfd) {
  int NUM_ARGS = 3;
  char hostname[MAXLINE];
  char request_port[MAXLINE];
  char query[MAXLINE];
  char address[MAXLINE];
  char path[MAXLINE];
  char port[MAXLINE];
  char httpr[MAXLINE];
  char *parsedInput[NUM_ARGS];
  char headers[MAXLINE];
  char my_url[MAXLINE];
  char *savepointer;
  char *token;

  strcpy(my_url, url);
  printf("%s", my_url);

  /* Find out where everything is */
  const char *start_of_url = strchr(url, '/') + 2;
  const char *start_of_query = strchr(start_of_url, '/');
  const char *end_of_query = strchr(start_of_query, ' ');

  /* Copy the strings into our memory */
  strncpy(address, start_of_url, 1000);
  strncpy(path, start_of_query, 1000);
  strncpy(port, start_of_query, 1000);
  strncpy(httpr, end_of_query, 1000);

  /* Null terminators (because strncpy does not provide them) */
  hostname[sizeof(hostname)-1] = 0;
  query[sizeof(query)-1] = 0;

  sscanf(address, "%[^/]s", hostname);
  sscanf(path, "%s ", query);
  // printf("%s\n", address);
  // printf("%s\n", hostname);

  if (sscanf(hostname, "%*[^:]s", request_port) == EOF) {
    strcpy(port, "80");
    sscanf(port, "%s", request_port);
  } else {
    savepointer = hostname;
    while((token = strtok_r(savepointer, ":", &savepointer))) {
      sscanf(token, "%s", request_port);
    }
  //  sscanf(hostname, ":\%s", request_port);
  }

  printf("%s\n", request_port);

  sscanf(hostname, "%[^:]s", hostname);

  // printf("\nHostname: %s\nQuery: %s\nPort: %s\n", hostname, query, request_port);
  parsedInput[0] = (char*) hostname;
  parsedInput[1] = (char*) request_port;
  parsedInput[2] = (char*) query;

  strcpy(headers, host_hdr);
  strcat(headers, hostname);
  strcat(headers, user_agent_hdr);
  strcat(headers, connection_hdr);
  strcat(headers, proxy_connection_hdr);

  printf("%s", headers);

  server(NUM_ARGS, parsedInput, headers, connfd);
// GET http://www.cmu.edu/hub/index.html HTTP/1.1
}

void isValid(char buf[], int connfd) {
  char get[MAXLINE];
  char url[MAXLINE];
  char http[MAXLINE];

  sscanf(buf, "%s %s %s", get, url, http);


  // check to make sure if it is valid input, send to parse URL if it is
  int check = !(strcmp(get, GET) & strcmp(http, EXPECTED_HTTP));

  if (check) {
    parseURL(buf, connfd);
  }
  else {
    printf("Invalid request!! Good try though! Try again? Maybe? :^)");
  }
}

void proxy(int connfd) {
  size_t n;
  char buf[MAXLINE];
  rio_t rio;
  Rio_readinitb(&rio, connfd);

  while((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {
    printf("Proxy server received %d bytes\n", (int)n);
    isValid(buf, connfd);
  }
}


void *my_thread(void *connfd_pointer) {
    int connfd = *((int *)connfd_pointer);
    printf("Thread created: %d\n", connfd);
    Pthread_detach(pthread_self());
    Free(connfd_pointer);
    proxy(connfd);
    Close(connfd);
    printf("Thread closed: %d\n", connfd);
    return NULL;
}

int main(int argc, char **argv) {
  int listenfd, *connfd;
  socklen_t clientlen;
  struct sockaddr_storage clientaddr; /* room for any addr */
  char client_hostname[MAXLINE], client_port[MAXLINE];
  pthread_t yarn;

  listenfd = Open_listenfd(argv[1]);

  while (1) {
    clientlen = sizeof(struct sockaddr_storage); /* Important! */
    connfd = Malloc(sizeof(int));
    *connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
    Getnameinfo((SA *) &clientaddr, clientlen, client_hostname, MAXLINE, client_port, MAXLINE, 0);
    printf("Connected to (%s, %s)\n", client_hostname, client_port);
    Pthread_create(&yarn, NULL, my_thread, connfd);
    // proxy(connfd);
    // Close(connfd);
  }
  exit(0);
}
