#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>

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
    throw_(r == -1, "something went wrong when setting env variables");

    // get query from user input
    const str* query = str_from(argv[1]);
    throw_(str_empty(query), "<query> must not be empty");

    // initialise marvel client
    marvel *marvel = marvel_create();
    throw_(marvel == NULL, "could not create marvel client");

    r = marvel->make_request(marvel, query);
    throw_(r == -1, "something went wrong when making request to marvel");

    marvel_destroy(marvel);

    return EXIT_SUCCESS;

    error:
    if (query) str_destroy(query);
    if (marvel) marvel_destroy(marvel);

    return EXIT_FAILURE;
}
