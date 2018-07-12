#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SFML/Graphics/Font.h>
#include <SFML/Graphics/Text.h>
#include <SFML/Graphics.h>
#include <yaml.h>

#include "common.h"
#include "idlist.h"
#include "vector.h"
#include "text.h"

static inline unsigned int utf8_char_len(unsigned char c)
{
    if (c > 0x00 && c < 0xC0)
        return 1;
    else if (c >= 0xC2 && c < 0xE0)
        return 2;
    else if (c >= 0xE0 && c < 0xF0)
        return 3;
    else if (c >= 0xF0 && c < 0xF5)
        return 4;
    else
        return 0;
}

unsigned long utf8_strlen(void *string)
{
    unsigned long len = 0, keep = 0;
    for (unsigned char *c = string; *c; (keep ? --keep : ++len), ++c)
        if (!keep)
            keep = (keep = utf8_char_len(*c)) ? keep - 1 : keep;
    return len;
}

void utf8to32_strcpy(wchar_t *dest, char *src)
{
    wchar_t *dc = dest;
    char *c = src;
    unsigned long len = 0;
    while (*c) {
        int clen = utf8_char_len(*c);
        if (clen == 1) {
            dc[len] = c[0] & 0x7f;
        } else if (clen == 2) {
            dc[len] = ((c[0] & 0x1f) << 6) | ((c[1] & 0x3f) << 0);
        } else if (clen == 3) {
            dc[len] = ((c[0] & 0x0f) << 12) | ((c[1] & 0x3f) << 6) | ((c[2] & 0x3f) << 0);
        } else if (clen == 4) {
            dc[len] = ((c[0] & 0x07) << 18) | ((c[1] & 0x3f) << 12) | ((c[2] & 0x3f) << 6) | ((c[3] & 0x3f) << 0);
        } else {
            dc[len] = 0;
            return;
        }
        c += clen;
        ++len;
    }
    dc[len] = 0;
}

static FILE *openFile(char *filename)
{
    FILE *file;
    if (!(file = fopen(filename, "rb"))){
        printf("ERROR: file \"%s\"cannot be opened\n", filename);
        exit(EXIT_FAILURE);
    }
    return file;
}

struct idlist *load_texts(char *filename)
{
    FILE *file = openFile(filename);
    yaml_parser_t parser;
    yaml_event_t event;
    yaml_parser_initialize(&parser);
    yaml_parser_set_input_file(&parser, file);

    struct idlist *texts = NULL;
    struct idlist *texts_node = texts;
    do {
        if (!yaml_parser_parse(&parser, &event)) {
             fprintf(stderr, "Parser error %d\n", parser.error);
             exit(EXIT_FAILURE);
        }

        switch (event.type)
        {
        case YAML_MAPPING_START_EVENT:
            if (!texts) {
                texts = list_new();
                texts_node = texts;
            } else
                texts_node = list_append(texts);
            texts_node->obj = calloc(1, sizeof(struct text));
            break;
        case YAML_SCALAR_EVENT:
            ;yaml_event_t ev;
            yaml_parser_parse(&parser, &ev);
            while (ev.type != YAML_SCALAR_EVENT) {
                yaml_event_delete(&ev);
                if (!yaml_parser_parse(&parser, &ev)) {
                    fprintf(stderr, "Parser error %d\n", parser.error);
                    exit(EXIT_FAILURE);
                }
            }
            struct text *text = texts_node->obj;
            if (!strcmp((char *)event.data.scalar.value, "type")) {
                text->type = malloc(sizeof(char) * (strlen((char *)ev.data.scalar.value) + 1));
                strcpy(text->type, (char *)ev.data.scalar.value);
            } else if (!strcmp((char *)event.data.scalar.value, "scene")) {
                text->scene = malloc(sizeof(char) * (strlen((char *)ev.data.scalar.value) + 1));
                strcpy(text->scene, (char *)ev.data.scalar.value);
            } else if (!strcmp((char *)event.data.scalar.value, "text")) {
                text->text = calloc((utf8_strlen(ev.data.scalar.value)) + 1, sizeof(unsigned int));
                utf8to32_strcpy(text->text, (char *)ev.data.scalar.value);
            } else if (!strcmp((char *)event.data.scalar.value, "font")) {
                text->font = malloc(sizeof(char) * (strlen((char *)ev.data.scalar.value) + 1));
                strcpy(text->font, (char *)ev.data.scalar.value);
            } else if (!strcmp((char *)event.data.scalar.value, "size")) {
                text->size = atoi((char *)ev.data.scalar.value);
            } else if (!strcmp((char *)event.data.scalar.value, "x")) {
                text->pos.x = atoi((char *)ev.data.scalar.value);
            } else if (!strcmp((char *)event.data.scalar.value, "y")) {
                text->pos.y = atoi((char *)ev.data.scalar.value);
            }
            yaml_event_delete(&ev);
            break;
        default:
            break;
        }

        if (event.type != YAML_STREAM_END_EVENT)
            yaml_event_delete(&event);
    } while (event.type != YAML_STREAM_END_EVENT);

    yaml_event_delete(&event);
    yaml_parser_delete(&parser);
    fclose(file);
    return texts;
}

void text_destroy(void *obj)
{
    struct text *text = obj;
    if (text->type)
        free(text->type);
    if (text->scene)
        free(text->scene);
    if (text->text)
        free(text->text);
    if (text->font)
        free(text->font);
    free(text);
}
