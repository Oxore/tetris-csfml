#include <stdlib.h>
#include <string.h>
#include <SFML/Graphics/RectangleShape.h>

#include "field.h"
#include "tet_conf.h"

/* Shapes maps */
extern char arrShapeL[4][4];
extern char arrShapeRL[4][4];
extern char arrShapeZ[4][4];
extern char arrShapeS[4][4];
extern char arrShapeB[4][4];
extern char arrShapeI[4][4];
extern char arrShapeT[4][4];

static void rotate_shape_left(struct shape *shape);
static void rotate_shape_right(struct shape *shape);
static int out_of_bounds(struct field *fld, struct shape *active);

void init_field(struct field *fld)
{
    sfVector2f fldCPos[22][10];
    for (int j = 0; j < fld->size.y; j++) {
        for (int i = 0; i < fld->size.x; i++) {
            fld->c[j][i].a = 0;    // Inactive = empty
            fldCPos[j][i].x
                = fld->pos.x + (i * (fld->cSize.x + 2 * fld->cOutThick));
            fldCPos[j][i].y
                = fld->pos.y - (j * (fld->cSize.y + 2 * fld->cOutThick));
            fld->p[j][i] = sfRectangleShape_create();
            sfRectangleShape_setFillColor(fld->p[j][i], UIBGCOLOR);
            sfRectangleShape_setSize(fld->p[j][i], fld->cSize);
            sfRectangleShape_setPosition(fld->p[j][i], fldCPos[j][i]);
            sfRectangleShape_setOutlineColor(fld->p[j][i], UIFGACTIVECOLOR);
            sfRectangleShape_setOutlineThickness(fld->p[j][i], fld->cOutThick);
        }
    }
}

void colorize_field(struct field *fld)
{
    for (int j = 0; j < fld->size.y; j++)
        for (int i = 0; i < fld->size.x; i++)
            if (fld->c[j][i].a) {
                sfRectangleShape_setFillColor(fld->p[j][i], fld->c[j][i].fColor);
                sfRectangleShape_setOutlineColor(fld->p[j][i], UIFGACTIVECOLOR);
            } else {
                sfRectangleShape_setFillColor(fld->p[j][i], UIBGCOLOR);
                sfRectangleShape_setOutlineColor(fld->p[j][i], UIFGINACTIVECOLOR);
            }
}

void colorize_field_random(struct field *fld)
{
    int a;
    for (int j = 0; j < fld->size.y; j++) {
        for (int i = 0; i < fld->size.x; i++) {
            a = rand() % 7 + 1;
            switch (a) {
                case 1 :
                    sfRectangleShape_setFillColor(fld->p[j][i], LCOLOR);
                    break;
                case 2 :
                    sfRectangleShape_setFillColor(fld->p[j][i], RLCOLOR);
                    break;
                case 3 :
                    sfRectangleShape_setFillColor(fld->p[j][i], ZCOLOR);
                    break;
                case 4 :
                    sfRectangleShape_setFillColor(fld->p[j][i], SCOLOR);
                    break;
                case 5 :
                    sfRectangleShape_setFillColor(fld->p[j][i], BCOLOR);
                    break;
                case 6 :
                    sfRectangleShape_setFillColor(fld->p[j][i], ICOLOR);
                    break;
                case 7 :
                    sfRectangleShape_setFillColor(fld->p[j][i], TCOLOR);
                    break;
            }
            sfRectangleShape_setOutlineColor(fld->p[j][i], UIFGACTIVECOLOR);
        }
    }
}

void init_next_shape_field(struct shape *next)
{
    sfVector2f nsPos;
    for (int j = 0; j < 4; j++) {
        for (int i = 0; i < 4; i++) {
            nsPos.x = next->x + i *(next->cSize.x + 2 * next->cOutThick);
            nsPos.y = next->y - j *(next->cSize.y + 2 * next->cOutThick);
            next->p[j][i] = sfRectangleShape_create();
            sfRectangleShape_setSize(next->p[j][i], next->cSize);
            sfRectangleShape_setPosition(next->p[j][i], nsPos);
            sfRectangleShape_setOutlineThickness(next->p[j][i], next->cOutThick);
        }
    }
}

