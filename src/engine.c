#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SFML/System/Clock.h>
#include <SFML/Graphics/RenderWindow.h>
#include <SFML/Graphics/Text.h>
#include <SFML/Graphics/RectangleShape.h>

#include "common.h"
#include "tet_conf.h"
#include "text.h"
#include "field.h"
#include "engine.h"

/* Externs from main.c */
extern struct game game;
extern struct shape active, next;
extern struct field fld;

extern sfFont *fontScore;

extern char arrKeys;    // Arrow keys states byte container

extern sfClock *gameTick;
extern sfClock *putTick;
extern sfClock *mTick;
extern sfClock *repPushDown; // Clock for repeat latency when Down key held
extern sfClock *repKeyLeft; // Clock for repeat latency when Left key held
extern sfClock *repKeyRight; // Clock for repeat latency when Left key held

void valueAfterTextDisplay(int value, List *texts, char *type)
{
    List *l = texts;
    while (l) {
        Text *text = (Text *)l->obj;
        if (text->type)
            if (!strcmp(text->type, type)) {
                char a[64];
                if (text->text) {
                    if (strlen(text->text) > 52) {
                        memset(text->text+52, '\0', 1);
                        strcpy(a, text->text);
                    }
                    sprintf(a, "%s%d", text->text, value);
                    sfText_setString(text->sfText, (char *)&a);
                    break;
                }
            }
        l = l->next;
    }
}

void checkLevelUp(struct game *game)
{
    while (game->lines >= LEVELUP_LINES) {
        game->level++;
        game->lines -= LEVELUP_LINES;
        game->moveLatency = getMoveLatencyOfLevel(game->level);
    }
}

int getMoveLatencyOfLevel(int level)
{
    if (level >= 29)
        return L29LATENCY;
    else if (level >= 19)
        return L19LATENCY;
    else if (level >= 16)
        return L16LATENCY;
    else if (level >= 13)
        return L13LATENCY;
    else if (level >= 10)
        return L10LATENCY;
    else if (level == 9)
        return L09LATENCY;
    else if (level == 8)
        return L08LATENCY;
    else if (level == 7)
        return L07LATENCY;
    else if (level == 6)
        return L06LATENCY;
    else if (level == 5)
        return L05LATENCY;
    else if (level == 4)
        return L04LATENCY;
    else if (level == 3)
        return L03LATENCY;
    else if (level == 2)
        return L02LATENCY;
    else if (level == 1)
        return L01LATENCY;
    else if (level == 0)
        return L00LATENCY;
    return L00LATENCY;
}


/*
 * Game tick
 *
 */
void tTick()
{     // If tick exceeds current level tick latency
    if (sfClock_getElapsedTime(game.gameTick).microseconds >= game.moveLatency) {
        sfClock_restart(game.gameTick);
        active.y--; // try
        if (collide(&fld, &active))
            active.y++; // fallback
        else
            sfClock_restart(game.putTick);
        if (sfClock_getElapsedTime(game.putTick).microseconds >= PUT_LATENCY) {
            if (out_of_field(&fld, &active)) {
                gameover(&game);
                return;
            } else {
                putShape(&fld, &active);
                int removedLines = rm_lines(&fld);
                game.lines += removedLines;
                switch (removedLines) {
                case 1:
                    game.scoreCurrent += RM_1LINES_SCORE * game.level;
                    break;
                case 2:
                    game.scoreCurrent += RM_2LINES_SCORE * game.level;
                    break;
                case 3:
                    game.scoreCurrent += RM_3LINES_SCORE * game.level;
                    break;
                case 4:
                    game.scoreCurrent += RM_4LINES_SCORE * game.level;
                    break;
                }
                active.t = next.t;
                resetActiveShape(&fld, &active);
                gen_shape(&next);
                checkLevelUp(&game);
            }
            sfClock_restart(game.putTick);
        }
    }
}

/*
 * Keys hold handler
 *
 */
