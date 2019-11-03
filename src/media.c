#include <stdbool.h>
#include <SFML/Window/Keyboard.h>

#include "media.h"
#include "media/csfml.h"

bool media_is_key_pressed(enum key_id key)
{
    return sfKeyboard_isKeyPressed(csfml_keymap[key]);
}
