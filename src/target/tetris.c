/*
 * tetris.c
 *
 * The main file. Here all the resources are initialized, ran and destroyed.
 *
 * */

#include <SFML/System/Clock.h>
#include <SFML/Graphics/RenderWindow.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#include "config.h"
#include "vector.h"
#include "input.h"
#include "hs_table.h"
#include "game.h"
#include "idlist.h"
#include "text.h"
#include "field.h"
#include "painter.h"

static struct idlist *handle_window_events(sfRenderWindow *window)
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
        .score = 0,
        .level = 1,
        .tick_period = CFG_L00_CLOCK_PERIOD,
        .rows = 0,
        .game_clock = NULL,
        .game_over_wait_clock = NULL,
        .put_clock = NULL,
        .menu_clock = NULL,
        .controls = {
            .keys = 0,
            .down_repeat_clock = NULL,
            .left_repeat_clock = NULL,
            .right_repeat_clock = NULL,
        },
        .fld = &fld,
        .nxt = &nxt,
        .texts = NULL,
        .input_name = {
            .id = -1,
            .attr = INPUT_ATTR_INVISIBLE,
            .fontsize = 20,
            .nbytes = 0,
            .pos = { .x = 30, .y = 200 },
        },
        .highscores = {
            .id = -1,
            .attr = HS_TABLE_ATTR_INVISIBLE,
            .nentries = 0,
            .fontsize = 20,
            .name_cell_size = { .x = 300, .y = 25 },
            .score_cell_size = { .x = 100, .y = 25 },
            .pos = { .x = 30, .y = 80 },
        },
    };

    srand(time(NULL));
    game.game_clock = sfClock_create();
    game.game_over_wait_clock = sfClock_create();
    game.put_clock = sfClock_create();
    game.menu_clock = sfClock_create();
    game.controls.down_repeat_clock = sfClock_create();
    game.controls.left_repeat_clock = sfClock_create();
    game.controls.right_repeat_clock = sfClock_create();

    hs_table_load_from_json_file(&game.highscores, "dat/highscores.json");

    painter_load_font("dat/arial.ttf");

    sfVideoMode mode = (sfVideoMode){450, 570, 32};
    window = sfRenderWindow_create(mode, CFG_WIN_NAME, sfResize | sfClose,
            NULL);
    if (!window)
        exit(EXIT_FAILURE);
    sfRenderWindow_setFramerateLimit(window, 60);
    painter_set_window(window);

    fld.pos = CFG_FLD_POS;
    fld.size = (struct vector2ui){.x = CFG_FLD_SIZE_X, .y = CFG_FLD_SIZE_Y};
    fld.bound = (struct vector2ui){.x = CFG_FLD_BOUND_X, .y = CFG_FLD_BOUND_Y};
    fld.shape_cnt = 2;
    field_init(&fld);
    fld.shape[0].attr |= SHP_ATTR_GHOST;

    nxt.pos = CFG_NXT_POS;
    nxt.size = CFG_NXT_SIZE;
    nxt.bound = CFG_NXT_SIZE;
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

    game.input_name.id = painter_register_input(&game.input_name);
    painter_update_input(game.input_name.id, &game.input_name);
    game.highscores.id = painter_register_hs_table(&game.highscores);

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
        struct idlist *events = handle_window_events(window);
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

    sfClock_destroy(game.game_clock);
    sfClock_destroy(game.game_over_wait_clock);
    sfClock_destroy(game.put_clock);
    sfClock_destroy(game.menu_clock);
    sfClock_destroy(game.controls.down_repeat_clock);
    sfClock_destroy(game.controls.left_repeat_clock);
    sfClock_destroy(game.controls.right_repeat_clock);

    return EXIT_SUCCESS;
}
