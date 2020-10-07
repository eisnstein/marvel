#ifndef __http_h__
#define __http_h__

#include <netdb.h>

#include "str.h"
#include "uri.h"

#define DEFAULT_PORT "80"
#define MAXDATASIZE 1024

#define http_free(H) \
  do {               \
    http_destroy(H); \
    H = NULL;        \
  } while (0);

#define http_response_free(R) \
  do {                        \
    http_response_destroy(R); \
    R = NULL;                 \
  } while (0);

typedef struct http {
  int sockfd;
  int connection;
  struct addrinfo *res;
  str *url;
  str *port;
} http;

typedef struct http_response {
  str *raw;
  uint16_t status_code;
  strlist *headers;
  str *body;
} http_response;

extern http *http_create();
extern bool http_connect(http *client, str *url, str *port);
extern bool http_send(http *client, uri_maker *uri);
extern str *http_receive(http *client);
extern void http_destroy(http *self);

extern http_response *http_response_create();
extern bool http_response_parse(http_response *response, str *response_raw);
extern void http_response_destroy(http_response *r);

#endif