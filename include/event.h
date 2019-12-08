#ifdef CFG_INPUT_EVENTS_ARRAY_SIZE
#   define INPUT_EVENTS_ARRAY_SIZE CFG_INPUT_EVENTS_ARRAY_SIZE
#else
#   define INPUT_EVENTS_ARRAY_SIZE 256
#endif

_Static_assert(
        INPUT_EVENTS_ARRAY_SIZE > 0,
        "INPUT_EVENTS_ARRAY_SIZE must be > 0");

/* Data types */

enum action_id {
    ACTION_ID_UNDEFINED = 0,
    ACTION_ID_FINISH_INPUT,
};

enum input_event_type {
    INPUT_EVENT_UNDEFINED = 0,
    INPUT_EVENT_TEXT_INPUT,
    INPUT_EVENT_ACTION,
};

enum event_type {
    EVENT_UNDEFINED = 0,
    EVENT_INPUT,
};

struct input_event_text {
    char codepoint[8];
};

struct input_event_action {
    enum action_id id;
};

struct input_event {
    enum input_event_type type;
    union {
        struct input_event_text text;
        struct input_event_action action;
    };
};

struct event {
    enum event_type type;
    union {
        struct input_event input;
    };
};

struct events_array {
    size_t ptr;
    struct event events[INPUT_EVENTS_ARRAY_SIZE];
};

/* Functions */

static inline bool events_array_is_empty(struct events_array *a)
{
    assert(a);
    return a->ptr == 0;
}

static inline bool events_array_is_full(struct events_array *a)
{
    assert(a);
    return a->ptr >= INPUT_EVENTS_ARRAY_SIZE;
}

static inline bool events_array_add_input(
        struct events_array *a,
        struct input_event e)
{
    assert(a);
    if (events_array_is_full(a)) {
        return false;
    }
    a->events[a->ptr++] = (struct event){ .type = EVENT_INPUT, .input = e, };
    return true;
}
