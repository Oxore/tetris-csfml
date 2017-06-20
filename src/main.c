#include "include/common.h"
#include "include/functions.h"

/* --- Variables --- */
int gameIsStarted = 0;
sfVideoMode mode = {450, 520, 32};
sfRenderWindow* window;
sfEvent event;
sfText* textScore;
sfFont* font; 
char *scoreDisp;

sfRectangleShape* fld[20][10];// Array of fld rectangles
sfVector2f fldCPos[20][10];	// Array of absolute coordinates of fld 
				// rectangles

int fldCOutThick = 1; 	// Field rectangles outline thickness	
sfVector2f fldCSize;		// Field rectangles size variable x/y
sfVector2i fldSize, fldPos;
sfVector2f textScore_pos;

uint8_t arrKeys = 0b00000000; // Arrow keys states byte container

int lvlLatency = 500000;
int scoreCurrent = 0;
/* --- Variables End --- */

int main()
{
	
	initAll();
	
	/* Create main window */
	window = sfRenderWindow_create(mode, 
					windowName_conf, 
					sfResize | sfClose, 
					NULL
					);
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
