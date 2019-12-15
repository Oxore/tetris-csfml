enum key_id {
    KEY_UNKNOWN,
    KEY_A,
    KEY_B,
    KEY_C,
    KEY_D,
    KEY_E,
    KEY_F,
    KEY_G,
    KEY_H,
    KEY_I,
    KEY_J,
    KEY_K,
    KEY_L,
    KEY_M,
    KEY_N,
    KEY_O,
    KEY_P,
    KEY_Q,
    KEY_R,
    KEY_S,
    KEY_T,
    KEY_U,
    KEY_V,
    KEY_W,
    KEY_X,
    KEY_Y,
    KEY_Z,
    KEY_0,
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_7,
    KEY_8,
    KEY_9,
    KEY_ESCAPE,
    KEY_LEFT_CONTROL,
    KEY_LEFT_SHIFT,
    KEY_LEFT_ALT,
    KEY_LEFT_SYSTEM,
    KEY_RIGHT_CONTROL,
    KEY_RIGHT_SHIFT,
    KEY_RIGHT_ALT,
    KEY_RIGHT_SYSTEM,
    KEY_MENU,
    KEY_LEFT_BRACKET,
    KEY_RIGHT_BRACKET,
    KEY_SEMICOLON,
    KEY_COMMA,
    KEY_PERIOD,
    KEY_QUOTE,
    KEY_SLASH,
    KEY_BACKSLASH,
    KEY_TILDE,
    KEY_EQUAL,
    KEY_HYPHEN,
    KEY_SPACE,
    KEY_ENTER,
    KEY_BACKSPACE,
    KEY_TAB,
    KEY_PAGE_UP,
    KEY_PAGE_DOWN,
    KEY_END,
    KEY_HOME,
    KEY_INSERT,
    KEY_DELETE,
    KEY_NUM_PLUS,
    KEY_NUM_MINUS,
    KEY_NUM_STAR,
    KEY_NUM_SLASH,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_UP,
    KEY_DOWN,
    KEY_NUM0,
    KEY_NUM1,
    KEY_NUM2,
    KEY_NUM3,
    KEY_NUM4,
    KEY_NUM5,
    KEY_NUM6,
    KEY_NUM7,
    KEY_NUM8,
    KEY_NUM9,
    KEY_F1,
    KEY_F2,
    KEY_F3,
    KEY_F4,
    KEY_F5,
    KEY_F6,
    KEY_F7,
    KEY_F8,
    KEY_F9,
    KEY_F10,
    KEY_F11,
    KEY_F12,
    KEY_F13,
    KEY_F14,
    KEY_F15,
    KEY_PAUSE,
    KEY_COUNT,
};

enum media_event_type {
    MEDIA_EVENT_UNDEFINED = 0,
    MEDIA_EVENT_WINDOW_CLOSED,
    MEDIA_EVENT_TEXT_ENTERED,
};

struct media_event_text {
    char codepoint[8];
};

struct media_event {
    enum media_event_type type;
    union {
        struct media_event_text text;
    };
};

// Implementation defined types, used only as a pointer
typedef void media_window_t;
typedef struct media_timeout media_timeout_t;
typedef struct media_event media_event_t;

bool media_key_is_pressed(enum key_id);

bool media_window_is_focused(const media_window_t *window);
bool media_window_poll_event(
        const media_window_t *window,
        media_event_t *event);
bool media_window_is_open(const media_window_t *window);
void media_window_close(media_window_t *window);
void media_window_destroy(media_window_t *window);

media_timeout_t *media_timeout_new(uint32_t milliseconds);
void media_timeout_destroy(media_timeout_t *timeout);
void media_timeout_reset(media_timeout_t *timeout, uint32_t milliseconds);
bool media_timeout_is_passed(const media_timeout_t *timeout);
