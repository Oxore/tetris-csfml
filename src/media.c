#include <stdbool.h>
#include <SFML/Window/Keyboard.h>

#include "media.h"
#include "media/csfml.h"

bool media_is_key_pressed(enum key_id key)
{
    if (key == KEY_UNKNOWN)
        return false;

    return sfKeyboard_isKeyPressed(csfml_keymap[key]);
}
