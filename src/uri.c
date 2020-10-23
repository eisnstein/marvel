#include "uri.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "marvel.h"
#include "util.h"

char *uri_maker_build_req(uri_maker *urim) {
  char *hash = NULL;
  throw_(urim == NULL, "No URI maker provided");

  hash = "hash";  // generate_hash(urim);
  char *req = calloc(1024, 1);
  throw_mem_(req);

  sprintf(req,
          "GET %s/%s?ts=%s&apikey=%s&hash=%s&limit=2 HTTP/1.1\r\n"
          "Host: gateway.marvel.com\r\n"
          "User-Agent: curl/7.58.0\r\n"
          "Accept: */*\r\n\r\n",
          str_data(urim->endpoint), str_data(urim->query), str_data(urim->ts),
          str_data(urim->pub_api_key), hash);

  free(hash);

  return req;

error:
  if (hash) free(hash);
  return NULL;
}
