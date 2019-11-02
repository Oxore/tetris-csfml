#include <assert.h>
#include <f8.h>
#include <SFML/Graphics/RenderWindow.h>
#include <SFML/Graphics/RectangleShape.h>
#include <SFML/Graphics/Font.h>
#include <SFML/Graphics/Text.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "config.h"
#include "vector.h"
#include "text.h"
#include "field.h"
#include "input.h"
#include "hs_table.h"
#include "painter.h"

#include "idlist.h"

static sfColor shape_color_map[] = {
    CFG_UI_BG_COLOR,
    CFG_SHAPE_L_COLOR,
    CFG_SHAPE_RL_COLOR,
    CFG_SHAPE_Z_COLOR,
    CFG_SHAPE_S_COLOR,
    CFG_SHAPE_B_COLOR,
    CFG_SHAPE_I_COLOR,
    CFG_SHAPE_T_COLOR,
};

enum t {
    TYPE_U,
    TYPE_FIELD,
    TYPE_TEXT,
    TYPE_INPUT,
    TYPE_HS_TABLE,
};

struct drawable {
    enum t t;
};

struct field_drawable {
    struct drawable;

    sfRectangleShape ***p;
    struct vector2ui    size;
    size_t        attr;
};

struct text_drawable {
    struct drawable;

    sfText *text;
    size_t attr;
};

struct input_drawable {
    struct drawable;

    sfText *text;
    size_t attr;
};

struct hs_table_drawable {
    struct drawable;

    struct {
        sfText *score;
        sfText *name;
    } lines[CFG_HS_TABLE_SIZE];

    size_t attr;
};

static sfRenderWindow *window;
static sfFont *font;

static struct idlist *drawables = NULL;

void painter_set_window(sfRenderWindow *w)
{
    window = w;
}

void painter_load_font(const char *filename)
{
    font = sfFont_createFromFile(filename);
    if (!font) {
        printf("%s font load failed", filename);
        // TODO: return -1 instead of exiting
        exit(EXIT_FAILURE);
    }
}

void painter_destroy_font()
{
    sfFont_destroy(font);
}

size_t painter_register_field(const struct field *fld)
{
    struct idlist *last;
    if (!drawables)
        last = drawables = list_new();
    else
        last = list_append(drawables);

    struct field_drawable *f = calloc(1, sizeof(struct field_drawable));
    f->t = TYPE_FIELD;
    f->size = fld->size;
    f->p = calloc(f->size.y, sizeof(sfRectangleShape **));
    for (size_t j = 0; j < f->size.y; j++) {
        f->p[j] = calloc(f->size.x, sizeof(sfRectangleShape *));
        for (size_t i = 0; i < f->size.x; i++) {
            f->p[j][i] = sfRectangleShape_create();
            sfVector2f cell_pos;
            cell_pos.x = fld->pos.x + (i * (CFG_CELL_SIZE_X + 2 * CFG_OUT_THICK));
            cell_pos.y = fld->pos.y - (j * (CFG_CELL_SIZE_Y + 2 * CFG_OUT_THICK));
            sfRectangleShape_setPosition(f->p[j][i], cell_pos);
            sfRectangleShape_setFillColor(f->p[j][i], (sfColor)CFG_UI_BG_COLOR);
            sfRectangleShape_setSize(f->p[j][i], CFG_CELL_SIZE);
            sfRectangleShape_setOutlineColor(f->p[j][i],
                    (sfColor)CFG_UI_FGACTIVE_COLOR);
            sfRectangleShape_setOutlineThickness(f->p[j][i], CFG_OUT_THICK);
        }
    }

    last->obj = f;
    return last->id;
}

