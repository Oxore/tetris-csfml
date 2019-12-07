#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <SFML/System/Clock.h>
#include <SFML/Window/Keyboard.h>
#include <SFML/Window/Window.h>

#include "media.h"
#include "media/csfml.h"

struct media_timeout {
    sfClock *clock;
    uint32_t time;
};

bool media_key_is_pressed(enum key_id key)
{
    if (key == KEY_UNKNOWN)
        return false;

    return sfKeyboard_isKeyPressed(csfml_keymap[key]);
}

bool media_window_is_focused(const media_window_t *window)
{
    assert(window);

    return sfWindow_hasFocus(window);
}

media_timeout_t *media_timeout_new(uint32_t milliseconds)
{
    struct media_timeout *timeout = calloc(1, sizeof(struct media_timeout));

    if (timeout) {
        *timeout = (struct media_timeout){
            .clock = sfClock_create(),
            .time = milliseconds,
        };
    }
    sfClock_restart(timeout->clock);

    return timeout;
}

void media_timeout_destroy(struct media_timeout *timeout)
{
    assert(timeout);
    assert(timeout->clock);

    sfClock_destroy(timeout->clock);
    free(timeout);
}

void media_timeout_reset(struct media_timeout *timeout, uint32_t milliseconds)
{
    if (!timeout)
        return;

    timeout->time = milliseconds;
    sfClock_restart(timeout->clock);
}

bool media_timeout_is_passed(const struct media_timeout *timeout)
{
    if (!timeout)
        return false;

    uint32_t elapsed =
        (uint32_t)(sfClock_getElapsedTime(timeout->clock).microseconds / 1000);

    if (elapsed >= timeout->time)
        return true;

    return false;
}
