#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <cJSON.h>

#include "idlist.h"
#include "vector.h"
#include "text.h"

static char *load_data_from_file(const char *filename)
{
    int ret = 0;
    FILE *file = NULL;
    char *data = NULL;

    file = fopen(filename, "rb");
    if (file == NULL) {
        perror(filename);
        return NULL;
    }

    ret |= fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    ret |= fseek(file, 0, SEEK_SET);
    if (ret == -1 || file_size == -1) {
        perror("fseek/ftell");
        goto cleanup_close;
    }

    data = calloc(file_size + 1, sizeof(char));
    if (data == NULL) {
        perror("calloc");
        goto cleanup_close;
    }

    size_t nbyte = fread(data, sizeof(char), file_size, file);
    if (nbyte == 0) {
        perror("fread");
        goto cleanup_free;
    }

    goto cleanup_close;

cleanup_free:
    free(data);
    data = NULL;

cleanup_close:
    fclose(file);
    return data;
}

static char *texts_cjson_get_string(cJSON *object, const char *key)
{
    char *string = NULL;
    cJSON *item = cJSON_GetObjectItem(object, key);
    if (cJSON_IsString(item)) {
        char *value = cJSON_GetStringValue(item);
        if (value != NULL) {
            size_t size = strlen(value) + 1;
            string = calloc(size, sizeof(char));
            if (string != NULL)
                strncpy(string, value, size);
        }
    }
    return string;
}

static int texts_cjson_get_int(cJSON *object, const char *key)
{
    int value = 0;
    cJSON *item = cJSON_GetObjectItem(object, key);
    if (cJSON_IsNumber(item))
        value = item->valueint;
    return value;
}

struct idlist *load_texts_from_json(const char *filename)
{
    struct cJSON_Hooks hooks = (struct cJSON_Hooks){
        .malloc_fn = malloc,
        .free_fn = free,
    };
    cJSON_InitHooks(&hooks);

    char *data = load_data_from_file(filename);
    if (data == NULL) {
        return NULL;
    }

    struct idlist *texts = NULL;
    struct idlist *texts_node = texts;

    cJSON *json_data = cJSON_Parse(data);
    if (cJSON_IsArray(json_data)) {
        size_t array_size = cJSON_GetArraySize(json_data);
        for (size_t i = 0; i < array_size; i++) {
            cJSON *item = cJSON_GetArrayItem(json_data, i);
            if (cJSON_IsObject(item)) {

                /* Allocate texts node to be filled */

                if (!texts) {
                    texts_node = texts = list_new();
                } else {
                    texts_node = list_append(texts);
                }
                texts_node->obj = calloc(1, sizeof(struct text));
                struct text *text = texts_node->obj;

                /* Fill the node */

                text->type = texts_cjson_get_string(item, "type");
                text->scene = texts_cjson_get_string(item, "scene");
                text->text = texts_cjson_get_string(item, "text");
                text->font = texts_cjson_get_string(item, "font");
                text->size = texts_cjson_get_int(item, "size");
                text->pos.x = texts_cjson_get_int(item, "x");
                text->pos.y = texts_cjson_get_int(item, "y");
            }
        }
    }
    free(data);
    cJSON_Delete(json_data);
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
