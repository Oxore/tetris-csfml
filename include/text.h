#include <SFML/Graphics.h>
#include "common.h"

FILE *openFile(char *filename);
void checkArgs(int argc, char **argv);
KeyMap *KeyMap_getLast(KeyMap **keyMap);
KeyMap *KeyMap_new(KeyMap **keyMap);
KeyMap *KeyMap_get(KeyMap **keyMap, const void *key);
KeyMap *KeyMap_put(KeyMap **keyMap, const void *key, const void *value);
List *List_getLast(List **list);
List *List_new(List **list);
List *ListOfKeyMapOfString_getFromYaml(char *filename);
void KeyMapOfString_free(KeyMap *keyMap);
void ListOfKeyMapOfString_free(List **list);
int _loadText_getInt(void *obj, char *key);
char *_loadText_getString(void *obj, char *key);
void _loadText_initSfText(Text *objo, void *obji);
List *ListOfText_getFromListOfKeyMapOfString(List *list);
void Text_free(Text *obj);
void ListOfText_free(List **list);
