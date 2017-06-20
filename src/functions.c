#include "include/functions.h"
#include "include/common.h"

/* Externs from main.c */

extern sfRectangleShape *fld[20][10];	// Array of field rectangles
extern sfVector2f fldCPos[20][10]; 	// Array of absolute coordinates of field rectangles
extern sfVector2i fldSize; 
extern sfVector2i fldPos;
extern sfVector2f fldCSize;	// Field rectangles size variable x/y
extern int fldCOutThick; 	// Field rectangles outline thickness	

extern sfVector2f textScore_pos;
extern sfText *textScore;
extern sfFont *font;
extern int gameIsStarted;

extern uint8_t arrKeys;		// arrow keys states byte container
/* arrKeys = ...n|7|6|5|4|3|2|1|0| (just a little bit of such called "bit fucking")
 * 0 - Right arrow pushed and held
 * 1 - Down arrow pushed and held
 * 2 - N/A
 * 3 - Left arrow pushed and held
 * 4 - Right arrow short repeat activated (after once long repeat)
 * 5 - N/A
 * 6 - N/A
 * 7 - Right arrow short repeat activated (after once long repeat)
 */

sfClock *gameTick;
sfClock *mTick;
sfClock *repPushDown;	// Clock for repeat latency when Down arrow long push
sfClock *repKeyLeft;	// Clock for repeat latency when Left arrow long push
sfClock *repKeyRight;	// Clock for repeat latency when Left arrow long push

extern int lvlLatency;
extern int scoreCurrent;
extern char *scoreDisp;
/* Shapes maps */
extern uint8_t arrShapeL_a1[4][4];
extern uint8_t arrShapeL_a2[4][4];
extern uint8_t arrShapeL_a3[4][4];
extern uint8_t arrShapeL_a4[4][4];

extern uint8_t arrShapeRL_a1[4][4];
extern uint8_t arrShapeRL_a2[4][4];
extern uint8_t arrShapeRL_a3[4][4];
extern uint8_t arrShapeRL_a4[4][4];

extern uint8_t arrShapeZ_a1[4][4];
extern uint8_t arrShapeZ_a2[4][4];
extern uint8_t arrShapeZ_a3[4][4];
extern uint8_t arrShapeZ_a4[4][4];

extern uint8_t arrShapeS_a1[4][4];
extern uint8_t arrShapeS_a2[4][4];
extern uint8_t arrShapeS_a3[4][4];
extern uint8_t arrShapeS_a4[4][4];

extern uint8_t arrShapeB_a1[4][4];
extern uint8_t arrShapeB_a2[4][4];
extern uint8_t arrShapeB_a3[4][4];
extern uint8_t arrShapeB_a4[4][4];

extern uint8_t arrShapeI_a1[4][4];
extern uint8_t arrShapeI_a2[4][4];
extern uint8_t arrShapeI_a3[4][4];
extern uint8_t arrShapeI_a4[4][4];

extern uint8_t arrShapeT_a1[4][4];
extern uint8_t arrShapeT_a2[4][4];
extern uint8_t arrShapeT_a3[4][4];
extern uint8_t arrShapeT_a4[4][4];


/*
 * Init routine. Performs once per program run
 *
 */
void initAll() 
{
	
	/* 
	 * Dimensions of every fld's block
	 * 19px - fill color 1px - for outline, 20 - at all
	 *
	 */
	fldCSize.x = 23;
	fldCSize.y = 23;
	
	fldPos.x = 10; // Field's bottom left corner coordinates 
	fldPos.y = 10+500-25;
	fldSize.x = 10; // Field size in blocks
	fldSize.y = 20;	
	
	srand( time(NULL) );
	gameTick = sfClock_create();
	mTick = sfClock_create();
	resetActiveShape();
		
	/* Create field */
	for (int j = 0; j < fldSize.y; j++) {
		for(int i = 0; i < fldSize.x; i++) {
			fldCAtt[j][i].a = 0;	// Inactive = empty
			fldCPos[j][i].x = fldPos.x
				+ (i * (fldCSize.x + 2 * fldCOutThick));
			fldCPos[j][i].y = fldPos.y - 
				(j * (fldCSize.y + 2 * fldCOutThick));
			fld[j][i] = sfRectangleShape_create();
			sfRectangleShape_setFillColor(fld[j][i], uiColor1);
			sfRectangleShape_setSize(fld[j][i], fldCSize);
			sfRectangleShape_setPosition(fld[j][i], fldCPos[j][i]);
			sfRectangleShape_setOutlineColor(fld[j][i], 
				uiColor3);
			sfRectangleShape_setOutlineThickness(fld[j][i], 
				fldCOutThick);
		}
	}
	font = sfFont_createFromFile("dat/arial.ttf");
	if (!font) {
		printf("dat/arial.ttf font load failed");
		exit(-1);
	}
	textScore_pos.x = 250+10+10;
	textScore_pos.y = 485;
	textScore = sfText_create();
	sfText_setFont(textScore, font);
	sfText_setCharacterSize(textScore, 20);
	sfText_setPosition(textScore, textScore_pos);
}

