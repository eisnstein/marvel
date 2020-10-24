#include "../src/uri.h"

#include <stdio.h>
#include <string.h>

#include "../src/str.h"
#include "minunit.h"

char *test_uri_create() {
  uri *uri = uri_create();

  mu_assert(uri != NULL, "Uri object should be created");

  uri_free(uri);

  return NULL;
}

char *test_uri_parse() {
  str *url =
      str_from("http://test.com/path/to/something?ts=1234&hash=hash1234");
  uri *u = uri_create();

  bool res = uri_parse(u, url);
  mu_assert(res == true, "Url should be parsed");

  mu_assert_v(strcmp(str_data(u->scheme), "http") == 0,
              "Scheme should be 'http', but is '%s'", str_data(u->scheme));
  mu_assert_v(strcmp(str_data(u->host), "test.com") == 0,
              "Scheme should be 'test.com', but is '%s'", str_data(u->host));
  mu_assert_v(strcmp(str_data(u->port), "80") == 0,
              "Scheme should be '80', but is '%s'", str_data(u->port));
  mu_assert_v(strcmp(str_data(u->path), "/path/to/something") == 0,
              "Scheme should be '/path/to/something', but is '%s'",
              str_data(u->path));
  mu_assert_v(strcmp(str_data(u->query), "?ts=1234&hash=hash1234") == 0,
              "Scheme should be '?ts=1234&hash=hash1234', but is '%s'",
              str_data(u->query));

  str_free(url);
  uri_free(u);

  return NULL;
}

char *all_tests() {
  mu_suite_start();

  mu_run_test(test_uri_create);
  mu_run_test(test_uri_parse);

  return NULL;
}

RUN_TESTS(all_tests)