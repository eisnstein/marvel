#ifndef __util_h__
#define __util_h__

#include <errno.h>
#include <stdio.h>
#include <string.h>

#ifdef NDEBUG
#define debug_(M)
#define debug_v_(M, ...)
#else
#define debug_(M)                                                    \
  do {                                                               \
    fprintf(stderr, "[DEBUG] %s:%d:%s: " M "\n", __FILE__, __LINE__, \
            __func__);                                               \
  } while (0)

#define debug_v_(M, ...)                                                       \
  do {                                                                         \
    fprintf(stderr, "[DEBUG] %s:%d:%s: " M "\n", __FILE__, __LINE__, __func__, \
            ##__VA_ARGS__);                                                    \
  } while (0)
#endif

#define info_(M)                                                               \
  do {                                                                         \
    fprintf(stdout, "[INFO] %s:%d:%s: " M "\n", __FILE__, __LINE__, __func__); \
  } while (0)

#define info_v_(M, ...)                                                       \
  do {                                                                        \
    fprintf(stdout, "[INFO] %s:%d:%s: " M "\n", __FILE__, __LINE__, __func__, \
            ##__VA_ARGS__);                                                   \
  } while (0)

#define warning_(M)                                                    \
  do {                                                                 \
    fprintf(stdout, "[WARNING] %s:%d:%s: " M "\n", __FILE__, __LINE__, \
            __func__);                                                 \
  } while (0)

#define warning_v_(M, ...)                                             \
  do {                                                                 \
    fprintf(stdout, "[WARNING] %s:%d:%s: " M "\n", __FILE__, __LINE__, \
            __func__, ##__VA_ARGS__);                                  \
  } while (0)

#define error_(M)                                                    \
  do {                                                               \
    fprintf(stderr, "[ERROR] %s:%d:%s: " M "\n", __FILE__, __LINE__, \
            __func__);                                               \
  } while (0)

#define error_v_(M, ...)                                                       \
  do {                                                                         \
    fprintf(stderr, "[ERROR] %s:%d:%s: " M "\n", __FILE__, __LINE__, __func__, \
            ##__VA_ARGS__);                                                    \
  } while (0)

#define clean_errno() (errno == 0 ? "None" : strerror(errno))

#define die_(A, M)                                                         \
  do {                                                                     \
    if ((A)) {                                                             \
      fprintf(stderr, "[ERROR] %s:%d:%s: %s: " M "\n", __FILE__, __LINE__, \
              __func__, clean_errno());                                    \
      exit(1);                                                             \
    }                                                                      \
  } while (0)

#define die_v_(A, M, ...)                                                  \
  do {                                                                     \
    if ((A)) {                                                             \
      fprintf(stderr, "[ERROR] %s:%d:%s: %s: " M "\n", __FILE__, __LINE__, \
              __func__, clean_errno(), ##__VA_ARGS__);                     \
      exit(1);                                                             \
    }                                                                      \
  } while (0)

#define throw_(A, M)                                                           \
  do {                                                                         \
    if ((A)) {                                                                 \
      fprintf(stderr, "[EXCEPTION] %s:%d:%s: %s: " M "\n", __FILE__, __LINE__, \
              __func__, clean_errno());                                        \
      errno = 0;                                                               \
      goto error;                                                              \
    }                                                                          \
  } while (0)

#define throw_v_(A, M, ...)                                                    \
  do {                                                                         \
    if ((A)) {                                                                 \
      fprintf(stderr, "[EXCEPTION] %s:%d:%s: %s: " M "\n", __FILE__, __LINE__, \
              __func__, clean_errno(), ##__VA_ARGS__);                         \
      errno = 0;                                                               \
      goto error;                                                              \
    }                                                                          \
  } while (0)

#define throw_mem_(A)                                 \
  do {                                                \
    throw_((A) == NULL, "Could not allocate memory"); \
  } while (0)

#endif