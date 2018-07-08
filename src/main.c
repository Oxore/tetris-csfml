#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <SFML/System/Clock.h>
#include <SFML/Window/Keyboard.h>
#include <SFML/Graphics/RenderWindow.h>
#include <SFML/Graphics/Font.h>

#include "common.h"
#include "idlist.h"
#include "vector.h"
#include "text.h"
#include "field.h"
#include "painter.h"
#include "engine.h"
#include "tet_conf.h"

sfRenderWindow *window;
struct idlist *texts;

sfFont       *fontScore;
struct field  fld, nxt;
struct game   game = {
    .isStarted = 0,
    .scoreCurrent = 0,
    .level = 1,
    .moveLatency = L00LATENCY,
    .lines = 0
};

char arrKeys = 0;

static void handleWindowEvents() {
    sfEvent event;
    while (sfRenderWindow_pollEvent(window, &event))
        if (event.type == sfEvtClosed)
            sfRenderWindow_close(window);
}

static void register_text(void *obj)
{
    struct text *text = obj;
    text->id = painter_register_text(text);
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
    painter_load_font("dat/arial.ttf");
    fontScore = sfFont_createFromFile("dat/arial.ttf");
    if (!fontScore) {
        printf("%s font load failed", "dat/arial.ttf");
        exit(EXIT_FAILURE);
    }

    sfVideoMode mode = (sfVideoMode){450, 570, 32};
    window = sfRenderWindow_create(mode, windowName_conf, sfResize | sfClose, NULL);
    if (!window)
        exit(EXIT_FAILURE);
    sfRenderWindow_setFramerateLimit(window, 60);
    painter_set_window(window);

    fld.pos = FLD_POS;
    fld.size = (struct vector2ui){.x = FLD_SIZE_X, .y = FLD_SIZE_Y};
    fld.bound = (struct vector2ui){.x = FLD_BOUND_X, .y = FLD_BOUND_Y};
    fld.shape_cnt = 2;
    field_init(&fld);
    fld.shape[0].attr |= SHP_ATTR_GHOST;

    nxt.pos = NXT_POS;
    nxt.size = NXT_SIZE;
    nxt.bound = NXT_SIZE;
    nxt.shape_cnt = 3;
    nxt.attr |= FLD_ATTR_HIDE_EMPTY_CELLS | FLD_ATTR_INVISIBLE;
    field_init(&nxt);
    nxt.shape[0].y = 4;
    nxt.shape[1].y = 1;
    nxt.shape[2].y = -2;

    fld.id = painter_register_field(&fld);
    nxt.id = painter_register_field(&nxt);
    field_fill_random(&fld);
    painter_update_field(fld.id, &fld);
    painter_update_field(nxt.id, &nxt);

    texts = load_texts("dat/texts.yaml");

    list_foreach(texts, register_text);

    transition_init();
    while (sfRenderWindow_isOpen(window)) {
        handleWindowEvents();
        main_loop();
    }

    list_foreach(texts, text_destroy);
    list_destroy(texts);

    painter_destroy_drawables();
    field_deinit(&fld);
    field_deinit(&nxt);

    if (window) {
        sfRenderWindow_destroy(window);
        window = 0;
    }
    sfFont_destroy(fontScore);
    painter_destroy_font();
    sfClock_destroy(game.gameTick);
    sfClock_destroy(game.putTick);
    sfClock_destroy(game.mTick);
    sfClock_destroy(game.repPushDown);
    sfClock_destroy(game.repKeyLeft);
    sfClock_destroy(game.repKeyRight);
    return EXIT_SUCCESS;
}
