#include <SFML/Graphics/RenderWindow.h>
#include <SFML/Graphics/RectangleShape.h>
#include <stdlib.h>

#include "tet_conf.h"
#include "field.h"
#include "draw.h"

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


struct field_drawable {
    sfRectangleShape ***p;
    struct vector2ui size;
    unsigned int attr;
};

/*
 * TODO: Must be static in future
 *
 * */
struct window w;

static struct idlist *fields_list = NULL;

void painter_init_window()
{
    w = (struct window){.mode = {450, 570, 32}};
    w.window = sfRenderWindow_create(w.mode, windowName_conf,
                   sfResize | sfClose, NULL);
    if (!w.window)
        exit(EXIT_FAILURE);
    sfRenderWindow_setFramerateLimit(w.window, 60);
}

void painter_destroy_window()
{
    if (w.window) {
        sfRenderWindow_destroy(w.window);
        w.window = 0;
    }
}

unsigned long painter_register_field(struct field *fld)
{
    struct idlist *last;
    if (!fields_list)
        last = fields_list = list_new();
    else
        last = list_append(fields_list);

    struct field_drawable *f = calloc(1, sizeof(struct field_drawable));
    f->size = fld->size;
    f->p = calloc(f->size.y, sizeof(sfRectangleShape **));
    for (unsigned int j = 0; j < f->size.y; j++) {
        f->p[j] = calloc(f->size.x, sizeof(sfRectangleShape *));
        for (unsigned int i = 0; i < f->size.x; i++) {
            f->p[j][i] = sfRectangleShape_create();
            sfVector2f cell_pos;
            cell_pos.x = fld->pos.x + (i * (CELL_SIZE.x + 2 * OUT_THICK));
            cell_pos.y = fld->pos.y - (j * (CELL_SIZE.y + 2 * OUT_THICK));
            sfRectangleShape_setPosition(f->p[j][i], cell_pos);
            sfRectangleShape_setFillColor(f->p[j][i], (sfColor)UIBGCOLOR);
            sfRectangleShape_setSize(f->p[j][i], CELL_SIZE);
            sfRectangleShape_setOutlineColor(f->p[j][i], (sfColor)UIFGACTIVECOLOR);
            sfRectangleShape_setOutlineThickness(f->p[j][i], OUT_THICK);
        }
    }

    last->obj = f;
    return last->id;
}

void painter_update_field(unsigned long id, struct field *fld)
{
    struct idlist *node = list_get(fields_list, id);
    if (!node)
        return;
    struct field_drawable *f = node->obj;
    f->attr = fld->attr;
    for (unsigned int j = 0; j < fld->size.y; j++) {
        for (unsigned int i = 0; i < fld->size.x; i++) {
            sfVector2f cell_pos;
            cell_pos.x = fld->pos.x + (i * (CELL_SIZE.x + 2 * OUT_THICK));
            cell_pos.y = fld->pos.y - (j * (CELL_SIZE.y + 2 * OUT_THICK));
            sfRectangleShape_setPosition(f->p[j][i], cell_pos);
            if (fld->c[j][i].a) {
                sfRectangleShape_setFillColor(f->p[j][i], shape_color_map[fld->c[j][i].color]);
                sfRectangleShape_setOutlineColor(f->p[j][i], (sfColor)UIFGACTIVECOLOR);
            } else if (f->attr & FLD_ATTR_TRANSPARENT) {
                sfRectangleShape_setFillColor(f->p[j][i], (sfColor)UITRANSPARENT);
                sfRectangleShape_setOutlineColor(f->p[j][i], (sfColor)UITRANSPARENT);
            } else {
                sfRectangleShape_setFillColor(f->p[j][i], (sfColor)UIBGCOLOR);
                sfRectangleShape_setOutlineColor(f->p[j][i], (sfColor)UIFGINACTIVECOLOR);
            }
        }
    }
    for (unsigned int s = 0; s < fld->shape_cnt; ++s)
        for (int j = 0; j < 4; j++)
            for (int i = 0; i < 4; i++)
                if (fld->shape[s].c[j][i] && j + fld->shape[s].y < (int)fld->size.y) {
                    sfRectangleShape_setFillColor(
                            f->p[j + fld->shape[s].y][i + fld->shape[s].x],
                            shape_color_map[fld->shape[s].color]);
                    sfRectangleShape_setOutlineColor(
                            f->p[j + fld->shape[s].y][i + fld->shape[s].x],
                            (sfColor)UIFGACTIVECOLOR);
                }
}

static void draw_field(void *field)
{
    struct field_drawable *f = field;
    if (!(f->attr & FLD_ATTR_INVISIBLE))
        for (unsigned int j = 0; j < f->size.y; j++)
            for (unsigned int i = 0; i < f->size.x; i++)
                sfRenderWindow_drawRectangleShape(w.window, f->p[j][i], NULL);
}

static void draw_fields()
{
    list_foreach(fields_list, draw_field);
}

static void destroy_field(void *field)
{
    struct field_drawable *f = field;
    for (unsigned int j = 0; j < f->size.y; j++) {
        for (unsigned int i = 0; i < f->size.x; i++)
             sfRectangleShape_destroy(f->p[j][i]);
        free(f->p[j]);
    }
    free(f->p);
    free(f);
}

void painter_destroy_field(unsigned long id)
{
    struct idlist *node = list_get(fields_list, id);
    destroy_field(node->obj);
    list_rm_node(node);
}

void painter_destroy_fields()
{
    list_foreach(fields_list, destroy_field);
}

void painter_draw()
{
    draw_fields();
}

void painter_destroy_drawables()
{
    painter_destroy_fields();
}

void painter_destroy_all()
{
    painter_destroy_drawables();
    painter_destroy_window();
}
