#define _GNU_SOURCE
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "env.h"
#include "http.h"
#include "marvel.h"
#include "str.h"
#include "uri.h"
#include "util.h"

#define VERSION "0.0.1"

int main(int argc, char *argv[])
{
    // check if useage
    die_(argc < 2, "Useage: marvel <query>");

    // initialise and set env variables
    int r = env_init();
    throw_(r == -1, "Could not initialize env.");

    // initialise http client
    http *client = http_create_client();
    throw_(client == NULL, "Could not create server.");

    const str *url = str_from(getenv(MARVEL_BASE_URL));
    throw_(url == NULL, "Could not get base url from env.");

    // connect to marvel api
    r = client->http_connect(client, url, NULL);
    throw_(r == -1, "Could not connect.");

    // get query from user input
    const str* query = str_from(argv[1]);
    throw_(str_empty(query), "Please provide a query.");

    // initialise the uri maker
    uri_maker *uri = uri_maker_create(query);
    throw_(uri == NULL, "Could not initialize uri maker.");

    // send the request to marvel
    r = client->http_send(client, uri);
    throw_(r == -1, "Could not send data.");

    // receive response from marvel
    str *response_raw = client->http_receive(client);
    throw_(response_raw == NULL, "Could not receive data.");

    debug_("size of response: %ld", str_length(response_raw));

    http_response *http_response = http_response_create();
    throw_(http_response == NULL, "could not create http response object");

    

    // write response into file
    FILE *output = fopen("output.json", "w+");
    throw_(output == NULL, "Could not open output file.");

    fputs(str_data(response_raw), output);

    fclose(output);

    str_destroy(query);
    uri_maker_destroy(uri);
    http_destroy(client);

    return EXIT_SUCCESS;

    error:
    if (query) str_destroy(query);
    if (uri) uri_maker_destroy(uri);
    if (response_raw) str_destroy(response_raw);
    if (client) http_destroy(client);

    return EXIT_FAILURE;
}
