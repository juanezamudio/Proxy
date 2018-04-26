/**
 * A proxy server that connects with a web server and browser
 *
 * Juan Zamudio - jzamudio
 * Cleo Foreman - cforman
 *
 * Proxy Port #: 45806
 * Tiny Server Port #: 45807
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <resolv.h>
#include <netdb.h>
#include <unistd.h>

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400
#define PROXY_PORT 61206
#define SERVER_PORT 61207
#define DEFAULT_PORT 80
#define PROXY_HTTP "HTTP/1.0"

/* You won't lose style points for including this long line in your code */
//static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";
//static const char *host_hdr = "Host: ";
//static const char *connection_hdr = "Connection: close";
//static const char *proxy_connection_hdr = "Proxy-Connection: close";

typedef struct {
  char* hostname;
  char* port_number;
  char* query;
} addr_url;

addr_url parseURL(char* url) {
  addr_url address_url;
  char address[100];
  char hostname[100];
  char path[100];
  char query[100];
  char port[100];
  char httpr[100];

  /* Find out where everything is */
  const char *start_of_url = strchr(url, '/') + 2;
  const char *start_of_query = strchr(start_of_url, '/');
  const char *start_of_port = strchr(start_of_query, ':');
  const char *end_of_query = strchr(start_of_port, ' ');

  /* Copy the strings into our memory */
  strncpy(address, start_of_url, 100);
  strncpy(path, start_of_query, 100);
  strncpy(port, start_of_port, 100);
  strncpy(httpr, end_of_query, 100);


  /* Null terminators (because strncpy does not provide them) */
  hostname[sizeof(hostname)-1] = 0;
  query[sizeof(query)-1] = 0;
  port[sizeof(port)-1] = 0;

  sscanf(address, "%[^/]s", address_url.hostname);
  sscanf(path, "%s ", address_url.query);
  sscanf(port, ":%s", address_url.port_number);

  printf("Hostname: %s\n\nQuery: %s\n\nPort: %s\n\n", hostname, query, port);

  return address_url;
}

int open_clientfd(char *hostname, char *port) {
  int clientfd;
  struct addrinfo hints, *listp, *p;

  /* Get a list of potential server addresses */
  memset(&hints, 0, sizeof(struct addrinfo));

  hints.ai_socktype = SOCK_STREAM; /* Open a connection */
  hints.ai_flags = AI_NUMERICSERV; /* Using numeric port arg. */
  hints.ai_flags |= AI_ADDRCONFIG; /* Recommended for connections */

  getaddrinfo(hostname, port, &hints, &listp);

  /* Walk the list for one that we can successfully connect to */
  for (p = listp; p; p = p->ai_next) {
    /* Create a socket descriptor */
    if ((clientfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0)
      continue; /* Socket failed, try the next */

    /* Connect to the server */
    if (connect(clientfd, p->ai_addr, p->ai_addrlen) != -1)
      break; /* Success */

    close(clientfd); /* Connect failed, try another */
  }

  /* Clean up */
  freeaddrinfo(listp);

  if (!p) /* All connects failed */
    return -1;
  else /* The last connect succeeded */
    return clientfd;
}

int open_listenfd(char *port) {
  struct addrinfo hints, *listp, *p;
  int listenfd, optval=1;

  /* Get a list of potential server addresses */
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_socktype = SOCK_STREAM; /* Accept connect. */
  hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG; /* On any IP addr */
  hints.ai_flags |= AI_NUMERICSERV; /* Using port no. */
  getaddrinfo(NULL, port, &hints, &listp);

  /* Walk the list for one that we can bind to */
  for (p = listp; p; p = p->ai_next) {
    /* Create a socket descriptor */
    if ((listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0)
      continue; /* Socket failed, try the next */

    /* Eliminates "Address already in use" error from bind */
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));

    /* Bind the descriptor to the address */
    if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0)
      break; /* Success */

    close(listenfd); /* Bind failed, try the next */
  }

  /* Clean up */
  freeaddrinfo(listp);

  if (!p) /* No address worked */
    return -1;

  /* Make it a listening socket ready to accept conn. requests */
  if (listen(listenfd, 0) < 0) { // backlog = 0
    close(listenfd);
    return -1;
  }

  return listenfd;
}

int main(int argc, char **argv) {
    char* port = argv[1];

    addr_url parsedURL = parseURL("GET http://www.cmu.edu/hub/index.html: HTTP/1.1");

    //int clientfd =
    open_clientfd(parsedURL.hostname, parsedURL.port_number);
    int serverfd = open_listenfd(port);

    //accept(listenfd, SA *addr, int *addrlen);
    accept(serverfd, NULL, NULL);

    //printf("%s", user_agent_hdr);
    return 0;
}