void tKeyCtrl()
{
    /* Up arrow key 'hold' handler */
    if (sfKeyboard_isKeyPressed(sfKeyUp)) {
        if (!(arrKeys & UP)) {
            arrKeys = arrKeys | UP;
            rotate_shape(&fld, &active);
        }
    } else {
        if ((arrKeys & UP)) {
            arrKeys = arrKeys & ~UP;
        }
    }

    /* Down Arrow Key 'hold' handler */
    if (sfKeyboard_isKeyPressed(sfKeyDown)) {
        if (!(arrKeys & DOWN)) {
            arrKeys = arrKeys | DOWN;
            active.y--;
            if (collide(&fld, &active))
                active.y++;
            else {
                // Avoid excess move down by gameTick
                sfClock_restart(game.putTick);
                sfClock_restart(game.gameTick);
                game.scoreCurrent++;
            }
            sfClock_restart(game.repPushDown);
        } else {
            if (sfClock_getElapsedTime(game.repPushDown).microseconds
                >= moveRepeatLatency2)
                arrKeys = arrKeys & ~DOWN;
        }
    } else {
        if ((arrKeys & DOWN)) {
            arrKeys = arrKeys & ~DOWN;
        }
    }

    /* Left Arrow Key 'hold' handler */
    if (sfKeyboard_isKeyPressed(sfKeyLeft)
    && !sfKeyboard_isKeyPressed(sfKeyRight)) {
        if (!(arrKeys & LEFT)) {
            arrKeys = arrKeys | LEFT;
            active.x--;
            if (collide(&fld, &active))
                active.x++;
            else
                sfClock_restart(game.putTick);
            sfClock_restart(game.repKeyLeft);
        } else {
            if (!(arrKeys & LEFTHOLD)) {
                if (sfClock_getElapsedTime(game.repKeyLeft).microseconds
                    >=  moveRepeatLatency1) {
                    arrKeys = arrKeys | LEFTHOLD;
                    arrKeys = arrKeys & ~LEFT;
                }
            } else {
                if (sfClock_getElapsedTime(game.repKeyLeft).microseconds
                    >=  moveRepeatLatency2)
                    arrKeys = arrKeys & ~LEFT;
            }
        }
    } else if (!sfKeyboard_isKeyPressed(sfKeyLeft)) {
        if ((arrKeys & LEFT)) {
            arrKeys = arrKeys & ~LEFT;
            arrKeys = arrKeys & ~LEFTHOLD;
        }
    }

    /* Right Arrow Key 'hold' handler */
    if (sfKeyboard_isKeyPressed(sfKeyRight)
    && !sfKeyboard_isKeyPressed(sfKeyLeft)) {
        if (!(arrKeys & RIGHT)) {
            arrKeys = arrKeys | RIGHT;
            active.x++;
            if (collide(&fld, &active))
                active.x--;
            else
                sfClock_restart(game.putTick);
            sfClock_restart(game.repKeyRight);
        } else {
            if (!(arrKeys & RIGHTHOLD)) {
                if (sfClock_getElapsedTime(game.repKeyRight).microseconds
                >=  moveRepeatLatency1) {
                    arrKeys = arrKeys | RIGHTHOLD;
                    arrKeys = arrKeys & ~RIGHT;
                }
            } else if (!sfKeyboard_isKeyPressed(sfKeyLeft)) {
                if (sfClock_getElapsedTime(game.repKeyRight).microseconds
                >=  moveRepeatLatency2) // Wait short time
                    arrKeys = arrKeys & ~RIGHT;
            }
        }
    } else {
        if ((arrKeys & RIGHT)) {
            arrKeys = arrKeys & ~RIGHT;
            arrKeys = arrKeys & ~RIGHTHOLD;
        }
    }
}

/*
 * Draw all fld cells
 *
 */
void drawFld(sfRenderWindow *window, struct field *fld)
{
    for (int j = 0; j < fld->size.y; j++)
        for (int i = 0; i < fld->size.x; i++)
            sfRenderWindow_drawRectangleShape(window, fld->p[j][i], NULL);
}

void gameover(struct game *game)
{
    game->isStarted = 0;
    game->scoreCurrent = 0;
    game->level = 0;
    game->moveLatency = L00LATENCY;
    game->lines = 0;
}

void drawNextShape(sfRenderWindow *window)
{
    for (int j = 0; j < 4; j++)
        for (int i = 0; i < 4; i++)
            if (next.c[j][i]) {
                sfRectangleShape_setFillColor(next.p[j][i], next.fColor);
                sfRectangleShape_setOutlineColor(next.p[j][i], UIFGACTIVECOLOR);
                sfRenderWindow_drawRectangleShape(window, next.p[j][i], NULL);
            }
}

