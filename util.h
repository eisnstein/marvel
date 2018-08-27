#ifndef __util_h__
#define __util_h__

#include <errno.h>
#include <stdio.h>
#include <string.h>

#define debug_(M, ...) do { \
    fprintf(stdout, "[DEBUG] %s:%d:%s: " M "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__); \
} while (0)

#define info_(M, ...) do { \
    fprintf(stdout, "[INFO] %s:%d:%s: " M "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__); \
} while (0)

#define warning_(M, ...) do { \
    fprintf(stdout, "[WARNING] %s:%d:%s: " M "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__); \
} while (0)

#define error_(M, ...) do { \
    fprintf(stderr, "[ERROR] %s:%d:%s: " M "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__); \
} while (0)

#define die_(A, M) do { \
    if((A)) { \
        fprintf(stderr, "[ERROR] %s:%d:%s: %s: " M "\n", __FILE__, __LINE__, __func__, strerror(errno)); \
        exit(1); \
    } \
} while (0)

#define throw_(A, M) do { \
    if ((A)) { \
        fprintf(stderr, "[WARNING] %s:%d:%s: %s: " M "\n", __FILE__, __LINE__, __func__, strerror(errno)); \
        errno = 0; \
        goto error; \
    } \
} while (0)

#define throw_mem_(A) do { \
    throw_((A) == NULL, "Could not allocate memory"); \
} while (0)

#endif