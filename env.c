#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "marvel.h"
#include "str.h"

void env_init()
{
    FILE *f = fopen(".env", "r");
    if (f == NULL) {
        perror("Could not open .env file. [Error]");
        exit(-1);
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read = 0;
    int ret = 0;

    char *env = NULL;
    str *envval = NULL;

    /*
     * Read every line of the .env file 
     * and set the env variable.
     */ 
    while ((read = getline(&line, &len, f)) != -1) {
        // get rid of the <newline> char
        if (line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }

        envval = str_from(line);

        printf("%s: %ld\n", line, strlen(line));
        
        // set the env variable (has to be in the form "name=value")
        ret = putenv(line);
        if (ret != 0) {
            perror("Could not put env variable. [Error]");
            exit(-1);
        }
    }

    env = getenv(MARVEL_PRIVATE_KEY);
    if (env) printf("prk: %s\n", env);
    env = getenv(MARVEL_BASE_URL);
    if (env) printf("url: %s\n", env);
    env = getenv(MARVEL_BASE_ENDPOINT);
    if (env) printf("endpoint: %s\n", env);
    env = getenv(MARVEL_PUBLIC_KEY);
    if (env) printf("pbk: %s\n", env);
}