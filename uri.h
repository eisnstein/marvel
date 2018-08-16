#ifndef __url_h__
#define __url_h__

#include "str.h"

typedef struct uri_maker {
    const str *pr_api_key;
    const str *pub_api_key;
    hasher_func *hasher;
} uri_maker;

typedef char *(*hasher_func)(uri_maker *self, const str *ts);

uri_maker *uri_maker_create(const str *pr_api_key, const str *pub_api_key);
void uri_maker_destroy(uri_maker *self);

#endif