void          painter_set_window(sfRenderWindow *window);

void          painter_load_font(char *filename);
void          painter_destroy_font();

unsigned long painter_register_field(struct field *fld);
void          painter_update_field(unsigned long id, struct field *fld);
unsigned long painter_register_text(struct text *txt);
void          painter_update_text(unsigned long id, struct text *txt);

void          painter_draw();

void          painter_destroy_drawable(unsigned long id);
void          painter_destroy_drawables();
void          painter_destroy_all();
