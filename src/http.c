#define _GNU_SOURCE
#include "http.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "util.h"

static bool http_get_address_info(http *self);
static inline void *get_in_addr(struct sockaddr *sa);
static int http_connect(http *self, str *url, str *port);
static int http_send(http *self, uri_maker *uri);
static str *http_receive(http *self);
static int http_response_parse(http_response *self, str *response_raw);

extern http *http_create_client() {
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

static bool http_get_address_info(http *self) {
  int status = 0;
  struct addrinfo hints;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  status = getaddrinfo(str_data(self->url), str_data(self->port), &hints,
                       &self->res);
  throw_(status != 0, "Could not get address info");
  throw_(self->res == NULL, "Could not get address info");

  return true;

error:
  return false;
}

static inline void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

static int http_get_socket_and_connect(http *self) {
  struct addrinfo *p;
  char ipstr[INET6_ADDRSTRLEN];

  for (p = self->res; p != NULL; p = p->ai_next) {
    if ((self->sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) ==
        -1) {
      perror("client: socket");
      continue;
    }

    if ((self->connection = connect(self->sockfd, p->ai_addr, p->ai_addrlen)) ==
        -1) {
      close(self->sockfd);
      perror("client: connect");
      continue;
    }

    break;
  }

  inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), ipstr,
            sizeof(ipstr));
  debug_v_("client: connecting to %s", ipstr);

  freeaddrinfo(self->res);

  return 1;
}

static int http_connect(http *self, str *url, str *port) {
  throw_(self == NULL, "self is NULL.");
  throw_(url == NULL, "Please provide url.");

  int r = 0;

  self->url = url;
  self->port = port ? port : str_from(DEFAULT_PORT);

  bool res = http_get_address_info(self);
  throw_(res == false, "Could not get address info");

  r = http_get_socket_and_connect(self);
  throw_(r == -1, "Could not get socket or connect");

  return 0;

error:
  return -1;
}

static int http_send(http *self, uri_maker *uri) {
  ssize_t bytes_sent = 0;

  // build the request
  char *req = uri->build_req(uri);

  debug_v_("req:\n%s", req);

  bytes_sent = send(self->sockfd, req, strlen(req), 0);
  throw_(bytes_sent == -1, "Could not send request.");

  free(req);

  return 0;

error:
  if (req) free(req);
  return -1;
}

static str *http_receive(http *self) {
  char buf[MAXDATASIZE];
  str *response_raw = str_create();
  throw_(response_raw == NULL, "Could not create string.");

  ssize_t bytes_received = 0;
  do {
    bytes_received = recv(self->sockfd, buf, MAXDATASIZE, 0);
    throw_(bytes_received == -1, "Could not receive data.");

    str_append(response_raw, buf);
  } while (bytes_received > 0);

  return response_raw;

error:
  if (response_raw) str_destroy(&response_raw);
  return NULL;
}

extern void http_destroy(http **self) {
  if (*self == NULL) {
    return;
  }

  close((*self)->sockfd);

  if ((*self)->url) {
    str_destroy(&(*self)->url);
    (*self)->url = NULL;
  }

  if ((*self)->port) {
    str_destroy(&(*self)->port);
    (*self)->port = NULL;
  }

  free(*self);
  *self = NULL;
}

extern http_response *http_response_create() {
  http_response *r = malloc(sizeof(http_response));
  throw_mem_(r);

  r->raw = NULL;
  r->status_code = 0;
  r->body = NULL;
  r->headers = NULL;
  r->http_response_parse = http_response_parse;

  return r;

error:
  return NULL;
}

static int http_response_parse(http_response *self, str *response_raw) {
  throw_(self == NULL, "http_response must be initialised");
  throw_(response_raw == NULL, "raw response cannot be empty");

  char method[10] = {'\0'};
  char status_code[4] = {'\0'};
  char status[3] = {'\0'};
  char line[200] = {'\0'};
  strlist *headers = strlist_create();
  str *tmp = NULL;

  self->raw = response_raw;

  char *data = str_data(response_raw);

  int r = sscanf(data, "%s %s %s\n", method, status_code, status);
  throw_(r != 3, "failed to parse first line of response");

  self->status_code = (uint16_t)atoi(status_code);

  // parse headers
  mtnl(data);
  while (*data != '\n' && *(data + 1) != '\n') {
    r = sscanf(data, "%s\n", line);
    throw_(r != 1, "failed to parse header line");

    tmp = str_from(line);
    str_strip_nl(tmp);

    strlist_push(headers, tmp);

    memset(line, '\0', 200);
    mtnl(data);
    str_destroy(&tmp);
  }

  self->headers = headers;

  mtnl(data);

  self->body = str_from(data);

  str_destroy(&tmp);

  return 0;

error:
  if (tmp) str_destroy(&tmp);
  return -1;
}

extern void http_response_destroy(http_response **r) {
  if (*r == NULL) {
    return;
  }

  if ((*r)->raw) {
    str_destroy(&(*r)->raw);
    (*r)->raw = NULL;
  }

  if ((*r)->headers) {
    strlist_destroy(&(*r)->headers);
    (*r)->headers = NULL;
  }

  if ((*r)->body) {
    str_destroy(&(*r)->body);
    (*r)->body = NULL;
  }

  free(*r);
  *r = NULL;
}