void painter_update_field(size_t id, const struct field *fld)
{
    struct idlist *node = list_get(drawables, id);
    if (!node)
        return;
    struct field_drawable *f = node->obj;
    f->attr = fld->attr;
    for (size_t j = 0; j < fld->size.y; j++) {
        for (size_t i = 0; i < fld->size.x; i++) {
            sfVector2f cell_pos;
            cell_pos.x
                = fld->pos.x + (i * (CFG_CELL_SIZE_X + 2 * CFG_OUT_THICK));
            cell_pos.y
                = fld->pos.y - (j * (CFG_CELL_SIZE_Y + 2 * CFG_OUT_THICK));
            sfRectangleShape_setPosition(f->p[j][i], cell_pos);
            if (fld->c[j][i].a) {
                sfRectangleShape_setFillColor(f->p[j][i],
                        shape_color_map[fld->c[j][i].color]);
                sfRectangleShape_setOutlineColor(f->p[j][i],
                        (sfColor)CFG_UI_FGACTIVE_COLOR);
            } else if (f->attr & FLD_ATTR_HIDE_EMPTY_CELLS) {
                sfRectangleShape_setFillColor(f->p[j][i],
                        (sfColor)CFG_UI_TRANSPARENT);
                sfRectangleShape_setOutlineColor(f->p[j][i],
                        (sfColor)CFG_UI_TRANSPARENT);
            } else {
                sfRectangleShape_setFillColor(f->p[j][i],
                        (sfColor)CFG_UI_BG_COLOR);
                sfRectangleShape_setOutlineColor(f->p[j][i],
                        (sfColor)CFG_UI_FGINACTIVE_COLOR);
            }
        }
    }

    for (size_t s = 0; s < fld->shape_cnt; ++s) {
        sfColor fill_color = shape_color_map[fld->shape[s].t];
        sfColor outline_color = (sfColor)CFG_UI_FGACTIVE_COLOR;

        if (fld->shape[s].attr && SHP_ATTR_GHOST) {
            fill_color.a = 100;
            outline_color.a = 100;
        }

        for (int j = 0; j < 4; j++)
            for (int i = 0; i < 4; i++)
                if (fld->shape[s].c[j][i] && j + fld->shape[s].y
                 < (int)fld->size.y) {
                    sfRectangleShape_setFillColor(
                            f->p[j + fld->shape[s].y][i + fld->shape[s].x],
                            fill_color);
                    sfRectangleShape_setOutlineColor(
                            f->p[j + fld->shape[s].y][i + fld->shape[s].x],
                            outline_color);
                }
    }
}

static void draw_field_drawable(struct drawable *d)
{
    struct field_drawable *f = (void *)d;
    if (!(f->attr & FLD_ATTR_INVISIBLE))
        for (size_t j = 0; j < f->size.y; j++)
            for (size_t i = 0; i < f->size.x; i++)
                sfRenderWindow_drawRectangleShape(window, f->p[j][i], NULL);
}

static void destroy_field_drawable(struct drawable *d)
{
    struct field_drawable *f = (void *)d;
    for (size_t j = 0; j < f->size.y; j++) {
        for (size_t i = 0; i < f->size.x; i++)
             sfRectangleShape_destroy(f->p[j][i]);
        free(f->p[j]);
    }
    free(f->p);
    free(f);
}

static void painter_update_text_drawable(struct text_drawable *t,
        const struct text *txt)
{
    t->attr = txt->attr;
    sfText_setCharacterSize(t->text, txt->size);
    sfVector2f pos = (sfVector2f){.x = txt->pos.x, .y = txt->pos.y};
    sfText_setPosition(t->text, pos);

    if (t->text && txt->text) {
        sfUint32 buf[BUFSIZ];
        buf[sizeof(buf)/sizeof(*buf) - 1] = 0;
        utf8to32_strcpy_s((int32_t *)buf, sizeof(buf), txt->text);
        sfText_setUnicodeString(t->text, buf);
    }
}

size_t painter_register_text(const struct text *txt)
{
    struct idlist *last;
    if (!drawables)
        last = drawables = list_new();
    else
        last = list_append(drawables);

    struct text_drawable *t = calloc(1, sizeof(struct text_drawable));
    t->t = TYPE_TEXT;
    t->text = sfText_create();
    sfText_setFont(t->text, font);
    painter_update_text_drawable(t, txt);

    last->obj = t;
    return last->id;
}

void painter_update_text(size_t id, const struct text *txt)
{
    assert(txt);
    assert(txt->text);

    struct idlist *node = list_get(drawables, id);
    if (node && txt)
        painter_update_text_drawable(node->obj, txt);

}

