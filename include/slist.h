/*
 * slist.h
 *
 * List - List datatype implementation. Element can be taken only by walking
 * through list until needed element is found.
 *
 * */

struct snode {
    void           *obj;
    struct snode   *next;
};

struct slist {
    struct snode *first;
    struct snode *last;
};

struct slist   *slist_new();
struct snode   *slist_append(struct slist *list);
void            slist_rm(struct slist *list, struct snode *node);
void            slist_destroy(struct slist *list);
#define         slist_first_node(list) (list)->first;
#define         snode_next(node) (node)->next;

#define SLIST_FOREACH(list, node) \
    for (struct snode *node = (list)->first; node; node = node->next)

#define SLIST_FOREACH_CONST(list, node) \
    for (const struct snode *node = (list)->first; node; node = node->next)
