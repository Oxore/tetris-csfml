#include <yaml.h>
#include <SFML/Graphics.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "text.h"

extern sfFont *fontScore;

FILE *openFile(char *filename)
{
    FILE *file;
    if (!(file = fopen(filename, "rb"))){
        printf("ERROR\n");
        exit(EXIT_FAILURE);
    }
    return file;
}

void checkArgs(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: %s file1.yaml ...\n", argv[0]);
        exit(EXIT_FAILURE);
    }
}

KeyMap *KeyMap_getLast(KeyMap **keyMap)
{
    KeyMap *keyMapLocal = *keyMap;
    if (keyMapLocal)
        while (keyMapLocal->next)
            keyMapLocal = keyMapLocal->next;
    return keyMapLocal;
}

KeyMap *KeyMap_new(KeyMap **keyMap)
{
    KeyMap *keyMapLocal = KeyMap_getLast(keyMap);
    if (keyMapLocal) {
        keyMapLocal->next = malloc(sizeof(KeyMap));
        ((KeyMap *)keyMapLocal->next)->prev = keyMapLocal;
        keyMapLocal = keyMapLocal->next;
    } else {
        *keyMap = malloc(sizeof(KeyMap));
        keyMapLocal = *keyMap;
        keyMapLocal->prev = 0;
    }
    keyMapLocal->pair = malloc(sizeof(Pair));
    keyMapLocal->next = 0;
    return keyMapLocal;
}

KeyMap *KeyMap_get(KeyMap **keyMap, const void *key)
{
    KeyMap *keyMapLocal = *keyMap;
    while (keyMapLocal)
        if (keyMapLocal->pair)
            if (keyMapLocal->pair->k)
                if (!strcmp(keyMapLocal->pair->k, key))
                    return keyMapLocal;
                else
                    keyMapLocal = keyMapLocal->next;
            else
                keyMapLocal = keyMapLocal->next;
        else
            keyMapLocal = keyMapLocal->next;
    return 0;
}

KeyMap *KeyMap_put(KeyMap **keyMap, const void *key, const void *value)
{
    KeyMap *keyMapLocal = KeyMap_get(keyMap, key);
    if (!keyMapLocal) {
        keyMapLocal = KeyMap_new(keyMap);
        keyMapLocal->pair = malloc(sizeof(Pair));
        keyMapLocal->pair->k = malloc(strlen(key)+1);
        strcpy(keyMapLocal->pair->k, key);
    }
    keyMapLocal->pair->v = malloc(strlen(value)+1);
    strcpy(keyMapLocal->pair->v, value);
    return keyMapLocal;
}

List *List_getLast(List **list)
{
    List *listLocal = *list;
    if (listLocal)
        while (listLocal->next)
            listLocal = listLocal->next;
    return listLocal;
}

List *List_new(List **list)
{
    List *listLocal = List_getLast(list);
    if (listLocal) {
        (listLocal)->next = malloc(sizeof(List));
        ((List *)listLocal->next)->prev = listLocal;
        listLocal = listLocal->next;
    } else {
        *list = malloc(sizeof(List));
        (*list)->prev = 0;
        listLocal = *list;
    }
    listLocal->obj = 0;
    listLocal->next = 0;
    return listLocal;
}

