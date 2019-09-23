/*
 * tetris.c
 *
 * The main file. Here all the resources are initialized, run and destroyed.
 *
 * */

#include <SFML/System/Clock.h>
#include <SFML/Graphics/RenderWindow.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#include "common.h"
#include "idlist.h"
#include "vector.h"
#include "text.h"
#include "field.h"
#include "painter.h"
#include "engine.h"
#include "tet_conf.h"

static struct idlist *handleWindowEvents(sfRenderWindow *window)
{
    sfEvent event;
    struct idlist *events = NULL;

    while (sfRenderWindow_pollEvent(window, &event)) {
        if (event.type == sfEvtClosed) {
            sfRenderWindow_close(window);

        } else {
            struct idlist *e = NULL;

            if (events == NULL) {
                e = events = list_new();
            } else {
                e = list_append(events);
            }

            e->obj = calloc(1, sizeof(sfEvent));
            *(sfEvent *)e->obj = event;
        }
    }
    return events;
}

static void register_text(void *obj)
{
    struct text *text = obj;
    text->id = painter_register_text(text);
}

int main()
{
    sfRenderWindow *window;

    struct idlist  *texts;
    struct field fld, nxt;
    struct game game = {
        .state = GS_MAIN_MENU,
        .scoreCurrent = 0,
        .level = 1,
        .moveLatency = L00LATENCY,
        .lines = 0,
        .gameTick = NULL,
        .over_wait_tick = NULL,
        .putTick = NULL,
        .mTick = NULL,
        .controls = {
            .keys = 0,
            .repPushDown = NULL,
            .repKeyLeft = NULL,
            .repKeyRight = NULL,
        },
        .fld = &fld,
        .nxt = &nxt,
        .texts = NULL,
    };

    srand(time(NULL));
    game.gameTick = sfClock_create();
    game.over_wait_tick = sfClock_create();
    game.putTick = sfClock_create();
    game.mTick = sfClock_create();
    game.controls.repPushDown = sfClock_create();
    game.controls.repKeyLeft = sfClock_create();
    game.controls.repKeyRight = sfClock_create();

    painter_load_font("dat/arial.ttf");

    sfVideoMode mode = (sfVideoMode){450, 570, 32};
    window = sfRenderWindow_create(mode, windowName_conf, sfResize | sfClose,
                                                                        NULL);
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

    texts = load_texts_from_json("dat/texts.json");
    if (texts == NULL) {
        goto cleanup_load_texts;
    }

    LIST_FOREACH(texts, text) {
        register_text(text->obj);
    }

    game.texts = texts;

    transition_init(&game);
    while (sfRenderWindow_isOpen(window)) {
        struct idlist *events = handleWindowEvents(window);
        main_loop(&game, events);
        LIST_FOREACH(events, event)
            free(event->obj);
        list_destroy(events);
    }

    LIST_FOREACH(texts, text) {
        text_destroy(text->obj);
    }

    list_destroy(texts);

cleanup_load_texts:
    painter_destroy_drawables();
    field_deinit(&fld);
    field_deinit(&nxt);

    if (window) {
        sfRenderWindow_destroy(window);
        window = 0;
    }
    painter_destroy_font();

    sfClock_destroy(game.gameTick);
    sfClock_destroy(game.over_wait_tick);
    sfClock_destroy(game.putTick);
    sfClock_destroy(game.mTick);
    sfClock_destroy(game.controls.repPushDown);
    sfClock_destroy(game.controls.repKeyLeft);
    sfClock_destroy(game.controls.repKeyRight);

    return EXIT_SUCCESS;
}
