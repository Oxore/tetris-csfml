struct window {
    sfVideoMode     mode;
    sfRenderWindow *window;
    sfEvent         event;
};

//struct field_conf {
//    sfVector2i        pos;
//    int               cOutThick;
//    sfVector2f        cSize;
//    sfVector2i        size;
//} field_conf;

void          painter_init_window();

unsigned long painter_register_field();
void          painter_update_field(unsigned long id, struct field *fld);

unsigned long painter_register_shape();
void          painter_update_shape(unsigned long id, struct shape *shape);

void          painter_draw();
void          painter_destroy_window();
