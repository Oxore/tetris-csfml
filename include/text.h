#define TXT_ATTR_INVISIBLE      (1 << 0)

typedef struct text {
    char             *type;
    char             *scene;
    char             *font;
    wchar_t          *text;
    unsigned int      size;
    struct vector2ui  pos;
    unsigned int      attr;
    unsigned long     id;
} Text;

unsigned long  utf8_strlen(void *string);
void           utf8to32_strcpy(wchar_t *dest, char *src);
struct idlist *load_texts(char *filename);
void           text_destroy(void *text);
