#include <SFML/Graphics/RenderWindow.h>
#include <SFML/Graphics/RectangleShape.h>
#include <stdlib.h>

#include "tet_conf.h"
#include "field.h"
#include "draw.h"

struct field_drawable {
    sfRectangleShape *p[FLD_BOUND_Y][FLD_SIZE_X];
};

struct idlist {
    unsigned long id;
    void        *obj;
    struct idlist *next;
};

static struct idlist *list_new()
{
    struct idlist *list = calloc(1, sizeof(struct idlist));
    list->id = 1;
    return list;
}

static struct idlist *list_append(struct idlist *list)
{
    unsigned long id = list->id + 1;
    struct idlist *last = list;
    while (last->next)
        last = last->next;
    last->next = calloc(1, sizeof(struct idlist));
    last = last->next;
    last->id = id;
    return last;
}

static struct idlist *list_get(const struct idlist *list, unsigned long id)
{
    const struct idlist *sought = list;
    if (sought) {
        if (sought->id == id)
            return (struct idlist *)sought;
        while (sought->next) {
            sought = sought->next;
            if (sought->id == id)
                return (struct idlist *)sought;
        }
    }
    return NULL;
}

#define BUFSIZE 100

/* Must be static in future */
struct window w;

static struct idlist *fields_list = NULL;

void painter_init_window()
{
    w = (struct window){.mode = {450, 570, 32}};
    w.window = sfRenderWindow_create(w.mode, windowName_conf,
                   sfResize | sfClose, NULL);
    if (!w.window)
        exit(EXIT_FAILURE);
}

unsigned long painter_register_field()
{
    struct idlist *last;
    if (!fields_list) {
        last = fields_list = list_new();
    } else
        last = list_append(fields_list);

    struct field_drawable *f = calloc(1, sizeof(struct field_drawable));
    for (int j = 0; j < FLD_SIZE_Y; j++)
        for (int i = 0; i < FLD_SIZE_X; i++) {
            f->p[j][i] = sfRectangleShape_create();
            sfVector2f cell_pos;
            cell_pos.x = FLD_POS.x + (i * (CELL_SIZE.x + 2 * OUT_THICK));
            cell_pos.y = FLD_POS.y - (j * (CELL_SIZE.y + 2 * OUT_THICK));
            sfRectangleShape_setFillColor(f->p[j][i], UIBGCOLOR);
            sfRectangleShape_setSize(f->p[j][i], CELL_SIZE);
            sfRectangleShape_setPosition(f->p[j][i], cell_pos);
            sfRectangleShape_setOutlineColor(f->p[j][i], UIFGACTIVECOLOR);
            sfRectangleShape_setOutlineThickness(f->p[j][i], OUT_THICK);
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
    for (int j = 0; j < fld->size.y; j++) {
        for (int i = 0; i < fld->size.x; i++) {
            switch (fld->c[j][i].color) {
                case 1 :
                    sfRectangleShape_setFillColor(f->p[j][i], LCOLOR);
                    break;
                case 2 :
                    sfRectangleShape_setFillColor(f->p[j][i], RLCOLOR);
                    break;
                case 3 :
                    sfRectangleShape_setFillColor(f->p[j][i], ZCOLOR);
                    break;
                case 4 :
                    sfRectangleShape_setFillColor(f->p[j][i], SCOLOR);
                    break;
                case 5 :
                    sfRectangleShape_setFillColor(f->p[j][i], BCOLOR);
                    break;
                case 6 :
                    sfRectangleShape_setFillColor(f->p[j][i], ICOLOR);
                    break;
                case 7 :
                    sfRectangleShape_setFillColor(f->p[j][i], TCOLOR);
                    break;
            }
            sfRectangleShape_setOutlineColor(f->p[j][i], UIFGACTIVECOLOR);
        }
    }
}

static void draw_field(struct field_drawable *f)
{
    for (unsigned int j = 0; j < FLD_SIZE_Y; j++)
        for (unsigned int i = 0; i < FLD_SIZE_X; i++)
            sfRenderWindow_drawRectangleShape(w.window, f->p[j][i], NULL);
}

static void draw_fields()
{
    struct idlist *node = fields_list;
    if (node) {
        if (node->obj)
            draw_field(node->obj);
        while (node->next) {
            node = node->next;
            if (node)
                if (node->obj)
                    draw_field(node->obj);
        }
    }
}

void painter_draw()
{
    draw_fields();
}

void painter_destroy_window()
{
    sfRenderWindow_destroy(w.window);
}
