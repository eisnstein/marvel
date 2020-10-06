#define _GNU_SOURCE
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "env.h"
#include "http.h"
#include "marvel.h"
#include "str.h"
#include "uri.h"
#include "util.h"

#define VERSION "0.0.2"

int main(int argc, char *argv[]) {
  marvel *marvel = NULL;

  // check if useage
  die_(argc < 2, "Usage: marvel <query>");

  // get query from user input
  str *query = str_from(argv[1]);
  throw_(str_empty(query), "<query> must not be empty");

  // initialise and set env variables
  bool res = env_init(NULL);
  throw_(res == false, "Could not initialize env variables");

  // initialise marvel client
  marvel = marvel_create();
  throw_(marvel == NULL, "Could not create marvel client");

  res = marvel_request(marvel, query);
  throw_(res == false, "Something went wrong when making request to marvel");

  marvel_destroy(&marvel);

  return EXIT_SUCCESS;

error:
  if (query) str_destroy(&query);
  if (marvel) marvel_destroy(&marvel);

  return EXIT_FAILURE;
}
