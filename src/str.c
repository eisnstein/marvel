#include "str.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

/**
 *-------------------------------------------------------------------
 * String
 *-------------------------------------------------------------------
 *
 * This section provides functions for creating and manipulating
 * string objects. String objects hold the raw string data
 * and some additional information about the string, like the
 * current length, the size and the expand rate.
 * String objects are initialised with an initial size, which means
 * that at creation a specific amount of memory is allocated for
 * the raw data. Should the data inserted be greater, the data
 * will be "expanded" by at least the amount which is needed.
 *
 *-------------------------------------------------------------------
 */

/**
 * Creates / initialises an empty string object with a
 * predefined initial size and expand size.
 *
 * Example:
 *    str *newString = str_create();
 *
 * @return str | NULL   pointer to a string object
 */
str *str_create() {
  str *s = malloc(sizeof(str));
  throw_mem_(s);

  s->size = STR_INITIAL_SIZE;
  s->expand = STR_EXPAND;
  s->len = 0;

  s->data = calloc(s->size, sizeof(char));
  throw_mem_(s->data);

  return s;

error:
  if (s) free(s);
  return NULL;
}

/**
 * Creates / initialises an empty string object with a
 * given initial size and expand size.
 *
 * Example:
 *    str *newString = str_create_v(256, 32);
 *
 * @return str | NULL   pointer to a string object
 */
str *str_create_v(size_t initialSize, size_t expandSize) {
  if (expandSize < STR_EXPAND) {
    expandSize = STR_EXPAND;
  }

  str *s = malloc(sizeof(str));
  throw_mem_(s);

  s->size = initialSize;
  s->expand = expandSize;
  s->len = 0;

  s->data = calloc(s->size, sizeof(char));
  throw_mem_(s->data);

  return s;

error:
  if (s) free(s);
  return NULL;
}

/**
 * Create a string object from a raw C char array.
 * The char array will be copied into the string object.
 *
 * Example:
 *    str *newString = str_from("example_string");
 *
 * @param const char *string    pointer to a char array
 *
 * @return str | NULL           pointer to a string object
 */
str *str_from(const char *string) {
  if (string == NULL) {
    return NULL;
  }

  str *s = str_create();
  throw_(s == NULL, "Could not create str.");

  // the raw string can just be appended
  bool res = str_append(s, string);
  throw_(res == false, "Could not append raw string");

  return s;

error:
  str_free(s);
  return NULL;
}

/**
 * Duplicate a string object.
 *
 * Example:
 *    str *oldString = str_from("old");
 *    str *newString = str_duplicate(oldString);
 *    // str_data(newString) == "old"
 *
 * @param const str *s    pointer to string object
 *
 * @return str | NULL     pointer to new string object
 */
str *str_duplicate(const str *s) {
  if (s == NULL) {
    return NULL;
  }

  return str_from(str_data(s));
}

/**
 * Append a raw char string onto an existing string object. If the new
 * string would be too long, memory will be reallocated to hold the whole
 * string data.
 *
 * Example:
 *    str *oldString = str_from("old_");
 *    str *newString = str_append(oldString, "new");
 *    // str_data(newString) == "old_new"
 *
 * @param s         pointer to a string object
 * @param append    pointer to a char array
 *
 * @return bool     true on success / false on error
 */
bool str_append(str *s, const char *append) {
  throw_(s == NULL, "String to append to is null");
  throw_(append == NULL, "String to append is null");

  size_t len = strlen(append);
  char *new = NULL;

  // calculate the needed size
  size_t new_len = str_length(s) + len;

  if (new_len >= s->size) {
    new = realloc(s->data, new_len + s->expand);
    throw_mem_(new);
    s->size += s->expand;
  } else {
    new = s->data;
  }

  // copy data into new object
  memcpy(&new[s->len], append, len);

  new[new_len] = '\0';
  s->data = new;
  s->len = new_len;

  return true;

error:
  return false;
}

