#include <stdlib.h>
#include "idlist.h"

struct idlist *idlist_new()
{
    return calloc(1, sizeof(struct idlist));
}

struct idnode *idlist_append(struct idlist *list)
{
    if (!list)
        return NULL;

    /* At this point it does not matter what calloc returns */

    struct idnode *current = calloc(1, sizeof(struct idnode));
    size_t id = 0;

    if (list->first) {
        struct idnode *last = list->first;
        while (last->next) {
            last = last->next;
        }
        last->next = current;
        id = last->id + 1;
    } else {
        list->first = current;
    }

    if (current) {
        *current = (struct idnode){ .id = id, .next = NULL, };
    }

    return current;
}

struct idnode *idlist_get(const struct idlist *list, size_t id)
{
    if (!list)
        return NULL;

    struct idnode *node = list->first;
    while (node) {
        if (node->id == id) {
            return node;
        }
        node = node->next;
    }
    return NULL;
}

void idlist_rm(struct idlist *list, size_t id)
{
    if (!list)
        return;

    struct idnode *current = list->first;
    struct idnode *prev = current;
    while (current) {
        if (current->id == id) {
            prev->next = current->next;
            free(current);
            if (current == list->first) {
                list->first = NULL;
            }
            return;
        }
        prev = current;
        current = current->next;
    }
}

void idlist_destroy(struct idlist *list)
{
    if (!list)
        return;

    struct idnode *current = list->first;
    struct idnode *prev = current;
    while (current) {
        prev = current;
        current = current->next;
        free(prev);
    }
    free(list);
}
