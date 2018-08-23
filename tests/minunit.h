#ifndef __minunit_h__
#define __minunit_h__

#include "util.h"

#define mu_assert(test, message, ...) do { \
    if (!(test)) { \
        error_(message, ##__VA_ARGS__); \
        return message; \
    } \
} while (0)

#define mu_run_test(test) do { \
    char *message = test(); \
    tests_run++; \
    if (message) { \
        return message; \
    } \
} while (0)

extern int tests_run;

#endif