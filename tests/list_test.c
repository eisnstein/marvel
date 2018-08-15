#include <assert.h>
#include <string.h>

#include "list.h"

void test_list_create()
{
    list *l = list_create();

    assert(l != NULL);
    assert(l->length == 0);
    assert(l->head == NULL);
    assert(l->tail == NULL);
}

void test_list_push()
{
    list *l = list_create();

    char *data1 = "data1";
    char *data2 = "data2";

    list_push(l, data1);
    list_push(l, data2);

    assert(l->length == 2);
}

void test_list_pop()
{
    list *l = list_create();

    char *data1 = "data1";
    char *data2 = "data2";

    list_push(l, data1);
    list_push(l, data2);

    assert(l->length == 2);

    char *new_data2 = list_pop(l);
    assert(new_data2 != NULL);
    assert(strcmp(data2, new_data2) == 0);
    assert(l->length == 1);

    char *new_data1 = list_pop(l);
    assert(new_data1 != NULL);
    assert(strcmp(data1, new_data1) == 0);
    assert(l->length == 0);
}

int main(int argc, char *argv[])
{
    test_list_create();
    test_list_push();
    test_list_pop();

    return 0;
}