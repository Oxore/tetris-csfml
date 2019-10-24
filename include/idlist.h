/*
 * idlist.h
 *
 * IDList - List datatype implementation. Element can be taken by id only.
 *
 * IDList is used by the Painter module and loadable texts.
 *
 * */

struct idlist {
    size_t          id;
    void           *obj;
    struct idlist  *next;
    struct idlist  *prev;
};

struct idlist  *list_new();
struct idlist  *list_append(struct idlist *list);
struct idlist  *list_get(const struct idlist *list, size_t id);
void            list_rm_node(struct idlist *node);
void            list_destroy(struct idlist *list);

#define LIST_FOREACH(head, e) \
    for (struct idlist *e = head; e; e = e->next)

#define LIST_FOREACH_CONST(head, e) \
    for (const struct idlist *e = head; e; e = e->next)
