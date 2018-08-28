#include <openssl/evp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "marvel.h"
#include "uri.h"
#include "util.h"

/**
 * Generates the payload which will be md5 hashed.
 * From marvel documentation: md5(ts + privateKey + publicKey)
 * 
 * @param uri_maker *self   pointer to uri maker
 * 
 * @return str | NULL
 */
static inline str *generate_payload(uri_maker *self)
{
    throw_(self == NULL, "self is NULL.");
    
    str *out = str_create();
    throw_mem_(out);

    str_append(out, str_data(self->ts), str_length(self->ts));
    str_append(out, str_data(self->pr_api_key), str_length(self->pr_api_key));
    str_append(out, str_data(self->pub_api_key), str_length(self->pub_api_key));

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
static char *get_hash(uri_maker *self)
{
    throw_(self == NULL, "self is NULL.");

    EVP_MD_CTX *mdctx;
    const EVP_MD *md;
    unsigned char md_value[EVP_MAX_MD_SIZE];
    unsigned int md_len, i;
    str *hash_payload = NULL;

    char *hash = calloc(1, EVP_MAX_MD_SIZE + 1);
    throw_mem_(hash);

    self->ts = str_from("123345");

    hash_payload = generate_payload(self);
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

    str_destroy(hash_payload);

    return hash;

    error:
    return NULL;
}

/**
 * Build 
 */
static str *build_req(uri_maker *self)
{
    throw_(self == NULL, "self is NULL.");

    str *url = str_from("hallo :: ");
    str *hash = str_from(self->get_hash(self));
    str_append(url, str_data(hash), str_length(hash));

    str_destroy(hash);

    return url;

    error:
    return NULL;
}

uri_maker *uri_maker_create(const str *pr_api_key, const str *pub_api_key)
{
    uri_maker *urm = malloc(sizeof(uri_maker));
    throw_mem_(urm);

    urm->ts = NULL;
    urm->pr_api_key = str_from(str_data(pr_api_key));
    urm->pub_api_key = str_from(str_data(pub_api_key));
    urm->get_hash = get_hash;
    urm->build_req = build_req;

    return urm;

    error:
    return NULL;
}

void uri_maker_destroy(uri_maker *self)
{
    if (self == NULL) {
        return;
    }

    str_destroy(self->ts);
    str_destroy(self->pr_api_key);
    str_destroy(self->pub_api_key);

    if (self) {
        free(self);
        self = NULL;
    }
}