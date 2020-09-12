#undef NDEBUG
#ifndef _minunit_h
#define _minunit_h

#include "../src/util.h"
#include <stdio.h>
#include <stdlib.h>

#define mu_suite_start() char *message = NULL

#define mu_assert(test, message)                                               \
  do {                                                                         \
    if (!(test)) {                                                             \
      error_(message);                                                         \
      return message;                                                          \
    }                                                                          \
  } while (0)

#define mu_assert_v(test, message, ...)                                        \
  do {                                                                         \
    if (!(test)) {                                                             \
      error_v_(message, ##__VA_ARGS__);                                        \
      return message;                                                          \
    }                                                                          \
  } while (0)

#define mu_run_test(test)                                                      \
  do {                                                                         \
    debug_v_("\n-----%s", " " #test);                                          \
    message = test();                                                          \
    tests_run++;                                                               \
    if (message)                                                               \
      return message;                                                          \
  } while (0)

#define RUN_TESTS(name)                                                        \
  int main(int argc, char *argv[]) {                                           \
    argc = 1;                                                                  \
    debug_v_("----- RUNNING: %s", argv[0]);                                    \
    printf("----\nRUNNING: %s\n", argv[0]);                                    \
    char *result = name();                                                     \
    if (result != 0) {                                                         \
      printf("FAILED: %s\n", result);                                          \
    } else {                                                                   \
      printf("ALL TESTS PASSED\n");                                            \
    }                                                                          \
    printf("Tests run: %d\n", tests_run);                                      \
    exit(result != 0);                                                         \
  }

int tests_run;

#endif