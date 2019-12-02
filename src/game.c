/*
 * game.c
 *
 * Game module - business logic. A couple of functions are exposed to use in
 * the main tetris module (tetris.c), see game.h.
 *
 * The game has simple state machine:
 * legend: [transition]--> STATE -->[transition]--> STATE
 *
 *          (start)
 *             |
 *           [init]
 *             |    +------------------+  +------[unpause]<------+
 *             V    |                  V  V                      |
 * +-[menu]-> MENU ----[game_start]--> GAME -->[game_over_wait]----+
 * |                |                  |  |                      | |
 * |                +----[put_shape]---+  +---[pause]--> PAUSE --+ |
 * |                                                               |
 * |    +-- GAME_OVER <---[game_over]------ GAME_OVER_WAIT <-------+
 * |    |
 * |    +---[input_name]--> INPUT_NAME --------+
 * |                                           |
 * +-- HIGHSCORES_TABLE <--[highscores_table]--+
 *
 * Transitions between states are implemented in a distinct functions. Each
 * transition function name starts with "transition_". State functions has names
 * with suffix "_loop" except for "main_loop" function - it is just an
 * aggregator of state functions.
 *
 * */

#include <f8.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SFML/System/Clock.h>
#include <SFML/Graphics/RenderWindow.h>

#include "media.h"

#include "config.h"
#include "vector.h"
#include "input.h"
#include "hs_table.h"
#include "game.h"
#include "idlist.h"
#include "text.h"
#include "field.h"
#include "painter.h"

static int level_move_latency[] = {
    CFG_L00_CLOCK_PERIOD,
    CFG_L01_CLOCK_PERIOD,
    CFG_L02_CLOCK_PERIOD,
    CFG_L03_CLOCK_PERIOD,
    CFG_L04_CLOCK_PERIOD,
    CFG_L05_CLOCK_PERIOD,
    CFG_L06_CLOCK_PERIOD,
    CFG_L07_CLOCK_PERIOD,
    CFG_L08_CLOCK_PERIOD,
    CFG_L09_CLOCK_PERIOD,
    CFG_L10_CLOCK_PERIOD,
    CFG_L11_CLOCK_PERIOD,
    CFG_L12_CLOCK_PERIOD,
    CFG_L13_CLOCK_PERIOD,
    CFG_L14_CLOCK_PERIOD,
    CFG_L15_CLOCK_PERIOD,
    CFG_L16_CLOCK_PERIOD,
    CFG_L17_CLOCK_PERIOD,
    CFG_L18_CLOCK_PERIOD,
    CFG_L19_CLOCK_PERIOD,
    CFG_L20_CLOCK_PERIOD,
    CFG_L21_CLOCK_PERIOD,
    CFG_L22_CLOCK_PERIOD,
    CFG_L23_CLOCK_PERIOD,
    CFG_L24_CLOCK_PERIOD,
    CFG_L25_CLOCK_PERIOD,
    CFG_L26_CLOCK_PERIOD,
    CFG_L27_CLOCK_PERIOD,
    CFG_L28_CLOCK_PERIOD,
    CFG_L29_CLOCK_PERIOD
};

static unsigned int reward_for_rows[] = {
    0,
    CFG_REWARD_FOR_1_ROW,
    CFG_REWARD_FOR_2_ROW,
    CFG_REWARD_FOR_3_ROW,
    CFG_REWARD_FOR_4_ROW
};

static void render_score_value(struct game *game, void *obj)
{
    struct text *text = obj;
    if (!strcmp(text->type, "score_value")) {
        if (!text->text)
            text->text = calloc(BUFSIZ, sizeof(char));
        snprintf(text->text, BUFSIZ - 1, "%d", game->score);
    }
}

static void render_level_value(struct game *game, void *obj)
{
    struct text *text = obj;
    if (!strcmp(text->type, "level_value")) {
        if (!text->text)
            text->text = calloc(BUFSIZ, sizeof(char));
        snprintf(text->text, BUFSIZ - 1, "%lu", (long unsigned)game->level);
    }
}

static int get_level_tick_period(size_t level)
{
    if (level > 29)
        return level_move_latency[29];
    else
        return level_move_latency[level];
}

