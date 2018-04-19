/**
 * A proxy server that connects with a web server and browser
 *
 * Juan Zamudio - jzamudio
 * Cleo Foreman - cforeman
 *
 * Proxy Port #: 45806
 * Tiny Server Port #: 45807
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400
#define PROXY_PORT 45806
#define SERVER_PORT 45807
#define PROXY_HTTP "HTTP/1.0"

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";
static const char *host_header = "Host: "

char address[100];
char hostname[100];
char path[100];
char query[100];
char httpr[100];

void parseURL(char* url) {

  /* Find out where everything is */
  const char *start_of_url = strchr(url, '/') + 2;
  const char *start_of_query = strchr(start_of_url, '/');
  const char *end_of_query = strchr(start_of_query, ' ');

  /* Copy the strings into our memory */
  strncpy(address, start_of_url, 100);
  strncpy(path, start_of_query, 100);
  strncpy(httpr, end_of_query, 100);


  /* Null terminators (because strncpy does not provide them) */
  hostname[sizeof(hostname)] = 0;
  query[sizeof(query)] = 0;

  sscanf(address, "%[^/]s", hostname);
  sscanf(path, "%s ", query);
  printf("Hostname: %s\n\nQuery: %s\n\n", hostname, query);
}

void httpValid(char* url) {

}

void readRequest() {

}

void readResponse() {

}

void GET() {

}

void POST() {

}

int main(int argc, char **argv) {
    int port = atoi(argv[0]);

    parseURL("GET http://www.cmu.edu/hub/index.html HTTP/1.1");
    printf("%s", user_agent_hdr);
    return 0;
}
