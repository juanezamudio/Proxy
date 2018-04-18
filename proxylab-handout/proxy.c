/**
 * A proxy server that connects with a web server and browser
 *
 * Juan Zamudio - jzamudio
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

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";

void parseURL(char* url) {
  char address[90];
  char* hostname;
  char* query;

  strcpy(address, url);
  sscanf(address, "http://%s/", hostname);
  printf("Hostname: %s\n\n", hostname);

}

int main()
{
    parseURL("http://www.cmu.edu/hub/index.html");
    printf("%s", user_agent_hdr);
    return 0;
}
