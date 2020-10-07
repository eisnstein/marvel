#include "marvel.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "http.h"
#include "uri.h"
#include "util.h"

marvel *marvel_create() {
  marvel *m = malloc(sizeof(marvel));
  throw_mem_(m);

  m->client = http_create();
  throw_(m->client == NULL, "Could not create http client");

  m->uri_maker = uri_maker_create();
  throw_(m->uri_maker == NULL, "Could not create uri maker");

  return m;

error:
  if (m && m->client) http_free(m->client);
  if (m) free(m);
  return NULL;
}

bool marvel_request(marvel *self, str *query) {
  bool r = false;
  http_response *http_response = NULL;

  str *url = str_from(getenv(MARVEL_BASE_URL));
  throw_(url == NULL, "Could not get base url from env");

  // connect to marvel api
  r = http_connect(self->client, url, NULL);
  throw_v_(r == false, "Could not connect to %s", str_data(url));

  // set query on uri_maker
  self->uri_maker->query = query;

  // send the request to marvel
  r = http_send(self->client, self->uri_maker);
  throw_(r == false, "Could not send request to marvel");

  // receive response from marvel
  str *response_raw = http_receive(self->client);
  throw_(response_raw == NULL, "Could not receive data");

  debug_v_("size of response: %ld", str_length(response_raw));

  http_response = http_response_create();
  throw_(http_response == NULL, "Could not create http response object");

  r = http_response_parse(http_response, response_raw);
  throw_(r == false, "Could not parse http response");

  // write response into file
  FILE *output = fopen("output.json", "w+");
  throw_(output == NULL, "Could not open output file");

  fputs(str_data(http_response->body), output);
  fclose(output);

  http_response_free(http_response);

  return true;

error:
  if (http_response) http_response_free(http_response);
  return false;
}

void marvel_destroy(marvel *self) {
  if (self == NULL) {
    return;
  }

  uri_maker_free(self->uri_maker);
  http_free(self->client);

  free(self);
}