#include "include/common.h"

/* --- Variables --- */
int gameIsStarted = 0;
sfVideoMode mode = {450, 520, 32};
sfRenderWindow* window;
sfEvent event;
sfText* textScore;
sfFont* font;
char *scoreDisp;

sfRectangleShape* fld[20][10];// Array of fld rectangles
sfVector2f fld_rPos[20][10];	// Array of absolute coordinates of fld 
				// rectangles

int fld_rOutThick = 1; 	// Field rectangles outline thickness	
sfVector2f fld_rSize;		// Field rectangles size variable x/y
sfVector2i actiShPos;   // Offset active shape relative to fld
sfVector2i fldSize, fldPos;
sfVector2f textScore_pos;

short arrKeys = 0b00000000; // Arrow keys states byte container

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

			/* Colorize active cells of fld */
			for(int j=0;j<20;j++){
				for(int i=0;i<10;i++){
					sfRectangleShape_setFillColor(
						fld[j][i], 
						fld_rAttr[j][i].fColor);
					sfRectangleShape_setOutlineColor(
						fld[j][i], 
						fld_rAttr[j][i].oColor);
				}
			}
			
			
			/* 
			 * Colorize active cells of active shape (overlay only 
			 * active cells above background of fld) 
			 *
			 */
			for(int j=0;j<4;j++){
				for(int i=0;i<4;i++){
					if (activeShape[j][i].a != 0){
						sfRectangleShape_setFillColor(
							fld[j
							+actiShPos.y]
							[i+actiShPos.x], 
							activeShape[j][i].fColor
							);
						sfRectangleShape_setOutlineColor(
							fld[j
							+actiShPos.y]
							[i+actiShPos.x], 
							activeShape[j][i].oColor
							);
					}
				}
			}
			
			/* 
			 * Draw all fld cells 
			 *
			 */
			for (int j=0;j<fldSize.y;j++){
				for(int i=0;i<fldSize.x;i++){
					sfRenderWindow_drawRectangleShape(
						window, 
						fld[j][i], 
						NULL);
				}
			}
			sfRenderWindow_drawText(window, textScore, NULL);
		} else {
			menuTick();
			if (sfKeyboard_isKeyPressed(sfKeyReturn) == 1) {
				gameIsStarted = 1;
				initAll();
			}
			menuTick();

			/* Draw all fld cells */
			for (int j=0;j<fldSize.y;j++){
				for(int i=0;i<fldSize.x;i++){
					sfRenderWindow_drawRectangleShape(
						window, 
						fld[j][i], 
						NULL);
				}
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