static void painter_update_input_drawable(struct input_drawable *idrwbl,
        const struct input *input)
{
    idrwbl->attr = input->attr;
    sfText_setCharacterSize(idrwbl->text, input->fontsize);
    sfVector2f pos = (sfVector2f){.x = input->pos.x, .y = input->pos.y};
    sfText_setPosition(idrwbl->text, pos);
    if (idrwbl->text) {
        sfUint32 buf[BUFSIZ];
        buf[sizeof(buf)/sizeof(*buf) - 1] = 0;
        utf8to32_strcpy_s((int32_t *)buf, sizeof(buf), input->text);
        sfText_setUnicodeString(idrwbl->text, buf);
    }
}

size_t painter_register_input(const struct input *input)
{
    struct idlist *last;
    if (!drawables)
        last = drawables = list_new();
    else
        last = list_append(drawables);

    struct input_drawable *idrwbl = calloc(1, sizeof(struct input_drawable));
    idrwbl->t = TYPE_INPUT;
    idrwbl->text = sfText_create();
    sfText_setFont(idrwbl->text, font);
    painter_update_input_drawable(idrwbl, input);

    last->obj = idrwbl;
    return last->id;
}

static inline void set_utf8_to_sfText(sfText *sf_text,
        const char *text, size_t size)
{
    assert(text);
    assert(sf_text);

    sfUint32 buf[size * 4];
    buf[sizeof(buf)/sizeof(*buf) - 1] = 0;
    utf8to32_strncpy_s((int32_t *)buf, sizeof(buf), text, size);
    sfText_setUnicodeString(sf_text, buf);
}

static inline void set_uint_to_sfText(sfText *sf_text,
        unsigned value)
{
    assert(sf_text);

    // 128 should be enough for size_t number
    sfUint32 buf[128] = {0};
    char buf_utf8[128] = {0};

    snprintf(buf_utf8, sizeof(buf_utf8), "%d", value);
    buf[sizeof(buf)/sizeof(*buf) - 1] = 0;

    utf8to32_strncpy_s((int32_t *)buf, sizeof(buf), buf_utf8, sizeof(buf_utf8));
    sfText_setUnicodeString(sf_text, buf);
}

static void painter_update_hs_table_drawable(struct hs_table_drawable *hstdrwbl,
        const struct hs_table *hs_table)
{
    hstdrwbl->attr = hs_table->attr;

    sfVector2f pos;
    for (size_t i = 0; i < CFG_HS_TABLE_SIZE; i++) {
        pos = (sfVector2f){
            .x = hs_table->pos.x,
            .y = hs_table->pos.y + i * hs_table->score_cell_size.y,
        };
        sfText_setPosition(hstdrwbl->lines[i].score, pos);
        sfText_setCharacterSize(hstdrwbl->lines[i].score, hs_table->fontsize);
        set_uint_to_sfText(hstdrwbl->lines[i].score,
                hs_table->entries[i].score);

        pos = (sfVector2f){
            .x = hs_table->pos.x + hs_table->score_cell_size.x,
            .y = hs_table->pos.y + i * hs_table->name_cell_size.y,
        };
        sfText_setPosition(hstdrwbl->lines[i].name, pos);
        sfText_setCharacterSize(hstdrwbl->lines[i].name, hs_table->fontsize);
        set_utf8_to_sfText(hstdrwbl->lines[i].name,
                hs_table->entries[i].name, CFG_NAME_MAX);
    }

}

void painter_update_input(size_t id, const struct input *input)
{
    struct idlist *node = list_get(drawables, id);
    if (node)
        painter_update_input_drawable(node->obj, input);
}

size_t painter_register_hs_table(const struct hs_table *hs_table)
{
    struct idlist *last;
    if (!drawables)
        last = drawables = list_new();
    else
        last = list_append(drawables);

    struct hs_table_drawable *hstdrwbl
        = calloc(1, sizeof(struct hs_table_drawable));

    hstdrwbl->t = TYPE_HS_TABLE;

    for (size_t i = 0; i < CFG_HS_TABLE_SIZE; i++) {
        hstdrwbl->lines[i].score = sfText_create();
        sfText_setFont(hstdrwbl->lines[i].score, font);
        hstdrwbl->lines[i].name = sfText_create();
        sfText_setFont(hstdrwbl->lines[i].name, font);
    }

    painter_update_hs_table_drawable(hstdrwbl, hs_table);

    last->obj = hstdrwbl;
    return last->id;
}

