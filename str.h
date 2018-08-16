#ifndef __str_h__
#define __str_h__

#include <stddef.h>
#include <stdint.h>

#define STR_INITIAL_SIZE (1024 * 50)
#define STR_EXPAND (1024 * 50)

#define str_length(S) ((S)->length)
#define str_data(S) ((S)->data)
#define str_strip_nl(S) { \
    size_t length = str_length(S); \
    if (length > 0 && (str_data(S)[(length - 1)] == '\n')) { \
        (str_data(S)[length - 1]) = '\0'; \
    } \
}

typedef struct str {
    uint32_t size;
    uint32_t expand;
    size_t length;
    char *data;
} str;

str *str_create();
str *str_from(const char *s);
void str_append(str *s, const char *append, size_t length);
void str_destroy(str *s);

#endif