#ifndef __list_h__
#define __list_h__

#include <stdint.h>

typedef struct list_node {
    struct list_node *next;
    struct list_node *prev;
    void *data;
} list_node;

typedef struct list {
    uint32_t length;
    struct list_node *head;
    struct list_node *tail;
} list;

list *list_create();
void list_push(list *l, void *data);
void *list_pop(list *l);
void *list_at(list *l, uint32_t index);
void list_destroy(list *l);

#endif