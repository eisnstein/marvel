#ifndef __marvel_h__
#define __marvel_h__

/*** includes ***/
#include "http.h"
#include "str.h"
#include "uri.h"

/*** defines ***/
#define MARVEL_BASE_URL "MARVEL_BASE_URL"
#define MARVEL_BASE_ENDPOINT "MARVEL_BASE_ENDPOINT"

#define MARVEL_PUBLIC_KEY "MARVEL_PUBLIC_KEY"
#define MARVEL_PRIVATE_KEY "MARVEL_PRIVATE_KEY"

/*** typedefs ***/
typedef struct marvel marvel;

/*** structs ***/
struct marvel {
  http *client;
  uri_maker *uri_maker;
};

/*** function declarations ***/
marvel *marvel_create();
int marvel_request(marvel *self, const str *query);
void marvel_destroy(marvel *self);

#endif