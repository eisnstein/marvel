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

#define MAXDATASIZE 102400

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main(int argc, char *argv[], char *env[])
{
    env_init();

    while (*env) {
        printf("%s\n", *env);
        env++;
    }

    int sockfd, numbytes;
    char buf[MAXDATASIZE];
    str *msg = str_create();
    struct addrinfo hints, *res, *p;
    int status = 0;
    int bytesReceived = 0;
    char ipstr[INET6_ADDRSTRLEN];
    char *ts = "12345";
    char *endpoint = getenv(MARVEL_BASE_URL);

    char *pr_api_key = getenv(MARVEL_PRIVATE_KEY);
    if (pr_api_key == NULL) {
        perror("Could not read private key from env. [Error]");
        exit(-1);
    }

    //char *pub_api_key = "asdf";
    char *pub_api_key = getenv(MARVEL_PUBLIC_KEY);
    if (pub_api_key == NULL) {
        perror("Could not read public key from env. [Error]");
        exit(-1);
    }

    char *hash = auth_hash(ts, pr_api_key, pub_api_key);

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo(endpoint, "80", &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return EXIT_FAILURE;
    }

    if (res == NULL) {
        fprintf(stderr, "getaddrinfo: \n");
        return EXIT_FAILURE;
    }

    int n = 0;
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
    printf("client: connecting to %s\n", ipstr);

    freeaddrinfo(res);

    /*char req[185] = { '\0' };
    sprintf(req, "GET /v1/public/comics?ts=%s&apikey=%s&hash=%s HTTP/1.1\r\n"
        "Host: gateway.marvel.com\r\n"
        "User-Agent: curl/7.58.0\r\n"
        "Accept: *\/*\r\n\r\n", ts, pub_api_key, hash);

    size_t len = strlen(req);
    size_t len2 = 0;
    char *s = NULL;
    for (s = req; *s != '\0'; s++) {
        len2++;
    }

    printf("len: %ld, len2: %ld\n", len, len2);

    printf("req:\n%s", req);
    //req = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n";
    if ((status = send(sockfd, req, 185, 0)) == -1) {
        perror("send");
        close(sockfd);
        exit(-1);
    }

    printf("Status: %d\n", status);

    int sum = 0;
    do {
        if ((bytesReceived = recv(sockfd, buf, MAXDATASIZE, 0)) == -1) {
            perror("recv");
            break;
        }

        str_append(msg, buf, bytesReceived);
    } while (bytesReceived > 0);


    printf("size: %u\n", msg->size);
    printf("len: %u\n", str_length(msg));
    printf("res:\n%s\n", str_data(msg));*/

    close(sockfd);
    str_destroy(msg);

    return EXIT_SUCCESS;
}
