#define _GNU_SOURCE
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "marvel.h"
#include "str.h"
#include "util.h"

bool env_init(const char *filename) {
  if (filename == NULL) {
    filename = ".env";
  }

  char *line = NULL;
  size_t len = 0;
  ssize_t read = 0;
  int ret = 0;

  strlist *list = NULL;
  str *envval = NULL;
  str *env_key = NULL;
  str *env_value = NULL;

  FILE *f = fopen(filename, "r");
  throw_v_(f == NULL, "Could not open %s file.", filename);

  envval = str_create();
  throw_(envval == NULL, "Could not create envval string object.");

  // Read every line of the .env file
  // and set the env variable.
  while ((read = getline(&line, &len, f)) != -1) {
    if (line[0] == '\n') continue;

    str_put_into(envval, line);

    // get rid of the <newline> char
    str_strip_nl(envval);

    list = str_split(envval, "=");
    if (list == NULL) continue;

    env_key = strlist_at(list, 0);
    throw_(env_key == NULL, "Could not get key of env variable.");

    env_value = strlist_at(list, 1);
    throw_(env_value == NULL, "Could not get value of env variable.");

    ret = setenv(str_data(env_key), str_data(env_value), 1);
    throw_(ret == -1, "Could not set env variable.");

    strlist_destroy(&list);
  }

  str_destroy(&envval);
  free(line);
  fclose(f);
  return true;

error:
  if (envval) str_destroy(&envval);
  if (line) free(line);
  if (f) fclose(f);
  return false;
}