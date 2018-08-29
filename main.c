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

#define MAXDATASIZE 1024


int main(int argc, char *argv[])
{
    // variable definitions
    int numbytes;
    char buf[MAXDATASIZE];
    str *msg = str_create();
    throw_(msg == NULL, "Could not create string.");

    int bytesReceived = 0;
    struct addrinfo hints;

    // check if useage
    if (argc < 2) {
        fprintf(stderr, "Useage: ./marvel <query>\n");
        exit(-1);
    }

    // initialise and set env variables
    int r = env_init();
    throw_(r == -1, "Could not initialize env.");

    // initialise http
    http *client = http_create_client();
    throw_(client == NULL, "Could not create server.");

    const str *url = str_from(getenv(MARVEL_BASE_URL));
    throw_(url == NULL, "Could not get base url from env.");

    r = client->http_connect(client, url, NULL);
    throw_(r == -1, "Could not connect.");

    const str* query = str_from(argv[1]);
    throw_(str_empty(query), "Please provide a query.");

    uri_maker *uri = uri_maker_create(query);
    throw_(uri == NULL, "Could not initialize uri maker.");

    r = client->http_send(client, uri);

    int sum = 0;
    do {
        if ((bytesReceived = recv(sockfd, buf, MAXDATASIZE, 0)) == -1) {
            perror("recv");
            break;
        }

        str_append(msg, buf, bytesReceived);
    } while (bytesReceived > 0);

    FILE *output = fopen("output.json", "w+");
    throw_(output == NULL, "Could not open output file.");

    fputs(str_data(msg), output);

    fclose(output);

    str_destroy(url);
    str_destroy(query);
    free(req);
    uri_maker_destroy(uri);
    close(sockfd);
    str_destroy(msg);

    return EXIT_SUCCESS;

    error:
    if (url) str_destroy(url);
    if (query) str_destroy(query);
    if (req) free(req);
    if (uri) uri_maker_destroy(uri);
    if (sockfd) close(sockfd);
    if (msg) str_destroy(msg);

    return EXIT_FAILURE;
}
