#include "../src/marvel.h"

#include <stdio.h>
#include <string.h>

#include "../src/env.h"
#include "minunit.h"

char *test_generate_hash() {
  const char *publicKey = "public_key";
  const char *privateKey = "private_key";
  const char *ts = "1234";

  const char *hash = generate_hash(ts, privateKey, publicKey);

  // https://www.hashgenerator.de/
  // -> select "MD5" -> write into the input field "1234private_keypublic_key"
  const char *expected = "4a43372a739f1ca7d2a2055935c3cd0d";
  mu_assert_v(strcmp(hash, expected) == 0,
              "Hash should equal '%s', but is '%s'", expected, hash);

  free(hash);

  return NULL;
}

char *test_generate_timestamp() {
  const char *ts = generate_timestamp();

  mu_assert_v(strlen(ts) == 10,
              "Timestamp should have length of 10, but has %zu", strlen(ts));

  free(ts);

  return NULL;
}

char *test_generate_url() {
  const char *expected =
      "http://test.com/api/"
      "comics?ts=1234&apikey=public_key&hash="
      "hash123";

  const char *url = generate_url("http://test.com/api", "comics", "1234",
                                 "public_key", "hash123");

  mu_assert_v(strcmp(url, expected) == 0,
              "Full url should be '%s', but is '%s'", expected, url);

  free(url);

  return NULL;
}

char *test_marvel_build_url() {
  str *query = str_from("comics");

  str *url = marvel_build_url(query);
  mu_assert(url == NULL, "Url should be null");

  int res = env_init(".env.test");

  url = marvel_build_url(query);
  const char *expected = "http://gateway.marvel.com/v1/public/comics?ts=";

  mu_assert(str_starts_with(url, expected) == true,
            "Url should start with right endpoint");

  str_free(query);
  str_free(url);

  return NULL;
}

char *all_tests() {
  mu_suite_start();

  mu_run_test(test_generate_hash);
  mu_run_test(test_generate_timestamp);
  mu_run_test(test_generate_url);
  mu_run_test(test_marvel_build_url);

  return NULL;
}

RUN_TESTS(all_tests)