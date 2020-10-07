#include "../src/str.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "minunit.h"

char *test_str_macros() {
  str *s = str_create();

  mu_assert_v(str_length(s) == 0, "String length should be 0, but is %zu",
              str_length(s));
  mu_assert(str_data(s) != NULL, "String data is NULL");
  mu_assert(str_empty(s) == true, "String is not empty");

  str_free(s);
  mu_assert(s == NULL, "String should be NULL");

  return NULL;
}

char *test_strlist_macros() {
  strlist *sl = strlist_create();

  mu_assert_v(strlist_size(sl) == 0, "Stringlist size should be 0, but is %zu",
              strlist_size(sl));
  mu_assert(strlist_empty(sl) == true, "Stringlist should be empty");

  strlist_free(sl);
  mu_assert(sl == NULL, "Stringlist should be NULL");

  return NULL;
}

char *test_str_create() {
  str *s = str_create();

  mu_assert_v(s->len == 0, "String len should be 0, but is %zu", s->len);
  mu_assert_v(s->size == STR_INITIAL_SIZE,
              "String initial size should be %d, but is %zu", STR_INITIAL_SIZE,
              s->size);
  mu_assert_v(s->expand == STR_EXPAND, "String expand should be %d, but is %zu",
              STR_EXPAND, s->expand);
  mu_assert(s->data != NULL, "String data is NULL");

  str_free(s);

  return NULL;
}

char *test_strlist_create() {
  strlist *sl = strlist_create();

  mu_assert_v(sl->size == 0, "Stringlist size should be 0, but is %zu",
              sl->size);
  mu_assert(sl->head == NULL, "Stringlist head pointer should be NULL");
  mu_assert(sl->tail == NULL, "Stringlist head pointer should be NULL");

  strlist_free(sl);
  mu_assert(sl == NULL, "Stringlist should be NULL");

  return NULL;
}

char *test_strlist_push() {
  strlist *sl = strlist_create();

  str *value1 = str_from("value1");
  str *value2 = str_from("value2");

  mu_assert(strlist_push(sl, value1) == true,
            "Value1 could not be added to list");
  mu_assert(strlist_push(sl, value2) == true,
            "Value2 could not be added to list");

  mu_assert_v(strlist_size(sl) == 2,
              "Stringlist should have size of 2, but is %zu", strlist_size(sl));

  str *value1FromList = strlist_at(sl, 0);
  str *value2FromList = strlist_at(sl, 1);

  mu_assert_v(strcmp(str_data(value1FromList), "value1") == 0,
              "Value 1 from list should be 'value1', but is %s",
              str_data(value1FromList));
  mu_assert_v(strcmp(str_data(value2FromList), "value2") == 0,
              "Value 2 from list should be 'value2', but is %s",
              str_data(value2FromList));

  strlist_free(sl);
  str_free(value1);
  str_free(value2);

  return NULL;
}

char *test_strlist_at() {
  strlist *sl = strlist_create();

  str *value1 = str_from("value1");
  str *value2 = str_from("value2");

  strlist_push(sl, value1);
  strlist_push(sl, value2);

  str *value1FromList = strlist_at(sl, 0);
  str *value2FromList = strlist_at(sl, 1);

  mu_assert_v(strcmp(str_data(value1FromList), "value1") == 0,
              "Value 1 from list should be 'value1', but is %s",
              str_data(value1FromList));
  mu_assert_v(strcmp(str_data(value2FromList), "value2") == 0,
              "Value 2 from list should be 'value2', but is %s",
              str_data(value2FromList));

  strlist_free(sl);
  str_free(value1);
  str_free(value2);

  return NULL;
}

char *test_str_destroy() {
  str *s = str_create();
  str_free(s);

  mu_assert(s == NULL, "String should be freed");

  return NULL;
}

char *test_strlist_destroy() {
  strlist *sl = strlist_create();
  strlist_free(sl);

  mu_assert(sl == NULL, "Stringlist should be freed");

  sl = strlist_create();
  str *value1 = str_from("value1");
  str *value2 = str_from("value2");

  strlist_push(sl, value1);
  strlist_push(sl, value2);

  mu_assert_v(strlist_size(sl) == 2,
              "Stringlist should have size of 2, but is %zu", strlist_size(sl));

  strlist_free(sl);
  mu_assert(sl == NULL, "Stringlist should be freed");

  str_free(value1);
  str_free(value2);

  return NULL;
}

