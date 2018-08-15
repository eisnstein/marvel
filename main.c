#define _GNU_SOURCE
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <openssl/evp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "str.h"

#define MAXDATASIZE 102400

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

char *generate_payload(const char *ts, const char *pr_api_key, const char *pub_api_key)
{
    size_t ts_len = strlen(ts);
    size_t pr_len = strlen(pr_api_key);
    size_t pub_len = strlen(pub_api_key);

    size_t len = ts_len + pr_len + pub_len;
    char *out = calloc(len + 1, sizeof(char));

    memcpy(out, ts, ts_len);
    memcpy(&out[ts_len], pr_api_key, pr_len);
    memcpy(&out[ts_len + pr_len], pub_api_key, pub_len);

    out[len] = '\0';

    return out;   
}



int main(int argc, char *argv[])
{
    int sockfd, numbytes;
    char buf[MAXDATASIZE];
    str *msg = str_create();
    struct addrinfo hints, *res, *p;
    int status = 0;
    int bytesReceived = 0;
    char ipstr[INET6_ADDRSTRLEN];
    char *ts = "12345";
    char *private_api_key = "";
    char *endpoint = "";
    EVP_MD_CTX *mdctx;
    const EVP_MD *md;
    char public_api_key[];
    unsigned char md_value[EVP_MAX_MD_SIZE];
    unsigned int md_len, i;
    char *hash_payload = NULL;
    char hash[EVP_MAX_MD_SIZE + 1] = { '\0' };

    hash_payload = generate_payload(ts, private_api_key, public_api_key);

    md = EVP_get_digestbyname("MD5");
    mdctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(mdctx, md, NULL);
    EVP_DigestUpdate(mdctx, hash_payload, strlen(hash_payload));
    EVP_DigestFinal_ex(mdctx, md_value, &md_len);
    EVP_MD_CTX_free(mdctx);

    for (i = 0; i < md_len; i++) {
        sprintf(&hash[i * 2], "%02x", md_value[i]);
    }

    if (argc != 2) {
        fprintf(stderr, "usage: main <hostmame>\n");
        return EXIT_FAILURE;
    }

    free(hash_payload);

    //endpoint = argv[1];

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

    char *req = "GET /v1/public/comics?ts=1234 HTTP/1.1\r\n"
        "Host: gateway.marvel.com\r\n"
        "User-Agent: curl/7.58.0\r\n"
        "Accept: */*\r\n\r\n";

    size_t len = strlen(req);
    size_t len2 = 0;
    char *s = NULL;
    for (s = req; *s != '\0'; s++) {
        len2++;
    }

    printf("len: %ld, len2: %ld\n", len, len2);

    printf("req:\n%s", req);
    //req = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n";
    if ((status = send(sockfd, req, 184, 0)) == -1) {
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
    printf("len: %u\n", msg->len);
    //printf("res:\n%s\n", msg.data);

    close(sockfd);
    str_destroy(msg);

    return EXIT_SUCCESS;
}