/**
 * Split a string into parts by a delimiter char. Each part
 * will be stored in a string object which itself will be
 * held in a node of a string list.
 *
 * Example:
 *    str *toBeSplitted = str_from("key:value");
 *    strlist *list = str_split(toBeSplitted, ':');
 *    str *key = strlist_at(0);
 *    str *value = strlist_at(1);
 *    // str_data(key) == "key"
 *    // str_data(value) == "value"
 *
 * @param s            pointer to a string object
 * @param delimiter    pointer to char by which the string should be split
 *
 * @return strlist | NULL   pointer to a string list
 */
strlist *str_split(str *s, const char *delimiter) {
  strlist *sl = strlist_create();
  throw_(sl == NULL, "Could not create string list.");

  // get the first token
  char *token = strtok(str_data(s), delimiter);
  if (token == NULL) {
    debug_v_("Could not split '%s' with delimiter '%s'", str_data(s),
             delimiter);
    goto error;
  }

  str *t = NULL;

  // if token is not NULL, the token will be
  // pushed onto the string list
  while (token != NULL) {
    t = str_from(token);
    strlist_push(sl, t);

    str_free(t);

    // get the next token
    token = strtok(NULL, delimiter);
  }

  return sl;

error:
  strlist_free(sl);
  return NULL;
}

/**
 * Put a raw string into an existing string object. Existing data
 * will be overwritten.
 *
 * @param s       pointer to a string object
 * @param put     pointer to data which will be copied
 *
 * @return bool   true on success / false on error
 */
bool str_put_into(str *s, const char *put) {
  // to copy / overwrite the existing data, we just set
  // the len to 0, so that the new string will be copied
  // to the beginning
  s->len = 0;
  return str_append(s, put);
}

/**
 * Extract a substring from a string object.
 *
 * Example:
 *    str *s = str_from("not this, but this");
 *    str *subStr = str_substr(s, 10, 8);
 *    // str_data(subStr) == "but this"
 *
 * @param s         pointer to string object
 * @param pos       start position
 * @param length    length of the substring
 *
 * @return str | NULL   pointer to str object
 */
str *str_substr(str *s, size_t pos, size_t length) {
  size_t len = str_length(s);
  // start position cannot be greate than length of string
  if (pos >= len) return NULL;
  // length of substring can max be the length of the string - start position
  if (length >= len) length = len - pos;
  // length of substring should be greate than 0
  if (length == 0) return NULL;

  const char *from = str_data(s);
  // move to start position
  from += pos;

  char *tmp = malloc((sizeof(char) * length) + 1);
  throw_mem_(tmp);

  // copy substring into tmp buffer
  strncpy(tmp, from, length);
  tmp[length] = '\0';

  str *substr = str_from(tmp);
  throw_v_(substr == NULL, "Could not create substr from '%s'", tmp);

  free(tmp);

  return substr;

error:
  if (tmp) free(tmp);
  return NULL;
}

/**
 * Check if a string starts with the given search value.
 *
 * Example:
 *    str *s = str_from("The quick brown Fox jumps over the lazy Dog.");
 *    const char *search = "The quick brow";
 *    bool startsWith = str_starts_with(s, search);
 *    // startsWith == true
 *
 * @param s         pointer to string object
 * @param search    pointer to search value
 *
 * @return bool     true if string starts with value / false otherwise
 */
bool str_starts_with(str *s, const char *search) {
  if (s == NULL) return false;
  if (search == NULL) return false;

  size_t searchLength = strlen(search);
  if (searchLength > str_length(s)) return false;
  if (searchLength == 0) return false;

  char *haystackFront = str_data(s);
  char *haystackBack = str_data(s) + (searchLength - 1);
  char *needleFront = search;
  char *needleBack = search + (searchLength - 1);

  // If the first characters of each other do not match
  size_t middle = searchLength / 2;
  for (size_t i = 0; i < middle; ++i) {
    if (*haystackFront != *needleFront) return false;
    if (*haystackBack != *needleBack) return false;

    haystackFront++;
    haystackBack--;
    needleFront++;
    needleBack--;
  }

  return true;
}

