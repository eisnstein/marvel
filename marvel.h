#ifndef __marvel_h__
#define __marvel_h__

#define MARVEL_BASE_URL "MARVEL_BASE_URL"
#define MARVEL_BASE_ENDPOINT "MARVEL_BASE_ENDPOINT"

#define MARVEL_PUBLIC_KEY "MARVEL_PUBLIC_KEY"
#define MARVEL_PRIVATE_KEY "MARVEL_PRIVATE_KEY"

char *auth_hash(const char *ts, const char *pr_api_key, const char *pub_api_key);

#endif