#include <stdlib.h>
#include "slist.h"

struct slist *slist_new()
{
    return calloc(1, sizeof(struct slist));
}

struct snode *slist_append(struct slist *list)
{
    if (list == NULL)
        return NULL;

    /* At this point it does not matter what calloc returns */

    struct snode *current = calloc(1, sizeof(struct snode));

    if (list->last) {
        list->last->next = current;
        list->last = current;
    } else {
        list->first = current;
        list->last = current;
    }

    return current;
}

void slist_rm(struct slist *list, struct snode *node)
{
    if (list == NULL || node == NULL)
        return;

    struct snode *current = list->first;
    struct snode *prev = NULL;

    while (current) {
        if (current == node) {
            if (prev) {
                prev->next = node->next;
            }
            if (list->first == node) {
                list->first = node->next;
            }
            if (list->last == node) {
                list->last = prev;
            }
            free(node);
            return;
        }
        prev = current;
        current = current->next;
    }
}

void slist_destroy(struct slist *list)
{
    if (list == NULL)
        return;

    struct snode *current = list->first;
    struct snode *prev = current;

    while (current) {
        prev = current;
        current = current->next;
        free(prev);
    }

    free(list);
}
