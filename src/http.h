#ifndef __http_h__
#define __http_h__

#include <netdb.h>

#include "str.h"
#include "uri.h"

#define DEFAULT_PORT "80"
#define MAXDATASIZE 1024

typedef struct http http;
typedef int (*connect_func)(http *self, str *url, str *port);
typedef int (*send_func)(http *self, uri_maker *uri);
typedef str *(*receive_func)(http *self);

typedef struct http_response http_response;
typedef int (*http_response_parse_func)(http_response *self, str *response_raw);

/*** structs ***/
struct http {
  int sockfd;
  int connection;
  struct addrinfo *res;
  str *url;
  str *port;
  connect_func http_connect;
  send_func http_send;
  receive_func http_receive;
};

struct http_response {
  str *raw;
  uint16_t status_code;
  strlist *headers;
  str *body;
  http_response_parse_func http_response_parse;
};

/*** function declarations ***/
extern http *http_create_client();
// static int http_connect(http *self, const str *url, const str *port);
// static int http_send(http *self, uri_maker *uri);
// static str *http_receive(http *self);
extern void http_destroy(http **self);

extern http_response *http_response_create();
// static int http_response_parse(http_response *self, const str *response_raw);
extern void http_response_destroy(http_response **r);

#endif