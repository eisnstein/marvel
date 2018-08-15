#ifndef __str_h__
#define __str_h__

#include <stddef.h>
#include <stdint.h>

#define STR_INITIAL_SIZE (1024 * 50)
#define STR_EXPAND (1024 * 50)

#define str_length(A) ((A)->len)
#define str_data(A) ((A)->data)

typedef struct str {
    uint32_t size;
    uint32_t expand;
    uint32_t len;
    char *data;
} str;

str *str_create();
void str_append(str *s, const char *append, size_t len);
void str_destroy(str *s);

#endif