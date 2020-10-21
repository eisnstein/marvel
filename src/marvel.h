#ifndef __marvel_h__
#define __marvel_h__

#include <stdbool.h>

#include "http.h"
#include "str.h"
#include "uri.h"

#define MARVEL_BASE_ENDPOINT "MARVEL_BASE_ENDPOINT"
#define MARVEL_PATH "MARVEL_PATH"
#define MARVEL_PUBLIC_KEY "MARVEL_PUBLIC_KEY"
#define MARVEL_PRIVATE_KEY "MARVEL_PRIVATE_KEY"

str *marvel_build_url(str *query);

#endif