static void level_up(struct game *game)
{
    while (game->rows >= CFG_ROWS_FOR_LEVELUP) {
        game->level++;
        game->rows -= CFG_ROWS_FOR_LEVELUP;
        game->tick_period = get_level_tick_period(game->level);
    }
}

static void show_menu_title_text(void *obj)
{
    struct text *text = obj;
    if (!strcmp(text->scene, "menu") && !strcmp(text->type, "title"))
        text->attr &= ~TXT_ATTR_INVISIBLE;
}

static void show_menu_press_key_text(void *obj)
{
    struct text *text = obj;
    if (!strcmp(text->scene, "menu") && !strcmp(text->type, "press_key"))
        text->attr &= ~TXT_ATTR_INVISIBLE;
}

static void show_game_score_text(void *obj)
{
    struct text *text = obj;
    if (!strcmp(text->scene, "game") && !strcmp(text->type, "score"))
        text->attr &= ~TXT_ATTR_INVISIBLE;
}

static void show_game_score_value_text(void *obj)
{
    struct text *text = obj;
    if (!strcmp(text->scene, "game") && !strcmp(text->type, "score_value"))
        text->attr &= ~TXT_ATTR_INVISIBLE;
}

static void show_game_level_text(void *obj)
{
    struct text *text = obj;
    if (!strcmp(text->scene, "game") && !strcmp(text->type, "level"))
        text->attr &= ~TXT_ATTR_INVISIBLE;
}

static void show_game_level_value_text(void *obj)
{
    struct text *text = obj;
    if (!strcmp(text->scene, "game") && !strcmp(text->type, "level_value"))
        text->attr &= ~TXT_ATTR_INVISIBLE;
}

static void show_game_next_shape_text(void *obj)
{
    struct text *text = obj;
    if (!strcmp(text->scene, "game") && !strcmp(text->type, "next_shape"))
        text->attr &= ~TXT_ATTR_INVISIBLE;
}

static void show_pause_text(void *obj)
{
    struct text *text = obj;
    if (!strcmp(text->scene, "game") && !strcmp(text->type, "pause"))
        text->attr &= ~TXT_ATTR_INVISIBLE;
}

static void show_game_over_title_text(void *obj)
{
    struct text *text = obj;
    if (!strcmp(text->scene, "game_over") && !strcmp(text->type, "title"))
        text->attr &= ~TXT_ATTR_INVISIBLE;
}

static void show_game_over_press_any_key_text(void *obj)
{
    struct text *text = obj;
    if (!strcmp(text->scene, "game_over") && !strcmp(text->type, "press_key"))
        text->attr &= ~TXT_ATTR_INVISIBLE;
}

static void show_input_name_text(void *obj)
{
    struct text *text = obj;
    if (!strcmp(text->scene, "input_name"))
        text->attr &= ~TXT_ATTR_INVISIBLE;
}

static void show_highscores_text(void *obj)
{
    struct text *text = obj;
    if (!strcmp(text->scene, "highscores"))
        text->attr &= ~TXT_ATTR_INVISIBLE;
}

static inline void hide_text(void *obj)
{
    ((struct text *)obj)->attr |= TXT_ATTR_INVISIBLE;
}

static inline void update_text(void *obj)
{
    painter_update_text(((struct text *)obj)->id, (struct text *)obj);
}

static void transition_menu(struct game *game)
{
    game->state = GS_MAIN_MENU;
    game->score = 0;
    game->level = 1;
    game->tick_period = get_level_tick_period(game->level);
    game->rows = 0;

    struct field *fld = game->fld;
    struct field *nxt = game->nxt;
    struct idlist *texts = game->texts;

    nxt->attr |= FLD_ATTR_INVISIBLE;
    painter_update_field(nxt->id, nxt);

    fld->shape[GHOST_SHAPE_INDEX].y = fld->size.y;
    fld->shape[ACTIVE_SHAPE_INDEX].y = fld->size.y;
    field_fill_random(fld);
    fld->attr &= ~FLD_ATTR_INVISIBLE;
    painter_update_field(fld->id, fld);

    LIST_FOREACH(texts, text) {
        hide_text(text->obj);
        show_menu_title_text(text->obj);
        show_menu_press_key_text(text->obj);
        update_text(text->obj);
    }

    game->highscores.attr |= HS_TABLE_ATTR_INVISIBLE;
    painter_update_hs_table(game->highscores.id, &game->highscores);
}