void painter_update_hs_table(size_t id, const struct hs_table *hs_table)
{
    struct idlist *node = list_get(drawables, id);
    if (node)
        painter_update_hs_table_drawable(node->obj, hs_table);
}

static void draw_text_drawable(struct drawable *d)
{
    struct text_drawable *t = (struct text_drawable *)d;
    if (!(t->attr & TXT_ATTR_INVISIBLE))
        sfRenderWindow_drawText(window, t->text, NULL);
}

static void draw_input_drawable(struct drawable *d)
{
    // TODO: make fancier
    struct input_drawable *idrwbl = (struct input_drawable *)d;
    if (!(idrwbl->attr & INPUT_ATTR_INVISIBLE))
        sfRenderWindow_drawText(window, idrwbl->text, NULL);
}

static void draw_hs_table_drawable(struct drawable *d)
{
    struct hs_table_drawable *hstdrwbl = (struct hs_table_drawable *)d;
    if (!(hstdrwbl->attr & HS_TABLE_ATTR_INVISIBLE)) {
        for (size_t i = 0; i < CFG_HS_TABLE_SIZE; i++) {
            sfRenderWindow_drawText(window, hstdrwbl->lines[i].score, NULL);
            sfRenderWindow_drawText(window, hstdrwbl->lines[i].name, NULL);
        }
    }
}

static void destroy_text_drawable(struct drawable *d)
{
    struct text_drawable *t = (struct text_drawable *)d;
    sfText_destroy(t->text);
    free(t);
}

static void destroy_input_drawable(struct drawable *d)
{
    struct input_drawable *idrwbl = (struct input_drawable *)d;
    sfText_destroy(idrwbl->text);
    free(idrwbl);
}

static void destroy_hs_table_drawable(struct drawable *d)
{
    struct hs_table_drawable *hstdrwbl = (struct hs_table_drawable *)d;
    for (size_t i = 0; i < CFG_HS_TABLE_SIZE; i++) {
        sfText_destroy(hstdrwbl->lines[i].score);
        sfText_destroy(hstdrwbl->lines[i].name);
    }
    free(hstdrwbl);
}

static void draw_drawable(void *obj)
{
    struct drawable *d = obj;
    switch (d->t) {
    case TYPE_FIELD:
        draw_field_drawable(d);
        break;
    case TYPE_TEXT:
        draw_text_drawable(d);
        break;
    case TYPE_INPUT:
        draw_input_drawable(d);
        break;
    case TYPE_HS_TABLE:
        draw_hs_table_drawable(d);
        break;
    case TYPE_U:
        fprintf(stderr, "ERROR: Unknown type of drawable\n");
        exit(EXIT_FAILURE);
        break;
    }
}

void painter_draw()
{
    sfRenderWindow_clear(window, (sfColor)CFG_UI_BG_COLOR);

    LIST_FOREACH(drawables, drawable) {
        draw_drawable(drawable->obj);
    }

    sfRenderWindow_display(window);
}

static void destroy_drawable(void *obj)
{
    struct drawable *d = obj;
    switch (d->t) {
    case TYPE_FIELD:
        destroy_field_drawable(d);
        break;
    case TYPE_TEXT:
        destroy_text_drawable(d);
        break;
    case TYPE_INPUT:
        destroy_input_drawable(d);
        break;
    case TYPE_HS_TABLE:
        destroy_hs_table_drawable(d);
        break;
    case TYPE_U:
        fprintf(stderr, "ERROR: Unknown type of drawable\n");
        exit(EXIT_FAILURE);
        break;
    }
}

void painter_destroy_drawable(size_t id)
{
    struct idlist *node = list_get(drawables, id);
    destroy_drawable(node->obj);
    list_rm_node(node);
}

void painter_destroy_drawables()
{
    LIST_FOREACH(drawables, drawable) {
        destroy_drawable(drawable->obj);
    }

    list_destroy(drawables);
    drawables = 0;
}

void painter_destroy_all()
{
    painter_destroy_drawables();
    painter_destroy_font();
    window = 0;
}
