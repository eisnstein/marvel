#ifndef __http_h__
#define __http_h__

#include <netdb.h>
#include "uri.h"
#include "str.h"

#define DEFAULT_PORT "80"

typedef struct http http;
typedef int (*connect_func)(http *self, const str *url, const str *port);
typedef int (*send_func)(http *self, const str *endpoint, const str *data);
typedef int (*receive_func)(http *self);

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

int http_connect(http *self, const str *url, const str *port);
int http_send(http *self, uri_maker *uri);
int http_receive(http *self);

#endif