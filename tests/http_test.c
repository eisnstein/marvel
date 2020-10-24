#include "../src/http.h"

#include <stdio.h>
#include <string.h>

#include "../src/env.h"
#include "../src/marvel.h"
#include "minunit.h"

char *test_http_client_create() {
  http_client *client = http_client_create();

  mu_assert(client != NULL, "Http client should be created");

  http_client_free(client);

  return NULL;
}

char *all_tests() {
  mu_suite_start();

  mu_run_test(test_http_client_create);

  return NULL;
}

RUN_TESTS(all_tests)