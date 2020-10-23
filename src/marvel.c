#include "marvel.h"

#include <openssl/evp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "http.h"
#include "util.h"

/**
 * Generates the md5 hash from the ts + privateKey + publicKey.
 * From marvel documentation: md5(ts + privateKey + publicKey)
 *
 * @param str* ts                   timestamp
 * @param char* privateKey    Marvel API private key
 * @param char* publicKey     Marvel API public key
 *
 * @return char*                    pointer to hash value
 */
char *generate_hash(const str *ts, const char *privateKey,
                    const char *publicKey) {
  EVP_MD_CTX *mdctx;
  const EVP_MD *md;
  unsigned char md_value[EVP_MAX_MD_SIZE];
  unsigned int md_len, i;
  str *hash_payload = NULL;

  char *hash = calloc(1, EVP_MAX_MD_SIZE + 1);
  throw_mem_(hash);

  hash_payload = str_create();
  throw_mem_(hash_payload);

  str_append(hash_payload, str_data(ts));
  str_append(hash_payload, privateKey);
  str_append(hash_payload, publicKey);

  md = EVP_get_digestbyname("MD5");
  mdctx = EVP_MD_CTX_new();
  EVP_DigestInit_ex(mdctx, md, NULL);
  EVP_DigestUpdate(mdctx, str_data(hash_payload), str_length(hash_payload));
  EVP_DigestFinal_ex(mdctx, md_value, &md_len);
  EVP_MD_CTX_free(mdctx);

  for (i = 0; i < md_len; i++) {
    sprintf(&hash[i * 2], "%02x", md_value[i]);
  }

  str_free(hash_payload);

  return hash;

error:
  if (hash) free(hash);
  return NULL;
}

/**
 * Generate a timestamp and retrun it as string.
 *
 * @return str*       pointer to timestamp string
 */
char *generate_timestamp() {
  char *tmp = malloc(32);
  time_t timeInSeconds = time(NULL);
  sprintf(tmp, "%d", timeInSeconds);
  return tmp;
}

char *generate_url(const char *endpoint, const char *query, const char *ts,
                   const char *publicKey, const char *hash) {
  char *tmp = malloc(256);

  sprintf(tmp, "%s/%s?ts=%s&apikey=%s&hash=%s", endpoint, query, ts, publicKey,
          hash);

  return tmp;
}

/**
 * Build Marvel request url
 *
 * @param str* query      Marvel query
 *
 * @return str* url       pointer to fully built Marvel request url
 */
str *marvel_build_url(str *query) {
  const char *endpoint = getenv(MARVEL_ENDPOINT);
  throw_(endpoint == NULL, "Could not get base url from env");

  const char *publicKey = getenv(MARVEL_PUBLIC_KEY);
  throw_(publicKey == NULL, "Could not get public key from env");

  const char *privateKey = getenv(MARVEL_PRIVATE_KEY);
  throw_(privateKey == NULL, "Could not get private key from env");

  const char *ts = generate_timestamp();
  const char *hash = generate_hash(ts, privateKey, publicKey);
  const char *tmp =
      generate_url(endpoint, str_data(query), ts, publicKey, hash);

  debug_v_("url: %s", tmp);
  str *url = str_from(tmp);
  throw_(url == NULL, "Could not create url from tmp");

  free(ts);
  free(hash);
  free(tmp);

  return url;

error:
  if (ts) free(ts);
  if (hash) free(hash);
  if (tmp) free(tmp);
  return NULL;
}