static void transition_highscores_input(struct game *game)
{
    game->nxt->attr |= FLD_ATTR_INVISIBLE;
    painter_update_field(game->nxt->id, game->nxt);
    game->fld->attr |= FLD_ATTR_INVISIBLE;
    painter_update_field(game->fld->id, game->fld);

    LIST_FOREACH(game->texts, text) {
        hide_text(text->obj);
        show_input_name_text(text->obj);
        update_text(text->obj);
    }

    game->state = GS_HIGHSCORES_INPUT;
    game->input_name.attr &= ~INPUT_ATTR_INVISIBLE;
    painter_update_input(game->input_name.id, &game->input_name);
    game->highscores.attr |= HS_TABLE_ATTR_INVISIBLE;
    painter_update_hs_table(game->highscores.id, &game->highscores);
}

static void transition_highscores_table(struct game *game)
{
    game->state = GS_HIGHSCORES_TABLE;

    game->input_name.attr |= INPUT_ATTR_INVISIBLE;
    painter_update_input(game->input_name.id, &game->input_name);

    hs_table_insert(&game->highscores, game->input_name.text,
            (size_t)game->score);
    hs_table_save_to_json_file(&game->highscores, CFG_HIGHSCORES_FNAME);

    input_clear(&game->input_name);

    game->highscores.attr &= ~HS_TABLE_ATTR_INVISIBLE;
    painter_update_hs_table(game->highscores.id, &game->highscores);

    LIST_FOREACH(game->texts, text) {
        hide_text(text->obj);
        show_highscores_text(text->obj);
        update_text(text->obj);
    }
}

static void transition_game_over_wait(struct game *game)
{
    game->state = GS_GAME_OVER_WAIT;

    sfClock_restart(game->game_over_wait_clock);

    struct idlist *texts = game->texts;

    LIST_FOREACH(texts, text) {
        hide_text(text->obj);
        show_game_score_text(text->obj);
        show_game_score_value_text(text->obj);
        show_game_level_text(text->obj);
        show_game_level_value_text(text->obj);
        show_game_next_shape_text(text->obj);
        show_game_over_title_text(text->obj);
        update_text(text->obj);
    }
}

static void transition_game_over(struct game *game)
{
    game->state = GS_GAME_OVER;

    struct idlist *texts = game->texts;

    LIST_FOREACH(texts, text) {
        hide_text(text->obj);
        show_game_score_text(text->obj);
        show_game_score_value_text(text->obj);
        show_game_level_text(text->obj);
        show_game_level_value_text(text->obj);
        show_game_next_shape_text(text->obj);
        show_game_over_title_text(text->obj);
        show_game_over_press_any_key_text(text->obj);
        update_text(text->obj);
    }
}

static void project_ghost_shape(struct field *fld, size_t idreal,
        size_t idghost)
{
    fld->shape[idghost].t = fld->shape[idreal].t;
    fld->shape[idghost].x = fld->shape[idreal].x;
    fld->shape[idghost].y = fld->shape[idreal].y;
    for (size_t j = 0; j < 4; j++)
        for (size_t i = 0; i < 4; i++)
            fld->shape[idghost].c[j][i] = fld->shape[idreal].c[j][i];
    do {
        --fld->shape[idghost].y;
    } while (!field_shape_collision(fld, &fld->shape[idghost]));
    ++fld->shape[idghost].y;
}

static void transition_put_shape(struct game *game)
{
    struct field *fld = game->fld;
    struct field *nxt = game->nxt;

    field_put_shape(fld, &fld->shape[ACTIVE_SHAPE_INDEX]);
    size_t num_rows_removed = field_rm_rows(fld);
    game->rows += num_rows_removed;
    game->score += reward_for_rows[num_rows_removed] * game->level;
    fld->shape[ACTIVE_SHAPE_INDEX].t = nxt->shape[0].t;
    field_reset_walking_shape(fld, 1);
    project_ghost_shape(fld, 1, 0);
    for (size_t s = 0; s < nxt->shape_cnt - 1; ++s) {
        nxt->shape[s] = nxt->shape[s + 1];
        nxt->shape[s].y = 4 - s * 3;
    }
    shape_gen_random(&nxt->shape[nxt->shape_cnt - 1]);
    level_up(game);
}

