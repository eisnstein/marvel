#include "../src/env.h"

#include <stdio.h>
#include <string.h>

#include "minunit.h"

char *test_read_env_vars() {
  int ret = env_init(".env.test");

  mu_assert(ret == 0, "Something went wrong parsing the .env.test file");
  mu_assert(strcmp(getenv("MARVEL_BASE_URL"), "http://gateway.marvel.com") == 0,
            "Url does not match");
  mu_assert(strcmp(getenv("MARVEL_BASE_ENDPOINT"), "/v1/public") == 0,
            "Endpoint does not match");
  mu_assert(strcmp(getenv("MARVEL_PUBLIC_KEY"), "public_key") == 0,
            "Public key does not match");
  mu_assert(strcmp(getenv("MARVEL_PRIVATE_KEY"), "private_key") == 0,
            "Private key does not match");

  return NULL;
}

char *all_tests() {
  mu_suite_start();

  mu_run_test(test_read_env_vars);

  return NULL;
}

RUN_TESTS(all_tests)