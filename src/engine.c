#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SFML/System/Clock.h>
#include <SFML/Window/Keyboard.h>
#include <SFML/Graphics/RenderWindow.h>
#include <SFML/Graphics/Text.h>

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
#define LEFTHOLD  (1 << 7)

int level_move_latency[] = {
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

int rmlines_score[] = {
    0,
    RM_1LINES_SCORE,
    RM_2LINES_SCORE,
    RM_3LINES_SCORE,
    RM_4LINES_SCORE
};

/* Externs from main.c */
extern struct game    game;
extern struct field   fld, nxt;
extern struct idlist *texts;
extern char           arrKeys;

static void render_score_value(void *obj)
{
    struct text *text = obj;
    if (!strcmp(text->type, "score.value")) {
        if (!text->text)
            text->text = malloc(sizeof(char) * BUFSIZ);
        sprintf(text->text, "%d", game.scoreCurrent);
    }
}

static void render_level_value(void *obj)
{
    struct text *text = obj;
    if (!strcmp(text->type, "level.value")) {
        if (!text->text)
            text->text = malloc(sizeof(char) * BUFSIZ);
        sprintf(text->text, "%d", game.level);
    }
}

static int getMoveLatencyOfLevel(unsigned int level)
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
        game->moveLatency = getMoveLatencyOfLevel(game->level);
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

static void transition_game_over()
{
    game.isStarted = 0;
    game.scoreCurrent = 0;
    game.level = 0;
    game.moveLatency = L00LATENCY;
    game.lines = 0;

    nxt.attr |= FLD_ATTR_INVISIBLE;
    painter_update_field(nxt.id, &nxt);

    fld.shape[0].y = fld.size.y;
    fld.shape[1].y = fld.size.y;
    field_fill_random(&fld);
    painter_update_field(fld.id, &fld);

    list_foreach(texts, show_menu_text);
    list_foreach(texts, hide_game_text);
    list_foreach(texts, update_menu_text);
    list_foreach(texts, update_game_text);
}

static void project_ghost_shape(struct field *fld, unsigned int idreal, unsigned int idghost)
{
    fld->shape[idghost].t = fld->shape[idreal].t;
    fld->shape[idghost].x = fld->shape[idreal].x;
    fld->shape[idghost].y = fld->shape[idreal].y;
    for (unsigned int j = 0; j < 4; j++)
        for (unsigned int i = 0; i < 4; i++)
            fld->shape[idghost].c[j][i] = fld->shape[idreal].c[j][i];
    do {
        --fld->shape[idghost].y;
    } while (!field_shape_collision(fld, &fld->shape[idghost]));
    ++fld->shape[idghost].y;
}

static void transition_put_shape()
{
    field_put_shape(&fld, &fld.shape[1]);
    int removedLines = field_rm_lines(&fld);
    game.lines += removedLines;
    game.scoreCurrent += rmlines_score[removedLines] * game.level;
    fld.shape[1].t = nxt.shape[0].t;
    field_reset_walking_shape(&fld, 1);
    project_ghost_shape(&fld, 1, 0);
    for (unsigned int s = 0; s < nxt.shape_cnt - 1; ++s) {
        nxt.shape[s] = nxt.shape[s + 1];
        nxt.shape[s].y = 4 - s * 3;
    }
    shape_gen_random(&nxt.shape[nxt.shape_cnt - 1]);
    level_up(&game);
}

static void game_tick()
{
    sfClock_restart(game.gameTick);
    if (field_move_shape_down(&fld, 1)) {
        project_ghost_shape(&fld, 1, 0);
        sfClock_restart(game.putTick);
    }
    if (sfClock_getElapsedTime(game.putTick).microseconds >= PUT_LATENCY) {
        if (field_shape_out_of_bounds(&fld, &fld.shape[1]))
            transition_game_over();
        else
            transition_put_shape();
        sfClock_restart(game.putTick);
    }
}

static void signal_up()
{
    field_rotate_shape(&fld, 1);
    project_ghost_shape(&fld, 1, 0);
}

static void signal_down()
{
    if (field_move_shape_down(&fld, 1)) {
        project_ghost_shape(&fld, 1, 0);
        sfClock_restart(game.gameTick);
        game.scoreCurrent++;
    }
    sfClock_restart(game.repPushDown);
}

static void signal_left()
{
    if (field_move_shape_left(&fld, 1)) {
        project_ghost_shape(&fld, 1, 0);
        sfClock_restart(game.putTick);
    }
    sfClock_restart(game.repKeyLeft);
}

static void signal_right()
{
    if (field_move_shape_right(&fld, 1)) {
        project_ghost_shape(&fld, 1, 0);
        sfClock_restart(game.putTick);
    }
    sfClock_restart(game.repKeyRight);
}

static void game_keys()
{
    /* UP */
    if (sfKeyboard_isKeyPressed(sfKeyUp)) {
        if (!(arrKeys & UP)) {
            arrKeys = arrKeys | UP;
            signal_up();
        }
    } else {
        arrKeys = arrKeys & ~UP;
    }

    /* DOWN */
    if (sfKeyboard_isKeyPressed(sfKeyDown)) {
        if (!(arrKeys & DOWN)) {
            arrKeys = arrKeys | DOWN;
            signal_down();
        } else {
            if (sfClock_getElapsedTime(game.repPushDown).microseconds >= moveRepeatLatency2)
                arrKeys = arrKeys & ~DOWN;
        }
    } else {
        arrKeys = arrKeys & ~DOWN;
    }

    /* LEFT */
    if (sfKeyboard_isKeyPressed(sfKeyLeft) && !sfKeyboard_isKeyPressed(sfKeyRight)) {
        if (!(arrKeys & LEFT)) {
            arrKeys = arrKeys | LEFT;
            signal_left();
        } else if (!(arrKeys & LEFTHOLD)) {
            if (sfClock_getElapsedTime(game.repKeyLeft).microseconds >= moveRepeatLatency1) {
                arrKeys = arrKeys | LEFTHOLD;
                arrKeys = arrKeys & ~LEFT;
            }
        } else {
            if (sfClock_getElapsedTime(game.repKeyLeft).microseconds >= moveRepeatLatency2)
                arrKeys = arrKeys & ~LEFT;
        }
    } else {
        arrKeys = arrKeys & ~LEFT;
        arrKeys = arrKeys & ~LEFTHOLD;
    }

    /* RIGHT */
    if (sfKeyboard_isKeyPressed(sfKeyRight) && !sfKeyboard_isKeyPressed(sfKeyLeft)) {
        if (!(arrKeys & RIGHT)) {
            arrKeys = arrKeys | RIGHT;
            signal_right();
        } else if (!(arrKeys & RIGHTHOLD)) {
            if (sfClock_getElapsedTime(game.repKeyRight).microseconds >= moveRepeatLatency1) {
                arrKeys = arrKeys | RIGHTHOLD;
                arrKeys = arrKeys & ~RIGHT;
            }
        } else {
            if (sfClock_getElapsedTime(game.repKeyRight).microseconds >= moveRepeatLatency2)
                arrKeys = arrKeys & ~RIGHT;
        }
    } else {
        arrKeys = arrKeys & ~RIGHT;
        arrKeys = arrKeys & ~RIGHTHOLD;
    }
}

static void menuTick()
{
    sfClock_restart(game.mTick);
    field_fill_random(&fld);
    painter_update_field(fld.id, &fld);
}

void transition_init(void)
{
    list_foreach(texts, show_menu_text);
    list_foreach(texts, hide_game_text);
    list_foreach(texts, update_menu_text);
    list_foreach(texts, update_game_text);
}

static void transition_game_start()
{
    game.isStarted = 1;
    field_clear(&fld);
    shape_gen_random(&fld.shape[1]);
    field_reset_walking_shape(&fld, 1);
    project_ghost_shape(&fld, 1, 0);
    shape_load(&fld.shape[1]);
    for (unsigned int i = 0; i < nxt.shape_cnt; ++i)
        shape_gen_random(&nxt.shape[i]);
    nxt.attr &= ~FLD_ATTR_INVISIBLE;
    list_foreach(texts, hide_menu_text);
    list_foreach(texts, show_game_text);
    list_foreach(texts, update_menu_text);
    list_foreach(texts, update_game_text);
    sfClock_restart(game.gameTick);
}

static void menu_loop() {
    if (sfClock_getElapsedTime(game.mTick).microseconds >= basicLatency)
        menuTick();
    if (sfKeyboard_isKeyPressed(sfKeyS) == 1)
        transition_game_start();
    painter_draw();
}

static void game_loop() {
    game_keys();
    if (sfClock_getElapsedTime(game.gameTick).microseconds >= game.moveLatency)
        game_tick();
    list_foreach(texts, render_score_value);
    list_foreach(texts, render_level_value);
    painter_update_field(fld.id, &fld);
    painter_update_field(nxt.id, &nxt);
    list_foreach(texts, update_game_text);
    painter_draw();
}

void main_loop()
{
    if (game.isStarted)
        game_loop();
    else
        menu_loop();
}

