#include <stdio.h>
#include <stdlib.h>

#include "list.h"

list *list_create()
{
    list *l = malloc(sizeof(list));
    if (l == NULL) {
        perror("Could not create list. [Error]");
        exit(-1);
    }

    l->length = 0;
    l->head = NULL;
    l->tail = NULL;

    return l;
}

void list_push(list *l, void *data)
{
    list_node *node = malloc(sizeof(list_node));
    if (node == NULL) {
        perror("Could not create list node. [Error]");
        exit(-1);
    }

    node->data = data;
    node->next = NULL;

    if (l->head == NULL) {
        l->head = node;
        l->tail = node;
        node->prev = NULL;
    } else {
        node->prev = l->tail;
        l->tail->next = node;
        l->tail = node;
    }

    l->length++;
}

void *list_pop(list *l)
{
    if (l->tail == NULL) {
        return NULL;
    }

    list_node *node = l->tail;

    if (l->head == l->tail) {
        l->head = NULL;
        l->tail = NULL;
    } else {
        l->tail = node->prev;
        l->tail->next = NULL;
    }

    l->length--;

    return node->data;
}

void *list_at(list *l, uint32_t index)
{
    if (l->length >= index) {
        return NULL;
    }

    list_node *node = l->head;
    for (int i = 0; i < index; i++) {
        node = node->next;
    }

    return node->data;
}