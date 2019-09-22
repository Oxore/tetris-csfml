/*
 * pg.c
 *
 * Playground template to try things, mostly SFML related.
 *
 * */

#include <SFML/Graphics.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <f8.h>

static size_t utf8_detect_back(const char *text)
{
    if ((*text-- & 0x80) == 0)
        return 1;

    if ((*text-- & 0xE0) == 0xC0)
        return 2;

    if ((*text-- & 0xF0) == 0xE0)
        return 3;

    if ((*text-- & 0xF8) == 0xF0)
        return 4;

    return 0;
}

static void handleWindowEvents(sfRenderWindow *window, char *text, size_t size)
{
    sfEvent event;

    while (sfRenderWindow_pollEvent(window, &event)) {
        if (event.type == sfEvtClosed)
            sfRenderWindow_close(window);

        if (size == 0)
            continue;

        if (event.type == sfEvtTextEntered) {
            size_t len = strnlen(text, size - 1);

            if (event.text.unicode == L'\b') {
                len -= len > 0 ? 1 : 0;
                size_t clen = utf8_detect_back(&text[len]);
                memset(&text[len + 1 - clen], 0, clen);
            } else {
                utf32to8_strcpy_s(&text[len], size - len, &event.text.unicode);
            }
        }
    }
}

int main()
{
    /* Initialization */

    char utf8_text[BUFSIZ] = {0};
    int32_t utf32_text[BUFSIZ] = {0};

    sfRenderWindow *window;

    sfVideoMode mode = (sfVideoMode){450, 570, 32};
    window = sfRenderWindow_create(mode, "CSFML", sfResize | sfClose, NULL);
    if (!window)
        exit(EXIT_FAILURE);
    sfRenderWindow_setFramerateLimit(window, 60);

    sfFont *font = sfFont_createFromFile("./dat/arial.ttf");
    sfText *text = sfText_create();
    sfText_setPosition(text, (sfVector2f){100, 100});
    sfText_setString(text, "aaaaaaaaaaaa");
    sfText_setColor(text, (sfColor){255, 255, 255, 255});
    sfText_setFillColor(text, (sfColor){255, 255, 255, 255});
    sfText_setCharacterSize(text, 20);
    sfText_setFont(text, font);

    sfRectangleShape *rect = sfRectangleShape_create();
    sfRectangleShape_setPosition(rect, (sfVector2f){100, 100});
    sfRectangleShape_setSize(rect, (sfVector2f){150, 30});
    sfRectangleShape_setOutlineThickness(rect, 1);
    sfRectangleShape_setOutlineColor(rect, sfGreen);
    sfRectangleShape_setFillColor(rect, (sfColor){0, 0, 0, 0});

    /* Main loop */

    while (sfRenderWindow_isOpen(window)) {
        handleWindowEvents(window, utf8_text, 40);

        sfText_setUnicodeString(text, utf8to32_strcpy_s(utf32_text, sizeof(utf8_text), utf8_text));
        sfRenderWindow_clear(window, (sfColor){26, 26, 26, 255});
        sfRenderWindow_drawRectangleShape(window, rect, NULL);
        sfRenderWindow_drawText(window, text, NULL);
        sfRenderWindow_display(window);
    }

    /* Cleanup */

    sfRectangleShape_destroy(rect);

    sfText_destroy(text);
    sfFont_destroy(font);

    if (window) {
        sfRenderWindow_destroy(window);
        window = 0;
    }

    return EXIT_SUCCESS;
}
