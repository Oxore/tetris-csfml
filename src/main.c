#include "include/common.h"

/* --- Variables --- */
int gameIsStarted = 0;
sfVideoMode mode = {450, 520, 32};
sfRenderWindow* window;
sfEvent event;
sfText* textScore;
sfFont* font;
char *scoreDisp;

sfRectangleShape* field[20][10];	// Array of field rectangles
sfVector2f field_rPos[20][10];		// Array of absolute coordinates of field rectangles

int field_rOutThick = 1; 	// Field rectangles outline thickness	
sfVector2f field_rSize;		// Field rectangles size variable x/y
sfVector2i offsetActiveShape;   // Offset active shape relative to field
sfVector2i fieldSize, fieldPos;
sfVector2f textScore_pos;

short arrKeys = 0b00000000; // Arrow keys states byte container

int lvlLatency = 500000;
int scoreCurrent = 0;
/* --- Variables End --- */

int main()
{
	
	initAll();
	
	/* Create main window */
	window = sfRenderWindow_create(mode, windowName_conf, sfResize | sfClose, NULL);
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
			scoreDisplay();

			/* Colorize active cells of field */
			for(int j=0;j<20;j++){
				for(int i=0;i<10;i++){
					sfRectangleShape_setFillColor(field[j][i], field_rAttr[j][i].fColor);
					sfRectangleShape_setOutlineColor(field[j][i], field_rAttr[j][i].oColor);
				}
			}
			
			
			/* Colorize active cells of active shape (overlay only active cells above background of field) */
			for(int j=0;j<4;j++){
				for(int i=0;i<4;i++){
					if (activeShape[j][i].a != 0){
						sfRectangleShape_setFillColor(field[j+offsetActiveShape.y][i+offsetActiveShape.x], activeShape[j][i].fColor);
						sfRectangleShape_setOutlineColor(field[j+offsetActiveShape.y][i+offsetActiveShape.x], activeShape[j][i].oColor);
					}
				}
			}
			
			/* Draw all field cells */
			for (int j=0;j<fieldSize.y;j++){
				for(int i=0;i<fieldSize.x;i++){
					sfRenderWindow_drawRectangleShape(window, field[j][i], NULL);
				}
			}
			sfRenderWindow_drawText(window, textScore, NULL);
		} 
		else {
			menuTick();
			if (sfKeyboard_isKeyPressed(sfKeyReturn) == 1) {
				gameIsStarted = 1;
				initAll();
			}
			menuTick();

			/* Draw all field cells */
			for (int j=0;j<fieldSize.y;j++){
				for(int i=0;i<fieldSize.x;i++){
					sfRenderWindow_drawRectangleShape(window, field[j][i], NULL);
				}
			}
		}
		/* Update the window */
		sfRenderWindow_display(window);
	}

	/* Just senseless printf */
	printf("%d\n", scoreCurrent);

	/* Cleanup resources */
	for (int j=0;j<fieldSize.y;j++){
		for(int i=0;i<fieldSize.x;i++){
			sfRectangleShape_destroy(field[j][i]);
		}
	}

	sfRenderWindow_destroy(window);

	return EXIT_SUCCESS;
}
