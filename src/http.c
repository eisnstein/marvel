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

static bool get_address_info(http_client *client);
static inline void *get_in_addr(struct sockaddr *sa);

http_client *http_client_create() {
  http_client *h = malloc(sizeof(http_client));
  throw_mem_(h);

  h->sockfd = 0;
  h->connection = 0;
  h->res = NULL;
  h->req = NULL;

  return h;

error:
  return NULL;
}

static bool get_address_info(http_client *client) {
  int status = 0;
  struct addrinfo hints;

  memset(&hints, 0, sizeof(hints));
  hints.ai_flags = AI_PASSIVE;
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  status = getaddrinfo(str_data(client->req->uri->host),
                       str_data(client->req->uri->port), &hints, &client->res);
  throw_(status != 0, "Could not get address info");
  throw_(client->res == NULL, "Could not get address info");

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

static bool get_socket_and_connect(http_client *client) {
  struct addrinfo *p;
  char ipstr[INET6_ADDRSTRLEN];

  for (p = client->res; p != NULL; p = p->ai_next) {
    if ((client->sockfd =
             socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      perror("client: socket");
      continue;
    }

    if ((client->connection =
             connect(client->sockfd, p->ai_addr, p->ai_addrlen)) == -1) {
      close(client->sockfd);
      perror("client: connect");
      continue;
    }

    break;
  }

  /*   inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
    ipstr, sizeof(ipstr)); debug_v_("client: connecting to %s", ipstr); */

  freeaddrinfo(client->res);

  return true;
}

static bool http_connect(http_client *client) {
  throw_(client == NULL, "No http client provided");

  bool res = get_address_info(client);
  throw_(res == false, "Could not get address info");

  res = get_socket_and_connect(client);
  throw_(res == false, "Could not get socket or connect");

  return true;

error:
  return false;
}

static bool http_send(http_client *client, uri *urim) {
  ssize_t bytes_sent = 0;

  // build the request
  char *req = "request";

  debug_v_("req:\n%s", req);

  bytes_sent = send(client->sockfd, req, strlen(req), 0);
  throw_(bytes_sent == -1, "Could not send request.");

  free(req);

  return true;

error:
  if (req) free(req);
  return false;
}

static str *http_receive(http_client *client) {
  char buf[MAXDATASIZE + 1] = {0};
  str *response_raw = str_create_v(MAXDATASIZE * 5, MAXDATASIZE);
  throw_(response_raw == NULL, "Could not create string.");

  ssize_t bytes_received = 0;
  do {
    bytes_received = recv(client->sockfd, buf, MAXDATASIZE, 0);
    throw_(bytes_received == -1, "Could not receive data.");

    debug_v_("Bytes received: %zu\n", bytes_received);

    str_append(response_raw, buf);
  } while (bytes_received > 0);

  return response_raw;

error:
  if (response_raw) str_free(response_raw);
  return NULL;
}

http_response *http_get(http_client *client, str *url) {
  http_response *http_response = NULL;
  bool r = false;

  http_request *request = http_request_create(url);
  throw_(request == NULL, "Could not create request object");

  client->req = request;

  // connect to marvel api
  r = http_connect(client);
  throw_v_(r == false, "Could not connect to %s", str_data(url));

  // send the request to marvel
  // r = http_send(client, uri_maker);
  // throw_(r == false, "Could not send request to marvel");

  // receive response from marvel
  // str *response_raw = http_receive(client);
  // throw_(response_raw == NULL, "Could not receive data");

  // debug_v_("size of response: %ld", str_length(response_raw));

  http_response = http_response_create();
  throw_(http_response == NULL, "Could not create http response object");

  // r = http_response_parse(http_response, response_raw);
  // throw_(r == false, "Could not parse http response");
  return NULL;

error:
  return NULL;
}

extern void http_client_destroy(http_client *client) {
  if (client == NULL) {
    return;
  }

  close(client->sockfd);
  http_request_free(client->req);

  free(client);
}

/**
 * HTTP Request
 */
http_request *http_request_create(str *url) {
  http_request *r = malloc(sizeof(http_request));
  throw_mem_(r);

  r->mode = GET;
  r->headers = NULL;
  r->body = NULL;

  return r;

error:
  return NULL;
}

void http_request_destroy(http_request *req) {
  if (req == NULL) {
    return;
  }

  strlist_free(req->headers);
  str_free(req->body);

  free(req);
}

/**
 * HTTP Response
 */

http_response *http_response_create() {
  http_response *r = malloc(sizeof(http_response));
  throw_mem_(r);

  r->raw = NULL;
  r->status_code = 0;
  r->body = NULL;
  r->headers = NULL;

  return r;

error:
  return NULL;
}

bool http_response_parse(http_response *response, str *response_raw) {
  str *tmp = NULL;
  throw_(response == NULL, "http_response must be initialised");
  throw_(response_raw == NULL, "raw response cannot be empty");

  char method[10] = {'\0'};
  char status_code[4] = {'\0'};
  char status[3] = {'\0'};
  char line[200] = {'\0'};
  strlist *headers = strlist_create();

  response->raw = response_raw;

  char *data = str_data(response_raw);

  int r = sscanf(data, "%s %s %s\n", method, status_code, status);
  throw_(r != 3, "failed to parse first line of response");

  response->status_code = (uint16_t)atoi(status_code);

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
    str_free(tmp);
  }

  response->headers = headers;

  mtnl(data);

  response->body = str_from(data);

  str_free(tmp);

  return true;

error:
  if (tmp) str_free(tmp);
  return false;
}

void http_response_destroy(http_response *r) {
  if (r == NULL) {
    return;
  }

  str_free(r->raw);
  strlist_free(r->headers);
  str_free(r->body);

  free(r);
}