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
typedef int (*make_request_func)(marvel *self, const str *query);

/*** structs ***/
struct marvel {
    http *client;
    uri_maker *uri_maker;
    make_request_func make_request;
};

/*** function declarations ***/
extern marvel *marvel_create();
static int make_request(marvel *self, const str *query);
extern void marvel_destroy(marvel *self);

#endif