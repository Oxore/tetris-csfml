#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <SFML/System/Clock.h>
#include <SFML/Window/Keyboard.h>
#include <SFML/Graphics/RenderWindow.h>
#include <SFML/Graphics/Font.h>

#include "common.h"
#include "text.h"
#include "field.h"
#include "draw.h"
#include "engine.h"
#include "tet_conf.h"

extern struct window w;

List         *texts;
sfFont       *fontScore;
struct shape  active, next;
struct field  fld;
struct game   game = {
    .isStarted = 0,
    .scoreCurrent = 0,
    .level = 1,
    .moveLatency = L00LATENCY,
    .lines = 0
};

char arrKeys = 0; // Arrow keys states byte container

void prepare() {
    sfRenderWindow_setFramerateLimit(w.window, 60);

    /*
     * Dimensions of every fld's cell
     * 23px - fill color 1px - for outline, 25 - at all
     */
    fld.cSize = CELL_SIZE; //Fld's cell size in pixels
    fld.cOutThick = 1;
    fld.pos = FLD_POS; // Fld bot left corner
    fld.size = (sfVector2i){.x = FLD_SIZE_X, .y = FLD_SIZE_Y}; // Field's size in blocks
    fld.bound = (sfVector2i){.x = FLD_BOUND_X, .y = FLD_BOUND_Y}; // Field's bound in blocks

    next = (struct shape){
        .x = 250 + 10 + 20,
        .y = 200,
        .cOutThick = 1,
        .cSize = {.x = 23, .y = 23}
    };

    init_field(&fld);
    init_next_shape(&next);
    gen_shape(&next);
    active.t = next.t;
    resetActiveShape(&fld, &active);
    gen_shape(&next);
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
    colorize_field(&fld);
    colorize_active_shape(&fld, &active);
    drawFld(w.window, &fld);
    drawNextShape(w.window);
    drawTextsAtScene(texts, "game", w.window);
}

void menuTick()
{
    if (sfClock_getElapsedTime(game.mTick).microseconds >= basicLatency) {
        sfClock_restart(game.mTick);
        colorize_field_random(&fld);
        painter_update_field(fld.id, &fld);
    }
}

void menuLoop() {
    menuTick();
    drawTextsAtScene(texts, "menu", w.window);
    if (sfKeyboard_isKeyPressed(sfKeyS) == 1) {
        game.isStarted = 1;
        free_field(&fld);
        free_shape(&next);
        init_field(&fld);
        init_next_shape(&next);
        sfClock_restart(game.gameTick);
    }
    painter_draw();
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
    srand(time(NULL));
    game.gameTick = sfClock_create();
    game.putTick = sfClock_create();
    game.mTick = sfClock_create();
    game.repPushDown = sfClock_create();
    game.repKeyLeft = sfClock_create();
    game.repKeyRight = sfClock_create();
    fontScore = sfFont_createFromFile("dat/arial.ttf");
    if (!fontScore) {
        printf("dat/arial.ttf font load failed");
        exit(-1);
    }
    painter_init_window();
    List *tmp = ListOfKeyMapOfString_getFromYaml("dat/texts.yaml");
    texts = ListOfText_getFromListOfKeyMapOfString(tmp);
    ListOfKeyMapOfString_free(&tmp);

    prepare();
    colorize_field_random(&fld);
    fld.id = painter_register_field();
    painter_update_field(fld.id, &fld);
    mainLoop();
    free_field(&fld);
    free_shape(&next);

    painter_destroy_window();
    sfFont_destroy(fontScore);
    sfClock_destroy(game.gameTick);
    sfClock_destroy(game.putTick);
    sfClock_destroy(game.mTick);
    sfClock_destroy(game.repPushDown);
    sfClock_destroy(game.repKeyLeft);
    sfClock_destroy(game.repKeyRight);
    ListOfText_free(&texts);
    return EXIT_SUCCESS;
}
