#include <openssl/evp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "marvel.h"

static char *generate_payload(const char *ts, const char *pr_api_key, const char *pub_api_key)
{
    size_t ts_len = strlen(ts);
    size_t pr_len = strlen(pr_api_key);
    size_t pub_len = strlen(pub_api_key);

    size_t len = ts_len + pr_len + pub_len;
    char *out = calloc(len + 1, sizeof(char));

    memcpy(out, ts, ts_len);
    memcpy(&out[ts_len], pr_api_key, pr_len);
    memcpy(&out[ts_len + pr_len], pub_api_key, pub_len);

    out[len] = '\0';

    return out;   
}

char *auth_hash(const char *ts, const char *pr_api_key, const char *pub_api_key)
{
    EVP_MD_CTX *mdctx;
    const EVP_MD *md;
    unsigned char md_value[EVP_MAX_MD_SIZE];
    unsigned int md_len, i;
    char *hash_payload = NULL;
    //char hash[EVP_MAX_MD_SIZE + 1] = { '\0' };
    char *hash = calloc(1, EVP_MAX_MD_SIZE + 1);

    hash_payload = generate_payload(ts, pr_api_key, pub_api_key);

    md = EVP_get_digestbyname("MD5");
    mdctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(mdctx, md, NULL);
    EVP_DigestUpdate(mdctx, hash_payload, strlen(hash_payload));
    EVP_DigestFinal_ex(mdctx, md_value, &md_len);
    EVP_MD_CTX_free(mdctx);

    for (i = 0; i < md_len; i++) {
        sprintf(&hash[i * 2], "%02x", md_value[i]);
    }

    free(hash_payload);

    return hash;
}