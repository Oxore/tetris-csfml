struct window {
    sfVideoMode     mode;
    sfRenderWindow *window;
    sfEvent         event;
};

void          painter_init_window();
void          painter_destroy_window();

unsigned long painter_register_field();
void          painter_update_field(unsigned long id, struct field *fld);
void          painter_destroy_field(unsigned long id);
void          painter_destroy_fields();

void          painter_draw();

void          painter_destroy_drawables();
void          painter_destroy_all();
