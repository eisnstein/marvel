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
#include "marvel.h"
#include "str.h"
#include "uri.h"
#include "util.h"

#define MAXDATASIZE 1024

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
    // variable definitions
    int sockfd, numbytes;
    char buf[MAXDATASIZE];
    str *msg = str_create();
    throw_(msg == NULL, "Could not create string.");

    struct addrinfo hints, *res, *p;
    int status = 0;
    int bytesReceived = 0;
    char ipstr[INET6_ADDRSTRLEN];

    // initialise and set env variables
    int r = env_init();
    throw_(r == -1, "Could not initialize env.");

    const str *url = str_from(getenv(MARVEL_BASE_URL));
    throw_(url == NULL, "Could not get base url from env.");

    const str* query = str_from(argv[1]);
    throw_(str_empty(query), "Please provide a query.");

    uri_maker *uri = uri_maker_create(query);
    throw_(uri == NULL, "Could not initialize uri maker.");

    // check if right useage
    if (argc < 2) {
        fprintf(stderr, "Useage: ./marvel <query>\n");
        exit(-1);
    }

    // build the request
    char *req = uri->build_req(uri);

    debug_("req:\n%s", req);

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo(str_data(url), "80", &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        goto error;
    }

    throw_(res == NULL, "Could not get address info.");

    int con = 0;
    for (p = res; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if ((con = connect(sockfd, p->ai_addr, p->ai_addrlen)) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), ipstr, sizeof(ipstr));
    debug_("client: connecting to %s", ipstr);

    freeaddrinfo(res);

    if ((status = send(sockfd, req, strlen(req), 0)) == -1) {
        perror("send");
        close(sockfd);
        exit(-1);
    }

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
