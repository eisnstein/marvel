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
  // check if useage
  die_(argc < 2, "Usage: marvel <query>");

  // get query from user input
  str *query = str_from(argv[1]);
  throw_(str_empty(query), "<query> must not be empty");

  // initialise and set env variables
  bool res = env_init(NULL);
  throw_(res == false, "Could not initialize env variables");

  // build full marvel url
  str *full_url = marvel_build_url(query);
  throw_(full_url == NULL, "Could not build full marvel url");

  // initalise http client
  http_client *client = http_client_create();
  throw_(client == NULL, "Could not create http client");

  // make get request
  http_response *response = http_get(client, full_url);

  // show response

  str_free(query);
  str_free(full_url);
  http_client_free(client);

  return EXIT_SUCCESS;

error:
  str_free(query);
  str_free(full_url);
  http_client_free(client);
  return EXIT_FAILURE;
}
