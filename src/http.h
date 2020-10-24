#ifndef __http_h__
#define __http_h__

#include <netdb.h>

#include "str.h"
#include "uri.h"

#define MAXDATASIZE 1024

#define http_client_free(H) \
  do {                      \
    http_client_destroy(H); \
    H = NULL;               \
  } while (0);

#define http_request_free(R) \
  do {                       \
    http_request_destroy(R); \
    R = NULL;                \
  } while (0);

#define http_response_free(R) \
  do {                        \
    http_response_destroy(R); \
    R = NULL;                 \
  } while (0);

typedef enum http_mode { GET, POST } http_mode;

typedef struct http_request {
  http_mode mode;
  strlist *headers;
  str *body;
  uri *uri;
} http_request;

typedef struct http_response {
  str *raw;
  int status_code;
  strlist *headers;
  str *body;
} http_response;

typedef struct http_client {
  int sockfd;
  int connection;
  struct addrinfo *res;
  http_request *req;
} http_client;

extern http_client *http_client_create();
extern http_response *http_get(http_client *client, str *url);
/* extern bool http_connect(http_client *client, str *url, str *port);
extern bool http_send(http_client *client, uri *uri);
extern str *http_receive(http_client *client); */
extern void http_client_destroy(http_client *client);
extern http_request *http_request_create(str *url);
extern void http_request_destroy(http_request *r);

extern http_response *http_response_create();
extern bool http_response_parse(http_response *response, str *response_raw);
extern void http_response_destroy(http_response *r);

#endif