#include "uri.h"

#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

static bool parse_url(uri *u, str *url) {
  const char *pattern = "^(https?):\/\/([a-zA-Z0-9.]+)(\/?[^\?]*)?\?\?(.+)?";
  regex_t preg;
  const size_t possibleMatchCount = 5;
  regmatch_t pm[possibleMatchCount];
  const size_t bufferSize = 256;
  char errorBuffer[bufferSize];
  char buf[bufferSize];

  int res = regcomp(&preg, pattern, REG_ICASE | REG_EXTENDED);
  if (res != 0) {
    regerror(res, &preg, errorBuffer, bufferSize);
    throw_v_(true, "Could not compile regex: %s", errorBuffer);
  }

  res = regexec(&preg, str_data(url), possibleMatchCount, pm, 0);
  if (res != 0) {
    regerror(res, &preg, errorBuffer, bufferSize);
    throw_v_(true, "Could not exec regex: %s\n", errorBuffer);
  }

  regoff_t len = 0;
  str *tmp = NULL;
  /*for (size_t i = 1; i < possibleMatchCount; i++) {
    if (pm[i].rm_so == -1) {
      break;
    }

    len = pm[i].rm_eo - pm[i].rm_so;
    tmp = str_substr(url, pm[i].rm_so, len);
    debug_v_("Value: %s\n", str_data(tmp));
    str_free(tmp);
  } */

  // scheme
  if (pm[1].rm_so != -1) {
    len = pm[1].rm_eo - pm[1].rm_so;
    tmp = str_substr(url, pm[1].rm_so, len);
    u->scheme = tmp;
  }

  // host
  if (pm[2].rm_so != -1) {
    len = pm[2].rm_eo - pm[2].rm_so;
    tmp = str_substr(url, pm[2].rm_so, len);
    u->host = tmp;
  }

  // path
  if (pm[3].rm_so != -1) {
    len = pm[3].rm_eo - pm[3].rm_so;
    tmp = str_substr(url, pm[3].rm_so, len);
    u->path = tmp;
  }

  // query
  if (pm[4].rm_so != -1) {
    len = pm[4].rm_eo - pm[4].rm_so;
    tmp = str_substr(url, pm[4].rm_so, len);
    u->query = tmp;
  }

  regfree(&preg);

  return true;

error:
  return false;
}

uri *uri_create() {
  uri *u = malloc(sizeof(uri));
  throw_mem_(u);

  u->scheme = NULL;
  u->host = NULL;
  u->port = str_from(DEFAULT_PORT);
  u->path = NULL;
  u->query = NULL;
  u->hash = NULL;

  return u;

error:
  uri_free(u);
  return NULL;
}

bool uri_parse(uri *u, str *url) {
  bool res = parse_url(u, url);
  throw_v_(res == false, "Could not parse url: %s", str_data(url));

  return true;

error:
  return false;
}

void uri_destroy(uri *u) {
  if (u == NULL) return;

  str_free(u->scheme);
  str_free(u->host);
  str_free(u->port);
  str_free(u->path);
  str_free(u->query);
  str_free(u->hash);

  free(u);
}

/* char *uri_maker_build_req(uri_maker *urim) {
  char *hash = NULL;
  throw_(urim == NULL, "No URI maker provided");

  hash = "hash";  // generate_hash(urim);
  char *req = calloc(1024, 1);
  throw_mem_(req);

  sprintf(req,
          "GET %s/%s?ts=%s&apikey=%s&hash=%s&limit=2 HTTP/1.1\r\n"
          "Host: gateway.marvel.com\r\n"
          "User-Agent: curl/7.58.0\r\n"
          "Accept: *\r\n\r\n",
          str_data(urim->endpoint), str_data(urim->query), str_data(urim->ts),
          str_data(urim->pub_api_key), hash);

  free(hash);

  return req;

error:
  if (hash) free(hash);
  return NULL;
} */
