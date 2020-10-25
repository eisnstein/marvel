#include "../src/http.h"

#include <stdio.h>
#include <string.h>

#include "../src/env.h"
#include "../src/marvel.h"
#include "minunit.h"

char *test_http_client_create() {
  http_client *client = http_client_create();

  mu_assert(client != NULL, "Http client should not be NULL");

  http_client_free(client);

  return NULL;
}

char *test_http_request_create() {
  http_request *req = http_request_create();

  mu_assert(req != NULL, "Http request should no be NULL");

  http_request_free(req);

  return NULL;
}

char *test_http_connect() {
  http_client *client = http_client_create();
  http_request *req = http_request_create();
  uri *u = uri_create();
  str *url = str_from("http://example.com");

  uri_parse(u, url);
  req->uri_data = u;

  bool res = http_connect(client, u->host, u->port);

  mu_assert(res == true, "Result of http connect should be true");

  str_free(url);
  http_client_free(client);
  http_request_free(req);

  return NULL;
}

char *test_http_request_build() {
  http_request *req = http_request_create();
  uri *u = uri_create();
  str *url = str_from(
      "http://gateway.marvel.com/v1/public?ts=1234&apikey=key&hash=1234hash");

  uri_parse(u, url);
  req->uri_data = u;

  char *expected =
      "GET /v1/public?ts=1234&apikey=key&hash=1234hash&limit=2 HTTP/1.1\r\n"
      "Host: gateway.marvel.com\r\n"
      "User-Agent: curl/7.58.0\r\n"
      "Accept: */*\r\n\r\n";
  char *req_raw = http_request_build(req);

  mu_assert_v(strcmp(req_raw, expected) == 0,
              "Request should be:\n%s\nbut is:\n%s\n", expected, req_raw);

  str_free(url);
  http_request_free(req);
  free(req_raw);

  return NULL;
}

char *all_tests() {
  mu_suite_start();

  mu_run_test(test_http_client_create);
  mu_run_test(test_http_request_create);
  mu_run_test(test_http_connect);
  mu_run_test(test_http_request_build);

  return NULL;
}

RUN_TESTS(all_tests)