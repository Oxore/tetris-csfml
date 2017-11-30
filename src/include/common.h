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

typedef struct tCell {
    uint8_t a; // active/empty state of cell
    sfColor fColor; // fill color
} tCell;
tCell fldCAtt[25][10];// fld cells attributes


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

typedef struct shapeSt {
    int x; // x coord of shape's left side
    int y; // y coord of shape's bottom
    int t; // shape type
    sfColor fColor; // shape color
    uint8_t c[4][4]; // array of shape cells
} shapeSt; 
shapeSt actiSh, nxtShape;

#endif
