#define FLD_SIZE_Y   22
#define FLD_SIZE_X   10
#define FLD_BOUND_Y FLD_SIZE_Y + 3
#define FLD_BOUND_X FLD_SIZE_X
/*
 *  Types
 *
 * */

struct cell {
    char a; // active/empty state of cell
    sfColor fColor; // fill color
};


/*
 * shape coords
 * y
 * ^. . . .
 * |. . . .
 * |. . . .
 * |. . . .
 * 0------->x
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
    sfColor fColor; // shape color
    struct cell c[FLD_BOUND_Y][FLD_SIZE_X]; // array of logic shape cells
    sfRectangleShape *p[FLD_BOUND_Y][FLD_SIZE_X]; // array of physical shape cells
    int cOutThick; // Field rectangles outline thickness
    sfVector2f cSize; // shape rectangles size variable x/y
    sfVector2i size;
    sfVector2i bound;
};

void init_field(struct field *fld);
void colorize_field(struct field *fld);
void colorize_field_random(struct field *fld);
void init_next_shape_field(struct shape *next);
void putShape(struct field *fld, struct shape *active);
int out_of_field(struct field *fld, struct shape *active);
void load_shape(struct shape *shape);
void rotate_shape(struct field *fld, struct shape *shape);
int collide(struct field *fld, struct shape *active);
void resetActiveShape(struct field *fld, struct shape *active);
