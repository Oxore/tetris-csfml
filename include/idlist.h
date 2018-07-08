struct idlist {
    unsigned long id;
    void        *obj;
    struct idlist *next;
    struct idlist *prev;
};

struct idlist *list_new();
struct idlist *list_append(struct idlist *list);
struct idlist *list_get(const struct idlist *list, unsigned long id);
void           list_rm_node(struct idlist *node);
void           list_foreach(struct idlist *list, void (*job)(void *));
void           list_destroy(struct idlist *list);
