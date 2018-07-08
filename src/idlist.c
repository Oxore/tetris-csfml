#include <stdlib.h>
#include "idlist.h"

struct idlist *list_new()
{
    struct idlist *list = calloc(1, sizeof(struct idlist));
    list->id = 1;
    return list;
}

struct idlist *list_append(struct idlist *list)
{
    if (!list)
        return list;
    struct idlist *last = list;
    while (last->next)
        last = last->next;
    last->next = calloc(1, sizeof(struct idlist));
    last->next->id = last->id + 1;
    last->next->prev = last;
    return last->next;
}

struct idlist *list_get(const struct idlist *list, unsigned long id)
{
    const struct idlist *sought = list;
    if (sought) {
        if (sought->id == id)
            return (struct idlist *)sought;
        while ((sought = sought->next))
            if (sought->id == id)
                return (struct idlist *)sought;
    }
    return NULL;
}

void list_rm_node(struct idlist *node)
{
    if (node) {
        if (node->prev)
            node->prev->next = node->next;
        if (node->next)
            node->next->prev = node->prev;
        free(node);
    }
}

void list_foreach(struct idlist *list, void (*job)(void *))
{
    if (list) {
        job(list->obj);
        while ((list = list->next))
            job(list->obj);
    }
}

void list_destroy(struct idlist *list)
{
    if (list) {
        while (list->next) {
            list = list->next;
            free(list->prev);
        }
        free(list);
    }
}
