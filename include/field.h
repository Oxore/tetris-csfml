#define FLD_ATTR_INVISIBLE          (1 << 0)
#define FLD_ATTR_HIDE_EMPTY_CELLS   (1 << 1)

#define SHP_ATTR_GHOST              (1 << 0)

#define GHOST_SHAPE_INDEX   0
#define ACTIVE_SHAPE_INDEX  1

struct cell {
    char    a; // active/empty state of cell
    size_t  color;
};

/*
 * field + shape coord system
 * y
 * ^. . . . . . .
 * |. . . . . . .
 * |. . . . . . .
 * |. . y . . . .
 * |. . ^ . . . .
 * |. . | . . . .
 * |. . | . . . .
 * |. . 0------>x
 * |. . . . . . .
 * |. . . . . . .
 * 0------------->x
 *
 */

struct vector2i;

struct shape {
    struct  vector2i; // position
    int     t; // shape type
    size_t  attr;
    char    c[4][4];
};

struct field {
    size_t              id;
    size_t              attr;
    struct vector2i     pos;
    struct vector2ui    size;
    struct vector2ui    bound;
    struct cell       **c; // array of logic shape cells
    size_t              shape_cnt;
    struct shape       *shape;
};

void    field_init(struct field *fld);
void    field_deinit(struct field *fld);
void    field_fill_random(struct field *fld);
void    field_clear(struct field *fld);
void    field_rotate_shape_clockwise(struct field *fld, size_t index);
void    field_rotate_shape_counter_clockwise(struct field *fld, size_t index);
int     field_move_shape_down(struct field *fld, size_t index);
int     field_move_shape_left(struct field *fld, size_t index);
int     field_move_shape_right(struct field *fld, size_t index);
void    field_put_shape(struct field *fld, struct shape *shape);
void    field_reset_walking_shape(struct field *fld, size_t index);
int     field_rm_rows(struct field *fld);
int     field_shape_collision(struct field *fld, struct shape *shape);
int     field_shape_out_of_bounds(struct field *fld, struct shape *shape);

void    shape_load(struct shape *shape);
void    shape_gen_random(struct shape *shape);
