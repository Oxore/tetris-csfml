#include <stdlib.h>
#include <string.h>
#include <SFML/Graphics/RectangleShape.h>

#include "vector.h"
#include "field.h"
#include "config.h"
#include "shape_maps.h"

static void rotate_shape_left(struct shape *shape);
static void rotate_shape_right(struct shape *shape);
static int out_of_bounds(struct field *fld, struct shape *active);

void field_init(struct field *fld)
{
    fld->c = calloc(fld->bound.y, sizeof(struct cell *));
    for (size_t j = 0; j < fld->bound.y; j++) {
        fld->c[j] = calloc(fld->bound.x, sizeof(struct cell));
        for (size_t i = 0; i < fld->size.x; i++)
            fld->c[j][i].a = 0;
    }
    fld->shape = calloc(fld->shape_cnt, sizeof(struct shape));
}

void field_deinit(struct field *fld) {
    for (int j = 0; j < (int)fld->bound.y; j++)
        free(fld->c[j]);
    free(fld->c);
    free(fld->shape);
}

void field_clear(struct field *fld)
{
    for (size_t j = 0; j < fld->bound.y; j++)
        for (size_t i = 0; i < fld->bound.x; i++) {
            fld->c[j][i].a = 0;
            fld->c[j][i].color = 0;
        }
}

void field_fill_random(struct field *fld)
{
    for (size_t j = 0; j < fld->size.y; j++)
        for (size_t i = 0; i < fld->size.x; i++) {
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
                fld->c[j+active->y][i+active->x].color = active->t;
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
    switch (shape->t) { // Copy cell active/inactive state
        case 1 :
            memcpy(&shape->c[0][0], &shape_L[0][0], sizeof(char)*4*4);
            break;
        case 2 :
            memcpy(&shape->c[0][0], &shape_RL[0][0], sizeof(char)*4*4);
            break;
        case 3 :
            memcpy(&shape->c[0][0], &shape_Z[0][0], sizeof(char)*4*4);
            break;
        case 4 :
            memcpy(&shape->c[0][0], &shape_S[0][0], sizeof(char)*4*4);
            break;
        case 5 :
            memcpy(&shape->c[0][0], &shape_B[0][0], sizeof(char)*4*4);
            break;
        case 6 :
            memcpy(&shape->c[0][0], &shape_I[0][0], sizeof(char)*4*4);
            break;
        case 7 :
            memcpy(&shape->c[0][0], &shape_T[0][0], sizeof(char)*4*4);
            break;
    }
}

void field_reset_walking_shape(struct field *fld, size_t index)
{
    struct shape *shape = &fld->shape[index];
    shape_load(shape);
    shape->x = 3;
    if (shape->t == 6)
        shape->y = 19;
    else
        shape->y = 18;
    while (field_shape_collision(fld, shape))
        shape->y++;
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

static int wall_kick(struct field *fld, struct shape *shape)
{
    // try kick the left wall
    shape->x++;
    if (field_shape_collision(fld, shape))
        shape->x--;
    else
        return 1;

    // try kick the right wall
    shape->x--;
    if (field_shape_collision(fld, shape)) {
        if (shape->t == 6) {
            shape->x--;
            if (field_shape_collision(fld, shape))
                shape->x++;
            else
                return 1;
        }
        shape->x++;
    } else {
        return 1;
    }

    // try kick the floor
    shape->y++;
    if (field_shape_collision(fld, shape)) {
        if (shape->t == 6) {
            shape->y++;
            if (field_shape_collision(fld, shape))
                shape->y--;
            else
                return 1;
        }
        shape->y--;
    } else {
        return 1;
    }

    return 0;
}

void field_rotate_shape_clockwise(struct field *fld, size_t index)
{
    struct shape *shape = &fld->shape[index];
    rotate_shape_right(shape);
    if (field_shape_collision(fld, shape))
        if (!wall_kick(fld, shape))
            rotate_shape_left(shape);
}

int field_move_shape_down(struct field *fld, size_t index)
{
    fld->shape[index].y--;
    if (field_shape_collision(fld, &fld->shape[index])) {
        fld->shape[index].y++;
        return 0;
    }
    return -1;
}

int field_move_shape_left(struct field *fld, size_t index)
{
    fld->shape[index].x--;
    if (field_shape_collision(fld, &fld->shape[index])) {
        fld->shape[index].x++;
        return 0;
    }
    return -1;
}

int field_move_shape_right(struct field *fld, size_t index)
{
    fld->shape[index].x++;
    if (field_shape_collision(fld, &fld->shape[index])) {
        fld->shape[index].x--;
        return 0;
    }
    return -1;
}

int field_rm_rows(struct field *fld)
{
    size_t rows = 0;
    for (size_t j = 0; j < fld->bound.y; j++) {
        size_t cells = 0;
        for (size_t i = 0; i < fld->bound.x; i++)
            if (fld->c[j][i].a)
                ++cells;
        if (cells == fld->bound.x) {
            ++rows;
            for (size_t n = j; n < fld->bound.y - 1; n++)
                for (size_t m = 0; m < fld->bound.x; m++) {
                    fld->c[n][m].a = fld->c[n + 1][m].a;
                    fld->c[n][m].color = fld->c[n + 1][m].color;
                }
            --j;
        }
    }
    return rows;
}
