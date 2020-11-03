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

/**
 * Create an http_client instance.
 *
 * @return An http_client instance or NULL on failure
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

/**
 * Get address info of host.
 *
 * @param client An http_client instance
 * @param host The host string of the peer
 * @param port The port to connect to as string
 *
 * @return True on success, false on failure
 */
static bool get_address_info(http_client *client, const str *host,
                             const str *port) {
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

/**
 * Create socket and connect to peer.
 *
 * @param client An http_client instance
 *
 * @return True on success, false on failure
 */
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

/**
 * Connect to the peer.
 *
 * @param client An http_client instance
 * @param host The host string of the peer
 * @param port The port to connect to as string
 *
 * @return True on success, false on failure
 */
bool http_connect(http_client *client, const str *host, const str *port) {
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
 * @param client An http_client instance
 * @param raw_req Pointer to the raw request string
 *
 * @return True on success, false on failure
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

/**
 * Receive data from the peer.
 *
 * @param client An http_client instance
 *
 * @return The raw response string or NULL on failure
 */
static str *http_receive(const http_client *client) {
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

  return raw_response;

error:
  str_free(raw_response);
  return NULL;
}

/**
 * Make an http GET request.
 *
 * @param client The http_client instance
 * @param url The full url to send the request to
 *
 * @return An http_response instance or NULL on failure
 */
http_response *http_get(http_client *client, str *url) {
  uri *u = NULL;
  char *raw_request = NULL;
  str *raw_response = NULL;
  http_response *response = NULL;
  bool res = false;

  http_request *request = http_request_create();
  throw_(request == NULL, "Could not create request object");

  u = uri_create();
  throw_(u == NULL, "Could not create uri object");

  res = uri_parse(u, url);
  throw_(res == false, "Could not parse url");

  request->uri_data = u;

  // connect to the host
  res = http_connect(client, u->host, u->port);
  throw_v_(res == false, "Could not connect to %s", str_data(url));

  raw_request = http_request_build(request);
  debug_v_("Raw request size: %zu", strlen(raw_request));

  // send the raw request
  res = http_send(client, raw_request);
  throw_(res == false, "Could not send request to marvel");

  // get the raw response
  raw_response = http_receive(client);
  throw_(raw_response == NULL, "Could not receive data");

  response = http_response_create();
  throw_(response == NULL, "Could not create http response object");

  // parse the raw response into an http_response instance
  res = http_response_parse(response, raw_response);
  throw_(res == false, "Could not parse http response");

  free(raw_request);
  http_request_free(request);
  return response;

error:
  if (raw_request) free(raw_request);
  str_free(raw_response);
  http_request_free(request);
  http_response_free(response);
  return NULL;
}

/**
 * Free an http_client instance.
 *
 * @param client Pointer to an http_client instance
 */
void http_client_destroy(http_client *client) {
  if (client == NULL) return;

  close(client->sockfd);
  if (client->address_info) {
    freeaddrinfo(client->address_info);
  }

  free(client);
}

/**
 * Create an http_request instance.
 *
 * @return An http_request instance or NULL on failure
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

/**
 * Build a raw http request string from an
 * http_request instance.
 *
 * @param request Pointer to the http_request instance which holds all
 * information for building a raw request string
 *
 * @return The raw request string or NULL on failure
 */
char *http_request_build(const http_request *request) {
  const uri *u = request->uri_data;
  char *req = calloc(256, sizeof(char));
  throw_mem_(req);

  sprintf(req,
          "GET %s%s&limit=1 HTTP/1.1\r\n"
          "Host: gateway.marvel.com\r\n"
          "User-Agent: my-marvel/1.0\r\n"
          "Accept: application/json\r\n\r\n",
          str_data(u->path), str_data(u->query));

  return req;

error:
  return NULL;
}

/**
 * Free an http_request instance.
 *
 * DO NOT use this function directly. Instead use the 'http_request_free'
 * macro.
 *
 * @param request Pointer to an http_request instance
 */
void http_request_destroy(http_request *request) {
  if (request == NULL) return;

  strlist_free(request->headers);
  str_free(request->body);
  uri_free(request->uri_data);

  free(request);
}

/**
 * Create an http_response instance.
 *
 * @return An http_response instance or NULL on failure
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
  char *data = NULL;
  throw_(response == NULL, "http_response must be initialised");
  throw_(raw_response == NULL, "raw response cannot be empty");

  char method[10] = {'\0'};
  char status_code[4] = {'\0'};
  char status[3] = {'\0'};
  char line[200] = {'\0'};
  strlist *headers = strlist_create();
  throw_(headers == NULL, "Could not create strlist for response headers");

  response->raw = raw_response;
  data = str_data(raw_response);

  int r = sscanf(data, "%s %s %s\n", method, status_code, status);
  throw_(r != 3, "failed to parse first line of response");

  response->status_code = (int)atoi(status_code);

  // parse headers
  mtnl(data);
  while (*data != '\n' && *(data + 1) != '\n') {
    r = sscanf(data, "%s\r\n", line);
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
  str_free(tmp);
  strlist_free(headers);
  return false;
}

/**
 * Free an http_response instance.
 *
 * DO NOT use this function directly. Instead use the 'http_response_free'
 * macro.
 *
 * @param response Pointer to an http_resonse instance
 */
void http_response_destroy(http_response *response) {
  if (response == NULL) return;

  str_free(response->raw);
  strlist_free(response->headers);
  str_free(response->body);

  free(response);
}