/**
 * Destroy (free) a string object.
 *
 * @deprecated Do not use this function directly. Use 'str_free()' instead.
 *
 * @param s pointer a string object
 */
void str_destroy(str *s) {
  if (s == NULL) return;

  if (s->data) {
    free(s->data);
    s->data = NULL;
  }

  free(s);
}

/**
 *-------------------------------------------------------------------
 * String list
 *-------------------------------------------------------------------
 *
 * This section provides functions for creating a double linked
 * list of str objects. For example the str_split function uses
 * a string list for storing each part of the splitted string
 * in a node.
 *
 *-------------------------------------------------------------------
 */

/**
 * Creates / initialises an empty string list.
 *
 * Example:
 *    strlist *list = strlist_create();
 *
 * @return strlist | NULL   pointer to a string list
 */
strlist *strlist_create() {
  strlist *sl = malloc(sizeof(strlist));
  throw_mem_(sl);

  sl->size = 0;
  sl->head = NULL;
  sl->tail = NULL;

  return sl;

error:
  return NULL;
}

/**
 * Push / append a string object at the end of the string list.
 * The str object will be copied into a new str object.
 *
 * @param sl        pointer to a string list
 * @param value     pointer to a string object
 *
 * @return bool     true on success / false on error
 */
bool strlist_push(strlist *sl, str *value) {
  strlistnode *sln = malloc(sizeof(strlistnode));
  throw_mem_(sln);

  sln->value = str_duplicate(value);
  throw_(sln->value == NULL, "Could not copy str into strlist");

  sln->next = NULL;
  sln->prev = NULL;

  // if list is empty, set head and tail to first node
  if (sl->head == NULL) {
    sl->head = sln;
    sl->tail = sln;
  } else {
    sl->tail->next = sln;
    sln->prev = sl->tail;
    sl->tail = sln;
  }

  sl->size++;

  return true;

error:
  if (sln) free(sln);
  return false;
}

/**
 * Get the last element of a string list.
 *
 * @param sl    pointer to a string list
 *
 * @return str  pointer to a string object
 */
str *strlist_pop(strlist *sl) {
  strlistnode *sln = sl->tail;

  sl->tail = sln->prev;
  sl->tail->next = NULL;
  sl->size--;

  return sln->value;
}

/**
 * Get the string object of a specific index in the string list.
 *
 * @param sl      pointer to a string list
 * @param index   0-based index of the string object
 *
 * @return str | NULL   pointer to a string object
 */
str *strlist_at(strlist *sl, size_t index) {
  if (sl == NULL) return NULL;
  // return if index is bigger than the actual size of the list
  if (sl->size - 1 < index) return NULL;

  strlistnode *sln = sl->head;

  // go to the node at the index
  for (size_t i = 0; i < index; i++) {
    sln = sln->next;
  }

  if (sln == NULL) return NULL;

  return sln->value;
}

/**
 * Destroy (free) a string list.
 *
 * @deprecated Do not use this function directly. Use 'strlist_free()' instead.
 *
 * @param sl   pointer to a string list
 */
void strlist_destroy(strlist *sl) {
  if (sl == NULL) {
    return;
  }

  // if list is empty, just free list and return
  if (strlist_empty(sl)) {
    free(sl);
    sl = NULL;
    return;
  }

  strlistnode *prev = sl->head;
  // need a second reference to free the node
  // but not lose the next node
  strlistnode *node = NULL;

  while (prev != NULL) {
    node = prev->next;

    if (prev->value) {
      str_free(prev->value);
    }

    free(prev);

    prev = node;
  }

  free(sl);
}
