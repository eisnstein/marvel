#include "../src/str.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "minunit.h"

char *test_str_macros() {
  str *s = str_create();

  mu_assert_v(str_length(s) == 0, "String length should be 0, but is %u",
              str_length(s));
  mu_assert(str_data(s) != NULL, "String data is NULL");
  mu_assert(str_empty(s) == true, "String is not empty");

  str_destroy(&s);

  return NULL;
}

char *test_str_create() {
  str *s = str_create();

  mu_assert_v(s->len == 0, "String len should be 0, but is %u", s->len);
  mu_assert_v(s->size == STR_INITIAL_SIZE,
              "String initial size should be %d, but is %u", STR_INITIAL_SIZE,
              s->size);
  mu_assert_v(s->expand == STR_EXPAND, "String expand should be %d, but is %u",
              STR_EXPAND, s->expand);
  mu_assert(s->data != NULL, "String data is NULL");

  str_destroy(&s);

  return NULL;
}

char *test_str_destroy() {
  str *s = str_create();
  str_destroy(&s);

  mu_assert(s == NULL, "String should be freed");

  return NULL;
}

char *test_str_from() {
  const char *raw_string = "raw_string with length 25";
  size_t len = strlen(raw_string);

  str *s = str_from(raw_string);

  mu_assert(s != NULL, "String should not be null.");
  mu_assert_v(str_length(s) == len, "String should have length %u, but is %u",
              len, str_length(s));
  mu_assert(strcmp(str_data(s), raw_string) == 0, "Strings should be the same");

  const char *end = &str_data(s)[len];
  mu_assert_v(*end == 0, "String should be null terminated but value is '%s'",
              end);

  str_destroy(&s);

  return NULL;
}

char *test_str_duplicate() {
  const char *raw_string = "raw_string";
  size_t len = strlen(raw_string);
  str *s = str_from(raw_string);

  str *dup = str_duplicate(s);
  mu_assert(dup != NULL, "String should not be null.");
  mu_assert_v(str_length(dup) == len, "String should have length %u, but is %u",
              len, str_length(dup));
  mu_assert(strcmp(str_data(dup), raw_string) == 0,
            "Strings should be the same");

  str_destroy(&s);
  str_destroy(&dup);

  return NULL;
}

char *test_str_append() {
  char *initial_str = "Initial string";
  char *append_1 = " append 1";
  char *append_2 = " append 2";

  size_t initial_str_len = strlen(initial_str);
  size_t append_1_len = strlen(append_1);
  size_t append_2_len = strlen(append_2);

  str *s = str_create();

  str_append(s, initial_str, initial_str_len);
  str_append(s, append_1, append_1_len);

  size_t sum_len = initial_str_len + append_1_len;

  mu_assert_v(str_length(s) == sum_len, "String length should be %u, but is %u",
              sum_len, str_length(s));
  mu_assert_v(s->size == STR_INITIAL_SIZE,
              "String size should be %d, but is %u", STR_INITIAL_SIZE, s->size);

  sum_len = sum_len + append_2_len;
  str_append(s, append_2, append_2_len);

  mu_assert_v(str_length(s) == sum_len, "String length should be %u, but is %u",
              sum_len, str_length(s));
  // Because of realloc, size should be expanded
  mu_assert_v(s->size == STR_INITIAL_SIZE * 2,
              "String size should be %d, but is %u", STR_INITIAL_SIZE * 2,
              s->size);

  str_destroy(&s);

  return NULL;
}

char *all_tests() {
  mu_suite_start();

  mu_run_test(test_str_macros);
  mu_run_test(test_str_create);
  mu_run_test(test_str_destroy);
  mu_run_test(test_str_from);
  mu_run_test(test_str_append);

  return NULL;
}

RUN_TESTS(all_tests)