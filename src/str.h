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
#define str_strip_nl(S)                                \
  do {                                                 \
    size_t len = str_length(S);                        \
    if (len > 0 && (str_data(S)[(len - 1)] == '\n')) { \
      (str_data(S)[len - 1]) = '\0';                   \
    }                                                  \
  } while (0)

// String List Macros
#define strlist_size(SL) ((SL)->size)
#define strlist_empty(SL) ((SL)->size == 0)

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
str *str_from(const char *s);
str *str_duplicate(const str *s);
void str_append(str *s, const char *append, size_t len);
strlist *str_split(str *s, const char *delimiter);
void str_put_into(str *s, const char *put, size_t len);
void str_destroy(str **s);

strlist *strlist_create();
bool strlist_push(strlist *sl, str *value);
str *strlist_pop(strlist *sl);
str *strlist_at(strlist *sl, size_t index);
void strlist_destroy(strlist **sl);

#endif