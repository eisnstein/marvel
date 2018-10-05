#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "http.h"
#include "marvel.h"
#include "uri.h"
#include "util.h"

marvel *marvel_create()
{
    marvel *m = malloc(sizeof(marvel));
    throw_mem_(m);

    m->client = http_create_client();
    throw_(m->client == NULL, "could not create http client");

    m->uri_maker = uri_maker_create();
    throw_(m->uri_maker == NULL, "could not create uri maker");

    m->make_request = make_request;

    return m;

    error:
    return NULL;
}

static int make_request(marvel *self, const str *query)
{
    int r = 0;

    const str *url = str_from(getenv(MARVEL_BASE_URL));
    throw_(url == NULL, "something went wrong when trying to get marvel base url from env");

    // connect to marvel api
    r = self->client->http_connect(self->client, url, NULL);
    throw_(r == -1, "something went wrong when connecting to %s", str_data(url));

    // set query on uri_maker
    self->uri_maker->query = query;

    // send the request to marvel
    r = self->client->http_send(self->client, self->uri_maker);
    throw_(r == -1, "something went wrong when sending request to marvel");

    // receive response from marvel
    const str *response_raw = self->client->http_receive(self->client);
    throw_(response_raw == NULL, "something went wrong when receiving data");

    debug_("size of response: %ld", str_length(response_raw));

    http_response *http_response = http_response_create();
    throw_(http_response == NULL, "could not create http response object");

    r = http_response->http_response_parse(http_response, response_raw);
    throw_(r == -1, "something went wrong when parsing the raw http response");

    // write response into file
    FILE *output = fopen("output.json", "w+");
    throw_(output == NULL, "Could not open output file.");

    fputs(str_data(http_response->body), output);

    fclose(output);
    http_response_destroy(http_response);

    return 0;

    error:
    if (http_response) http_response_destroy(http_response);
    return -1;
}

void marvel_destroy(marvel *self)
{
    if (self == NULL) {
        return;
    }

    uri_maker_destroy(self->uri_maker);
    http_destroy(self->client);

    free(self);
    self = NULL;
}