static void transition_pause(struct game *game)
{
    game->state = GS_PAUSED;
    int elapsed = sfClock_getElapsedTime(game->game_clock).microseconds;
    if (game->tick_period - elapsed >= 0)
        game->tick_period -= elapsed;
    else
        game->tick_period = get_level_tick_period(game->level);
    sfClock_restart(game->game_clock);

    struct idlist *texts = game->texts;

    LIST_FOREACH(texts, text) {
        hide_text(text->obj);
        show_pause_text(text->obj);
        show_game_score_text(text->obj);
        show_game_score_value_text(text->obj);
        show_game_level_text(text->obj);
        show_game_level_value_text(text->obj);
        show_game_next_shape_text(text->obj);
        update_text(text->obj);
    }
}

static void transition_unpause(struct game *game)
{
    game->state = GS_STARTED;
    sfClock_restart(game->game_clock);

    struct idlist *texts = game->texts;

    LIST_FOREACH(texts, text) {
        hide_text(text->obj);
        show_game_score_text(text->obj);
        show_game_score_value_text(text->obj);
        show_game_level_text(text->obj);
        show_game_level_value_text(text->obj);
        show_game_next_shape_text(text->obj);
        update_text(text->obj);
    }
}

#define GAME_TICK_STEP      0
#define GAME_TICK_PUT_SHAPE 1
#define GAME_TICK_GAME_OVER 2
static int game_tick(struct game *game)
{
    sfClock_restart(game->game_clock);

    struct field *fld = game->fld;
    struct field *nxt = game->nxt;

    game->tick_period = get_level_tick_period(game->level);

    if (field_move_shape_down(fld, 1)) {
        project_ghost_shape(fld, 1, 0);
        sfClock_restart(game->put_clock);
    }

    if (sfClock_getElapsedTime(game->put_clock).microseconds
     >= CFG_PUT_CLOCK_PERIOD) {
        sfClock_restart(game->put_clock);
        if (field_shape_out_of_bounds(fld, &fld->shape[ACTIVE_SHAPE_INDEX]))
            return GAME_TICK_GAME_OVER;
        else
            return GAME_TICK_PUT_SHAPE;
    }

    struct idlist *texts = game->texts;

    LIST_FOREACH(texts, text) {
        render_score_value(game, text->obj);
        render_level_value(game, text->obj);
    }

    painter_update_field(fld->id, fld);
    painter_update_field(nxt->id, nxt);

    return GAME_TICK_STEP;
}

static void signal_rotate_right(struct game *game)
{
    struct field *fld = game->fld;

    field_rotate_shape_clockwise(fld, 1);
    project_ghost_shape(fld, 1, 0);
    sfClock_restart(game->put_clock);
}

static void signal_rotate_left(struct game *game)
{
    struct field *fld = game->fld;

    field_rotate_shape_counter_clockwise(fld, 1);
    project_ghost_shape(fld, 1, 0);
    sfClock_restart(game->put_clock);
}

static void signal_down(struct game *game)
{
    struct field *fld = game->fld;

    if (field_move_shape_down(fld, 1)) {
        project_ghost_shape(fld, 1, 0);
        sfClock_restart(game->game_clock);
        game->score++;
    }
}

static void signal_left(struct game *game)
{
    struct field *fld = game->fld;

    if (field_move_shape_left(fld, 1)) {
        project_ghost_shape(fld, 1, 0);
        sfClock_restart(game->put_clock);
    }
}

static void signal_right(struct game *game)
{
    struct field *fld = game->fld;

    if (field_move_shape_right(fld, 1)) {
        project_ghost_shape(fld, 1, 0);
        sfClock_restart(game->put_clock);
    }
}

