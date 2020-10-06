#include "uri.h"

#include <openssl/evp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "marvel.h"
#include "util.h"

/**
 * Generates the payload which will be md5 hashed.
 * From marvel documentation: md5(ts + privateKey + publicKey)
 *
 * @param uri_maker *urim     URI maker
 *
 * @return str | NULL
 */
static inline str *generate_payload(uri_maker *urim) {
  throw_(urim == NULL, "No URI maker provided");

  str *out = str_create();
  throw_mem_(out);

  str_append(out, str_data(urim->ts));
  str_append(out, str_data(urim->pr_api_key));
  str_append(out, str_data(urim->pub_api_key));

  return out;

error:
  return NULL;
}

/**
 * Generates the md5 hash from the ts + privateKey + publicKey.
 *
 * @param uri_maker *self   pointer to uri_maker
 *
 * @return *char
 */
static char *generate_hash(uri_maker *urim) {
  char *hash = NULL;
  throw_(urim == NULL, "No URI make provided");

  EVP_MD_CTX *mdctx;
  const EVP_MD *md;
  unsigned char md_value[EVP_MAX_MD_SIZE];
  unsigned int md_len, i;
  str *hash_payload = NULL;

  hash = calloc(1, EVP_MAX_MD_SIZE + 1);
  throw_mem_(hash);

  hash_payload = generate_payload(urim);
  throw_(hash_payload == NULL, "Could not generate hash payload.");

  md = EVP_get_digestbyname("MD5");
  mdctx = EVP_MD_CTX_new();
  EVP_DigestInit_ex(mdctx, md, NULL);
  EVP_DigestUpdate(mdctx, str_data(hash_payload), str_length(hash_payload));
  EVP_DigestFinal_ex(mdctx, md_value, &md_len);
  EVP_MD_CTX_free(mdctx);

  for (i = 0; i < md_len; i++) {
    sprintf(&hash[i * 2], "%02x", md_value[i]);
  }

  str_destroy(&hash_payload);

  return hash;

error:
  if (hash) free(hash);
  return NULL;
}

char *uri_maker_build_req(uri_maker *urim) {
  char *hash = NULL;
  throw_(urim == NULL, "No URI maker provided");

  hash = generate_hash(urim);
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

uri_maker *uri_maker_create() {
  uri_maker *urm = malloc(sizeof(uri_maker));
  throw_mem_(urm);

  str *endpoint = str_from(getenv(MARVEL_BASE_ENDPOINT));
  throw_(endpoint == NULL, "Could not get base endpoint from env.");

  str *pr_api_key = str_from(getenv(MARVEL_PRIVATE_KEY));
  throw_(pr_api_key == NULL, "Could not get private key from env.");

  str *pub_api_key = str_from(getenv(MARVEL_PUBLIC_KEY));
  throw_(pub_api_key == NULL, "Could not get public key from env.");

  urm->ts = str_from("12345");
  urm->endpoint = endpoint;
  urm->query = NULL;
  urm->pr_api_key = pr_api_key;
  urm->pub_api_key = pub_api_key;

  return urm;

error:
  return NULL;
}

void uri_maker_destroy(uri_maker **self) {
  if (*self == NULL) {
    return;
  }

  str_destroy(&(*self)->ts);
  str_destroy(&(*self)->endpoint);
  str_destroy(&(*self)->query);
  str_destroy(&(*self)->pr_api_key);
  str_destroy(&(*self)->pub_api_key);

  if (*self) {
    free(*self);
    *self = NULL;
  }
}