List *ListOfKeyMapOfString_getFromYaml(char *filename)
{
    List *list = 0;
    FILE *file = openFile(filename);
    yaml_parser_t parser;
    yaml_event_t event;
    yaml_parser_initialize(&parser);
    yaml_parser_set_input_file(&parser, file);
    do {
        if (!yaml_parser_parse(&parser, &event)) {
             printf("Parser error %d\n", parser.error);
             exit(EXIT_FAILURE);
        }
        switch(event.type)
        {
            case YAML_MAPPING_START_EVENT:
                List_new(&list);
                break;
            case YAML_SCALAR_EVENT:
                ;yaml_event_t ev;
                yaml_parser_parse(&parser, &ev);
                while (ev.type != YAML_SCALAR_EVENT) {
                    yaml_event_delete(&ev);
                    if (!yaml_parser_parse(&parser, &ev)) {
                         printf("Parser error %d\n", parser.error);
                         exit(EXIT_FAILURE);
                    }
                }
                KeyMap **keyMap = (KeyMap **)&((List *)List_getLast(&list))->obj;
                KeyMap_put(keyMap, event.data.scalar.value, ev.data.scalar.value);
                yaml_event_delete(&ev);
                break;
            default: break;
        }
        if(event.type != YAML_STREAM_END_EVENT)
            yaml_event_delete(&event);
    } while(event.type != YAML_STREAM_END_EVENT);
    yaml_event_delete(&event);
    yaml_parser_delete(&parser);
    fclose(file);
    return list;
}

void KeyMapOfString_free(KeyMap *keyMap)
{
    KeyMap *keyMapLocal = KeyMap_getLast(&keyMap);
    while (keyMapLocal) {
        free(keyMapLocal->pair->k);
        free(keyMapLocal->pair->v);
        free(keyMapLocal->pair);
        KeyMap *prev = keyMapLocal->prev;
        free(keyMapLocal);
        keyMapLocal = prev;
    }
}

void ListOfKeyMapOfString_free(List **list)
{
    List *listLocal = List_getLast(list);
    while (listLocal) {
        KeyMapOfString_free(listLocal->obj);
        List *prev = listLocal->prev;
        free(listLocal);
        listLocal = prev;
    }
    *list = 0;
}

int _loadText_getInt(void *obj, char *key)
{
    int v = 0;
    KeyMap *keyMap = KeyMap_get((KeyMap **)&obj, key);
    if (keyMap)
        if (keyMap->pair)
            if (keyMap->pair->v)
                v = atoi(keyMap->pair->v);
    return v;
}

char *_loadText_getString(void *obj, char *key)
{
    char *v = 0;
    KeyMap *keyMap = KeyMap_get((KeyMap **)&obj, key);
    if (keyMap)
        if (keyMap->pair)
            if (keyMap->pair->v) {
                v = malloc(strlen(keyMap->pair->v)+1);
                strcpy(v, keyMap->pair->v);
            }
    return v;
}

void _loadText_initSfText(Text *objo, void *obji)
{
    sfVector2f pos = {
        .x = _loadText_getInt(obji, "x"), 
        .y = _loadText_getInt(obji, "y")
    };
    int size = _loadText_getInt(obji, "size");
    char *text = _loadText_getString(obji, "text");
    objo->sfText = sfText_create();
    //sfText_setFont(objo->sfText, fontScore);
    sfText_setCharacterSize(objo->sfText, size);
    sfText_setPosition(objo->sfText, pos);
    sfText_setString(objo->sfText, text);
    free(text);
}

List *ListOfText_getFromListOfKeyMapOfString(List *list)
{
    List *li = list;
    List *lo = 0;
    List *l = 0;
    while (li) {
        List_new(&lo);
        l = List_getLast(&lo);
        l->obj = malloc(sizeof(Text));
        ((Text *)l->obj)->type = _loadText_getString(li->obj, "type");
        ((Text *)l->obj)->font = _loadText_getString(li->obj, "font");
        ((Text *)l->obj)->scene = _loadText_getString(li->obj, "scene");
        ((Text *)l->obj)->text = _loadText_getString(li->obj, "text");
        _loadText_initSfText(l->obj, li->obj);
        li = li->next;
    }
    return lo;
}

void Text_free(Text *obj)
{
    free(obj->type);
    free(obj->scene);
    free(obj->text);
    free(obj->font);
    //sfText_destroy(obj->sfText);
    free(obj);
}

void ListOfText_free(List **list)
{
    List *listLocal = List_getLast(list);
    while (listLocal) {
        Text_free(listLocal->obj);
        List *prev = listLocal->prev;
        free(listLocal);
        listLocal = prev;
    }
    *list = 0;
}
