#ifndef TCOMMON_H
#define TCOMMON_H

#include <SFML/Audio.h>
#include <SFML/Graphics.h>
#include <SFML/System.h>
#include <SFML/Window/Keyboard.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include "tet_conf.h"

typedef struct Cell {
    uint8_t a; // active/empty state of cell
    sfColor fColor; // fill color
} Cell;


/*
 * shape coords
 * y
 * ^. . . .
 * |. . . .
 * |. . . .
 * |. . . .
 * 0------->x
 *
 */

typedef struct Shape {
    int x; // x coord of shape's left side
    int y; // y coord of shape's bottom
    int t; // shape type
    sfColor fColor; // shape color
    uint8_t c[4][4]; // array of logic shape cells
    sfRectangleShape *p[4][4]; // array of physical shape cells
    sfVector2f cSize; // shape rectangles size variable x/y
} Shape; 

typedef struct Field {
    sfVector2i pos;
    sfColor fColor; // shape color
    Cell c[25][10]; // array of logic shape cells
    sfRectangleShape *p[25][10]; // array of physical shape cells
    int cOutThick; // Field rectangles outline thickness
    sfVector2f cSize; // shape rectangles size variable x/y
    sfVector2i size;
} Field; 

typedef struct Window {
    sfVideoMode mode;
    sfRenderWindow *window;
    sfEvent event;
} Window;

typedef struct Game {
    int isStarted;
    int scoreCurrent;
    int level;
} Game;

/* ======== text.[c|h] types =========== */

typedef struct List {
    void *obj;
    void *next;
    void *prev;
} List;

typedef struct Pair {
    void *k;
    void *v;
} Pair;

typedef struct KeyMap {
    Pair *pair;
    void *next;
    void *prev;
} KeyMap;

typedef struct Text {
    char *font;
    char *type;
    char *scene;
    char *text;
    void *sfText;
} Text;

#endif