/*
 * Refreshes score
 *
 */
void scoreDisplay(int score, sfText *textScore) 
{
	char a[64];
	char b[8];
	sprintf(b, "Score: ");
	sprintf(a, "%d", score);
	for (int i = 63; i >= 7; i--) 
		a[i] = a[i-7];
	for (int i = 0; i < 7; i++) 
		a[i] = b[i];
	sfText_setString(textScore, (char *)&a);
}

/*
 * Removes line when cells all are in row in it
 *
 */
int linesRmScore() 
{
	int k = 0; // "Filled line" indicator
	int s = 0;
	for (int j = 0; j < 20; j++) {
		for (int i = 0; i < 10; i++) {
			if (fldCAtt[j][i].a != 0) 
				k++;
		}
		if (k >= 10) {	// If line is full
			s++;	// Give scores for line
			for (int n = j; n < 20; n++) {	// Drop all lines down
				if (n == 19) {
					for (int m = 0; m < 10; m++) {
						fldCAtt[n][m].a = 0;
						fldCAtt[n][m].fColor = uiColor1;
					}
					break;
				}
				for (int m = 0; m < 10; m++) {
					fldCAtt[n][m].a = fldCAtt[n+1][m].a;
					fldCAtt[n][m].fColor 
						= fldCAtt[n+1][m].fColor;
				}
			}
			j--; 	// Do not let loop to go to next line because 
				// next line go down by itself =)
		}
		k = 0;	// Clear line fill indicator 
	}
	return s; // Return number of deleted lines
}

/*
 * Inserts shape into field, runs filled lines searching, puts new shape
 * into the game at the top.
 *
 */
void putShape() 
{
	for (int j = 0; j < 4; j++) {
		for (int i = 0; i < 4; i++) {
			if (actiSh.c[j][i]) {
				fldCAtt[j+actiSh.y][i+actiSh.x].a 
					= actiSh.c[j][i];
				if ((j+actiSh.y >= 0) 
				&& (i+actiSh.x >= 0)) {
					fldCAtt[j+actiSh.y][i+actiSh.x].fColor 
						= actiSh.fColor;
				}
			}
		}
	}
	scoreCurrent += linesRmScore()*100; 	// Remove filled lines and 
						// get score;
	resetActiveShape();
}

void resetActiveShape() 
{
	actiSh.x = 3;
	actiSh.y = 16;
	actiSh.r = 1;
	actiSh.t = (rand()%7)+1;	// Insert new random shape of 7 variants
	switch (actiSh.t) { // Copy cell active/inactive state
		case 1 :
			memcpy(&actiSh.c[0][0],
				&arrShapeL_a1[0][0],
				sizeof(uint8_t)*4*4);
			actiSh.fColor = tOrange;
			break;
		case 2 :
			memcpy(&actiSh.c[0][0],
				&arrShapeRL_a1[0][0],
				sizeof(uint8_t)*4*4);
			actiSh.fColor = tBlue;
			break;
		case 3 :
			memcpy(&actiSh.c[0][0],
				&arrShapeZ_a1[0][0],
				sizeof(uint8_t)*4*4);
			actiSh.fColor = tRed;
			break;
		case 4 :
			memcpy(&actiSh.c[0][0],
				&arrShapeS_a1[0][0],
				sizeof(uint8_t)*4*4);
			actiSh.fColor = tGreen;
			break;
		case 5 :
			memcpy(&actiSh.c[0][0],
				&arrShapeB_a1[0][0],
				sizeof(uint8_t)*4*4);
			actiSh.fColor = tYellow;
			break;
		case 6 :
			memcpy(&actiSh.c[0][0],
				&arrShapeI_a1[0][0],
				sizeof(uint8_t)*4*4);
			actiSh.fColor = tCyan;
			break;
		case 7 :
			memcpy(&actiSh.c[0][0],
				&arrShapeT_a1[0][0],
				sizeof(uint8_t)*4*4);
			actiSh.fColor = tMagneta;
			break;
	}
}

