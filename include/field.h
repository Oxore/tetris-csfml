#define FLD_SIZE_Y  22
#define FLD_SIZE_X  10
#define FLD_BOUND_Y FLD_SIZE_Y + 3
#define FLD_BOUND_X FLD_SIZE_X
#define FLD_POS     (sfVector2i){.x = 10, .y = 10+550-24}
#define FLD_SIZE    (sfVector2f){.x = FLD_SIZE_X, .y = FLD_SIZE_Y}
#define CELL_SIZE   (sfVector2f){.x = 23, .y = 23}

struct cell {
    char a; // active/empty state of cell
    sfColor fColor; // fill color
    unsigned int color;
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

struct shape {
    int x; // x coord of shape's left side
    int y; // y coord of shape's bottom
    int t; // shape type
    sfColor fColor; // shape color
    char c[4][4]; // array of logic shape cells
    sfRectangleShape *p[4][4]; // array of physical shape cells
    int cOutThick; // shape rectangles outline thickness
    sfVector2f cSize; // shape rectangles size variable x/y
};

struct field {
    sfVector2i pos;
    struct cell c[FLD_BOUND_Y][FLD_SIZE_X]; // array of logic shape cells
    sfRectangleShape *p[FLD_SIZE_Y][FLD_SIZE_X]; // array of physical shape cells
    int cOutThick; // Field rectangles outline thickness
    sfVector2f cSize; // shape rectangles size variable x/y
    sfVector2i size;
    sfVector2i bound;
    unsigned int id;
};

void init_field(struct field *fld);
void colorize_field(struct field *fld);
void colorize_field_random(struct field *fld);
void colorize_active_shape(struct field *fld, struct shape *shape);
void init_next_shape(struct shape *next);
void putShape(struct field *fld, struct shape *active);
int  out_of_field(struct field *fld, struct shape *active);
void load_shape(struct shape *shape);
void rotate_shape(struct field *fld, struct shape *shape);
int  collide(struct field *fld, struct shape *active);
void resetActiveShape(struct field *fld, struct shape *active);
void gen_shape(struct shape *shape);
int  rm_lines(struct field *fld);
void free_field(struct field *fld);
void free_shape(struct shape *shape);
