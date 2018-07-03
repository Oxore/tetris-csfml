#include <stdlib.h>
#include <string.h>
#include <SFML/Graphics/RectangleShape.h>

#include "field.h"
#include "tet_conf.h"

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

void field_init(struct field *fld)
{
    fld->c = calloc(fld->bound.y, sizeof(struct cell *));
    for (unsigned int j = 0; j < fld->bound.y; j++) {
        fld->c[j] = calloc(fld->bound.x, sizeof(struct cell));
        for (unsigned int i = 0; i < fld->size.x; i++)
            fld->c[j][i].a = 0;
    }
    fld->shape = calloc(fld->shape_cnt, sizeof(struct shape));
}

void field_clear(struct field *fld)
{
    for (unsigned int j = 0; j < fld->bound.y; j++)
        for (unsigned int i = 0; i < fld->bound.x; i++) {
            fld->c[j][i].a = 0;
            fld->c[j][i].color = 0;
        }
}

void field_fill_random(struct field *fld)
{
    for (unsigned int j = 0; j < fld->size.y; j++)
        for (unsigned int i = 0; i < fld->size.x; i++) {
            fld->c[j][i].a = 1;
            fld->c[j][i].color = rand() % 7 + 1;
        }
}

void field_put_shape(struct field *fld, struct shape *active)
{
    for (int j = 0; j < 4; j++)
        for (int i = 0; i < 4; i++)
            if (active->c[j][i]) {
                fld->c[j+active->y][i+active->x].a = active->c[j][i];
                fld->c[j+active->y][i+active->x].color = active->color;
            }
}

int field_shape_out_of_bounds(struct field *fld, struct shape *active)
{
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (active->c[i][j] && active->y + i >= (int)fld->size.y)
                return 1;
    return 0;
}

void shape_load(struct shape *shape)
{
    shape->color = shape->t;
    switch (shape->t) { // Copy cell active/inactive state
        case 1 :
            memcpy(&shape->c[0][0], &arrShapeL[0][0], sizeof(char)*4*4);
            break;
        case 2 :
            memcpy(&shape->c[0][0], &arrShapeRL[0][0], sizeof(char)*4*4);
            break;
        case 3 :
            memcpy(&shape->c[0][0], &arrShapeZ[0][0], sizeof(char)*4*4);
            break;
        case 4 :
            memcpy(&shape->c[0][0], &arrShapeS[0][0], sizeof(char)*4*4);
            break;
        case 5 :
            memcpy(&shape->c[0][0], &arrShapeB[0][0], sizeof(char)*4*4);
            break;
        case 6 :
            memcpy(&shape->c[0][0], &arrShapeI[0][0], sizeof(char)*4*4);
            break;
        case 7 :
            memcpy(&shape->c[0][0], &arrShapeT[0][0], sizeof(char)*4*4);
            break;
    }
}

void field_reset_walking_shape(struct field *fld, unsigned int index)
{
    struct shape *active = &fld->shape[index];
    shape_load(active);
    active->x = 3;
    if (active->t == 6)
        active->y = 19;
    else
        active->y = 18;
    while (field_shape_collision(fld, active))
        active->y++;
}

void shape_gen_random(struct shape *shape)
{
    shape->t = (rand() % 7) + 1; // Insert new random shape of 7 variants
    shape_load(shape);
    if (shape->t == 5)
        for (int j = 0; j < 3; j++)
            for (int i = 0; i < 4; i++)
                shape->c[i][j] = shape->c[i][j+1];
}

int field_shape_collision(struct field *fld, struct shape *shape)
{
    if (out_of_bounds(fld, shape))
        return 1;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (shape->c[i][j] && fld->c[i + shape->y][j + shape->x].a)
                return 1;
    return 0;
}

static int out_of_bounds(struct field *fld, struct shape *active)
{
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (active->c[i][j])
                if (active->x + j >= (int)fld->bound.x || active->x + j < 0
                    || active->y + i >= (int)fld->bound.y || active->y + i < 0)
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

void field_rotate_shape(struct field *fld, unsigned int index)
{
    struct shape *shape = &fld->shape[index];
    rotate_shape_right(shape);
    if (field_shape_collision(fld, shape))
        rotate_shape_left(shape);
}

int field_rm_lines(struct field *fld)
{
    unsigned int lines = 0;
    for (unsigned int j = 0; j < fld->bound.y; j++) {
        unsigned int cells = 0;
        for (unsigned int i = 0; i < fld->bound.x; i++)
            if (fld->c[j][i].a)
                ++cells;
        if (cells == fld->bound.x) {
            ++lines;
            for (unsigned int n = j; n < fld->bound.y - 1; n++)
                for (unsigned int m = 0; m < fld->bound.x; m++) {
                    fld->c[n][m].a = fld->c[n + 1][m].a;
                    fld->c[n][m].color = fld->c[n + 1][m].color;
                }
            --j;
        }
    }
    return lines;
}

void field_deinit(struct field *fld) {
    for (int j = 0; j < (int)fld->bound.y; j++)
        free(fld->c[j]);
    free(fld->c);
    free(fld->shape);
}
