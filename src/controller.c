#include <assert.h>
#include <SFML/Graphics/RenderWindow.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "config.h"
#include "event.h"
#include "media.h"
#include "controller.h"

// Bits in bitmap of keys
#define ROTRIGHT  (1 << 0)
#define ROTLEFT   (1 << 1)
#define RIGHT     (1 << 2)
#define LEFT      (1 << 3)
#define DOWN      (1 << 4)
#define RIGHTHOLD (1 << 5)
#define HARDDROP  (1 << 6)
#define PAUSE     (1 << 7)
#define LEFTHOLD  (1 << 8)
#define START     (1 << 9)


void controller_gather_window_events(
        media_window_t *window,
        struct events_array *events)
{
    const struct {
        enum key_id key;
        enum action_id action;
    } keys[] = {
        {
            .key = KEY_ENTER,
            .action = ACTION_ID_FINISH_INPUT,
        },
    };
    struct media_event event;

    if (media_window_is_focused(window) == false) {
        return;
    }

    while (media_window_poll_event(window, &event)) {
        if (event.type == MEDIA_EVENT_WINDOW_CLOSED) {
            media_window_close(window);
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

    for (size_t i = 0; i < sizeof(keys) / sizeof(*keys); i++) {
        if (events_array_is_full(events))
            break;

        if (media_key_is_pressed(keys[i].key)) {
            struct input_event ie = {
                .type = INPUT_EVENT_ACTION,
                .action.id = keys[i].action,
            };
            events_array_add_input(events, ie);
        }
    }

    /* Poll for any key press */

    for (enum key_id anykey = KEY_UNKNOWN; anykey < KEY_COUNT; anykey++) {
        if (events_array_is_full(events))
            break;

        if (media_key_is_pressed(anykey)) {
            struct input_event ie = {
                .type = INPUT_EVENT_ACTION,
                .action.id = ACTION_ID_ANYKEY_PRESSED,
            };
            events_array_add_input(events, ie);
            break;
        }
    }
}

static uint32_t handle_game_keys(
        struct controls *ctl,
        const struct config *config)
{
    uint32_t ret = 0;

    /* PAUSE */
    if (media_key_is_pressed(config->keys.pause)) {
        if (!(ctl->keys & PAUSE)) {
            ctl->keys |= PAUSE;
            ret |= PAUSE;
        }
    } else {
        ctl->keys &= ~PAUSE;
    }

    /* ROTRIGHT */
    if (media_key_is_pressed(config->keys.rotate_right)) {
        if (!(ctl->keys & ROTRIGHT)) {
            ctl->keys |= ROTRIGHT;
            ret |= ROTRIGHT;
        }
    } else {
        ctl->keys = ctl->keys & ~ROTRIGHT;
    }

    /* ROTLEFT */
    if (media_key_is_pressed(config->keys.rotate_left)) {
        if (!(ctl->keys & ROTLEFT)) {
            ctl->keys |= ROTLEFT;
            ret |= ROTLEFT;
        }
    } else {
        ctl->keys = ctl->keys & ~ROTLEFT;
    }

    /* HARDDROP */
    if (media_key_is_pressed(config->keys.drop)) {
        if (!(ctl->keys & HARDDROP)) {
            ctl->keys |= HARDDROP;
            ret |= HARDDROP;
        }
    } else {
        ctl->keys &= ~HARDDROP;
    }

    /* DOWN */
    if (media_key_is_pressed(config->keys.move_down)) {
        if (!(ctl->keys & DOWN)) {
            ctl->keys |= DOWN;
            ret |= DOWN;
            media_timeout_reset(ctl->down_repeat_timeout, CFG_REPEAT_LATENCY_MS);
        } else {
            if (media_timeout_is_passed(ctl->down_repeat_timeout)) {
                ctl->keys &= ~DOWN;
            }
        }
    } else {
        ctl->keys &= ~DOWN;
    }

    /* LEFT */
    if (media_key_is_pressed(config->keys.move_left)
     && !media_key_is_pressed(config->keys.move_right)) {
        if (!(ctl->keys & LEFT)) {
            ctl->keys |= LEFT;
            ret |= LEFT;
            media_timeout_reset(
                    ctl->left_repeat_timeout,
                    CFG_PREREPEAT_LATENCY_MS);
        } else if (!(ctl->keys & LEFTHOLD)) {
            if (media_timeout_is_passed(ctl->left_repeat_timeout)) {
                ctl->keys |= LEFTHOLD;
                media_timeout_reset(
                        ctl->left_repeat_timeout,
                        CFG_REPEAT_LATENCY_MS);
            }
        } else {
            if (media_timeout_is_passed(ctl->left_repeat_timeout)) {
                ctl->keys &= ~LEFTHOLD;
                ret |= LEFT;
            }
        }
    } else {
        ctl->keys &= ~LEFT;
        ctl->keys &= ~LEFTHOLD;
    }

    /* RIGHT */
    if (media_key_is_pressed(config->keys.move_right)
     && !media_key_is_pressed(config->keys.move_left)) {
        if (!(ctl->keys & RIGHT)) {
            ctl->keys |= RIGHT;
            ret |= RIGHT;
            media_timeout_reset(
                    ctl->right_repeat_timeout,
                    CFG_PREREPEAT_LATENCY_MS);
        } else if (!(ctl->keys & RIGHTHOLD)) {
            if (media_timeout_is_passed(ctl->right_repeat_timeout)) {
                ctl->keys |= RIGHTHOLD;
                media_timeout_reset(
                        ctl->right_repeat_timeout,
                        CFG_REPEAT_LATENCY_MS);
            }
        } else {
            if (media_timeout_is_passed(ctl->right_repeat_timeout)) {
                ctl->keys &= ~RIGHTHOLD;
                ret |= RIGHT;
            }
        }
    } else {
        ctl->keys &= ~RIGHT;
        ctl->keys &= ~RIGHTHOLD;
    }

    /* START */
    if (media_key_is_pressed(config->keys.start)) {
        if (!(ctl->keys & START)) {
            ctl->keys |= START;
            ret |= START;
        }
    } else {
        ctl->keys &= ~START;
    }

    return ret;
}

void controller_gather_controls_events(
        media_window_t *window,
        const struct config *config,
        struct controls *controls,
        struct events_array *events)
{
    const struct {
        uint32_t key;
        enum action_id action_id;
    } keys[] = {
        {
            .key = PAUSE,
            .action_id = ACTION_ID_PAUSE,
        },
        {
            .key = ROTRIGHT,
            .action_id = ACTION_ID_ROTATE_RIGHT,
        },
        {
            .key = ROTLEFT,
            .action_id = ACTION_ID_ROTATE_LEFT,
        },
        {
            .key = DOWN,
            .action_id = ACTION_ID_MOVE_DOWN,
        },
        {
            .key = HARDDROP,
            .action_id = ACTION_ID_DROP,
        },
        {
            .key = LEFT,
            .action_id = ACTION_ID_MOVE_LEFT,
        },
        {
            .key = RIGHT,
            .action_id = ACTION_ID_MOVE_RIGHT,
        },
        {
            .key = START,
            .action_id = ACTION_ID_START,
        },
    };

    if (media_window_is_focused(window) == false) {
        return;
    }

    uint32_t ret_keys = handle_game_keys(controls, config);

    for (size_t i = 0; i < sizeof(keys) / sizeof(*keys); i++) {
        if (ret_keys & keys[i].key) {
            struct input_event ie = {
                .type = INPUT_EVENT_ACTION,
                .action.id = keys[i].action_id,
            };
            events_array_add_input(events, ie);
        }
    }
}
