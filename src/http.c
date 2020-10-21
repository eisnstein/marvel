#define _GNU_SOURCE
#include "http.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "util.h"

static bool http_get_address_info(http *client);
static inline void *get_in_addr(struct sockaddr *sa);

http *http_client() {
  http *h = malloc(sizeof(http));
  throw_mem_(h);

  h->sockfd = 0;
  h->connection = 0;
  h->res = NULL;
  h->url = NULL;
  h->port = NULL;

  return h;

error:
  return NULL;
}

static bool http_get_address_info(http *client) {
  int status = 0;
  struct addrinfo hints;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  status = getaddrinfo(str_data(client->url), str_data(client->port), &hints,
                       &client->res);
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

static bool http_get_socket_and_connect(http *client) {
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

bool http_connect(http *client, str *url, str *port) {
  throw_(client == NULL, "No http client provided");
  throw_(url == NULL, "No URL provided");

  client->url = url;
  client->port = port ? port : str_from(DEFAULT_PORT);

  bool res = http_get_address_info(client);
  throw_(res == false, "Could not get address info");

  res = http_get_socket_and_connect(client);
  throw_(res == false, "Could not get socket or connect");

  return true;

error:
  return false;
}

bool http_send(http *client, uri_maker *urim) {
  ssize_t bytes_sent = 0;

  // build the request
  char *req = uri_maker_build_req(urim);

  debug_v_("req:\n%s", req);

  bytes_sent = send(client->sockfd, req, strlen(req), 0);
  throw_(bytes_sent == -1, "Could not send request.");

  free(req);

  return true;

error:
  if (req) free(req);
  return false;
}

str *http_receive(http *client) {
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

http_response *http_get(str *url) {
  http_response *http_response = NULL;
  bool r = false;
  http_request *request = http_request_create(url);
  throw_(request == NULL, "Could not create request object");

  uri *u = parse_url(url);

  // connect to marvel api
  r = http_connect(client, url, NULL);
  throw_v_(r == false, "Could not connect to %s", str_data(url));

  // send the request to marvel
  r = http_send(client, uri_maker);
  throw_(r == false, "Could not send request to marvel");

  // receive response from marvel
  str *response_raw = http_receive(client);
  throw_(response_raw == NULL, "Could not receive data");

  debug_v_("size of response: %ld", str_length(response_raw));

  http_response = http_response_create();
  throw_(http_response == NULL, "Could not create http response object");

  r = http_response_parse(http_response, response_raw);
  throw_(r == false, "Could not parse http response");
  return NULL;

error:
  return NULL;
}

extern void http_client_destroy(http *h) {
  if (h == NULL) {
    return;
  }

  close(h->sockfd);
  str_free(h->url);
  str_free(h->port);

  free(h);
}

uri *parse_url(str *url) {
  const char *pattern = "^(https?):\/\/([a-zA-Z0-9.]+)(\/?[^\?]*)?\?\?(.+)?";
  regex_t preg;
  const size_t possibleMatchCount = 5;
  regmatch_t pm[possibleMatchCount];
  const size_t bufferSize = 256;
  char errorBuffer[bufferSize];
  char buf[bufferSize];

  uri *u = malloc(sizeof(uri));
  throw_mem_(u);

  int res = regcomp(&preg, pattern, REG_ICASE | REG_EXTENDED);
  if (res != 0) {
    regerror(res, &preg, errorBuffer, bufferSize);
    throw_v_(true, "Could not compile regex: %s", errorBuffer);
  }

  res = regexec(&preg, str_data(url), possibleMatchCount, pm, 0);
  if (res != 0) {
    regerror(res, &preg, errorBuffer, bufferSize);
    throw_v_(true, "Could not exec regex: %s\n", errorBuffer);
  }

  regoff_t len = 0;
  str *tmp = NULL;
  for (size_t i = 1; i < possibleMatchCount; i++) {
    if (pm[i].rm_so == -1) {
      break;
    }

    len = pm[i].rm_eo - pm[i].rm_so;
    tmp = str_substr(url, pm[i].rm_so, len);
    debug_v_("Value: %s\n", str_data(tmp));
  }

  regfree(&preg);
  str_free(tmp);

error:
  return NULL;
}

/**
 * HTTP Request
 */
http_request *http_request_create(str *url) {
  http_request *r = malloc(sizeof(http_request));
  throw_mem_(r);

  r->mode = GET;
  r->base_endpoint = NULL;
  r->path = NULL;
  r->query = NULL;
  r->headers = NULL;
  r->body = NULL;

  return r;

error:
  return NULL;
}

void http_request_destroy(http_request *r) {
  if (r == NULL) {
    return;
  }

  str_free(r->base_endpoint);
  str_free(r->path);
  str_free(r->query);
  strlist_free(r->headers);
  str_free(r->body);

  free(r);
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