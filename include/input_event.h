enum input_event_type {
    INPUT_EVENT_UNDEFINED = 0,
    INPUT_EVENT_TEXT_INPUT,
    INPUT_EVENT_ACTION,
};

struct input_event_text {
    char codepoint[8];
};

struct input_event_action {
    uint32_t mask;
};

union input_event_u {
    struct input_event_text text;
    struct input_event_action action;
};

struct input_event {
    enum input_event_type type;
    union input_event_u;
};
