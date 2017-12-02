#include "common.h"
#include "functions.h"
#include "text.h"

/* --- Variables --- */
Window w = {.mode = {450, 520, 32}};
Game game = {.isStarted = 0, .scoreCurrent = 0, .level = 1};
List *texts;
Text menu1;
Text menu2;
Text score;
Text level;
sfFont *fontScore;
Shape active, next;
Field fld;

uint8_t arrKeys = 0b00000000; // Arrow keys states byte container
/* --- Variables End --- */

sfClock *gameTick;
sfClock *mTick;
sfClock *repPushDown;    // Clock for repeat latency when Down arrow long push
sfClock *repKeyLeft;    // Clock for repeat latency when Left arrow long push
sfClock *repKeyRight;    // Clock for repeat latency when Left arrow long push

void prepare() {
    srand( time(NULL) );
    gameTick = sfClock_create();
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
    fld.pos = (sfVector2i){.x = 10, .y = 10+500-24}; // Fld bot left corner
    fld.size = (sfVector2i){.x = 10, .y = 25}; // Field's size in blocks

    next = (Shape){.x = 250+10+20, .y = 200,
        .cSize = {.x = 23, .y = 23}};

    initFld();

    texts = ListOfText_getFromListOfKeyMapOfString(ListOfKeyMapOfString_getFromYaml("dat/ya.yaml"));
    score.sfText = sfText_create();
    sfText_setFont(score.sfText, fontScore);
    sfText_setCharacterSize(score.sfText, 20);
    sfText_setPosition(score.sfText, (sfVector2f){.x = 250+10+10, .y = 10});

    level.sfText = sfText_create();
    sfText_setFont(level.sfText, fontScore);
    sfText_setCharacterSize(level.sfText, 20);
    sfText_setPosition(level.sfText, (sfVector2f){.x = 250+10+10, .y = 44});

    /*
     * Menu texts
     *
     */
    menu1.sfText = sfText_create();
    sfText_setFont(menu1.sfText, fontScore);
    sfText_setCharacterSize(menu1.sfText, 36);
    sfText_setPosition(menu1.sfText, (sfVector2f){.x = 250+10+30, .y = 100});
    sfText_setString(menu1.sfText, "TETRIS");

    menu2.sfText = sfText_create();
    sfText_setFont(menu2.sfText, fontScore);
    sfText_setCharacterSize(menu2.sfText, 20);
    sfText_setPosition(menu2.sfText, (sfVector2f){.x = 250+10+16, .y = 200});
    sfText_setString(menu2.sfText, "Press \"S\" to start");

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

void gameLoop() {
    tTick();
    tKeyCtrl();
    scoreDisplay(game.scoreCurrent, &score);
    levelDisplay(game.level, &level);
    colorizeFld();
    colorizeActive();
    drawFld(w.window);
    drawNextShape(w.window);
    sfRenderWindow_drawText(w.window, score.sfText, NULL);
    sfRenderWindow_drawText(w.window, level.sfText, NULL);
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
    sfRenderWindow_drawText(w.window, menu1.sfText, NULL);
    sfRenderWindow_drawText(w.window, menu2.sfText, NULL);
    if (sfKeyboard_isKeyPressed(sfKeyS) == 1) {
        game.isStarted = 1;
        freeFld();
        initFld();
    }
}

void mainLoop() {
    while (sfRenderWindow_isOpen(w.window)) {
        handleWindowEvents();
        sfRenderWindow_clear(w.window, sfBlack);
        if (game.isStarted) {
            gameLoop();
        } else {
            menuLoop();
        }
        sfRenderWindow_display(w.window);
    }
}

int main()
{
    prepare();
    colorizeRandom(&fld);
    mainLoop();
    /* Just senseless printf */
    printf("%d\n", game.scoreCurrent);
    freeFld();
    sfRenderWindow_destroy(w.window);
    sfText_destroy(score.sfText);
    sfText_destroy(menu1.sfText);
    sfText_destroy(menu2.sfText);

    return EXIT_SUCCESS;
}
