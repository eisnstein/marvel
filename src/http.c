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

#include "uri.h"
#include "util.h"

// static bool get_address_info(http_client *client);
// static inline void *get_in_addr(struct sockaddr *sa);

/**
 * Create http client instance.
 *
 * @return http_client | NULL
 */
http_client *http_client_create() {
  http_client *h = malloc(sizeof(http_client));
  throw_mem_(h);

  h->sockfd = 0;
  h->connection = 0;
  h->address_info = NULL;

  return h;

error:
  return NULL;
}

static bool get_address_info(http_client *client, str *host, str *port) {
  int status = 0;
  struct addrinfo hints;

  memset(&hints, 0, sizeof(hints));
  hints.ai_flags = AI_PASSIVE;
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = 0;

  status = getaddrinfo(str_data(host), str_data(port), &hints,
                       &client->address_info);
  throw_(status != 0, "Could not get address info");

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
  // char ipstr[INET6_ADDRSTRLEN];

  for (p = client->address_info; p != NULL; p = p->ai_next) {
    if ((client->sockfd =
             socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      continue;
    }

    if ((client->connection =
             connect(client->sockfd, p->ai_addr, p->ai_addrlen)) == -1) {
      close(client->sockfd);
      continue;
    }

    // We have a socket and a connection, so we can break here
    break;
  }

  freeaddrinfo(client->address_info);
  client->address_info = NULL;

  if (client->sockfd == -1 || client->connection == -1) {
    return false;
  }

  /*   inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
    ipstr, sizeof(ipstr)); debug_v_("client: connecting to %s", ipstr); */

  return true;
}

bool http_connect(http_client *client, str *host, str *port) {
  bool res = get_address_info(client, host, port);
  throw_(res == false, "Could not get address info");

  res = get_socket_and_connect(client);
  throw_(res == false, "Could not get socket or connection");

  return true;

error:
  return false;
}

/**
 * Send the request.
 *
 * @param client Http client instance
 *
 * @return bool Success or failure of operation
 */
static bool http_send(http_client *client, const char *raw_req) {
  ssize_t bytes_sent = 0;

  bytes_sent = send(client->sockfd, raw_req, strlen(raw_req), 0);
  throw_(bytes_sent == -1, "Could not send request");

  debug_v_("Bytes send: %zu", bytes_sent);

  return true;

error:
  return false;
}

static str *http_receive(http_client *client) {
  char buf[MAXDATASIZE + 1] = {0};
  str *raw_response = str_create_v(MAXDATASIZE * 5, MAXDATASIZE);
  throw_(raw_response == NULL, "Could not create string");

  ssize_t bytes_received = 0;
  do {
    bytes_received = recv(client->sockfd, buf, MAXDATASIZE, 0);
    throw_(bytes_received == -1, "Could not receive data");

    debug_v_("Bytes received: %zu", bytes_received);
    buf[bytes_received] = '\0';

    str_append(raw_response, buf);
  } while (bytes_received > 0);

  debug_v_("Size of response: %zu", str_length(raw_response));
  debug_v_("Raw response:\n%s", str_data(raw_response));

  return raw_response;

error:
  str_free(raw_response);
  return NULL;
}

/**
 * Function for making HTTP GET requests.
 *
 * @param client Http client instance
 * @param url The full url to send the request to
 *
 * @return http_response | NULL Http response object
 */
http_response *http_get(http_client *client, str *url) {
  http_response *http_response = NULL;
  bool res = false;

  http_request *request = http_request_create();
  throw_(request == NULL, "Could not create request object");

  uri *u = uri_create();
  throw_(u == NULL, "Could not create uri object");

  res = uri_parse(u, url);
  throw_(res == false, "Could not parse url");

  request->uri_data = u;

  // connect to marvel api
  res = http_connect(client, u->host, u->port);
  throw_v_(res == false, "Could not connect to %s", str_data(url));

  const char *raw_request = http_request_build(request);
  debug_v_("Raw request size: %zu", strlen(raw_request));

  // send the request to marvel
  res = http_send(client, raw_request);
  throw_(res == false, "Could not send request to marvel");

  // receive response from marvel
  str *raw_response = http_receive(client);
  throw_(raw_response == NULL, "Could not receive data");

  http_response = http_response_create();
  throw_(http_response == NULL, "Could not create http response object");

  // r = http_response_parse(http_response, raw_response);
  // throw_(r == false, "Could not parse http response");
  free(raw_request);
  str_free(raw_response);
  http_request_free(request);
  return http_response;

error:
  if (raw_request) free(raw_request);
  str_free(raw_response);
  http_request_free(request);
  return NULL;
}

extern void http_client_destroy(http_client *client) {
  if (client == NULL) {
    return;
  }

  close(client->sockfd);
  if (client->address_info) {
    freeaddrinfo(client->address_info);
  }

  free(client);
}

/**
 * Create http request instance.
 *
 * @return http_request | NULL
 */
http_request *http_request_create() {
  http_request *r = malloc(sizeof(http_request));
  throw_mem_(r);

  r->mode = GET;
  r->headers = NULL;
  r->body = NULL;
  r->uri_data = NULL;

  return r;

error:
  return NULL;
}

char *http_request_build(http_request *request) {
  const uri *u = request->uri_data;
  char *req = calloc(256, sizeof(char));
  throw_mem_(req);

  sprintf(req,
          "GET %s%s&limit=1 HTTP/1.1\r\n"
          "Host: gateway.marvel.com\r\n"
          "User-Agent: curl/7.58.0\r\n"
          "Accept: */*\r\n\r\n",
          str_data(u->path), str_data(u->query));

  return req;

error:
  return NULL;
}

void http_request_destroy(http_request *request) {
  if (request == NULL) return;

  strlist_free(request->headers);
  str_free(request->body);
  uri_free(request->uri_data);

  free(request);
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

bool http_response_parse(http_response *response, str *raw_response) {
  str *tmp = NULL;
  throw_(response == NULL, "http_response must be initialised");
  throw_(raw_response == NULL, "raw response cannot be empty");

  char method[10] = {'\0'};
  char status_code[4] = {'\0'};
  char status[3] = {'\0'};
  char line[200] = {'\0'};
  strlist *headers = strlist_create();

  response->raw = raw_response;

  char *data = str_data(raw_response);

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