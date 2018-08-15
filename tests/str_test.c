#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "str.h"

void test_str_create()
{
    str *s = str_create();

    assert(s->len == 0);
    assert(s->size == STR_INITIAL_SIZE);
    assert(s->expand == STR_EXPAND);
    assert(s->data != NULL);

    str_destroy(s);
}

void test_str_append()
{
    char *initial_str = "Initial string";
    char *append_1 = " append 1";
    char *append_2 = " append 2";

    size_t initial_str_len = strlen(initial_str);
    size_t append_1_len = strlen(append_1);
    size_t append_2_len = strlen(append_2);

    str *s = str_create();

    str_append(s, initial_str, initial_str_len);
    str_append(s, append_1, append_1_len);
    str_append(s, append_2, append_2_len);

    assert(s->len == (initial_str_len + append_1_len + append_2_len));
    assert(s->size == STR_INITIAL_SIZE);

    str_destroy(s);
}

void test_str_macros()
{
    str *s = str_create();

    assert(str_length(s) == 0);
    assert(str_data(s) != NULL);

    str_destroy(s);
}

int main(int argc, char *argv[])
{
    test_str_create();
    test_str_append();
    test_str_macros();

    return 0;
}