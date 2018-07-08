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

static FILE *openFile(char *filename)
{
    FILE *file;
    if (!(file = fopen(filename, "rb"))){
        printf("ERROR: fild \"%s\"cannot be opened\n", filename);
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
                text->text = malloc(sizeof(char) * (strlen((char *)ev.data.scalar.value) + 1));
                strcpy(text->text, (char *)ev.data.scalar.value);
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
