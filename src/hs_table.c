#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "cJSON.h"

#include "vector.h"
#include "tet_conf.h"
#include "hs_table.h"

static int store_data_to_file(const char *fname, const char *data,
        size_t size)
{
    FILE *file = NULL;

    file = fopen(fname, "w+");
    if (file == NULL) {
        perror(fname);
        return -1;
    }

    size_t nbyte = fwrite(data, sizeof(char), size, file);
    if (nbyte != size) {
        perror("fwrite");
    }

    fclose(file);
    return 0;
}

static char *load_data_from_file(const char *fname)
{
    int ret = 0;
    FILE *file = NULL;
    char *data = NULL;

    file = fopen(fname, "rb");
    if (file == NULL) {
        perror(fname);
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

static int hs_table_qsort_callback(const void *a, const void *b)
{
    const struct entry *e1 = a;
    const struct entry *e2 = b;

    return e2->score - e1->score;
}

static void hs_table_sort(struct hs_table *table)
{
    qsort(table->entries, table->nentries, sizeof(*table->entries),
            hs_table_qsort_callback);
}

void hs_table_insert(struct hs_table *table, const char *name, size_t score)
{
    assert(table);
    assert(name);
    assert(sizeof(table->entries));

    struct entry *e = &table->entries[table->nentries];

    if (table->nentries < sizeof(table->entries)/sizeof(*table->entries))
        table->nentries++;
    else if (e->score > score)
        return;

    e->score = score;
    strncpy(e->name, name, sizeof(e->name) - 1);
    e->name[sizeof(e->name) - 1] = 0;

    hs_table_sort(table);
}

static char *hs_table_cjson_get_string(cJSON *object, const char *key)
{
    cJSON *item = cJSON_GetObjectItem(object, key);
    if (cJSON_IsString(item))
        return cJSON_GetStringValue(item);
    return NULL;
}

static int hs_table_cjson_get_int(cJSON *object, const char *key)
{
    int value = 0;
    cJSON *item = cJSON_GetObjectItem(object, key);
    if (cJSON_IsNumber(item))
        value = item->valueint;
    return value;
}

int hs_table_load_from_json_file(struct hs_table *table, const char *fname)
{
    if (table == NULL || fname == NULL)
        return -1;

    struct cJSON_Hooks hooks = (struct cJSON_Hooks){
        .malloc_fn = malloc,
        .free_fn = free,
    };
    cJSON_InitHooks(&hooks);

    char *data = load_data_from_file(fname);
    if (data == NULL) {
        return -1;
    }

    cJSON *json_data = cJSON_Parse(data);
    free(data);

    if (cJSON_IsArray(json_data)) {
        size_t array_size = cJSON_GetArraySize(json_data);
        for (size_t i = 0; i < array_size; i++) {
            cJSON *item = cJSON_GetArrayItem(json_data, i);
            if (cJSON_IsObject(item)) {
                size_t score = (size_t)hs_table_cjson_get_int(item, "score");
                const char *name = hs_table_cjson_get_string(item, "name");

                assert(name != NULL);
                hs_table_insert(table, name, score);
            }
        }
    }
    cJSON_Delete(json_data);

    return 0;
}

int hs_table_save_to_json_file(const struct hs_table *table,
        const char *fname)
{
    if (table == NULL || fname == NULL)
        return -1;

    struct cJSON_Hooks hooks = (struct cJSON_Hooks){
        .malloc_fn = malloc,
        .free_fn = free,
    };
    cJSON_InitHooks(&hooks);

    cJSON *highscores = cJSON_CreateArray();

    for (size_t i = 0; i < table->nentries; i++) {
        cJSON *entry = cJSON_CreateObject();
        cJSON_AddStringToObject(entry, "name", table->entries[i].name);
        cJSON_AddNumberToObject(entry, "score", table->entries[i].score);
        cJSON_AddItemToArray(highscores, entry);
    }

    char *data = cJSON_Print(highscores);
    cJSON_Delete(highscores);
    store_data_to_file(fname, data, strlen(data));
    cJSON_free(data);

    return 0;
}