static uint32_t game_keys(struct controls *ctl, struct config *config)
{
    uint32_t ret = 0;

    /* PAUSE */
    if (media_is_key_pressed(config->keys.pause)) {
        if (!(ctl->keys & PAUSE)) {
            ctl->keys |= PAUSE;
            ret |= PAUSE;
        }
    } else {
        ctl->keys &= ~PAUSE;
    }

    /* ROTRIGHT */
    if (media_is_key_pressed(config->keys.rotate_right)) {
        if (!(ctl->keys & ROTRIGHT)) {
            ctl->keys |= ROTRIGHT;
            ret |= ROTRIGHT;
        }
    } else {
        ctl->keys = ctl->keys & ~ROTRIGHT;
    }

    /* ROTLEFT */
    if (media_is_key_pressed(config->keys.rotate_left)) {
        if (!(ctl->keys & ROTLEFT)) {
            ctl->keys |= ROTLEFT;
            ret |= ROTLEFT;
        }
    } else {
        ctl->keys = ctl->keys & ~ROTLEFT;
    }

    /* HARDDROP */
    if (media_is_key_pressed(config->keys.drop)) {
        if (!(ctl->keys & HARDDROP)) {
            ctl->keys |= HARDDROP;
            ret |= HARDDROP;
        }
    } else {
        ctl->keys &= ~HARDDROP;
    }

    /* DOWN */
    if (media_is_key_pressed(config->keys.move_down)) {
        if (!(ctl->keys & DOWN)) {
            ctl->keys |= DOWN;
            ret |= DOWN;
            sfClock_restart(ctl->down_repeat_clock);
        } else {
            if (sfClock_getElapsedTime(ctl->down_repeat_clock).microseconds
             >= CFG_REPEAT_LATENCY)
                ctl->keys &= ~DOWN;
        }
    } else {
        ctl->keys &= ~DOWN;
    }

    /* LEFT */
    if (media_is_key_pressed(config->keys.move_left)
     && !media_is_key_pressed(config->keys.move_right)) {
        if (!(ctl->keys & LEFT)) {
            ctl->keys |= LEFT;
            ret |= LEFT;
            sfClock_restart(ctl->left_repeat_clock);
        } else if (!(ctl->keys & LEFTHOLD)) {
            if (sfClock_getElapsedTime(ctl->left_repeat_clock).microseconds
             >= CFG_PREREPEAT_LATENCY) {
                ctl->keys |= LEFTHOLD;
                ctl->keys &= ~LEFT;
            }
        } else {
            if (sfClock_getElapsedTime(ctl->left_repeat_clock).microseconds
             >= CFG_REPEAT_LATENCY)
                ctl->keys &= ~LEFT;
        }
    } else {
        ctl->keys &= ~LEFT;
        ctl->keys &= ~LEFTHOLD;
    }

    /* RIGHT */
    if (media_is_key_pressed(config->keys.move_right)
     && !media_is_key_pressed(config->keys.move_left)) {
        if (!(ctl->keys & RIGHT)) {
            ctl->keys |= RIGHT;
            ret |= RIGHT;
            sfClock_restart(ctl->right_repeat_clock);
        } else if (!(ctl->keys & RIGHTHOLD)) {
            if (sfClock_getElapsedTime(ctl->right_repeat_clock).microseconds
             >= CFG_PREREPEAT_LATENCY) {
                ctl->keys |= RIGHTHOLD;
                ctl->keys &= ~RIGHT;
            }
        } else {
            if (sfClock_getElapsedTime(ctl->right_repeat_clock).microseconds
             >= CFG_REPEAT_LATENCY)
                ctl->keys &= ~RIGHT;
        }
    } else {
        ctl->keys &= ~RIGHT;
        ctl->keys &= ~RIGHTHOLD;
    }

    return ret;
}

static void menu_tick(struct game *game)
{
    struct field *fld = game->fld;

    sfClock_restart(game->menu_clock);
    field_fill_random(fld);
    painter_update_field(fld->id, fld);
}

void transition_init(struct game *game)
{
    struct idlist *texts = game->texts;

    LIST_FOREACH(texts, text) {
        hide_text(text->obj);
        show_menu_title_text(text->obj);
        show_menu_press_key_text(text->obj);
        render_score_value(game, text->obj);
        render_level_value(game, text->obj);
        update_text(text->obj);
    }
}

