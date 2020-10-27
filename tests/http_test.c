#include "../src/http.h"

#include <stdio.h>
#include <string.h>

#include "../src/env.h"
#include "../src/marvel.h"
#include "minunit.h"

char *test_http_client_create() {
  http_client *client = http_client_create();

  mu_assert(client != NULL, "Http client should not be NULL");
  mu_assert(client->address_info == NULL,
            "Address info pointer should be NULL");
  mu_assert(client->sockfd == 0, "Socket file descriptior should be 0");
  mu_assert(client->connection == 0, "Connection should be 0");

  http_client_free(client);

  return NULL;
}

char *test_http_request_create() {
  http_request *req = http_request_create();

  mu_assert(req != NULL, "Http request should not be NULL");
  mu_assert(req->mode == GET, "Http mode should be GET");
  mu_assert(req->body == NULL, "Request body should be NULL");
  mu_assert(req->headers == NULL, "Request headers should be NULL");
  mu_assert(req->uri_data == NULL, "Request uri_data should be NULL");

  http_request_free(req);

  return NULL;
}

char *test_http_response_create() {
  http_response *res = http_response_create();

  mu_assert(res != NULL, "Http response should not be NULL");
  mu_assert(res->raw == NULL, "Raw should be NULL");
  mu_assert(res->body == NULL, "Response body should be NULL");
  mu_assert(res->headers == NULL, "Response headers should be NULL");
  mu_assert(res->status_code == 0, "Response status code should be 0");

  http_response_free(res);

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
      "GET /v1/public?ts=1234&apikey=key&hash=1234hash&limit=1 HTTP/1.1\r\n"
      "Host: gateway.marvel.com\r\n"
      "User-Agent: curl/7.58.0\r\n"
      "Accept: application/json\r\n\r\n";
  char *req_raw = http_request_build(req);

  mu_assert_v(strcmp(req_raw, expected) == 0,
              "Request should be:\n%s\nbut is:\n%s\n", expected, req_raw);

  str_free(url);
  http_request_free(req);
  free(req_raw);

  return NULL;
}

char *test_http_response_parse() {
  str *raw_response = str_from(
      "HTTP/1.1 200 OK\r\n"
      "Content-Encoding: gzip\r\n"
      "ETag: 63b7844012f331ce4e4eda1eda994e843f5ecf74\r\n"
      "Content-Type: application/json; charset=utf-8\r\n"
      "Date: Tue, 27 Oct 2020 21:41:06 GMT\r\n"
      "Connection: keep-alive\r\n"
      "Transfer-Encoding: chunked\r\n\r\n"
      "{\"code\":200,\"status\":\"Ok\"}");

  http_response *response = http_response_create();

  bool res = http_response_parse(response, raw_response);
  mu_assert(res == true, "Http response parse result should be 'true'");
  mu_assert_v(response->status_code == 200,
              "Http status code should be 200, but is %d",
              response->status_code);
  mu_assert_v(
      strcmp(str_data(response->body), "{\"code\":200,\"status\":\"Ok\"}") == 0,
      "Body should be ..., but is '%s'", str_data(response->body));

  http_response_free(response);

  return NULL;
}

char *all_tests() {
  mu_suite_start();

  mu_run_test(test_http_client_create);
  mu_run_test(test_http_request_create);
  mu_run_test(test_http_connect);
  mu_run_test(test_http_request_build);
  mu_run_test(test_http_response_parse);

  return NULL;
}

RUN_TESTS(all_tests)