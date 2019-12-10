#include <assert.h>
#include <SFML/Graphics/RenderWindow.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "event.h"
#include "media.h"
#include "controller.h"

#define NUM_KEYS_TO_POLL 1

struct key_to_poll {
    enum key_id key;
    enum action_id action;
};

static struct key_to_poll keys_to_poll[NUM_KEYS_TO_POLL] = {
    {
        .key = KEY_ENTER,
        .action = ACTION_ID_FINISH_INPUT,
    },
};

void controller_handle_window_events(
        media_window_t *window,
        struct events_array *events)
{
    struct media_event event;

    while (media_window_poll_event(window, &event)) {
        if (event.type == MEDIA_EVENT_WINDOW_CLOSED) {
            sfRenderWindow_close(window);
            return;

        } else if (events && !events_array_is_full(events)) {
            if (event.type == MEDIA_EVENT_TEXT_ENTERED) {
                struct input_event ie = {.type = INPUT_EVENT_TEXT_INPUT, };
                strncpy(
                        ie.text.codepoint,
                        event.text.codepoint,
                        sizeof(ie.text.codepoint));
                events_array_add_input(events, ie);
            }
        }
    }

    for (size_t i = 0; i < NUM_KEYS_TO_POLL; i++) {
        if (events_array_is_full(events))
                break;

        if (media_key_is_pressed(keys_to_poll[i].key)) {
            struct input_event ie = {
                .type = INPUT_EVENT_ACTION,
                .action.id = keys_to_poll[i].action,
            };
            events_array_add_input(events, ie);
        }
    }
}