static void transition_game_start(struct game *game)
{
    struct field *fld = game->fld;
    struct field *nxt = game->nxt;

    game->state = GS_STARTED;
    field_clear(fld);
    shape_gen_random(&fld->shape[ACTIVE_SHAPE_INDEX]);
    field_reset_walking_shape(fld, 1);
    project_ghost_shape(fld, 1, 0);
    shape_load(&fld->shape[ACTIVE_SHAPE_INDEX]);
    for (size_t i = 0; i < nxt->shape_cnt; ++i)
        shape_gen_random(&nxt->shape[i]);
    nxt->attr &= ~FLD_ATTR_INVISIBLE;

    struct idlist *texts = game->texts;

    LIST_FOREACH(texts, text) {
        hide_text(text->obj);
        show_game_score_text(text->obj);
        show_game_score_value_text(text->obj);
        show_game_level_text(text->obj);
        show_game_level_value_text(text->obj);
        show_game_next_shape_text(text->obj);
        render_score_value(game, text->obj);
        render_level_value(game, text->obj);
        update_text(text->obj);
    }

    painter_update_field(fld->id, fld);
    painter_update_field(nxt->id, nxt);
    sfClock_restart(game->game_clock);
}

#define MENU_LOOP_GAME_START 1
static int menu_loop(struct game *game)
{
    int ret = 0;

    if (sfClock_getElapsedTime(game->menu_clock).microseconds
     >= CFG_MENU_CLOCK_PERIOD)
        menu_tick(game);

    if (media_is_key_pressed(game->config->keys.start)) {
        if (!(game->controls.keys & GAMEOVER)) {
            ret = MENU_LOOP_GAME_START;
        }
    } else {
        game->controls.keys = 0;
    }

    return ret;
}

#define GAME_LOOP_PAUSE 1
#define GAME_LOOP_GAME_OVER 2
static int game_loop(struct game *game)
{
    int ret = 0;
    struct field *fld = game->fld;
    struct field *nxt = game->nxt;

    if (media_is_window_focused(game->window)) {

        // TODO: Elaborate on precedence of timers and ctl->keys checking
        // Here should be only one return statement - at the end of the function

        uint32_t ret_keys = game_keys(&game->controls, game->config);

        if (ret_keys & PAUSE) {
            ret = GAME_LOOP_PAUSE;
            sfClock_restart(game->put_clock);
        }

        if (ret_keys & ROTRIGHT) {
            signal_rotate_right(game);
        }

        if (ret_keys & ROTLEFT) {
            signal_rotate_left(game);
        }

        if (ret_keys & DOWN) {
            signal_down(game);
        }

        if (ret_keys & HARDDROP) {
            struct field *fld = game->fld;

            while (field_move_shape_down(fld, 1))
                game->score++;

            sfClock_restart(game->game_clock);
            sfClock_restart(game->put_clock);

            if (field_shape_out_of_bounds(fld, &fld->shape[ACTIVE_SHAPE_INDEX]))
                return GAME_LOOP_GAME_OVER;
            else
                transition_put_shape(game);
        }

        if (ret_keys & LEFT) {
            signal_left(game);
        }

        if (ret_keys & RIGHT) {
            signal_right(game);
        }
    }

    if (sfClock_getElapsedTime(game->game_clock).microseconds
     >= game->tick_period) {
        switch (game_tick(game)) {
        case GAME_TICK_PUT_SHAPE:
            transition_put_shape(game);
            break;

        case GAME_TICK_GAME_OVER:
            return GAME_LOOP_GAME_OVER;
            break;

        case GAME_TICK_STEP:
        default:
            break;
        }
    }

    struct idlist *texts = game->texts;

    LIST_FOREACH(texts, text) {
        render_score_value(game, text->obj);
        render_level_value(game, text->obj);
        update_text(text->obj);
    }
    painter_update_field(fld->id, fld);
    painter_update_field(nxt->id, nxt);

    return ret;
}

