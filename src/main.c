#include "include/common.h"
#include "include/functions.h"

/* --- Variables --- */
int gameIsStarted = 0;
sfVideoMode mode = {450, 520, 32};
sfRenderWindow* window;
sfEvent event;

sfText *textMenu1;
sfVector2f textMenu1_pos;

sfFont *fontScore;
sfText *textScore;
sfVector2f textScore_pos;
char *scoreDisp;
int scoreCurrent = 0;

sfRectangleShape* ns[4][4];   // Array of next shape image rectangles
sfRectangleShape* fld[25][10];  // Array of fld rectangles
sfVector2f fldCPos[25][10];	// Array of absolute coordinates of fld
				// rectangles
sfRectangleShape* ns[4][4];   // Array of next shape image rectangles
sfVector2f nsCSize; // Next shape rectangles size variable x/y
int fldCOutThick = 1; 	// Field rectangles outline thickness
sfVector2f fldCSize;	// Field rectangles size variable x/y
sfVector2i fldSize;
sfVector2f fldPos;

uint8_t arrKeys = 0b00000000; // Arrow keys states byte container

int lvlLatency = 500000;
/* --- Variables End --- */

sfClock *gameTick;
sfClock *mTick;
sfClock *repPushDown;	// Clock for repeat latency when Down arrow long push
sfClock *repKeyLeft;	// Clock for repeat latency when Left arrow long push
sfClock *repKeyRight;	// Clock for repeat latency when Left arrow long push

int main()
{
	srand( time(NULL) );
	gameTick = sfClock_create();
	mTick = sfClock_create();
	fontScore = sfFont_createFromFile("dat/arial.ttf");
	if (!fontScore) {
		printf("dat/arial.ttf font load failed");
		exit(-1);
	}


	textScore_pos = (sfVector2f){.x = 250+10+10, .y = 10};
	textScore = sfText_create();
	sfText_setFont(textScore, fontScore);
	sfText_setCharacterSize(textScore, 20);
	sfText_setPosition(textScore, textScore_pos);

	/*
	 * Dimensions of every fld's cell
	 * 23px - fill color 1px - for outline, 25 - at all
	 */
	fldCSize = (sfVector2f){.x = 23, .y = 23}; //Fld's cell size in pixels
	fldPos = (sfVector2f){.x = 10, .y = 10+500-20}; // Fld bot left corner
	fldSize = (sfVector2i){.x = 10, .y = 25}; // Field's size in blocks

	nsCSize = (sfVector2f){.x = 23, .y = 23};
	nxtShape = (struct shapeSt){.x = 250+10+20, .y = 200};

	initFld();

	/*
	 * Menu texts
	 *
	 */
	textMenu1_pos.x = 10+250+30;
	textMenu1_pos.y = 100;
	textMenu1 = sfText_create();
	sfText_setFont(textMenu1, fontScore);
	sfText_setCharacterSize(textMenu1, 36);
	sfText_setPosition(textMenu1, textMenu1_pos);
	char b[7];
	sprintf(b, "TETRIS");
	sfText_setString(textMenu1, (char *)&b);

	/* Create main window */
	window = sfRenderWindow_create(mode,
					windowName_conf,
					sfResize | sfClose,
					NULL);
	if (!window)
		return EXIT_FAILURE;

	/* colorize field once at start */
	colorizeRandom();

	/* Start the game loop */
	while (sfRenderWindow_isOpen(window))
	{
		/* Process events */
		while (sfRenderWindow_pollEvent(window, &event))
		{
			/* Close window : exit */
			if (event.type == sfEvtClosed)
				sfRenderWindow_close(window);
		}
		/* Clear the screen */
		sfRenderWindow_clear(window, sfBlack);

		if (gameIsStarted) {
			tTick();
			tKeyCtrl();
			scoreDisplay(scoreCurrent, textScore);
			colorizeFld();
			colorizeActiSh();
			drawFld(window);
			drawNextShape(window);

			sfRenderWindow_drawText(window, textScore, NULL);
		} else {
			menuTick();
			/* Draw all fld cells */
			drawFld(window);
			sfRenderWindow_drawText(window, textMenu1, NULL);

			if (sfKeyboard_isKeyPressed(sfKeyS) == 1) {
				gameIsStarted = 1;
				cleanup();
				initFld();
			}
		}
		/* Update the window */
		sfRenderWindow_display(window);
	}

	/* Just senseless printf */
	printf("%d\n", scoreCurrent);

	cleanup();
	sfRenderWindow_destroy(window);
	sfText_destroy(textScore);
	sfText_destroy(textMenu1);

	return EXIT_SUCCESS;
}
