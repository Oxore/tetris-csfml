#define INPUT_ATTR_INVISIBLE (1 << 0)
struct input {
    size_t id;
    size_t attr;
    char text[CFG_NAME_MAX * sizeof(uint32_t) + 1];
    size_t nbytes;
    size_t fontsize;
    struct vector2i pos;
};

void input_clear(struct input *input);
void input_append_utf32char(struct input *input, int32_t c);
void input_rm_last_char(struct input *input);
