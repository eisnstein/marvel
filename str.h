#ifndef __str_h__
#define __str_h__

#include <stddef.h>
#include <stdint.h>

#define STR_INITIAL_SIZE (1024 * 50)
#define STR_EXPAND (1024 * 50)

#define str_length(S) ((S)->len)
#define str_data(S) ((S)->data)
#define str_strip_nl(S) do { \
    size_t len = str_length(S); \
    if (len > 0 && (str_data(S)[(len - 1)] == '\n')) { \
        (str_data(S)[len - 1]) = '\0'; \
    } \
} while (0)

typedef struct str {
    uint32_t size;
    uint32_t expand;
    size_t len;
    char *data;
} str;

str *str_create();
str *str_from(const char *s);
void str_append(str *s, const char *append, size_t len);
void str_destroy(str *s);

#endif