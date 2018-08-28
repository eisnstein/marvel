#ifndef __str_h__
#define __str_h__

#include <stddef.h>
#include <stdint.h>

#define STR_INITIAL_SIZE (1024 * 1)
#define STR_EXPAND (1024 * 1)

// String related
#define str_length(S) ((S)->len)
#define str_data(S) ((S)->data)
#define str_strip_nl(S) do { \
    size_t len = str_length(S); \
    if (len > 0 && (str_data(S)[(len - 1)] == '\n')) { \
        (str_data(S)[len - 1]) = '\0'; \
    } \
} while (0)

// String list related
#define strlist_length(SL) ((SL)->size)
#define strlist_empty(SL) ((SL)->size == 0)

typedef struct str {
    uint32_t size;
    uint32_t expand;
    size_t len;
    char *data;
} str;

typedef struct strlistnode {
    struct strlistnode *next;
    struct strlistnode *prev;
    str *value;
} strlistnode;

typedef struct strlist {
    size_t size;
    strlistnode *head;
    strlistnode *tail;
} strlist;

str *str_create();
str *str_from(const char *s);
void str_append(str *s, const char *append, size_t len);
strlist *str_split(str *s, const char *delimiter);
void str_put_into(str *s, const char *put, size_t len);
void str_destroy(str *s);

strlist *strlist_create();
void *strlist_push(strlist *sl, str *value);
str *strlist_pop(strlist *sl);
str *strlist_at(strlist *sl, size_t index);
void strlist_destroy(strlist *sl);

#endif