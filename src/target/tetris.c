/*
 * tetris.c
 *
 * The main file. Here all the resources are initialized, ran and destroyed.
 *
 * */

#include <assert.h>
#include <SFML/System/Clock.h>
#include <SFML/Graphics/RenderWindow.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "config.h"
#include "vector.h"
#include "input.h"
#include "hs_table.h"
#include "game.h"
#include "idlist.h"
#include "slist.h"
#include "text.h"
#include "field.h"
#include "painter.h"
#include "event.h"
#include "media.h"
#include "controller.h"

static void register_text(void *obj)
{
    struct text *text = obj;
    text->id = painter_register_text(text);
}

int main()
{
    int ret = EXIT_SUCCESS;
    struct slist *texts;
    struct field fld = {
        .id = SIZE_MAX,
        .attr = 0,
        .pos = CFG_FLD_POS,
        .size = {
            .x = CFG_FLD_SIZE_X,
            .y = CFG_FLD_SIZE_Y,
        },
        .bound = {
            .x = CFG_FLD_BOUND_X,
            .y = CFG_FLD_BOUND_Y,
        },
        .c = NULL,
        .shape_cnt = 2,
        .shape = NULL,
    };

    field_init(&fld);
    fld.shape[0].attr |= SHP_ATTR_GHOST;

    struct field nxt = {
        .id = SIZE_MAX,
        .attr = FLD_ATTR_HIDE_EMPTY_CELLS | FLD_ATTR_INVISIBLE,
        .pos = CFG_NXT_POS,
        .size = CFG_NXT_SIZE,
        .bound = CFG_NXT_SIZE,
        .c = NULL,
        .shape_cnt = 3,
        .shape = NULL,
    };

    field_init(&nxt);
    nxt.shape[0].y = 4;
    nxt.shape[1].y = 1;
    nxt.shape[2].y = -2;

    struct controls controls = {
        .keys = 0,
        .down_repeat_timeout = media_timeout_new(0),
        .left_repeat_timeout = media_timeout_new(0),
        .right_repeat_timeout = media_timeout_new(0),
    };

    struct game game = {
        .state = GS_MAIN_MENU,
        .score = 0,
        .level = 1,
        .tick_period = CFG_L00_CLOCK_PERIOD,
        .rows = 0,
        .game_clock = sfClock_create(),
        .game_over_wait_clock = sfClock_create(),
        .put_clock = sfClock_create(),
        .menu_clock = sfClock_create(),
        .window = NULL,
        .config = &g_config,
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

    hs_table_load_from_json_file(&game.highscores, CFG_HIGHSCORES_FNAME);
    config_load_from_json_file(&g_config, CFG_CONFIG_FNAME);

    painter_init();
    painter_load_font("dat/arial.ttf");

    sfVideoMode mode = (sfVideoMode){450, 570, 32};
    sfRenderWindow *window = sfRenderWindow_create(
            mode,
            CFG_WIN_NAME,
            sfResize | sfClose,
            NULL);
    if (!window) {
        ret = EXIT_FAILURE;
        goto cleanup_create_window;
    }

    sfRenderWindow_setFramerateLimit(window, 60);
    painter_set_window(window);
    game.window = window;

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
        ret = EXIT_FAILURE;
        goto cleanup_load_texts;
    }

    SLIST_FOREACH(texts, text) {
        register_text(text->obj);
    }

    game.texts = texts;

    transition_init(&game);
    while (media_window_is_open(window)) {
        struct events_array events = {0};
        memset(&events, 0, sizeof(events));
        controller_gather_window_events(window, &events);
        controller_gather_controls_events(
                window,
                &g_config,
                &controls,
                &events);

        for (size_t i = 0; i < events.ptr; i++) {
            struct event e = events.events[i];
            if (e.type == EVENT_INPUT) {
                main_loop(&game, &e.input);
            }
        }

        /* FIXME: Workaround, because timers for tetramino advancing steps are
         * inside the `game` structure. Probably this is kinda OK way. */
        main_loop(&game, NULL);

        /* Effectively this is the only blocking function call in the whole
         * program. It is blocking because underlying graphics library strives
         * to ensure 60 FPS by blocking on drawing procedure. */
        painter_draw();
    }

    SLIST_FOREACH(texts, text) {
        text_destroy(text->obj);
    }

    slist_destroy(texts);

cleanup_load_texts:
    painter_destroy_drawables();

    if (window) {
        media_window_destroy(window);
        window = 0;
    }

cleanup_create_window:
    painter_destroy_font();
    painter_deinit();

    sfClock_destroy(game.game_clock);
    sfClock_destroy(game.game_over_wait_clock);
    sfClock_destroy(game.put_clock);
    sfClock_destroy(game.menu_clock);
    media_timeout_destroy(controls.down_repeat_timeout);
    media_timeout_destroy(controls.left_repeat_timeout);
    media_timeout_destroy(controls.right_repeat_timeout);

    field_deinit(&fld);
    field_deinit(&nxt);

    return ret;
}
