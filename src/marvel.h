#ifndef __marvel_h__
#define __marvel_h__

#include <stdbool.h>

#include "http.h"
#include "str.h"
#include "uri.h"

#define MARVEL_BASE_ENDPOINT "MARVEL_BASE_ENDPOINT"
#define MARVEL_PATH "MARVEL_PATH"
#define MARVEL_PUBLIC_KEY "MARVEL_PUBLIC_KEY"
#define MARVEL_PRIVATE_KEY "MARVEL_PRIVATE_KEY"

#define marvel_free(M) \
  do {                 \
    marvel_destroy(M); \
    M = NULL;          \
  } while (0);

typedef struct marvel {
  http *client;
  uri_maker *uri_maker;
} marvel;

marvel *marvel_create();
bool marvel_request(marvel *self, str *query);
void marvel_destroy(marvel *self);

#endif