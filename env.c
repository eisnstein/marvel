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

    str *envval = NULL;

    /*
     * Read every line of the .env file 
     * and set the env variable.
     */ 
    while ((read = getline(&line, &len, f)) != -1) {
        envval = str_from(line);

        // get rid of the <newline> char
        str_strip_nl(envval);

        printf("%s", str_data(envval));
        
        // set the env variable (has to be in the form "name=value")
        ret = putenv(str_data(envval));
        if (ret != 0) {
            perror("Could not put env variable. [Error]");
            exit(-1);
        }
    }

    fclose(f);
}