typedef struct List {
    void *obj;
    void *next;
    void *prev;
} List;

typedef struct Pair {
    void *k;
    void *v;
} Pair;

typedef struct KeyMap {
    Pair *pair;
    void *next;
    void *prev;
} KeyMap;

typedef struct Text {
    char *font;
    char *type;
    char *scene;
    char *text;
    void *sfText;
} Text;

FILE   *openFile(char *filename);
void    checkArgs(int argc, char **argv);
KeyMap *KeyMap_getLast(KeyMap **keyMap);
KeyMap *KeyMap_new(KeyMap **keyMap);
KeyMap *KeyMap_get(KeyMap **keyMap, const void *key);
KeyMap *KeyMap_put(KeyMap **keyMap, const void *key, const void *value);
List   *List_getLast(List **list);
List   *List_new(List **list);
List   *ListOfKeyMapOfString_getFromYaml(char *filename);
void    KeyMapOfString_free(KeyMap *keyMap);
void    ListOfKeyMapOfString_free(List **list);
List   *ListOfText_getFromListOfKeyMapOfString(List *list);
void    Text_free(Text *obj);
void    ListOfText_free(List **list);
