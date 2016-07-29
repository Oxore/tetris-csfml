#include <SFML/Audio.h>
#include <SFML/Graphics.h>
#include <SFML/System.h>
#include <SFML/Window/Keyboard.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "functions.h"
#include "tet_conf.h"

//#define EXIT_FAILURE -1
//#define EXIT_SUCCESS 0

struct tCell {
	short a; // active/empty state of cell
	sfColor fColor, oColor; // fill and outline color of cell
};

struct tCell 	activeShape[4][4], 	// 4x4 block of active crawling shape
				field_rAttr[20][10];// field cells attributess

