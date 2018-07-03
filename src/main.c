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
struct field  fld, nxt;
struct game   game = {
    .isStarted = 0,
    .scoreCurrent = 0,
    .level = 1,
    .moveLatency = L00LATENCY,
    .lines = 0
};

char arrKeys = 0; // Arrow keys states byte container

void prepare() {
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
    painter_update_field(fld.id, &fld);
    painter_update_field(nxt.id, &nxt);
    drawTextsAtScene(texts, "game", w.window);
    painter_draw();
}

void menuTick()
{
    if (sfClock_getElapsedTime(game.mTick).microseconds >= basicLatency) {
        sfClock_restart(game.mTick);
        field_fill_random(&fld);
        painter_update_field(fld.id, &fld);
    }
}

void menuLoop() {
    menuTick();
    drawTextsAtScene(texts, "menu", w.window);
    if (sfKeyboard_isKeyPressed(sfKeyS) == 1) {
        game.isStarted = 1;
        field_clear(&fld);
        shape_gen_random(&fld.shape[0]);
        field_reset_walking_shape(&fld, 0);
        for (unsigned int i = 0; i < nxt.shape_cnt; ++i)
            shape_gen_random(&nxt.shape[i]);
        nxt.attr &= ~FLD_ATTR_INVISIBLE;
        sfClock_restart(game.gameTick);
    }
    painter_draw();
}

void mainLoop() {
    while (sfRenderWindow_isOpen(w.window)) {
        handleWindowEvents();
        sfRenderWindow_clear(w.window, (sfColor)UIBGCOLOR);
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

    fld.pos = FLD_POS;
    fld.size = (struct vector2ui){.x = FLD_SIZE_X, .y = FLD_SIZE_Y};
    fld.bound = (struct vector2ui){.x = FLD_BOUND_X, .y = FLD_BOUND_Y};
    fld.shape_cnt = 1;
    field_init(&fld);

    nxt.pos = NXT_POS;
    nxt.size = NXT_SIZE;
    nxt.bound = NXT_SIZE;
    nxt.shape_cnt = 3;
    nxt.attr |= FLD_ATTR_TRANSPARENT | FLD_ATTR_INVISIBLE;
    field_init(&nxt);
    nxt.shape[0].y = 4;
    nxt.shape[1].y = 1;
    nxt.shape[2].y = -2;

    fld.id = painter_register_field(&fld);
    nxt.id = painter_register_field(&nxt);
    field_fill_random(&fld);
    painter_update_field(fld.id, &fld);
    painter_update_field(nxt.id, &nxt);

    mainLoop();
    painter_destroy_drawables();
    field_deinit(&fld);
    field_deinit(&nxt);

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
