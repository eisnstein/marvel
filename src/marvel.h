#ifndef __marvel_h__
#define __marvel_h__

#include "str.h"

#define MARVEL_ENDPOINT "MARVEL_ENDPOINT"
#define MARVEL_PUBLIC_KEY "MARVEL_PUBLIC_KEY"
#define MARVEL_PRIVATE_KEY "MARVEL_PRIVATE_KEY"

char *generate_hash(const char *ts, const char *privateKey,
                    const char *publicKey);
char *generate_timestamp();
char *generate_url(const char *endpoint, const char *query, const char *ts,
                   const char *publicKey, const char *hash);
str *marvel_build_url(str *query);

#endif