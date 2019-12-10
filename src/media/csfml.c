/* Cumbersome CSFML platform abstraction logic parts.
 * */

#include <f8.h>
#include <stdint.h>
#include <stdbool.h>
#include <SFML/Window/Keyboard.h>
#include <SFML/Window/Event.h>

#include "media.h"
#include "media/csfml.h"

sfKeyCode csfml_keymap[] = {
    [KEY_UNKNOWN]       = sfKeyUnknown,
    [KEY_A]             = sfKeyA,
    [KEY_B]             = sfKeyB,
    [KEY_C]             = sfKeyC,
    [KEY_D]             = sfKeyD,
    [KEY_E]             = sfKeyE,
    [KEY_F]             = sfKeyF,
    [KEY_G]             = sfKeyG,
    [KEY_H]             = sfKeyH,
    [KEY_I]             = sfKeyI,
    [KEY_J]             = sfKeyJ,
    [KEY_K]             = sfKeyK,
    [KEY_L]             = sfKeyL,
    [KEY_M]             = sfKeyM,
    [KEY_N]             = sfKeyN,
    [KEY_O]             = sfKeyO,
    [KEY_P]             = sfKeyP,
    [KEY_Q]             = sfKeyQ,
    [KEY_R]             = sfKeyR,
    [KEY_S]             = sfKeyS,
    [KEY_T]             = sfKeyT,
    [KEY_U]             = sfKeyU,
    [KEY_V]             = sfKeyV,
    [KEY_W]             = sfKeyW,
    [KEY_X]             = sfKeyX,
    [KEY_Y]             = sfKeyY,
    [KEY_Z]             = sfKeyZ,
    [KEY_0]             = sfKeyNum0,
    [KEY_1]             = sfKeyNum1,
    [KEY_2]             = sfKeyNum2,
    [KEY_3]             = sfKeyNum3,
    [KEY_4]             = sfKeyNum4,
    [KEY_5]             = sfKeyNum5,
    [KEY_6]             = sfKeyNum6,
    [KEY_7]             = sfKeyNum7,
    [KEY_8]             = sfKeyNum8,
    [KEY_9]             = sfKeyNum9,
    [KEY_ESCAPE]        = sfKeyEscape,
    [KEY_LEFT_CONTROL]  = sfKeyLControl,
    [KEY_LEFT_SHIFT]    = sfKeyLShift,
    [KEY_LEFT_ALT]      = sfKeyLAlt,
    [KEY_LEFT_SYSTEM]   = sfKeyLSystem,
    [KEY_RIGHT_CONTROL] = sfKeyRControl,
    [KEY_RIGHT_SHIFT]   = sfKeyRShift,
    [KEY_RIGHT_ALT]     = sfKeyRAlt,
    [KEY_RIGHT_SYSTEM]  = sfKeyRSystem,
    [KEY_MENU]          = sfKeyMenu,
    [KEY_LEFT_BRACKET]  = sfKeyLBracket,
    [KEY_RIGHT_BRACKET] = sfKeyRBracket,
    [KEY_COMMA]         = sfKeyComma,
    [KEY_PERIOD]        = sfKeyPeriod,
    [KEY_QUOTE]         = sfKeyQuote,
    [KEY_SLASH]         = sfKeySlash,
    [KEY_TILDE]         = sfKeyTilde,
    [KEY_EQUAL]         = sfKeyEqual,
#if (CSFML_VERSION_MAJOR == 2) && (CSFML_VERSION_MINOR <= 4)
    [KEY_BACKSLASH]     = sfKeyBackSlash,
    [KEY_SEMICOLON]     = sfKeySemiColon,
    [KEY_HYPHEN]        = sfKeyDash,
    [KEY_ENTER]         = sfKeyReturn,
    [KEY_BACKSPACE]     = sfKeyBack,
#else
    [KEY_BACKSLASH]     = sfKeyBackslash,
    [KEY_SEMICOLON]     = sfKeySemicolon,
    [KEY_HYPHEN]        = sfKeyHyphen,
    [KEY_ENTER]         = sfKeyEnter,
    [KEY_BACKSPACE]     = sfKeyBackspace,
#endif
    [KEY_SPACE]         = sfKeySpace,
    [KEY_TAB]           = sfKeyTab,
    [KEY_PAGE_UP]       = sfKeyPageUp,
    [KEY_PAGE_DOWN]     = sfKeyPageDown,
    [KEY_END]           = sfKeyEnd,
    [KEY_HOME]          = sfKeyHome,
    [KEY_INSERT]        = sfKeyInsert,
    [KEY_DELETE]        = sfKeyDelete,
    [KEY_NUM_PLUS]      = sfKeyAdd,
    [KEY_NUM_MINUS]     = sfKeySubtract,
    [KEY_NUM_STAR]      = sfKeyMultiply,
    [KEY_NUM_SLASH]     = sfKeyDivide,
    [KEY_LEFT]          = sfKeyLeft,
    [KEY_RIGHT]         = sfKeyRight,
    [KEY_UP]            = sfKeyUp,
    [KEY_DOWN]          = sfKeyDown,
    [KEY_NUM0]          = sfKeyNumpad0,
    [KEY_NUM1]          = sfKeyNumpad1,
    [KEY_NUM2]          = sfKeyNumpad2,
    [KEY_NUM3]          = sfKeyNumpad3,
    [KEY_NUM4]          = sfKeyNumpad4,
    [KEY_NUM5]          = sfKeyNumpad5,
    [KEY_NUM6]          = sfKeyNumpad6,
    [KEY_NUM7]          = sfKeyNumpad7,
    [KEY_NUM8]          = sfKeyNumpad8,
    [KEY_NUM9]          = sfKeyNumpad9,
    [KEY_F1]            = sfKeyF1,
    [KEY_F2]            = sfKeyF2,
    [KEY_F3]            = sfKeyF3,
    [KEY_F4]            = sfKeyF4,
    [KEY_F5]            = sfKeyF5,
    [KEY_F6]            = sfKeyF6,
    [KEY_F7]            = sfKeyF7,
    [KEY_F8]            = sfKeyF8,
    [KEY_F9]            = sfKeyF9,
    [KEY_F10]           = sfKeyF10,
    [KEY_F11]           = sfKeyF11,
    [KEY_F12]           = sfKeyF12,
    [KEY_F13]           = sfKeyF13,
    [KEY_F14]           = sfKeyF14,
    [KEY_F15]           = sfKeyF15,
    [KEY_PAUSE]         = sfKeyPause,
};

struct media_event csfml_sfevent_to_media_event(sfEvent sf_event)
{
    struct media_event event = {0};
    switch (sf_event.type) {
    default:
        break;

    case sfEvtClosed:
        event = (struct media_event){ .type = MEDIA_EVENT_WINDOW_CLOSED, };
        break;

    case sfEvtTextEntered:
        event = (struct media_event) {
            .type = MEDIA_EVENT_TEXT_ENTERED,
        };
        utf32to8_strncpy_s(
                event.text.codepoint,
                sizeof(event.text.codepoint),
                (int32_t *)&sf_event.text.unicode,
                1);
        break;
    };

    return event;
}
