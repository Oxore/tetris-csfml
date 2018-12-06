/*
 * painter.h
 *
 * Painter is a module for drawing tetris objects on a window. It is implemented
 * as a singletone as OO programmers speak. It internally has global static
 * arrays for objects referenced by handles. You register an object and get a
 * handle (id) which can be used for updating an object - modifying it's
 * drawable representation. Painter is used as described in following steps:
 *
 * 1. At first assign a window via painter_set_window() function.
 * 2. Load a font if you are going to draw any text.
 * 3. Register an object (field, text, etc.) using painter_register_*()
 *    function.
 * 4. Update an object attribute using painter_update_*() function. Update all
 *    your objects for next frame before drawing.
 * 5. Draw everything using painter_draw() function. Objects are drawn in order
 *    they were registered.
 * 6. At the end you must destroy your drawable objects and fonts. Objects are
 *    registered each in it's own way, but have unified interface for
 *    destroying - painter_destroy_drawable() function. You can destroy them
 *    all at once using painter_destroy_drawables() function or even destroy
 *    everything that has been allocated by the Painter, even loaded fonts.
 *
 * */

void    painter_set_window(sfRenderWindow *window);

void    painter_load_font(char *filename);
void    painter_destroy_font();

size_t  painter_register_field(struct field *fld);
void    painter_update_field(size_t id, struct field *fld);
size_t  painter_register_text(struct text *txt);
void    painter_update_text(size_t id, struct text *txt);

void    painter_draw();

void    painter_destroy_drawable(size_t id);
void    painter_destroy_drawables();
void    painter_destroy_all();