/*
 * Game tick
 *
 */
void tTick() 
{ 	// If tick exceeds current level tick latency 
	if (sfClock_getElapsedTime(gameTick).microseconds >= lvlLatency) {
		sfClock_restart(gameTick); // Restart gameTick
		/* if bottom not reached */
		if ((wallCollisionCheck(0b0010) == 0) &&
		(cellCollisionCheck(0b0010) == 0))
			actiSh.y--; // Move
		else
			putShape(); // Just put the shape
	}
}


/*
 * Rotate matrix left routine
 *
 */
void rotateLeft() 
{
	uint8_t arr[4][4];
	memcpy(&arr[0][0], &actiSh.c[0][0], sizeof(uint8_t)*4*4);
	if (actiSh.t == 5)
		return;
	if (actiSh.t == 6) {
		for (int j = 3; j >= 0; j--)
			for (int i = 0; i < 4; i++)
				actiSh.c[j][i] = arr[3-i][3-j];
		return;
	}
	for (int j = 3; j > 0; j--)
		for (int i = 0; i < 3; i++)
			actiSh.c[j][i] = arr[3-i][j-1];
}


/*
 * Rotate matrix right routine
 *
 */
void rotateRight() 
{
	uint8_t arr[4][4];
	memcpy(&arr[0][0], &actiSh.c[0][0], sizeof(uint8_t)*4*4);
	if (actiSh.t == 5)
		return;
	if (actiSh.t == 6) {
		for (int j = 3; j >= 0; j--)
			for (int i = 0; i < 4; i++)
				actiSh.c[j][i] = arr[3-i][3-j];
		return;
	}
	for (int j = 3; j > 0; j--)
		for (int i = 0; i < 3; i++)
			actiSh.c[j][i] = arr[i+1][3-j];
}


/*
 * Rotates active shape with callling collision checkers
 *
 */
void rotateShape() 
{
	rotateRight(); // Make rotate
	if ((wallRotCollisionCheck() == 1) 
	|| (cellRotCollisionCheck() == 1)) 
		rotateLeft(); // Just rotate back =)
}

int cellRotCollisionCheck() 
{
	for (int j = 0; j < 4; j++) {
		for (int i = 0; i < 4; i++) {
			if (actiSh.c[j][i] 
			&& fldCAtt[j+actiSh.y][i+actiSh.x].a)
				return 1; // Collision happens
		}
	}
	return 0; // Else it do not
}

int wallRotCollisionCheck() 
{
	if(actiSh.y < 0) //If shape has crossed Bottom border
		for(int i = 0; i < 4; i++)
			if (actiSh.c[-1-actiSh.y][i]) 
				return 1;	// Collision happens
	if(actiSh.x < 0) //If shape has crossed Left border
		for(int i = 0; i < 4; i++)
			if (actiSh.c[i][-1-actiSh.x])
				return 1;	// Collision happens
	if(actiSh.x > 6) //If shape has crossed Right border
		for(int i = 0; i < 4; i++)
			if (actiSh.c[i][3-(actiSh.x-7)])
				return 1;	// Collision happens
	return 0; // If no conditions are met collision is absent
}

