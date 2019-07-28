/*
 * engine.c
 *
 * Engine module - business logic. A couple of functions are exposed to use in
 * the main tetris module (tetris.c), see engine.h.
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
 * |                +----[put_shape]---+  +-->[pause]--> PAUSE --+ |
 * |                                                               |
 * +----- GAME_OVER <-----[game_over]<----- GAME_OVER_WAIT <-------+
 *
 * Transitions between states are implemented in a distinct functions. Each
 * transition function name starts with "transition_". State functions has names
 * with suffix "_loop" except for "main_loop" function - it is just an
 * aggregator of state functions.
 *
 * */

#include <f8.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SFML/System/Clock.h>
#include <SFML/Window/Keyboard.h>
#include <SFML/Graphics/RenderWindow.h>

#include "common.h"
#include "idlist.h"
#include "tet_conf.h"
#include "vector.h"
#include "text.h"
#include "field.h"
#include "painter.h"
#include "engine.h"

#define RIGHT     (1 << 0)
#define UP        (1 << 1)
#define DOWN      (1 << 2)
#define LEFT      (1 << 3)
#define RIGHTHOLD (1 << 4)
#define HARDDROP  (1 << 5)
#define PAUSE     (1 << 6)
#define LEFTHOLD  (1 << 7)
#define GAMEOVER  (1 << 8)

static int level_move_latency[] = {
    L00LATENCY,
    L01LATENCY,
    L02LATENCY,
    L03LATENCY,
    L04LATENCY,
    L05LATENCY,
    L06LATENCY,
    L07LATENCY,
    L08LATENCY,
    L09LATENCY,
    L10LATENCY,
    L11LATENCY,
    L12LATENCY,
    L13LATENCY,
    L14LATENCY,
    L15LATENCY,
    L16LATENCY,
    L17LATENCY,
    L18LATENCY,
    L19LATENCY,
    L20LATENCY,
    L21LATENCY,
    L22LATENCY,
    L23LATENCY,
    L24LATENCY,
    L25LATENCY,
    L26LATENCY,
    L27LATENCY,
    L28LATENCY,
    L29LATENCY
};

static int rmlines_score[] = {
    0,
    RM_1LINES_SCORE,
    RM_2LINES_SCORE,
    RM_3LINES_SCORE,
    RM_4LINES_SCORE
};

static int keys = 0;

static void render_score_value(struct game *game, void *obj)
{
    struct text *text = obj;
    if (!strcmp(text->type, "score.value")) {
        if (!text->text)
            text->text = calloc(BUFSIZ, sizeof(char));
        char *a = calloc(BUFSIZ, sizeof(char));
        sprintf(a, "%d", game->scoreCurrent);
        utf8to32_strcpy(text->text, a);
        free(a);
    }
}

static void render_level_value(struct game *game, void *obj)
{
    struct text *text = obj;
    if (!strcmp(text->type, "level.value")) {
        if (!text->text)
            text->text = calloc(BUFSIZ, sizeof(char));
        char *a = calloc(BUFSIZ, sizeof(char));
        sprintf(a, "%ld", game->level);
        utf8to32_strcpy(text->text, a);
        free(a);
    }
}

static int get_level_latency(size_t level)
{
    if (level > 29)
        return level_move_latency[29];
    else
        return level_move_latency[level];
}

static void level_up(struct game *game)
{
    while (game->lines >= LEVELUP_LINES) {
        game->level++;
        game->lines -= LEVELUP_LINES;
        game->moveLatency = get_level_latency(game->level);
    }
}

static void hide_menu_text(void *obj)
{
    struct text *text = obj;
    if (!strcmp(text->scene, "menu"))
        text->attr |= TXT_ATTR_INVISIBLE;
}

static void show_menu_text(void *obj)
{
    struct text *text = obj;
    if (!strcmp(text->scene, "menu"))
        text->attr &= ~TXT_ATTR_INVISIBLE;
}

static void hide_game_text(void *obj)
{
    struct text *text = obj;
    if (!strcmp(text->scene, "game"))
        text->attr |= TXT_ATTR_INVISIBLE;
}

