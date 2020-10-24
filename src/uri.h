#ifndef __url_h__
#define __url_h__

#include "str.h"

#define DEFAULT_PORT "80"

#define uri_free(U) \
  do {              \
    uri_destroy(U); \
    U = NULL;       \
  } while (0);

typedef struct uri {
  str *scheme;
  str *host;
  str *port;
  str *path;
  str *query;
  str *hash;
} uri;

extern uri *uri_create();
extern bool uri_parse(uri *u, str *url);
extern void uri_destroy(uri *u);

#endif