int cellCollisionCheck(int dir) 
{
	for (int j = 0; j < 4; j++) {
		for (int i = 0; i < 4; i++) {
			if ((dir & 0b0001) // Check Right
			&& (j+actiSh.y >= 0) // Avoiding nonexisting fld cells
			&& (i+actiSh.x+1 >= 0) // ---
			&& actiSh.c[j][i] // Check activity
			&& fldCAtt[j+actiSh.y][i+actiSh.x+1].a)
				return 1; // Collision happens
			if ((dir & 0b1000) // Check Left
			&& (j+actiSh.y >= 0) // Avoiding nonexisting fld cells
			&& (i+actiSh.x-1 >= 0) // ---
			&& actiSh.c[j][i] // Check activity
			&& fldCAtt[j+actiSh.y][i+actiSh.x-1].a)
				return 1; // Collision happens
			if ((dir & 0b0010) // Check Bottom			
			&& (j+actiSh.y-1 >= 0) // Avoiding nonexisting fld cells
			&& (i+actiSh.x >= 0) // ---
			&& actiSh.c[j][i] // Check activity
			&& fldCAtt[j+actiSh.y-1][i+actiSh.x].a)
				return 1; // Collision happens
		}
	}
	return 0; // Else it do not
}

int wallCollisionCheck(int dir) 
{
	if (dir & 0b0001) { // Right collision request
		if (actiSh.x >= 6) // If shape has reached Right boreder
			for (int i = 0 ; i < 4; i++)
				if (actiSh.c[i][3-(actiSh.x-6)])
					return 1; // Collision happens
	} else if (dir & 0b0010) { // Bottom collision request
		if (actiSh.y <= 0) //If shape has reached Bottom border
			for (int i = 0; i < 4; i++)
				if (actiSh.c[-actiSh.y][i])
					return 1; // Collision happens
	} else if (dir & 0b1000) // Left collision request
		if (actiSh.x <= 0) // If shape has reached Left border	
			for (int i = 0; i < 4; i++)		
				if (actiSh.c[i][-actiSh.x])
					return 1; // Collision happens
	return 0;
}


/* 
 * Keys hold handler 
 *
 */
void tKeyCtrl() 
{
	/* Up arrow key 'hold' handler */
	if (sfKeyboard_isKeyPressed(sfKeyUp) == 1) {
		if ((arrKeys & 0b0100) == 0) {
			arrKeys = arrKeys | 0b0100;
			rotateShape();
		}
	} else {
		if ((arrKeys & 0b0100) != 0) {
			arrKeys = arrKeys & ~0b0100;
		}
	}
	
	/* Down Arrow Key 'hold' handler */
	if (sfKeyboard_isKeyPressed(sfKeyDown) == 1) {
		if ((arrKeys & 0b0010) == 0) {
			arrKeys = arrKeys | 0b0010;
			if ((wallCollisionCheck(0b0010) == 0) 
			&& (cellCollisionCheck(0b0010) == 0)) {
				actiSh.y--;
				// Avoid excess move down by gameTick
				sfClock_restart(gameTick);
				scoreCurrent++;
			}
			repPushDown = sfClock_create();
		} else {
			if (sfClock_getElapsedTime(repPushDown).microseconds 
			>= moveRepeatLatency2) 
				arrKeys = arrKeys & ~0b0010;
		}
	} else {
		if ((arrKeys & 0b0010) != 0) {
			arrKeys = arrKeys & ~0b0010;
			arrKeys = arrKeys & ~0b100000;
		}
	}
	
	/* Left Arrow Key 'hold' handler */
	if ((sfKeyboard_isKeyPressed(sfKeyLeft) == 1) 
	&& (sfKeyboard_isKeyPressed(sfKeyRight) == 0)) {
		if ((arrKeys & 0b1000) == 0){
			arrKeys = arrKeys | 0b1000;
			if ((wallCollisionCheck(0b1000) == 0)
			&& (cellCollisionCheck(0b1000) == 0))
				actiSh.x--;
			repKeyLeft = sfClock_create();
		} else { 
			if ((arrKeys & 0b10000000) == 0) {
				if (sfClock_getElapsedTime(repKeyLeft).microseconds 
				>=  moveRepeatLatency1) {
					arrKeys = arrKeys | 0b10000000;
					arrKeys = arrKeys & ~0b1000;
				}
			} else {
				if (sfClock_getElapsedTime(repKeyLeft).microseconds 
				>=  moveRepeatLatency2) 
					arrKeys = arrKeys & ~0b1000;
			}
		}
	} else if (sfKeyboard_isKeyPressed(sfKeyLeft) == 0) {
		if ((arrKeys & 0b1000) != 0){
			arrKeys = arrKeys & ~0b1000;
			arrKeys = arrKeys & ~0b10000000;
		}
	}
	
	/* Right Arrow Key 'hold' handler */
	if ((sfKeyboard_isKeyPressed(sfKeyRight) == 1) 
	&& (sfKeyboard_isKeyPressed(sfKeyLeft) == 0)) {	
		if ((arrKeys & 0b0001) == 0){
			arrKeys = arrKeys | 0b0001;
			if ((wallCollisionCheck(0b0001) == 0) 
			&& (cellCollisionCheck(0b0001) == 0))
				actiSh.x++;
			repKeyRight = sfClock_create();
		} else {
			if ((arrKeys & 0b10000) == 0) {
				if (sfClock_getElapsedTime(repKeyRight).microseconds 
				>=  moveRepeatLatency1) {
					arrKeys = arrKeys | 0b10000;
					arrKeys = arrKeys & ~0b0001;
				}
			} else if (sfKeyboard_isKeyPressed(sfKeyLeft) == 0) {
				if (sfClock_getElapsedTime(repKeyRight).microseconds 
				>=  moveRepeatLatency2) // Wait short time
					arrKeys = arrKeys & ~0b0001;
			}
		}
	} else {
		if ((arrKeys & 0b0001) != 0) {
			arrKeys = arrKeys & ~0b0001;
			arrKeys = arrKeys & ~0b10000;
		}				
	}
}


