#include "marvel.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "http.h"
#include "util.h"

str *marvel_build_url(str *query) {
  str *endpoint = str_from(getenv(MARVEL_BASE_ENDPOINT));
  throw_(endpoint == NULL, "Could not get base url from env");

  str *path = str_from(getenv(MARVEL_PATH));
  throw_(path == NULL, "Could not get base url from env");

  str *apikey = str_from(getenv(MARVEL_PUBLIC_KEY));
  throw_(apikey == NULL, "Could not get public key from env");

  char tmp[256] = {0};
  sprintf(tmp, "http://%s%s/%s?ts=%s&apikey=%s&hash=%s", str_data(endpoint),
          str_data(path), std_data(query), "12324", str_data(apikey), "hash");
  debug_v_("url: %s", tmp);
  str *url = str_from(tmp);

  return url;

error:
  return NULL;
}