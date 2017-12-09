#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <SFML/System/Clock.h>
#include <SFML/Window/Keyboard.h>
#include <SFML/Graphics/RenderWindow.h>
#include <SFML/Graphics/Font.h>

#include "common.h"
#include "functions.h"
#include "text.h"
#include "tet_conf.h"

Window w = {.mode = {450, 570, 32}};
Game game = {.isStarted = 0, .scoreCurrent = 0, .level = 1};
List *texts;
sfFont *fontScore;
Shape active, next;
Field fld;

char arrKeys = 0b00000000; // Arrow keys states byte container

sfClock *gameTick;
sfClock *putTick;
sfClock *mTick;
sfClock *repPushDown;    // Clock for repeat latency when Down arrow long push
sfClock *repKeyLeft;    // Clock for repeat latency when Left arrow long push
sfClock *repKeyRight;    // Clock for repeat latency when Left arrow long push

void prepare() {
    srand( time(NULL) );
    gameTick = sfClock_create();
    putTick = sfClock_create();
    mTick = sfClock_create();
    fontScore = sfFont_createFromFile("dat/arial.ttf");
    if (!fontScore) {
        printf("dat/arial.ttf font load failed");
        exit(-1);
    }

    /*
     * Dimensions of every fld's cell
     * 23px - fill color 1px - for outline, 25 - at all
     */
    fld.cSize = (sfVector2f){.x = 23, .y = 23}; //Fld's cell size in pixels
    fld.cOutThick = 1;
    fld.pos = (sfVector2i){.x = 10, .y = 10+550-24}; // Fld bot left corner
    fld.size = (sfVector2i){.x = 10, .y = 22}; // Field's size in blocks

    next = (Shape){.x = 250+10+20, .y = 200,
        .cSize = {.x = 23, .y = 23}};

    initFld();
    texts = ListOfText_getFromListOfKeyMapOfString(ListOfKeyMapOfString_getFromYaml("dat/texts.yaml"));
    w.window = sfRenderWindow_create(w.mode,
            windowName_conf,
            sfResize | sfClose,
            NULL);
    if (!w.window)
        exit(EXIT_FAILURE);
}

void handleWindowEvents() {
    while (sfRenderWindow_pollEvent(w.window, &w.event))
        if (w.event.type == sfEvtClosed)
            sfRenderWindow_close(w.window);
}

void drawTextsAtScene(List *texts, char *scene, sfRenderWindow *window) {
    List *t = texts;
    while (t) {
        if (!strcmp(((Text *)t->obj)->scene, scene))
            sfRenderWindow_drawText(window, ((Text *)t->obj)->sfText, NULL);
        t = t->next;
    }
}

void gameLoop() {
    tTick();
    tKeyCtrl();
    valueAfterTextDisplay(game.scoreCurrent, texts, "score");
    valueAfterTextDisplay(game.level, texts, "level");
    colorizeFld();
    colorizeActive();
    drawFld(w.window);
    drawNextShape(w.window);
    drawTextsAtScene(texts, "game", w.window);
}

void menuTick()
{
    if(sfClock_getElapsedTime(mTick).microseconds >= basicLatency/game.level) {
        sfClock_restart(mTick);
        colorizeRandom(&fld);
    }
}

void menuLoop() {
    menuTick();
    drawFld(w.window);
    drawTextsAtScene(texts, "menu", w.window);
    if (sfKeyboard_isKeyPressed(sfKeyS) == 1) {
        game.isStarted = 1;
        freeFld();
        initFld();
        sfClock_restart(gameTick);
    }
}

void mainLoop() {
    while (sfRenderWindow_isOpen(w.window)) {
        handleWindowEvents();
        sfRenderWindow_clear(w.window, UIBGCOLOR);
        if (game.isStarted)
            gameLoop();
        else
            menuLoop();
        sfRenderWindow_display(w.window);
    }
}

int main()
{
    prepare();
    colorizeRandom(&fld);
    mainLoop();
    freeFld();
    sfRenderWindow_destroy(w.window);
    ListOfText_free(&texts);
    return EXIT_SUCCESS;
}
