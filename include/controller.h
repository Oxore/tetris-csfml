struct controls {
    uint32_t keys;                          // Bitmap of pressed keys
    media_timeout_t *down_repeat_timeout;   // Repeat timeout for Down button
    media_timeout_t *left_repeat_timeout;   // Repeat timeout for Left button
    media_timeout_t *right_repeat_timeout;  // Repeat timeout for Right button
};

void controller_gather_window_events(
        media_window_t *window,
        struct events_array *events);

void controller_gather_controls_events(
        media_window_t *window,
        const struct config *config,
        struct controls *ctl,
        struct events_array *events);
