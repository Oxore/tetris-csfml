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

/*
 * Removes line when cells all are in row in it
 *
 */
int rmLines()
{
    int k = 0; // "Filled line" indicator
    int s = 0;
    for (int j = 0; j < 22; j++) {
        for (int i = 0; i < 10; i++)
            if (fld.c[j][i].a != 0)
                k++;
        if (k >= 10) {    // If line is full
            s++;    // Give scores for line
            for (int n = j; n < 22; n++) {    // Drop all lines down
                if (n == 21) {
                    for (int m = 0; m < 10; m++) {
                        fld.c[n][m].a = 0;
                        fld.c[n][m].fColor = UIBGCOLOR;
                    }
                    break;
                }
                for (int m = 0; m < 10; m++) {
                    fld.c[n][m].a = fld.c[n+1][m].a;
                    fld.c[n][m].fColor = fld.c[n+1][m].fColor;
                }
            }
            j--;     // Do not let loop to go to next line because
                // next line go down by itself =)
        }
        k = 0;    // Clear line fill indicator
    }
    return s; // Return number of deleted lines
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
    if (sfClock_getElapsedTime(gameTick).microseconds >= game.moveLatency) {
        sfClock_restart(gameTick);
        active.y--; // try
        if (collide(&fld, &active))
            active.y++; // fallback
        else
            sfClock_restart(putTick);
        if (sfClock_getElapsedTime(putTick).microseconds >= PUT_LATENCY) {
            if (out_of_field(&fld, &active)) {
                gameover(&game);
                return;
            } else {
                putShape(&fld, &active);
                int removedLines = rmLines();
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
                genNextShape();
                checkLevelUp(&game);
            }
            sfClock_restart(putTick);
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
                sfClock_restart(putTick);
                sfClock_restart(gameTick);
                game.scoreCurrent++;
            }
            sfClock_restart(repPushDown);
        } else {
            if (sfClock_getElapsedTime(repPushDown).microseconds
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
                sfClock_restart(putTick);
            sfClock_restart(repKeyLeft);
        } else {
            if (!(arrKeys & LEFTHOLD)) {
                if (sfClock_getElapsedTime(repKeyLeft).microseconds
                    >=  moveRepeatLatency1) {
                    arrKeys = arrKeys | LEFTHOLD;
                    arrKeys = arrKeys & ~LEFT;
                }
            } else {
                if (sfClock_getElapsedTime(repKeyLeft).microseconds
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
                sfClock_restart(putTick);
            sfClock_restart(repKeyRight);
        } else {
            if (!(arrKeys & RIGHTHOLD)) {
                if (sfClock_getElapsedTime(repKeyRight).microseconds
                >=  moveRepeatLatency1) {
                    arrKeys = arrKeys | RIGHTHOLD;
                    arrKeys = arrKeys & ~RIGHT;
                }
            } else if (!sfKeyboard_isKeyPressed(sfKeyLeft)) {
                if (sfClock_getElapsedTime(repKeyRight).microseconds
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
 * Colorize active cells of active shape (overlay only
 * active cells above background of fld)
 *
 */
void colorizeActive()
{
    for (int j = 0; j < 4; j++)
        for (int i = 0; i < 4; i++)
            if (active.c[j][i] && j+active.y < 22) {
                sfRectangleShape_setFillColor(fld.p[j + active.y][i + active.x],
                        active.fColor);
                sfRectangleShape_setOutlineColor(
                        fld.p[j + active.y][i + active.x], UIFGACTIVECOLOR);
            }
}


/*
 * Draw all fld cells
 *
 */
void drawFld(sfRenderWindow *window)
{
    for (int j = 0; j < fld.size.y; j++)
        for (int i = 0; i < fld.size.x; i++)
            sfRenderWindow_drawRectangleShape(window, fld.p[j][i], NULL);
}




void gameover(struct game *game)
{
    game->isStarted = 0;
    game->scoreCurrent = 0;
    game->level = 0;
    game->moveLatency = L00LATENCY;
    game->lines = 0;
}


void genNextShape()
{
    next.t = (rand() % 7) + 1; // Insert new random shape of 7 variants
    load_shape(&next);
    if (next.t == 5)
        for (int j = 0; j < 3; j++)
            for (int i = 0; i < 4; i++)
                next.c[i][j] = next.c[i][j+1];
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

void freeFld() {
    for (int j = 0; j < fld.size.y; j++)
        for (int i = 0; i < fld.size.x; i++)
            sfRectangleShape_destroy(fld.p[j][i]);
    for (int j = 0; j < 4; j++)
        for (int i = 0; i < 4; i++)
            sfRectangleShape_destroy(next.p[j][i]);
}