/*
 * Inserts shape into field, runs filled lines searching, puts new shape
 * into the game at the top.
 *
 */
void putShape(struct field *fld, struct shape *active)
{
    for (int j = 0; j < 4; j++)
        for (int i = 0; i < 4; i++)
            if (active->c[j][i]) {
                fld->c[j+active->y][i+active->x].a = active->c[j][i];
                fld->c[j+active->y][i+active->x].fColor = active->fColor;
            }
}

int out_of_field(struct field *fld, struct shape *active)
{
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (active->c[i][j] && active->y + i >= fld->size.y)
                return 1;
    return 0;
}

void load_shape(struct shape *shape)
{
    switch (shape->t) { // Copy cell active/inactive state
        case 1 :
            memcpy(&shape->c[0][0], &arrShapeL[0][0], sizeof(char)*4*4);
            shape->fColor = LCOLOR;
            break;
        case 2 :
            memcpy(&shape->c[0][0], &arrShapeRL[0][0], sizeof(char)*4*4);
            shape->fColor = RLCOLOR;
            break;
        case 3 :
            memcpy(&shape->c[0][0], &arrShapeZ[0][0], sizeof(char)*4*4);
            shape->fColor = ZCOLOR;
            break;
        case 4 :
            memcpy(&shape->c[0][0], &arrShapeS[0][0], sizeof(char)*4*4);
            shape->fColor = SCOLOR;
            break;
        case 5 :
            memcpy(&shape->c[0][0], &arrShapeB[0][0], sizeof(char)*4*4);
            shape->fColor = BCOLOR;
            break;
        case 6 :
            memcpy(&shape->c[0][0], &arrShapeI[0][0], sizeof(char)*4*4);
            shape->fColor = ICOLOR;
            break;
        case 7 :
            memcpy(&shape->c[0][0], &arrShapeT[0][0], sizeof(char)*4*4);
            shape->fColor = TCOLOR;
            break;
    }
}

void resetActiveShape(struct field *fld, struct shape *active)
{
    load_shape(active);
    active->x = 3;
    if (active->t == 6)
        active->y = 19;
    else
        active->y = 18;
    while (collide(fld, active)) {
        active->y++;
    }
}

int collide(struct field *fld, struct shape *active)
{
    if (out_of_bounds(fld, active))
        return 1;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (active->c[i][j] && fld->c[i + active->y][j + active->x].a)
                return 1;
    return 0;
}

static int out_of_bounds(struct field *fld, struct shape *active)
{
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (active->c[i][j])
                if (active->x + j >= fld->bound.x || active->x + j < 0
                    || active->y + i >= fld->bound.y || active->y + i < 0)
                    return 1;
    return 0;
}

static void rotate_shape_left(struct shape *shape)
{
    char arr[4][4];
    memcpy(&arr[0][0], &shape->c[0][0], sizeof(char)*4*4);
    if (shape->t == 5)
        return;
    if (shape->t == 6) {
        for (int j = 3; j >= 0; j--)
            for (int i = 0; i < 4; i++)
                shape->c[j][i] = arr[3-i][3-j];
        return;
    }
    for (int j = 3; j > 0; j--)
        for (int i = 0; i < 3; i++)
            shape->c[j][i] = arr[3-i][j-1];
}

static void rotate_shape_right(struct shape *shape)
{
    char arr[4][4];
    memcpy(&arr[0][0], &shape->c[0][0], sizeof(char)*4*4);
    if (shape->t == 5)
        return;
    if (shape->t == 6) {
        for (int j = 3; j >= 0; j--)
            for (int i = 0; i < 4; i++)
                shape->c[j][i] = arr[3-i][3-j];
        return;
    }
    for (int j = 3; j > 0; j--)
        for (int i = 0; i < 3; i++)
            shape->c[j][i] = arr[i+1][3-j];
}

void rotate_shape(struct field *fld, struct shape *shape)
{
    rotate_shape_right(shape);
    if (collide(fld, shape))
        rotate_shape_left(shape);
}
