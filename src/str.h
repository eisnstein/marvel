#ifndef __str_h__
#define __str_h__

#include <stdbool.h>
#include <stddef.h>

#define STR_INITIAL_SIZE (32 * 1)
#define STR_EXPAND (32 * 1)

// move to next line
#define mtnl(P)            \
  do {                     \
    while (*(P) != '\n') { \
      (P)++;               \
    }                      \
    (P)++;                 \
  } while (0)

// String Macros
#define str_length(S) ((S)->len)
#define str_empty(S) ((S)->len == 0)
#define str_data(S) ((S)->data)
#define str_size(S) ((S)->size)
#define str_strip_nl(S)                                \
  do {                                                 \
    size_t len = str_length(S);                        \
    if (len > 0 && (str_data(S)[(len - 1)] == '\n')) { \
      (str_data(S)[len - 1]) = '\0';                   \
    }                                                  \
  } while (0)
#define str_free(S) \
  do {              \
    str_destroy(S); \
    S = NULL;       \
  } while (0)

// String List Macros
#define strlist_size(SL) ((SL)->size)
#define strlist_empty(SL) ((SL)->size == 0)
#define strlist_free(SL) \
  do {                   \
    strlist_destroy(SL); \
    SL = NULL;           \
  } while (0)

typedef struct str {
  size_t size;
  size_t expand;
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
str *str_create_v(size_t initialSize, size_t expandSize);
str *str_from(const char *s);
str *str_duplicate(const str *s);
bool str_append(str *s, const char *append);
strlist *str_split(str *s, const char *delimiter);
bool str_put_into(str *s, const char *put);
str *str_substr(str *s, size_t pos, size_t length);
bool str_starts_with(str *s, const char *search);
void str_destroy(str *s);

strlist *strlist_create();
bool strlist_push(strlist *sl, str *value);
str *strlist_pop(strlist *sl);
str *strlist_at(strlist *sl, size_t index);
void strlist_destroy(strlist *sl);

#endif