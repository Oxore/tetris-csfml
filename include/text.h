#define TXT_ATTR_INVISIBLE      (1 << 0)

typedef struct text {
    char             *type;
    char             *scene;
    char             *font;
    char             *text;
    unsigned int      size;
    struct vector2ui  pos;
    unsigned int      attr;
    unsigned long     id;
} Text;

struct idlist *load_texts(char *filename);
void           text_destroy(void *text);
