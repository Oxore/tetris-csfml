/* Here is a place for implementations of functions from media.h. All #ifdefs
 * for separating different supported platforms must be in this file only.
 *
 * If you think that implementation need some cumbersome logic related to the
 * media platform being abstracted, then putting some details to
 * media/<platform>.c should be considered.
 *
 * Currently CSFML platform only is supported.
 * */

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <SFML/System/Clock.h>
#include <SFML/Graphics/RenderWindow.h>

#include "media.h"
#include "media/csfml.h"

struct media_timeout {
    sfClock *clock;
    uint32_t time;
};

bool media_key_is_pressed(enum key_id key)
{
    if (key <= KEY_UNKNOWN)
        return false;

    if (key >= KEY_COUNT)
        return false;

    return sfKeyboard_isKeyPressed(csfml_keymap[key]);
}

bool media_window_is_focused(const media_window_t *window)
{
    assert(window);

    return sfWindow_hasFocus(window) == sfTrue ? true : false;
}

bool media_window_poll_event(
        const media_window_t *window,
        struct media_event *event)
{
    sfEvent sf_event = {0};
    bool have_event = (sfTrue == sfRenderWindow_pollEvent(
                (sfRenderWindow *)window,
                &sf_event));
    if (have_event && window != NULL) {
        *event = csfml_sfevent_to_media_event(sf_event);
    }
    return have_event;
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

bool media_window_is_open(const media_window_t *window)
{
    return sfRenderWindow_isOpen(window) == sfTrue ? true : false;
}

void media_window_close(media_window_t *window)
{
    sfRenderWindow_close(window);
}

void media_window_destroy(media_window_t *window)
{
    sfRenderWindow_destroy(window);
}
