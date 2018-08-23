#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "minunit.h"
#include "str.h"

int tests_run = 0;

static char *test_str_create()
{
    str *s = str_create();

    mu_assert(s->len == 0, "String len should be 0, but is %u", s->len);
    mu_assert(s->size == STR_INITIAL_SIZE, "String initial size should be %d, but is %u", STR_INITIAL_SIZE, s->size);
    mu_assert(s->expand == STR_EXPAND, "String expand should be %d, but is %u", STR_EXPAND, s->expand);
    mu_assert(s->data != NULL, "String data is NULL");

    str_destroy(s);
    
    return NULL;
}

static char *test_str_append()
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

    size_t sum_len = initial_str_len + append_1_len + append_2_len;
    
    mu_assert(s->len == sum_len, "String length should be %u, but is %u", sum_len, s->len);
    mu_assert(s->size == STR_INITIAL_SIZE, "String size should be %d, but is %u", STR_INITIAL_SIZE, s->size);

    str_destroy(s);

    return NULL;
}

static char *test_str_macros()
{
    str *s = str_create();

    mu_assert(str_lengt(s) == 0, "String length should be 0, but is %u", str_lengt(s));
    mu_assert(str_data(s) != NULL, "String data is NULL");

    str_destroy(s);

    return NULL;
}

static char *all_tests()
{
    mu_run_test(test_str_create);
    mu_run_test(test_str_append);
    mu_run_test(test_str_macros);

    return NULL;
}

int main(int argc, char *argv[])
{
    char *msg = all_tests();

    if (msg != NULL) {
        printf("Failed: %s\n", msg);
    } else {
        printf("All tests passed.\n");
    }

    printf("Tests run: %d\n", tests_run);

    return 0;
}