#define GAME_OVER_WAIT_LOOP_GAME_OVER 1
static int game_over_wait_loop(struct game *game)
{
    if (sfClock_getElapsedTime(game->game_over_wait_clock).microseconds
     > CFG_GAMEOVER_SHOWTIME_PERIOD)
        return GAME_OVER_WAIT_LOOP_GAME_OVER;

    return 0;
}

#define GAME_OVER_LOOP_HIGHSCORES_INPUT 1
static int game_over_loop(struct controls *ctl)
{
    int anykey = 0;

    for (int keycode = KEY_UNKNOWN; keycode < KEY_COUNT; keycode++)
        if (media_is_key_pressed(keycode))
            anykey = 1;

    if (anykey) {
        if (!(ctl->keys & GAMEOVER)) {
            ctl->keys |= GAMEOVER;
            return GAME_OVER_LOOP_HIGHSCORES_INPUT;
        }
    } else {
        ctl->keys &= ~GAMEOVER;
    }

    return 0;
}

#define HIGHSCORES_INPUT_LOOP_HIGHSCORES_TABLE 1
static int highscores_input_loop(struct game *game, const struct idlist *events)
{
#define UTF32_BACKSPACE L'\b'
    if (media_is_key_pressed(KEY_ENTER))
        return HIGHSCORES_INPUT_LOOP_HIGHSCORES_TABLE;

    LIST_FOREACH_CONST(events, event) {
        const sfEvent *e = (sfEvent *)event->obj;
        if (e->type == sfEvtTextEntered) {
            int32_t c = e->text.unicode;
            if (c == UTF32_BACKSPACE)
                input_rm_last_char(&game->input_name);
            else
                input_append_utf32char(&game->input_name, c);

            painter_update_input(game->input_name.id, &game->input_name);
        }
    }

    return 0;
}

#define HIGHSCORES_TABLE_LOOP_MENU 1
static int highscores_table_loop(struct controls *ctl, struct config *config)
{
    if (media_is_key_pressed(config->keys.start)) {
        if (!(ctl->keys & GAMEOVER)) {
            ctl->keys |= GAMEOVER;
            return HIGHSCORES_TABLE_LOOP_MENU;
        }
    } else {
        ctl->keys &= ~GAMEOVER;
    }

    return 0;
}

#define PAUSE_LOOP_UNPAUSE 1
static int pause_loop(struct game *game)
{
    if (media_is_window_focused(game->window)) {
        uint32_t ret = game_keys(&game->controls, game->config);
        if (ret & PAUSE) {
            sfClock_restart(game->put_clock);
            return PAUSE_LOOP_UNPAUSE;
        }
    }

    return 0;
}

void main_loop(struct game *game, const struct idlist *events)
{
    int ret;
    switch (game->state) {
    case GS_STARTED:
        ret = game_loop(game);
        if (ret == GAME_LOOP_GAME_OVER)
            transition_game_over_wait(game);
        else if (ret == GAME_LOOP_PAUSE)
            transition_pause(game);
        break;

    case GS_PAUSED:
        ret = pause_loop(game);
        if (ret == PAUSE_LOOP_UNPAUSE)
            transition_unpause(game);
        break;

    case GS_GAME_OVER:
        ret = game_over_loop(&game->controls);
        if (ret == GAME_OVER_LOOP_HIGHSCORES_INPUT)
            transition_highscores_input(game);
        break;

    case GS_GAME_OVER_WAIT:
        ret = game_over_wait_loop(game);
        if (ret == GAME_OVER_WAIT_LOOP_GAME_OVER)
            transition_game_over(game);
        break;

    case GS_HIGHSCORES_INPUT:
        ret = highscores_input_loop(game, events);
        if (ret == HIGHSCORES_INPUT_LOOP_HIGHSCORES_TABLE)
            transition_highscores_table(game);
        break;

    case GS_HIGHSCORES_TABLE:
        ret = highscores_table_loop(&game->controls, game->config);
        if (ret == HIGHSCORES_TABLE_LOOP_MENU)
            transition_menu(game);
        break;

    case GS_MAIN_MENU:
    default:
        ret = menu_loop(game);
        if (ret == MENU_LOOP_GAME_START)
            transition_game_start(game);
        break;
    }

    painter_draw();
}
