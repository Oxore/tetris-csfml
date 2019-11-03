#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cJSON.h"

#include "config.h"
#include "media.h"

struct config g_config = {
    .keys = {
        .rotate_left = KEY_UNKNOWN,
        .rotate_right = KEY_UP,
        .move_left = KEY_LEFT,
        .move_right = KEY_RIGHT,
        .move_down = KEY_DOWN,
        .drop = KEY_SPACE,
        .pause = KEY_P,
        .start = KEY_S,
    },
};

struct config_key_name {
    const char *name;
    enum key_id key;
};

static struct config_key_name config_keymap[] = {
    { .name = "",                     .key = KEY_UNKNOWN,       },
    { .name = "key_a",                .key = KEY_A,             },
    { .name = "key_b",                .key = KEY_B,             },
    { .name = "key_c",                .key = KEY_C,             },
    { .name = "key_d",                .key = KEY_D,             },
    { .name = "key_e",                .key = KEY_E,             },
    { .name = "key_f",                .key = KEY_F,             },
    { .name = "key_g",                .key = KEY_G,             },
    { .name = "key_h",                .key = KEY_H,             },
    { .name = "key_i",                .key = KEY_I,             },
    { .name = "key_j",                .key = KEY_J,             },
    { .name = "key_k",                .key = KEY_K,             },
    { .name = "key_l",                .key = KEY_L,             },
    { .name = "key_m",                .key = KEY_M,             },
    { .name = "key_n",                .key = KEY_N,             },
    { .name = "key_o",                .key = KEY_O,             },
    { .name = "key_p",                .key = KEY_P,             },
    { .name = "key_q",                .key = KEY_Q,             },
    { .name = "key_r",                .key = KEY_R,             },
    { .name = "key_s",                .key = KEY_S,             },
    { .name = "key_t",                .key = KEY_T,             },
    { .name = "key_u",                .key = KEY_U,             },
    { .name = "key_v",                .key = KEY_V,             },
    { .name = "key_w",                .key = KEY_W,             },
    { .name = "key_x",                .key = KEY_X,             },
    { .name = "key_y",                .key = KEY_Y,             },
    { .name = "key_z",                .key = KEY_Z,             },
    { .name = "key_0",                .key = KEY_0,             },
    { .name = "key_1",                .key = KEY_1,             },
    { .name = "key_2",                .key = KEY_2,             },
    { .name = "key_3",                .key = KEY_3,             },
    { .name = "key_4",                .key = KEY_4,             },
    { .name = "key_5",                .key = KEY_5,             },
    { .name = "key_6",                .key = KEY_6,             },
    { .name = "key_7",                .key = KEY_7,             },
    { .name = "key_8",                .key = KEY_8,             },
    { .name = "key_9",                .key = KEY_9,             },
    { .name = "key_escape",           .key = KEY_ESCAPE,        },
    { .name = "key_left_control",     .key = KEY_LEFT_CONTROL,  },
    { .name = "key_left_shift",       .key = KEY_LEFT_SHIFT,    },
    { .name = "key_left_alt",         .key = KEY_LEFT_ALT,      },
    { .name = "key_left_system",      .key = KEY_LEFT_SYSTEM,   },
    { .name = "key_right_control",    .key = KEY_RIGHT_CONTROL, },
    { .name = "key_right_shift",      .key = KEY_RIGHT_SHIFT,   },
    { .name = "key_right_alt",        .key = KEY_RIGHT_ALT,     },
    { .name = "key_right_system",     .key = KEY_RIGHT_SYSTEM,  },
    { .name = "key_menu",             .key = KEY_MENU,          },
    { .name = "key_left_bracket",     .key = KEY_LEFT_BRACKET,  },
    { .name = "key_right_bracket",    .key = KEY_RIGHT_BRACKET, },
    { .name = "key_semicolon",        .key = KEY_SEMICOLON,     },
    { .name = "key_comma",            .key = KEY_COMMA,         },
    { .name = "key_period",           .key = KEY_PERIOD,        },
    { .name = "key_quote",            .key = KEY_QUOTE,         },
    { .name = "key_slash",            .key = KEY_SLASH,         },
    { .name = "key_backslash",        .key = KEY_BACKSLASH,     },
    { .name = "key_tilde",            .key = KEY_TILDE,         },
    { .name = "key_equal",            .key = KEY_EQUAL,         },
    { .name = "key_hyphen",           .key = KEY_HYPHEN,        },
    { .name = "key_space",            .key = KEY_SPACE,         },
    { .name = "key_enter",            .key = KEY_ENTER,         },
    { .name = "key_backspace",        .key = KEY_BACKSPACE,     },
    { .name = "key_tab",              .key = KEY_TAB,           },
    { .name = "key_page_up",          .key = KEY_PAGE_UP,       },
    { .name = "key_page_down",        .key = KEY_PAGE_DOWN,     },
    { .name = "key_end",              .key = KEY_END,           },
    { .name = "key_home",             .key = KEY_HOME,          },
    { .name = "key_insert",           .key = KEY_INSERT,        },
    { .name = "key_delete",           .key = KEY_DELETE,        },
    { .name = "key_add",              .key = KEY_NUM_PLUS,      },
    { .name = "key_subtract",         .key = KEY_NUM_MINUS,     },
    { .name = "key_multiply",         .key = KEY_NUM_STAR,      },
    { .name = "key_divide",           .key = KEY_NUM_SLASH,     },
    { .name = "key_left",             .key = KEY_LEFT,          },
    { .name = "key_right",            .key = KEY_RIGHT,         },
    { .name = "key_up",               .key = KEY_UP,            },
    { .name = "key_down",             .key = KEY_DOWN,          },
    { .name = "key_num0",             .key = KEY_NUM0,          },
    { .name = "key_num1",             .key = KEY_NUM1,          },
    { .name = "key_num2",             .key = KEY_NUM2,          },
    { .name = "key_num3",             .key = KEY_NUM3,          },
    { .name = "key_num4",             .key = KEY_NUM4,          },
    { .name = "key_num5",             .key = KEY_NUM5,          },
    { .name = "key_num6",             .key = KEY_NUM6,          },
    { .name = "key_num7",             .key = KEY_NUM7,          },
    { .name = "key_num8",             .key = KEY_NUM8,          },
    { .name = "key_num9",             .key = KEY_NUM9,          },
    { .name = "key_f1",               .key = KEY_F1,            },
    { .name = "key_f2",               .key = KEY_F2,            },
    { .name = "key_f3",               .key = KEY_F3,            },
    { .name = "key_f4",               .key = KEY_F4,            },
    { .name = "key_f5",               .key = KEY_F5,            },
    { .name = "key_f6",               .key = KEY_F6,            },
    { .name = "key_f7",               .key = KEY_F7,            },
    { .name = "key_f8",               .key = KEY_F8,            },
    { .name = "key_f9",               .key = KEY_F9,            },
    { .name = "key_f10",              .key = KEY_F10,           },
    { .name = "key_f11",              .key = KEY_F11,           },
    { .name = "key_f12",              .key = KEY_F12,           },
    { .name = "key_f13",              .key = KEY_F13,           },
    { .name = "key_f14",              .key = KEY_F14,           },
    { .name = "key_f15",              .key = KEY_F15,           },
    { .name = "key_pause",            .key = KEY_PAUSE,         },
};

