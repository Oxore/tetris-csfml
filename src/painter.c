#include <f8.h>
#include <SFML/Graphics/RenderWindow.h>
#include <SFML/Graphics/RectangleShape.h>
#include <SFML/Graphics/Font.h>
#include <SFML/Graphics/Text.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "tet_conf.h"
#include "vector.h"
#include "text.h"
#include "field.h"
#include "painter.h"

#include "idlist.h"

static sfColor shape_color_map[] = {
    UIBGCOLOR,
    LCOLOR,
    RLCOLOR,
    ZCOLOR,
    SCOLOR,
    BCOLOR,
    ICOLOR,
    TCOLOR,
};

enum t {
    TYPE_U,
    TYPE_FIELD,
    TYPE_TEXT
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

static sfRenderWindow *window;
static sfFont *font;

static struct idlist *drawables = NULL;

void painter_set_window(sfRenderWindow *w)
{
    window = w;
}

void painter_load_font(char *filename)
{
    font = sfFont_createFromFile(filename);
    if (!font) {
        printf("%s font load failed", filename);
        exit(EXIT_FAILURE);
    }
}

void painter_destroy_font()
{
    sfFont_destroy(font);
}

size_t painter_register_field(struct field *fld)
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
            cell_pos.x = fld->pos.x + (i * (CELL_SIZE.x + 2 * OUT_THICK));
            cell_pos.y = fld->pos.y - (j * (CELL_SIZE.y + 2 * OUT_THICK));
            sfRectangleShape_setPosition(f->p[j][i], cell_pos);
            sfRectangleShape_setFillColor(f->p[j][i], (sfColor)UIBGCOLOR);
            sfRectangleShape_setSize(f->p[j][i], CELL_SIZE);
            sfRectangleShape_setOutlineColor(f->p[j][i],
                    (sfColor)UIFGACTIVECOLOR);
            sfRectangleShape_setOutlineThickness(f->p[j][i], OUT_THICK);
        }
    }

    last->obj = f;
    return last->id;
}

void painter_update_field(size_t id, struct field *fld)
{
    struct idlist *node = list_get(drawables, id);
    if (!node)
        return;
    struct field_drawable *f = node->obj;
    f->attr = fld->attr;
    for (size_t j = 0; j < fld->size.y; j++) {
        for (size_t i = 0; i < fld->size.x; i++) {
            sfVector2f cell_pos;
            cell_pos.x = fld->pos.x + (i * (CELL_SIZE.x + 2 * OUT_THICK));
            cell_pos.y = fld->pos.y - (j * (CELL_SIZE.y + 2 * OUT_THICK));
            sfRectangleShape_setPosition(f->p[j][i], cell_pos);
            if (fld->c[j][i].a) {
                sfRectangleShape_setFillColor(f->p[j][i],
                        shape_color_map[fld->c[j][i].color]);
                sfRectangleShape_setOutlineColor(f->p[j][i],
                        (sfColor)UIFGACTIVECOLOR);
            } else if (f->attr & FLD_ATTR_HIDE_EMPTY_CELLS) {
                sfRectangleShape_setFillColor(f->p[j][i],
                        (sfColor)UITRANSPARENT);
                sfRectangleShape_setOutlineColor(f->p[j][i],
                        (sfColor)UITRANSPARENT);
            } else {
                sfRectangleShape_setFillColor(f->p[j][i], (sfColor)UIBGCOLOR);
                sfRectangleShape_setOutlineColor(f->p[j][i],
                        (sfColor)UIFGINACTIVECOLOR);
            }
        }
    }

    for (size_t s = 0; s < fld->shape_cnt; ++s) {
        sfColor fill_color = shape_color_map[fld->shape[s].t];
        sfColor outline_color = (sfColor)UIFGACTIVECOLOR;

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
        struct text *txt)
{
    t->attr = txt->attr;
    sfText_setCharacterSize(t->text, txt->size);
    sfVector2f pos = (sfVector2f){.x = txt->pos.x, .y = txt->pos.y};
    sfText_setPosition(t->text, pos);
    if (t->text) {
        sfUint32 buf[BUFSIZ];
        buf[sizeof(buf)/sizeof(*buf) - 1] = 0;
        utf8to32_strcpy_s((int32_t *)buf, sizeof(buf), txt->text);
        sfText_setUnicodeString(t->text, buf);
    }
}

size_t painter_register_text(struct text *txt)
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

void painter_update_text(size_t id, struct text *txt)
{
    struct idlist *node = list_get(drawables, id);
    if (node)
        painter_update_text_drawable(node->obj, txt);
}

static void draw_text_drawable(struct drawable *d)
{
    struct text_drawable *t = (struct text_drawable *)d;
    if (!(t->attr & TXT_ATTR_INVISIBLE))
        sfRenderWindow_drawText(window, t->text, NULL);
}

static void destroy_text_drawable(struct drawable *d)
{
    struct text_drawable *t = (struct text_drawable *)d;
    sfText_destroy(t->text);
    free(t);
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
    case TYPE_U:
        fprintf(stderr, "ERROR: Unknown type of drawable\n");
        exit(EXIT_FAILURE);
        break;
    }
}

void painter_draw()
{
    sfRenderWindow_clear(window, (sfColor)UIBGCOLOR);

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
