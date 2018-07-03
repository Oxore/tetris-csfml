#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SFML/System/Clock.h>
#include <SFML/Window/Keyboard.h>
#include <SFML/Graphics/RenderWindow.h>
#include <SFML/Graphics/Text.h>

#include "common.h"
#include "tet_conf.h"
#include "text.h"
#include "field.h"
#include "draw.h"
#include "engine.h"

/* Externs from main.c */
extern struct game game;
extern struct field fld, nxt;

extern char arrKeys;    // Arrow keys states byte container

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

int getMoveLatencyOfLevel(unsigned int level)
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
        fld.shape[0].y--; // try
        if (field_shape_collision(&fld, &fld.shape[0]))
            fld.shape[0].y++; // fallback
        else
            sfClock_restart(game.putTick);
        if (sfClock_getElapsedTime(game.putTick).microseconds >= PUT_LATENCY) {
            if (field_shape_out_of_bounds(&fld, &fld.shape[0])) {
                gameover(&game);
                fld.shape[0].y = fld.size.y;
                field_fill_random(&fld);
                nxt.attr |= FLD_ATTR_INVISIBLE;
                painter_update_field(nxt.id, &nxt);
                return;
            } else {
                field_put_shape(&fld, &fld.shape[0]);
                int removedLines = field_rm_lines(&fld);
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
                fld.shape[0].t = nxt.shape[0].t;
                field_reset_walking_shape(&fld, 0);
                for (unsigned int s = 0; s < nxt.shape_cnt - 1; ++s) {
                    nxt.shape[s] = nxt.shape[s + 1];
                    nxt.shape[s].y = 4 - s * 3; 
                }
                shape_gen_random(&nxt.shape[nxt.shape_cnt - 1]);
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
            field_rotate_shape(&fld, 0);
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
            fld.shape[0].y--;
            if (field_shape_collision(&fld, &fld.shape[0]))
                fld.shape[0].y++;
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
            fld.shape[0].x--;
            if (field_shape_collision(&fld, &fld.shape[0]))
                fld.shape[0].x++;
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
            fld.shape[0].x++;
            if (field_shape_collision(&fld, &fld.shape[0]))
                fld.shape[0].x--;
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

void gameover(struct game *game)
{
    game->isStarted = 0;
    game->scoreCurrent = 0;
    game->level = 0;
    game->moveLatency = L00LATENCY;
    game->lines = 0;
}