static enum key_id string_to_key(const char *string)
{
    assert(string);

    if (string == NULL)
        return KEY_UNKNOWN;

    for (size_t i = 0; i < sizeof(config_keymap)/sizeof(*config_keymap); i++)
        if (!strncmp(config_keymap[i].name, string, CFG_NAME_MAX))
            return config_keymap[i].key;

    return KEY_UNKNOWN;
}

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

static const char *cjson_get_string(cJSON *object, const char *key)
{
    cJSON *item = cJSON_GetObjectItem(object, key);
    if (cJSON_IsString(item))
        return cJSON_GetStringValue(item);

    return NULL;
}

static int keys_load_from_cjson(struct config_keys *keys, cJSON *keys_jo)
{
    keys->rotate_left = string_to_key(
            cjson_get_string(keys_jo, "rotate_left"));
    keys->rotate_right = string_to_key(
            cjson_get_string(keys_jo, "rotate_right"));
    keys->move_down = string_to_key(
            cjson_get_string(keys_jo, "move_down"));
    keys->move_left = string_to_key(
            cjson_get_string(keys_jo, "move_left"));
    keys->move_right = string_to_key(
            cjson_get_string(keys_jo, "move_right"));
    keys->drop = string_to_key(
            cjson_get_string(keys_jo, "drop"));
    keys->pause = string_to_key(
            cjson_get_string(keys_jo, "pause"));
    keys->start = string_to_key(
            cjson_get_string(keys_jo, "start"));
    return 0;
}

int config_load_from_json_file(struct config *config, const char *filename)
{
    struct cJSON_Hooks hooks = (struct cJSON_Hooks){
        .malloc_fn = malloc,
        .free_fn = free,
    };
    cJSON_InitHooks(&hooks);

    char *data = load_data_from_file(filename);
    if (data == NULL) {
        return -1;
    }

    cJSON *json_data = cJSON_Parse(data);
    if (json_data == NULL) {
        fprintf(stderr, "Error parsing json file \"%s\"\n", filename);
        return -1;
    }

    if (cJSON_HasObjectItem(json_data, "keys")) {
        cJSON *keys_jo = cJSON_GetObjectItemCaseSensitive(json_data, "keys");
        if (keys_load_from_cjson(&config->keys, keys_jo) == -1) {
            fprintf(stderr, "invalid keys configuration\n");
            return -1;
        }
    }

    free(data);
    cJSON_Delete(json_data);
    return 0;
}
