#include <openssl/evp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "marvel.h"
#include "util.h"
#include "uri.h"

uri_maker *uri_maker_create(const str *pr_api_key, const str *pub_api_key)
{
    uri_maker *urm = malloc(sizeof(uri_maker));
    verify_memory(urm);

    urm->pr_api_key = pr_api_key;
    urm->pub_api_key = pub_api_key;
    urm->hasher = get_hash;

    return urm;
}

static inline str *generate_payload(uri_maker *self, const str *ts)
{
    /*size_t ts_len = str_length(ts);
    size_t pr_len = str_length(self->pr_api_key);
    size_t pub_len = str_length(self->pub_api_key);

    size_t len = ts_len + pr_len + pub_len;
    char *out = calloc(len + 1, sizeof(char));*/
    str *out = str_create();

    str_append(out, str_data(ts), str_length(ts));
    str_append(out, str_data(self->pr_api_key), str_length(self->pr_api_key));
    str_append(out, str_data(self->pub_api_key), str_length(self->pub_api_key));

    /*memcpy(out, ts, ts_len);
    memcpy(&out[ts_len], str_data(pr_api_key), pr_len);
    memcpy(&out[ts_len + pr_len], str_data(pub_api_key), pub_len);

    out[len] = '\0';*/

    return out;   
}

static char *get_hash(uri_maker *self, const char *ts)
{
    EVP_MD_CTX *mdctx;
    const EVP_MD *md;
    unsigned char md_value[EVP_MAX_MD_SIZE];
    unsigned int md_len, i;
    str *hash_payload = NULL;
    //char hash[EVP_MAX_MD_SIZE + 1] = { '\0' };
    char *hash = calloc(1, EVP_MAX_MD_SIZE + 1);

    str *ts = str_from("12345");

    hash_payload = generate_payload(self, ts);

    md = EVP_get_digestbyname("MD5");
    mdctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(mdctx, md, NULL);
    EVP_DigestUpdate(mdctx, str_data(hash_payload), str_length(hash_payload));
    EVP_DigestFinal_ex(mdctx, md_value, &md_len);
    EVP_MD_CTX_free(mdctx);

    for (i = 0; i < md_len; i++) {
        sprintf(&hash[i * 2], "%02x", md_value[i]);
    }

    str_destroy(hash_payload);
    str_destroy(ts);

    return hash;
}

void uri_maker_destry(uri_maker *self)
{
    if (self) {
        free(self);
    }
}