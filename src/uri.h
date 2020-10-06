#ifndef __url_h__
#define __url_h__

#include "str.h"

typedef struct uri_maker {
  str *endpoint;
  str *query;
  str *ts;
  str *pr_api_key;
  str *pub_api_key;
} uri_maker;

extern uri_maker *uri_maker_create();
extern void uri_maker_destroy(uri_maker **self);
char *uri_maker_build_req(uri_maker *urim);

#endif