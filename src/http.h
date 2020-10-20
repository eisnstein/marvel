#ifndef __http_h__
#define __http_h__

#include <netdb.h>

#include "str.h"
#include "uri.h"

#define DEFAULT_PORT "80"
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

typedef struct http {
  int sockfd;
  int connection;
  struct addrinfo *res;
  str *url;
  str *port;
} http;

typedef enum http_mode { GET, POST } http_mode;

typedef struct http_request {
  http_mode mode;
  str *base_endpoint;
  str *path;
  str *query;
  strlist *headers;
  str *body;
} http_request;

typedef struct http_response {
  str *raw;
  int status_code;
  strlist *headers;
  str *body;
} http_response;

typedef struct uri {
  str *scheme;
  str *host;
  str *port;
  str *path;
  str *query;
  str *hash;
} uri;

extern http *http_client();
extern http_response *http_get(http *client, str *url);
extern bool http_connect(http *client, str *url, str *port);
extern bool http_send(http *client, uri_maker *uri);
extern str *http_receive(http *client);
extern void http_client_destroy(http *self);
extern uri *parse_url(str *url);
extern http_request *http_request_create();
extern void http_reqeust_destroy(http_request *r);

extern http_response *http_response_create();
extern bool http_response_parse(http_response *response, str *response_raw);
extern void http_response_destroy(http_response *r);

#endif