enum input_event_type {
    INPUT_EVENT_UNDEFINED = 0,
    INPUT_EVENT_TEXT_INPUT,
    INPUT_EVENT_ACTION,
};

struct input_event_text {
    char codepoint[8];
};

enum action_id {
    ACTION_ID_UNDEFINED = 0,
    ACTION_ID_FINISH_INPUT,
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
