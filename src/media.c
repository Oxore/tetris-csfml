#include <assert.h>
#include <stdbool.h>
#include <SFML/Window/Keyboard.h>
#include <SFML/Window/Window.h>

#include "media.h"
#include "media/csfml.h"

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