char *test_str_from() {
  const char *raw_string = "raw_string with length 25";
  size_t len = strlen(raw_string);

  str *s = str_from(raw_string);

  mu_assert(s != NULL, "String should not be null.");
  mu_assert_v(str_length(s) == len, "String should have length %zu, but is %zu",
              len, str_length(s));
  mu_assert(strcmp(str_data(s), raw_string) == 0, "Strings should be the same");

  const char *end = &str_data(s)[len];
  mu_assert_v(*end == 0, "String should be null terminated but value is '%s'",
              end);

  str_free(s);

  str *s2 = str_from(NULL);
  mu_assert(s2 == NULL, "String should be null");

  s2 = str_from("");
  mu_assert(s2 != NULL, "String should not be null");

  str_free(s2);

  return NULL;
}

char *test_str_duplicate() {
  const char *raw_string = "raw_string";
  size_t len = strlen(raw_string);
  str *s = str_from(raw_string);

  str *dup = str_duplicate(s);
  mu_assert(dup != NULL, "String should not be null.");
  mu_assert_v(str_length(dup) == len,
              "String should have length %zu, but is %zu", len,
              str_length(dup));
  mu_assert(strcmp(str_data(dup), raw_string) == 0,
            "Strings should be the same");
  mu_assert(s->data != dup->data, "Strings should not point to the same data");

  str_free(s);
  str_free(dup);

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
  bool r = false;

  r = str_append(s, initial_str);
  mu_assert(r == true, "Result should be 'true', but got 'false'");
  r = str_append(s, append_1);
  mu_assert(r == true, "Result should be 'true', but got 'false'");

  size_t sum_len = initial_str_len + append_1_len;

  mu_assert_v(str_length(s) == sum_len,
              "String length should be %zu, but is %zu", sum_len,
              str_length(s));
  mu_assert_v(s->size == STR_INITIAL_SIZE,
              "String size should be %d, but is %zu", STR_INITIAL_SIZE,
              s->size);

  sum_len = sum_len + append_2_len;
  r = str_append(s, append_2);
  mu_assert(r == true, "Result should be true, but got false");

  mu_assert_v(str_length(s) == sum_len,
              "String length should be %zu, but is %zu", sum_len,
              str_length(s));
  // Because of realloc, size should be expanded
  mu_assert_v(s->size == STR_INITIAL_SIZE + STR_EXPAND,
              "String size should be %d, but is %zu",
              STR_INITIAL_SIZE + STR_EXPAND, s->size);

  str_free(s);

  str *s2 = str_create();
  r = str_append(s2, NULL);
  mu_assert(r == false, "Result should be 'false', but is 'true'");

  str_free(s2);

  return NULL;
}

char *test_str_split() {
  str *s = str_from("key=value");
  strlist *sl = str_split(s, "=");

  mu_assert_v(strlist_size(sl) == 2,
              "Stringlist should have size of 2, but is %zu", strlist_size(sl));
  const str *key = strlist_at(sl, 0);
  const str *value = strlist_at(sl, 1);
  mu_assert_v(strcmp(str_data(key), "key") == 0,
              "Key should be 'key', but is '%s'", str_data(key));
  mu_assert_v(strcmp(str_data(value), "value") == 0,
              "Key should be 'value', but is '%s'", str_data(value));

  str_free(s);
  strlist_free(sl);

  return NULL;
}

char *test_str_put_into() {
  str *s = str_from("some string");

  bool res = str_put_into(s, "another string");
  mu_assert(res == true, "Should be true, but is false");
  mu_assert(strcmp(str_data(s), "another string") == 0, "String not same");

  res = str_put_into(s, "");
  mu_assert(res == true, "Should be true, but is false");
  mu_assert(strcmp(str_data(s), "") == 0, "String not same");

  str_free(s);

  return NULL;
}

char *all_tests() {
  mu_suite_start();

  mu_run_test(test_str_macros);
  mu_run_test(test_strlist_macros);
  mu_run_test(test_str_create);
  mu_run_test(test_strlist_create);
  mu_run_test(test_str_destroy);
  mu_run_test(test_strlist_destroy);

  mu_run_test(test_str_from);
  mu_run_test(test_str_duplicate);
  mu_run_test(test_str_append);
  mu_run_test(test_str_split);
  mu_run_test(test_str_put_into);

  mu_run_test(test_strlist_push);
  mu_run_test(test_strlist_at);

  return NULL;
}

RUN_TESTS(all_tests)