static void show_game_text(void *obj)
{
    struct text *text = obj;
    if (!strcmp(text->scene, "game"))
        text->attr &= ~TXT_ATTR_INVISIBLE;
}

static void hide_pause_text(void *obj)
{
    struct text *text = obj;
    if (!strcmp(text->scene, "game") && !strcmp(text->type, "pause"))
        text->attr |= TXT_ATTR_INVISIBLE;
}

static void show_pause_text(void *obj)
{
    struct text *text = obj;
    if (!strcmp(text->scene, "game") && !strcmp(text->type, "pause"))
        text->attr &= ~TXT_ATTR_INVISIBLE;
}

static void hide_game_over_text(void *obj)
{
    struct text *text = obj;
    if (!strcmp(text->scene, "game_over"))
        text->attr |= TXT_ATTR_INVISIBLE;
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

static void update_game_over_text(void *obj)
{
    struct text *text = obj;
    if (!strcmp(text->scene, "game_over"))
        painter_update_text(text->id, text);
}

static void update_menu_text(void *obj)
{
    struct text *text = obj;
    if (!strcmp(text->scene, "menu"))
        painter_update_text(text->id, text);
}

static void update_game_text(void *obj)
{
    struct text *text = obj;
    if (!strcmp(text->scene, "game"))
        painter_update_text(text->id, text);
}

static void transition_menu(struct game *game)
{
    game->over = 0;
    game->started = 0;
    game->scoreCurrent = 0;
    game->level = 1;
    game->moveLatency = get_level_latency(game->level);
    game->lines = 0;

    struct field *fld = game->fld;
    struct field *nxt = game->nxt;
    struct idlist *texts = game->texts;

    nxt->attr |= FLD_ATTR_INVISIBLE;
    painter_update_field(nxt->id, nxt);

    fld->shape[GHOST_SHAPE_INDEX].y = fld->size.y;
    fld->shape[ACTIVE_SHAPE_INDEX].y = fld->size.y;
    field_fill_random(fld);
    painter_update_field(fld->id, fld);

    LIST_FOREACH(texts, text) {
        hide_game_over_text(text->obj);
        show_menu_text(text->obj);
        hide_game_text(text->obj);
        update_game_over_text(text->obj);
        update_menu_text(text->obj);
        update_game_text(text->obj);
    }
}

static void transition_game_over_wait(struct game *game)
{
    game->over_wait = 1;
    sfClock_restart(game->over_wait_tick);

    struct idlist *texts = game->texts;

    LIST_FOREACH(texts, text) {
        show_game_over_title_text(text->obj);
        update_game_over_text(text->obj);
    }
}

static void transition_game_over(struct game *game)
{
    game->over_wait = 0;
    game->over = 1;

    struct idlist *texts = game->texts;

    LIST_FOREACH(texts, text) {
        show_game_over_press_any_key_text(text->obj);
        update_game_over_text(text->obj);
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
    int removedLines = field_rm_lines(fld);
    game->lines += removedLines;
    game->scoreCurrent += rmlines_score[removedLines] * game->level;
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
    game->paused = 1;
    int elapsed = sfClock_getElapsedTime(game->gameTick).microseconds;
    if (game->moveLatency - elapsed >= 0)
        game->moveLatency -= elapsed;
    else
        game->moveLatency = get_level_latency(game->level);
    sfClock_restart(game->gameTick);

    struct idlist *texts = game->texts;

    LIST_FOREACH(texts, text) {
        show_pause_text(text->obj);
    }
}

static void transition_unpause(struct game *game)
{
    game->paused = 0;
    sfClock_restart(game->gameTick);

    struct idlist *texts = game->texts;

    LIST_FOREACH(texts, text) {
        hide_pause_text(text->obj);
    }
}

static void game_tick(struct game *game)
{
    sfClock_restart(game->gameTick);

    struct field *fld = game->fld;
    struct field *nxt = game->nxt;

    game->moveLatency = get_level_latency(game->level);
    if (field_move_shape_down(fld, 1)) {
        project_ghost_shape(fld, 1, 0);
        sfClock_restart(game->putTick);
    }
    if (sfClock_getElapsedTime(game->putTick).microseconds >= PUT_LATENCY) {
        if (field_shape_out_of_bounds(fld, &fld->shape[ACTIVE_SHAPE_INDEX]))
            transition_game_over_wait(game);
        else
            transition_put_shape(game);
        sfClock_restart(game->putTick);
    }

    struct idlist *texts = game->texts;

    LIST_FOREACH(texts, text) {
        render_score_value(game, text->obj);
        render_level_value(game, text->obj);
    }

    painter_update_field(fld->id, fld);
    painter_update_field(nxt->id, nxt);
}

static void signal_up(struct game *game)
{
    struct field *fld = game->fld;

    field_rotate_shape_clockwise(fld, 1);
    project_ghost_shape(fld, 1, 0);
    sfClock_restart(game->putTick);
}

static void signal_harddrop(struct game *game)
{
    struct field *fld = game->fld;

    while (field_move_shape_down(fld, 1))
        game->scoreCurrent++;
    if (field_shape_out_of_bounds(fld, &fld->shape[ACTIVE_SHAPE_INDEX]))
        transition_game_over_wait(game);
    else
        transition_put_shape(game);
    sfClock_restart(game->gameTick);
    sfClock_restart(game->putTick);
}

static void signal_down(struct game *game)
{
    struct field *fld = game->fld;

    if (field_move_shape_down(fld, 1)) {
        project_ghost_shape(fld, 1, 0);
        sfClock_restart(game->gameTick);
        game->scoreCurrent++;
    }
}

static void signal_left(struct game *game)
{
    struct field *fld = game->fld;

    if (field_move_shape_left(fld, 1)) {
        project_ghost_shape(fld, 1, 0);
        sfClock_restart(game->putTick);
    }
}

static void signal_right(struct game *game)
{
    struct field *fld = game->fld;

    if (field_move_shape_right(fld, 1)) {
        project_ghost_shape(fld, 1, 0);
        sfClock_restart(game->putTick);
    }
}

static void signal_pause(struct game *game)
{
    if (game->paused)
        transition_unpause(game);
    else
        transition_pause(game);
    sfClock_restart(game->putTick);
}

static void game_keys(struct game *game)
{
    /* PAUSE */
    if (sfKeyboard_isKeyPressed(sfKeyP)) {
        if (!(keys & PAUSE)) {
            keys |= PAUSE;
            signal_pause(game);
        }
    } else {
        keys &= ~PAUSE;
    }

    /* UP */
    if (sfKeyboard_isKeyPressed(sfKeyUp)) {
        if (!(keys & UP)) {
            keys = keys | UP;
            signal_up(game);
        }
    } else {
        keys = keys & ~UP;
    }

    /* HARDDROP */
    if (sfKeyboard_isKeyPressed(sfKeySpace)) {
        if (!(keys & HARDDROP)) {
            keys |= HARDDROP;
            signal_harddrop(game);
        }
    } else {
        keys &= ~HARDDROP;
    }

    /* DOWN */
    if (sfKeyboard_isKeyPressed(sfKeyDown)) {
        if (!(keys & DOWN)) {
            keys = keys | DOWN;
            signal_down(game);
            sfClock_restart(game->repPushDown);
        } else {
            if (sfClock_getElapsedTime(game->repPushDown).microseconds
             >= moveRepeatLatency2)
                keys &= ~DOWN;
        }
    } else {
        keys &= ~DOWN;
    }

    /* LEFT */
    if (sfKeyboard_isKeyPressed(sfKeyLeft)
     && !sfKeyboard_isKeyPressed(sfKeyRight)) {
        if (!(keys & LEFT)) {
            keys = keys | LEFT;
            signal_left(game);
            sfClock_restart(game->repKeyLeft);
        } else if (!(keys & LEFTHOLD)) {
            if (sfClock_getElapsedTime(game->repKeyLeft).microseconds
             >= moveRepeatLatency1) {
                keys |= LEFTHOLD;
                keys &= ~LEFT;
            }
        } else {
            if (sfClock_getElapsedTime(game->repKeyLeft).microseconds
             >= moveRepeatLatency2)
                keys &= ~LEFT;
        }
    } else {
        keys &= ~LEFT;
        keys &= ~LEFTHOLD;
    }

    /* RIGHT */
    if (sfKeyboard_isKeyPressed(sfKeyRight)
     && !sfKeyboard_isKeyPressed(sfKeyLeft)) {
        if (!(keys & RIGHT)) {
            keys = keys | RIGHT;
            signal_right(game);
            sfClock_restart(game->repKeyRight);
        } else if (!(keys & RIGHTHOLD)) {
            if (sfClock_getElapsedTime(game->repKeyRight).microseconds
             >= moveRepeatLatency1) {
                keys |= RIGHTHOLD;
                keys &= ~RIGHT;
            }
        } else {
            if (sfClock_getElapsedTime(game->repKeyRight).microseconds
             >= moveRepeatLatency2)
                keys &= ~RIGHT;
        }
    } else {
        keys &= ~RIGHT;
        keys &= ~RIGHTHOLD;
    }
}

static void pause_keys(struct game *game)
{
    /* PAUSE */
    if (sfKeyboard_isKeyPressed(sfKeyP)) {
        if (!(keys & PAUSE)) {
            keys |= PAUSE;
            signal_pause(game);
        }
    } else {
        keys &= ~PAUSE;
    }
}

static void menu_tick(struct game *game)
{
    struct field *fld = game->fld;

    sfClock_restart(game->mTick);
    field_fill_random(fld);
    painter_update_field(fld->id, fld);
}

void transition_init(struct game *game)
{
    struct idlist *texts = game->texts;

    LIST_FOREACH(texts, text) {
        show_menu_text(text->obj);
        hide_game_text(text->obj);
        hide_game_over_text(text->obj);
        update_menu_text(text->obj);
        update_game_text(text->obj);
        update_game_over_text(text->obj);
    }
}

static void transition_game_start(struct game *game)
{
    struct field *fld = game->fld;
    struct field *nxt = game->nxt;

    game->started = 1;
    game->paused = 0;
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
        hide_menu_text(text->obj);
        show_game_text(text->obj);
        hide_pause_text(text->obj);
        update_menu_text(text->obj);
        update_game_text(text->obj);
    }

    painter_update_field(fld->id, fld);
    painter_update_field(nxt->id, nxt);
    sfClock_restart(game->gameTick);
}

static void menu_loop(struct game *game)
{
    if (sfClock_getElapsedTime(game->mTick).microseconds >= basicLatency)
        menu_tick(game);

    if (sfKeyboard_isKeyPressed(sfKeyS)) {
        if (!(keys & GAMEOVER))
            transition_game_start(game);
    } else {
        keys = 0;
    }
}

static void game_loop(struct game *game)
{
    struct field *fld = game->fld;
    struct field *nxt = game->nxt;

    game_keys(game);
    if (sfClock_getElapsedTime(game->gameTick).microseconds >= game->moveLatency) {
        game_tick(game);
    }

    struct idlist *texts = game->texts;

    LIST_FOREACH(texts, text) {
        render_score_value(game, text->obj);
        render_level_value(game, text->obj);
        update_game_text(text->obj);
    }
    painter_update_field(fld->id, fld);
    painter_update_field(nxt->id, nxt);
}

static void game_over_wait_loop(struct game *game)
{
    if (sfClock_getElapsedTime(game->over_wait_tick).microseconds > GAMEOVERWAIT)
        transition_game_over(game);
}

static void game_over_loop(struct game *game)
{
    int anykey = 0;

    for (int keycode = sfKeyUnknown; keycode < sfKeyCount; keycode++)
        if (sfKeyboard_isKeyPressed(keycode))
            anykey = 1;

    if (anykey) {
        if (!(keys & GAMEOVER)) {
            keys |= GAMEOVER;
            transition_menu(game);
        }
    } else {
        keys &= ~GAMEOVER;
    }
}

static void pause_loop(struct game *game)
{
    pause_keys(game);
}

void main_loop(struct game *game)
{
    if (game->started) {
        if (game->paused)
            pause_loop(game);
        else if (game->over_wait)
            game_over_wait_loop(game);
        else if (game->over)
            game_over_loop(game);
        else
            game_loop(game);
    } else {
        menu_loop(game);
    }
    painter_draw();
}
