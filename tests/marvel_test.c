#include "../src/marvel.h"

#include <stdio.h>
#include <string.h>

#include "../src/env.h"
#include "minunit.h"

char *test_generate_hash() {
  int ret = env_init(".env.test");

  mu_assert(ret == true, "Something went wrong parsing the .env.test file");
  const char *endpoint = getenv("MARVEL_ENDPOINT");
  mu_assert(strcmp(endpoint, "http://gateway.marvel.com/v1/public") == 0,
            "Url does not match");

  const char *publicKey = getenv("MARVEL_PUBLIC_KEY");
  mu_assert(strcmp(publicKey, "public_key") == 0, "Public key does not match");
  const char *privateKey = getenv("MARVEL_PRIVATE_KEY");
  mu_assert(strcmp(privateKey, "private_key") == 0,
            "Private key does not match");

  str *ts = str_from("1234");
  char *hash = generate_hash(ts, privateKey, publicKey);

  // https://www.hashgenerator.de/
  // -> select "MD5" -> write into the input field "1234private_keypublic_key"
  const char *expected = "4a43372a739f1ca7d2a2055935c3cd0d";
  mu_assert_v(strcmp(hash, expected) == 0,
              "Hash should equal '%s', but is '%s'", expected, hash);

  str_free(ts);
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
  int ret = env_init(".env.test");
  const char *endpoint = getenv("MARVEL_ENDPOINT");
  const char *publicKey = getenv("MARVEL_PUBLIC_KEY");

  const char *expected =
      "http://gateway.marvel.com/v1/public/"
      "comics?ts=1234&apikey=public_key&hash="
      "hash123";

  const char *url =
      generate_url(endpoint, "comics", "1234", publicKey, "hash123");

  mu_assert_v(strcmp(url, expected) == 0,
              "Full url should be '%s', but is '%s'", expected, url);

  free(url);

  return NULL;
}

char *all_tests() {
  mu_suite_start();

  mu_run_test(test_generate_hash);
  mu_run_test(test_generate_timestamp);
  mu_run_test(test_generate_url);

  return NULL;
}

RUN_TESTS(all_tests)