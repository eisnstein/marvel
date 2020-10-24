#undef NDEBUG
#ifndef _minunit_h
#define _minunit_h

#include <stdio.h>
#include <stdlib.h>

#include "../src/util.h"

#define mu_suite_start() char *message = NULL

#define mu_assert(test, message) \
  do {                           \
    if (!(test)) {               \
      error_(message);           \
      return message;            \
    }                            \
  } while (0)

#define mu_assert_v(test, message, ...) \
  do {                                  \
    if (!(test)) {                      \
      error_v_(message, ##__VA_ARGS__); \
      return message;                   \
    }                                   \
  } while (0)

#define mu_run_test(test)                \
  do {                                   \
    printf("------\n");                  \
    printf("Running Test: %s\n", #test); \
    message = test();                    \
    tests_run++;                         \
    if (message) return message;         \
  } while (0)

#define RUN_TESTS(name)                   \
  int main(int argc, char *argv[]) {      \
    argc = 1;                             \
    printf("------\n");                   \
    printf("TEST SUIT: %s\n", argv[0]);   \
    char *result = name();                \
    if (result != 0) {                    \
      printf("Tests Failed\n");           \
    } else {                              \
      printf("------\n");                 \
      printf("All Tests Passed\n");       \
    }                                     \
    printf("Tests run: %d\n", tests_run); \
    exit(result != 0);                    \
  }

int tests_run = 0;

#endif