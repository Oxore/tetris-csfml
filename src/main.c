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

sfRectangleShape* fld[25][10];  // Array of fld rectangles
sfVector2f fldCPos[25][10];	// Array of absolute coordinates of fld
 				// rectangles
int fldCOutThick = 1; 	// Field rectangles outline thickness
sfVector2f fldCSize;	// Field rectangles size variable x/y
sfVector2i fldSize;
sfVector2i fldPos;

uint8_t arrKeys = 0b00000000; // Arrow keys states byte container

int lvlLatency = 500000;
/* --- Variables End --- */

int main()
{

	initAll();

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

		if (gameIsStarted == 1) {
			tTick();
			tKeyCtrl();
			scoreDisplay(scoreCurrent, textScore);
			colorizeFld();
			colorizeActiSh();

			/*
			 * Draw all fld cells
			 *
			 */
			for (int j = 0; j < fldSize.y; j++){
				for(int i = 0; i < fldSize.x; i++){
					sfRenderWindow_drawRectangleShape(
						window,
						fld[j][i],
						NULL);
				}
			}
			sfRenderWindow_drawText(window, textScore, NULL);
		} else {
			menuTick();
			/* Draw all fld cells */
			for (int j=0; j < fldSize.y; j++){
				for(int i = 0; i < fldSize.x; i++){
					sfRenderWindow_drawRectangleShape(
						window,
						fld[j][i],
						NULL);
				}
			}
			sfRenderWindow_drawText(window, textMenu1, NULL);

			if (sfKeyboard_isKeyPressed(sfKeyS) == 1) {
				gameIsStarted = 1;
				initAll();
			}
		}
		/* Update the window */
		sfRenderWindow_display(window);
	}

	/* Just senseless printf */
	printf("%d\n", scoreCurrent);

	/* Cleanup resources */
	for (int j=0;j<fldSize.y;j++){
		for(int i=0;i<fldSize.x;i++){
			sfRectangleShape_destroy(fld[j][i]);
		}
	}

	sfRenderWindow_destroy(window);

	return EXIT_SUCCESS;
}
