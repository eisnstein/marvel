#define _GNU_SOURCE
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "http.h"
#include "util.h"

http *http_create_client()
{
    http *h = malloc(sizeof(http));
    throw_mem_(h);

    h->sockfd = 0;
    h->connection = 0;
    h->res = NULL;
    h->url = NULL;
    h->port = NULL;
    h->http_connect = http_connect;
    h->http_send = http_send;
    h->http_receive = http_receive;

    return h;

    error:
    return NULL;   
}

static int get_address_info(http *self)
{
    int status = 0;
    struct addrinfo hints;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    status = getaddrinfo(str_data(self->url), str_data(self->port), &hints, &self->res);
    throw_(status != 0, "Could not get address info.");

    throw_(self->res == NULL, "Could not get address info.");

    return 1;

    error:
    return -1;
}

static inline void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

static inline int get_socket_and_connect(http *self)
{
    struct addrinfo *p;
    char ipstr[INET6_ADDRSTRLEN];

    for (p = self->res; p != NULL; p = p->ai_next) {
        if ((self->sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if ((self->connection = connect(self->sockfd, p->ai_addr, p->ai_addrlen)) == -1) {
            close(self->sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), ipstr, sizeof(ipstr));
    debug_("client: connecting to %s", ipstr);

    freeaddrinfo(self->res);

    return 1;
}

int http_connect(http *self, const str *url, const str *port)
{
    throw_(self == NULL, "self is NULL.");
    throw_(url == NULL, "Please provide url.");

    int r = 0;

    self->url = url;
    self->port = port ? port : str_from(DEFAULT_PORT);

    r = get_address_info(self);
    throw_(r == -1, "Could not get address info.");

    r = get_socket_and_connect(self);
    throw_(r == -1, "Could not get socket or connect.");

    return 1;

    error:
    return -1;
}

int http_send(http *self, uri_maker *uri)
{
    int status;

    // build the request
    char *req = uri->build_req(uri);

    debug_("req:\n%s", req);

    status = send(self->sockfd, req, strlen(req), 0);

    return 1;

    error:
    return -1;
}

int http_receive(http *self)
{

}