/*
 * idlist.h
 *
 * IDList - Singly linked list with id's datatype implementation. Element can be
 * taken by id only.
 *
 * IDList is used by the Painter module.
 *
 * */

struct idnode {
    size_t          id;
    void           *obj;
    struct idnode  *next;
};

struct idlist {
    struct idnode *first;
};

struct idlist  *idlist_new();
struct idnode  *idlist_append(struct idlist *list);
struct idnode  *idlist_get(const struct idlist *list, size_t id);
void            idlist_rm(struct idlist *list, size_t id);
void            idlist_destroy(struct idlist *list);

#define IDLIST_FOREACH(list, node) \
    for (struct idnode *node = (list)->first; node; node = node->next)

#define IDLIST_FOREACH_CONST(list, node) \
    for (const struct idnode *node = (list)->first; node; node = node->next)
