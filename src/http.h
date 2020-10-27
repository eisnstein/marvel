#ifndef __http_h__
#define __http_h__

#include <netdb.h>

#include "str.h"
#include "uri.h"

#define MAXDATASIZE 2048

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

typedef struct addrinfo adri;
typedef enum http_mode { GET, POST } http_mode;

typedef struct http_request {
  http_mode mode;
  strlist *headers;
  str *body;
  uri *uri_data;
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
  adri *address_info;
} http_client;

extern http_client *http_client_create();
extern http_response *http_get(http_client *client, str *url);
extern bool http_connect(http_client *client, const str *host, const str *port);
extern void http_client_destroy(http_client *client);
extern http_request *http_request_create();
extern char *http_request_build(const http_request *request);
extern void http_request_destroy(http_request *request);

extern http_response *http_response_create();
extern bool http_response_parse(http_response *response, str *raw_response);
extern void http_response_destroy(http_response *r);

#endif