/* 
 * Colorize active cells of field 
 *
 */
void colorizeFld()
{
	for(int j = 0; j < 20; j++) {
		for(int i = 0; i < 10; i++) {
			if (fldCAtt[j][i].a) {
				sfRectangleShape_setFillColor(
					fld[j][i], 
					fldCAtt[j][i].fColor);
				sfRectangleShape_setOutlineColor(
					fld[j][i], 
					uiColor3);
			} else {
				sfRectangleShape_setFillColor(
					fld[j][i], 
					uiColor1);
				sfRectangleShape_setOutlineColor(
					fld[j][i], 
					uiColor2);
			}
		}
	}
}


/* 
 * Colorize active cells of active shape (overlay only 
 * active cells above background of fld) 
 *
 */
void colorizeActiSh()
{
	for(int j = 0; j < 4; j++) {
		for(int i = 0; i < 4; i++) {
			if (actiSh.c[j][i]) {
				sfRectangleShape_setFillColor(
						fld[j+actiSh.y][i+actiSh.x], 
						actiSh.fColor);
				sfRectangleShape_setOutlineColor(
						fld[j+actiSh.y][i+actiSh.x], 
						uiColor3);
			}
		}
	}
}


void menuTick() 
{
	if(sfClock_getElapsedTime(mTick).microseconds >= lvlLatency){
		sfClock_restart(mTick);
		colorizeRandom();
	}
}


void colorizeRandom() 
{
	for (int j = 0; j < fldSize.y; j++) {
		for (int i = 0; i < fldSize.x; i++) {
			int a;
			a = rand()%7+1;
			switch (a) {;
				case 1 :
					sfRectangleShape_setFillColor(fld[j][i], 
								tOrange);
					break;
				case 2 :
					sfRectangleShape_setFillColor(fld[j][i], 
								tBlue);
					break;
				case 3 :
					sfRectangleShape_setFillColor(fld[j][i], 
								tRed);
					break;
				case 4 :
					sfRectangleShape_setFillColor(fld[j][i], 
								tGreen);
					break;
				case 5 :
					sfRectangleShape_setFillColor(fld[j][i], 
								tYellow);
					break;
				case 6 :
					sfRectangleShape_setFillColor(fld[j][i], 
								tCyan);
					break;
				case 7 :
					sfRectangleShape_setFillColor(fld[j][i], 
								tMagneta);
					break;
			}
			sfRectangleShape_setOutlineColor(fld[j][i], uiColor3);
		}
	}
}
