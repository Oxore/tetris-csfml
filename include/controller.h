struct controls {
    uint32_t keys;         // bitmap of keys pressed
    sfClock *down_repeat_clock;  // repeat latency when hold Down arrow
    sfClock *left_repeat_clock;   // repeat latency when hold Left arrow
    sfClock *right_repeat_clock;  // repeat latency when hold Right arrow
};

void controller_gather_window_events(
        media_window_t *window,
        struct events_array *events);

void controller_gather_controls_events(
        media_window_t *window,
        const struct config *config,
        struct controls *ctl,
        struct events_array *events);
