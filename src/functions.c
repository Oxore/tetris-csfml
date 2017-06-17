#include "include/functions.h"
#include "include/common.h"

/* Externs from main.c */
int activeShape_s = 1; // Rotate state of activeShape
int activeShape_t = 1; // Type of shape (look at shape_maps.c)
extern sfVector2f textScore_pos;
extern sfText* textScore;
extern sfFont* font;
extern int gameIsStarted;

extern sfRectangleShape* fld[20][10];	// Array of field rectangles
extern sfVector2f fld_rPos[20][10]; 	// Array of absolute coordinates of field rectangles

extern int fld_rOutThick; 	// Field rectangles outline thickness	
extern sfVector2f fld_rSize;	// Field rectangles size variable x/y

extern sfVector2i actiShPos;   // Active shape offset relative to field

extern sfVector2i fldSize, fldPos;
extern sfVector2i actiShPos;	// Offset active shape relative to field
extern short arrKeys;		// arrow keys states byte container
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

sfClock* gameTick;
sfClock* mTick;
sfClock* repPushDown;	// Clock for repeat latency when Down arrow long push
sfClock* repPushLeft;	// Clock for repeat latency when Left arrow long push
sfClock* repPushRight;	// Clock for repeat latency when Left arrow long push

extern int lvlLatency;
extern int scoreCurrent;
extern char* scoreDisp;
/* Shapes maps */
extern short arrShapeL_a1[4][4];
extern short arrShapeL_a2[4][4];
extern short arrShapeL_a3[4][4];
extern short arrShapeL_a4[4][4];

extern short arrShapeRL_a1[4][4];
extern short arrShapeRL_a2[4][4];
extern short arrShapeRL_a3[4][4];
extern short arrShapeRL_a4[4][4];

extern short arrShapeZ_a1[4][4];
extern short arrShapeZ_a2[4][4];
extern short arrShapeZ_a3[4][4];
extern short arrShapeZ_a4[4][4];

extern short arrShapeS_a1[4][4];
extern short arrShapeS_a2[4][4];
extern short arrShapeS_a3[4][4];
extern short arrShapeS_a4[4][4];

extern short arrShapeB_a1[4][4];
extern short arrShapeB_a2[4][4];
extern short arrShapeB_a3[4][4];
extern short arrShapeB_a4[4][4];

extern short arrShapeI_a1[4][4];
extern short arrShapeI_a2[4][4];
extern short arrShapeI_a3[4][4];
extern short arrShapeI_a4[4][4];

extern short arrShapeT_a1[4][4];
extern short arrShapeT_a2[4][4];
extern short arrShapeT_a3[4][4];
extern short arrShapeT_a4[4][4];


/*
 * Init routine. Performs once per program run
 *
 */
void initAll() {
	
	/* 
	 * Dimensions of every fld's block
	 * 19px - fill color 1px - for outline, 20 - at all
	 *
	 */
	fld_rSize.x = 23;
	fld_rSize.y = 23;
	
	fldPos.x = 10; // Field's bottom left corner coordinates 
	fldPos.y = 10+500-25;
	fldSize.x = 10; // Field size in blocks
	fldSize.y = 20;	
	
	srand( time(NULL) );
	gameTick = sfClock_create();
	mTick = sfClock_create();
	resetActiveShape();
		
	/* Create field */
	for (int j=0;j<fldSize.y;j++){
		for(int i=0;i<fldSize.x;i++){
			fld_rAttr[j][i].fColor = uiColor1; // Fill empty color
			fld_rAttr[j][i].oColor = uiColor2; // Outlie empty color
			fld_rAttr[j][i].a = 0;	// Inactive = empty
			fld_rPos[j][i].x = fldPos.x 
				+ (i * (fld_rSize.x + 2 * fld_rOutThick));
			fld_rPos[j][i].y = fldPos.y - 
				(j * (fld_rSize.y + 2 * fld_rOutThick));
			fld[j][i] = sfRectangleShape_create();
			sfRectangleShape_setFillColor(fld[j][i], 
				fld_rAttr[j][i].fColor);
			sfRectangleShape_setSize(fld[j][i], fld_rSize);
			sfRectangleShape_setPosition(fld[j][i], fld_rPos[j][i]);
			sfRectangleShape_setOutlineColor(fld[j][i], 
				fld_rAttr[j][i].oColor);
			sfRectangleShape_setOutlineThickness(fld[j][i], 
				fld_rOutThick);
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
			if (fld_rAttr[j][i].a == 1) 
				k++;
		}
		if (k >= 10) {		// If line is full
			s++;		// Take scores for line
			for (int n = j; n < 20; n++) {	// Drop all lines down
				if (n == 19) {
					for (int m = 0; m < 10; m++) {
						fld_rAttr[n][m].a = 0;
						fld_rAttr[n][m].fColor 
							= uiColor1;	
						fld_rAttr[n][m].oColor 
							= uiColor2;
					}
				} else {
					for (int m = 0; m < 10; m++) {
						fld_rAttr[n][m].a 
							= fld_rAttr[n+1][m].a;
						fld_rAttr[n][m].fColor 
							= fld_rAttr[n+1]
							[m].fColor;
						fld_rAttr[n][m].oColor 
							= fld_rAttr[n+1]
							[m].oColor;
					}
				}
			}
			j--; 	// Do not let loop to go to next line because 
				// next line go down by itself =)
		}
		k = 0;	// Clear line fill indicator 
	}
	return s; // Return number of deleted lines
}

void putShape() 
{
	for (int j = 0; j < 4; j++) {
		for (int i = 0; i < 4; i++) {
			if (activeShape[j][i].a == 1) {
				fld_rAttr[j+actiShPos.y]
					[i+actiShPos.x].a 
					= activeShape[j][i].a;
				if ((j+actiShPos.y >= 0) 
				&& (i+actiShPos.x >= 0)) {
					fld_rAttr[j+actiShPos.y]
						[i+actiShPos.x].fColor 
						= activeShape[j][i].fColor;
					fld_rAttr[j+actiShPos.y]
						[i+actiShPos.x].oColor 
						= activeShape[j][i].oColor;
				}
			}
		}
	}
	resetActiveShape();
	scoreCurrent += linesRmScore()*100; 	// Remove filled lines and 
						// get score;
}

void resetActiveShape() 
{
	actiShPos.x = 3;
	actiShPos.y = 16;
	activeShape_s = 1;
	activeShape_t = (rand()%7)+1;	// Insert new random shape of 7 variants
	for (int j=0;j<4;j++) {
		for (int i=0;i<4;i++) {
			switch (activeShape_t) {; // Copy cell active/inactive state
				case 1 :
					activeShape[j][i].a = arrShapeL_a1[j][i];
					activeShape[j][i].fColor = tOrange;
					break;
				case 2 :
					activeShape[j][i].a = arrShapeRL_a1[j][i];
					activeShape[j][i].fColor = tBlue;
					break;
				case 3 :
					activeShape[j][i].a = arrShapeZ_a1[j][i];
					activeShape[j][i].fColor = tRed;
					break;
				case 4 :
					activeShape[j][i].a = arrShapeS_a1[j][i];
					activeShape[j][i].fColor = tGreen;
					break;
				case 5 :
					activeShape[j][i].a = arrShapeB_a1[j][i];
					activeShape[j][i].fColor = tYellow;
					break;
				case 6 :
					activeShape[j][i].a = arrShapeI_a1[j][i];
					activeShape[j][i].fColor = tCyan;
					break;
				case 7 :
					activeShape[j][i].a = arrShapeT_a1[j][i];
					activeShape[j][i].fColor = tMagneta;
					break;
			}
			activeShape[j][i].oColor = sfBlack;
		}
	}
}

/*
 * Game tick
 *
 * */
void tTick() 
{ 	// If tick exceeds current level tick latency 
	if (sfClock_getElapsedTime(gameTick).microseconds >= lvlLatency) {
		sfClock_restart(gameTick); // Restart gameTick
		/* if bottom not reached */
		if ((wallCollisionCheck(0b0010) == 0) &&
		(cellCollisionCheck(0b0010) == 0))
			actiShPos.y--;	// Move
		else
			putShape(); // Just put the shape
	}
}

void rotateS1() 
{
	for (int j=0;j<4;j++) {
		for (int i=0;i<4;i++) {
			switch (activeShape_t) {
				case 1 :
					activeShape[j][i].a = arrShapeL_a1[j][i]; break; // Copy cell active/inactive state L type
				case 2 :
					activeShape[j][i].a = arrShapeRL_a1[j][i]; break; // Copy cell active/inactive state RL type
				case 3 :
					activeShape[j][i].a = arrShapeZ_a1[j][i]; break; // Copy cell active/inactive state Z type
				case 4 :
					activeShape[j][i].a = arrShapeS_a1[j][i]; break; // Copy cell active/inactive state S type
				case 5 :
					activeShape[j][i].a = arrShapeB_a1[j][i]; break; // Copy cell active/inactive state B type
				case 6 :
					activeShape[j][i].a = arrShapeI_a1[j][i]; break; // Copy cell active/inactive state I type
				case 7 :
					activeShape[j][i].a = arrShapeT_a1[j][i]; break; // Copy cell active/inactive state T type
				default : //If type is invalid
					activeShape_t = 1; //Set valid type
			}
		}
	}	
}

void rotateS2() 
{
	for (int j=0;j<4;j++) {
		for (int i=0;i<4;i++) {
			switch (activeShape_t) {
				case 1 :
					activeShape[j][i].a = arrShapeL_a2[j][i]; break; // Copy cell active/inactive state L type
				case 2 :
					activeShape[j][i].a = arrShapeRL_a2[j][i]; break; // Copy cell active/inactive state RL type
				case 3 :
					activeShape[j][i].a = arrShapeZ_a2[j][i]; break; // Copy cell active/inactive state Z type
				case 4 :
					activeShape[j][i].a = arrShapeS_a2[j][i]; break; // Copy cell active/inactive state S type
				case 5 :
					activeShape[j][i].a = arrShapeB_a2[j][i]; break; // Copy cell active/inactive state B type
				case 6 :
					activeShape[j][i].a = arrShapeI_a2[j][i]; break; // Copy cell active/inactive state I type
				case 7 :
					activeShape[j][i].a = arrShapeT_a2[j][i]; break; // Copy cell active/inactive state T type
				default : //If type is invalid
					activeShape_t = 1; //Set valid type
			}
		}
	}
}

void rotateS3() 
{
	for (int j=0;j<4;j++) {
		for (int i=0;i<4;i++) {
			switch (activeShape_t) {
				case 1 :
					activeShape[j][i].a = arrShapeL_a3[j][i]; break; // Copy cell active/inactive state L type
				case 2 :
					activeShape[j][i].a = arrShapeRL_a3[j][i]; break; // Copy cell active/inactive state RL type
				case 3 :
					activeShape[j][i].a = arrShapeZ_a3[j][i]; break; // Copy cell active/inactive state Z type
				case 4 :
					activeShape[j][i].a = arrShapeS_a3[j][i]; break; // Copy cell active/inactive state S type
				case 5 :
					activeShape[j][i].a = arrShapeB_a3[j][i]; break; // Copy cell active/inactive state B type
				case 6 :
					activeShape[j][i].a = arrShapeI_a3[j][i]; break; // Copy cell active/inactive state I type
				case 7 :
					activeShape[j][i].a = arrShapeT_a3[j][i]; break; // Copy cell active/inactive state T type
				default : //If type is invalid
					activeShape_t = 1; //Set valid type
			}
		}
	}	
}

void rotateS4() 
{
	for (int j=0;j<4;j++) {
		for (int i=0;i<4;i++) {
			switch (activeShape_t) {
				case 1 :
					activeShape[j][i].a = arrShapeL_a4[j][i]; break; // Copy cell active/inactive state L type
				case 2 :
					activeShape[j][i].a = arrShapeRL_a4[j][i]; break; // Copy cell active/inactive state RL type
				case 3 :
					activeShape[j][i].a = arrShapeZ_a4[j][i]; break; // Copy cell active/inactive state Z type
				case 4 :
					activeShape[j][i].a = arrShapeS_a4[j][i]; break; // Copy cell active/inactive state S type
				case 5 :
					activeShape[j][i].a = arrShapeB_a4[j][i]; break; // Copy cell active/inactive state B type
				case 6 :
					activeShape[j][i].a = arrShapeI_a4[j][i]; break; // Copy cell active/inactive state I type
				case 7 :
					activeShape[j][i].a = arrShapeT_a4[j][i]; break; // Copy cell active/inactive state T type
				default : //If type is invalid
					activeShape_t = 1; //Set valid type
			}
		}
	}	
}

void rotateShape() 
{
	switch (activeShape_s) {
		case 1 :
			rotateS2(); // Make rotate
			if ((wallRotCollisionCheck() == 1) ||			// If collision happens
				(cellRotCollisionCheck() == 1)) rotateS1(); // Just rotate back =)
			else activeShape_s++; // Change state to next
			break;
		case 2 :
			rotateS3(); // Make rotate
			if ((wallRotCollisionCheck() == 1) ||			// If collision happens
				(cellRotCollisionCheck() == 1)) rotateS2(); // Just rotate back =)
			else activeShape_s++; // Change state to next
			break;
		case 3 :
			rotateS4(); // Make rotate
			if ((wallRotCollisionCheck() == 1) ||			// If collision happens
				(cellRotCollisionCheck() == 1)) rotateS3(); // Just rotate back =)
			else activeShape_s++; // Change state to next
			break;
		case 4 :
			rotateS1(); // Make rotate
			if ((wallRotCollisionCheck() == 1) ||			// If collision happens
				(cellRotCollisionCheck() == 1)) rotateS4(); // Just rotate back =)
			else activeShape_s = 1; // Change state to next
			break;
		default:	// If rotate state is invalid
			activeShape_s = 1;	// Set valid state
			rotateShape();		// And repeat rotate function
	}
}

int cellRotCollisionCheck() 
{
	for (int j=0;j<4;j++) {
		for (int i=0;i<4;i++) {
			if ((activeShape[j][i].a == 1) && // If any active cell of shape meet any active cell  
				(fld_rAttr[j+actiShPos.y][i+actiShPos.x].a == 1)) // of fld
				return 1; // Collision happens
		}
	}
	return 0; // Else it do not
}

int wallRotCollisionCheck() 
{
	if(actiShPos.y < 0) {	//If shape has crossed Bottom border
		for(int i=0;i<4;i++) {
			if (activeShape[-1-actiShPos.y][i].a != 0){	// If active cell is out of fld
				return 1;	// Collision happens
			}
		}
	}
	if(actiShPos.x < 0) {	//If shape has crossed Left border
		for(int i=0;i<4;i++) {
			if (activeShape[i][-1-actiShPos.x].a != 0){	// If active cell is out of fld
				return 1;	// Collision happens
			}
		}
	}
	if(actiShPos.x > 6) {	//If shape has crossed Right border
		for(int i=0;i<4;i++) {
			if (activeShape[i][3-(actiShPos.x-7)].a != 0){	// If active cell is out of fld
				return 1;	// Collision happens
			}
		}
	}
	return 0; // If no conditions are met collision is absent
}

int cellCollisionCheck(int dir) 
{
	for (int j=0;j<4;j++) {
		for (int i=0;i<4;i++) {
			if((dir & 0b0001) != 0){ // Check Right
				if ((j+actiShPos.y >= 0) && (i+actiShPos.x+1 >= 0)) { // Avoiding checking nonexisted fld cells	
					if ((activeShape[j][i].a == 1) && // If any active cell of shape meet any active cell  
						(fld_rAttr[j+actiShPos.y][i+actiShPos.x+1].a == 1)) // of fld
						return 1; // Collision happens
				}
			}
			if((dir & 0b1000) != 0){ // Check Left
				if ((j+actiShPos.y >= 0) && (i+actiShPos.x-1 >= 0)) { // Avoiding checking nonexisted fld cells	
					if ((activeShape[j][i].a == 1) && // If any active cell of shape meet any active cell  
						(fld_rAttr[j+actiShPos.y][i+actiShPos.x-1].a == 1)) // of fld
						return 1; // Collision happens
				}
			}
			if((dir & 0b0010) != 0){ // Check Bottom			
				if ((j+actiShPos.y-1 >= 0) && (i+actiShPos.x >= 0)) { // Avoiding checking nonexisted fld cells	
					if ((activeShape[j][i].a == 1) && // If any active cell of shape meet any active cell  
						(fld_rAttr[j+actiShPos.y-1][i+actiShPos.x].a == 1)) // of fld
						return 1; // Collision happens
				}
			}
		}
	}
	return 0; // Else it do not
}

int wallCollisionCheck(int dir) 
{
	if((dir & 0b0001) != 0) { 		// Right collision request
		if(actiShPos.x >= 6) { // If shape has reached Right boreder
			for(int i=0;i<4;i++) {
				if(activeShape[i][3-(actiShPos.x-6)].a != 0) {
					return 1; // Collision happens
				}
			}
		}
	} else if((dir & 0b0010) != 0){	// Bottom collision request
		if(actiShPos.y <= 0) {	//If shape has reached Bottom border
			for(int i=0;i<4;i++) {
				if (activeShape[-actiShPos.y][i].a != 0){
					return 1;	// Collision happens
				}
			}
		}
	} else if((dir & 0b1000) != 0){ // Left collision request
		if(actiShPos.x <= 0) {	// If shape has reached Left border	
			for(int i=0;i<4;i++) {		
				if(activeShape[i][-actiShPos.x].a != 0) {
					return 1;	// Collision happens
				}
			}
		}
	} else {
		return -1;
	}
	return 0;
}


/* Control keys handle */
void tKeyCtrl() 
{
	
	/* Up arrow key */
	if (sfKeyboard_isKeyPressed(sfKeyUp) == 1){
		if ((arrKeys & 0b0100) == 0){				// If "Up arrow button hold" bit is not enabled 
			arrKeys = arrKeys | 0b0100;			// Enable the "Up arrow button hold" bit
			rotateShape();
		}
	} else {								// If Down arrow button not pressed
		if ((arrKeys & 0b0100) != 0){				// and if "Up arrow button hold" bit is enabled
			arrKeys = arrKeys & ~0b0100;	// it means that button is already released
		}									// So it is time to clear "Up arrow button hold" bit
	}
	
	/* Down Arrow Key */
	if (sfKeyboard_isKeyPressed(sfKeyDown) == 1){ 	// If Down arrow button is pressed
		if ((arrKeys & 0b0010) == 0){				// If "pushed down" key of Down arrow button not enabled 
			arrKeys = arrKeys | 0b0010;			// Enable "pushed down" key of down arrow button
			if ((wallCollisionCheck(0b0010) == 0) &&	// If collision do not happen
				(cellCollisionCheck(0b0010) == 0)) {	// 0b0010 for "Down"
				actiShPos.y--;	// then do move
				sfClock_restart(gameTick);	// Avoid excess move down by gameTick
				scoreCurrent++;
			}
			repPushDown = sfClock_create();		// Create/restart clock
		} else {
			if (sfClock_getElapsedTime(repPushDown).microseconds >= moveRepeatLatency2) arrKeys = arrKeys & ~0b0010;
		}
	} else {								// If Down arrow button not pressed
		if ((arrKeys & 0b0010) != 0){				// and if "pushed down" key is enabled
			arrKeys = arrKeys & ~0b0010;	// it means that button is already released
			arrKeys = arrKeys & ~0b100000;
		}									// So it is time to clear "down arrow pushed down" bit
	}
	
	/* Left Arrow Key */
	if ((sfKeyboard_isKeyPressed(sfKeyLeft) == 1) && (sfKeyboard_isKeyPressed(sfKeyRight) == 0)){ 	// If left arrow button is pressed
		if ((arrKeys & 0b1000) == 0){			// If "pushed down" key of left arrow button disabled 
			arrKeys = arrKeys | 0b1000;			// Enable "pushed down" key of left arrow button
			if ((wallCollisionCheck(0b1000) == 0) &&	// If collision do not happen
				(cellCollisionCheck(0b1000) == 0))		// 0b1000 for "left"
				actiShPos.x--;				// then do move
			repPushLeft = sfClock_create();		// Create/restart clock
		} else { // Push held action repeat handler
			if ((arrKeys & 0b10000000) == 0) {	// If "push Left repeat" bit is not set
				if (sfClock_getElapsedTime(repPushLeft).microseconds >=  moveRepeatLatency1) {	// Wait long time
					arrKeys = arrKeys | 0b10000000;		// set "push Left repeat" bit
					arrKeys = arrKeys & ~0b1000;		// unset "pushed down" bit to repeat move
				}
			} else {						// If "push Left repeat" bit is set
				if (sfClock_getElapsedTime(repPushLeft).microseconds >=  moveRepeatLatency2) // Wait short time
					arrKeys = arrKeys & ~0b1000;		// unset "pushed down" bit to repeat movse
			}
		}
	} else if (sfKeyboard_isKeyPressed(sfKeyLeft) == 0) { 	// If left arrow button is released
		if ((arrKeys & 0b1000) != 0){				// and if "pushed down" key is enabled
			arrKeys = arrKeys & ~0b1000;	// it means that button is already released
			arrKeys = arrKeys & ~0b10000000;// So it is time to clear "right arrow pushed down" bit
		}									// And "push repeat" bit
	}
	
	/* Right Arrow Key */
	if ((sfKeyboard_isKeyPressed(sfKeyRight) == 1) && (sfKeyboard_isKeyPressed(sfKeyLeft) == 0)) {	// If right arrow button is pressed 
		if ((arrKeys & 0b0001) == 0){				// If "pushed down" key of right arrow button not enabled 
			arrKeys = arrKeys | 0b0001;			// Enable "pushed down" key of right arrow button
			if ((wallCollisionCheck(0b0001) == 0) && // If collision do not happen
				(cellCollisionCheck(0b0001) == 0))		// 0b0001 for "right"
				actiShPos.x++;	// then do move
			repPushRight = sfClock_create();
		} else { // Push held action repeat handler
			if ((arrKeys & 0b10000) == 0) {	// If "push Right repeat" bit is not set
				if (sfClock_getElapsedTime(repPushRight).microseconds >=  moveRepeatLatency1) {	// Wait long time
					arrKeys = arrKeys | 0b10000;		// set "push Right repeat" bit
					arrKeys = arrKeys & ~0b0001;		// unset "pushed down" bit to repeat move
				}
			} else if (sfKeyboard_isKeyPressed(sfKeyLeft) == 0) {						// If "push Right repeat" bit is set
				if (sfClock_getElapsedTime(repPushRight).microseconds >=  moveRepeatLatency2) // Wait short time
					arrKeys = arrKeys & ~0b0001;		// unset "pushed down" bit to repeat movse
			}
		}
	} else {								// If Right arrow button not pressed
		if ((arrKeys & 0b0001) != 0){		// and if "pushed down" key is enabled
			arrKeys = arrKeys & ~0b0001;	// <---+  it means that button is already released
			arrKeys = arrKeys & ~0b10000;	// <-+ +--So it is time to clear "right arrow pushed down" bit
		}									//   +-And "push repeat" bit
	}
}



void menuTick() 
{
	if(sfClock_getElapsedTime(mTick).microseconds >= lvlLatency){
		sfClock_restart(mTick);
		for (int j=0;j<fldSize.y;j++){
			for(int i=0;i<fldSize.x;i++){
				int a;
				a = rand()%7+1;
				switch (a) {;
					case 1 :
						fld_rAttr[j][i].fColor = tOrange;
						break;
					case 2 :
						fld_rAttr[j][i].fColor = tBlue;
						break;
					case 3 :
						fld_rAttr[j][i].fColor = tRed;
						break;
					case 4 :
						fld_rAttr[j][i].fColor = tGreen;
						break;
					case 5 :
						fld_rAttr[j][i].fColor = tYellow;
						break;
					case 6 :
						fld_rAttr[j][i].fColor = tCyan;
						break;
					case 7 :
						fld_rAttr[j][i].fColor = tMagneta;
						break;
				}
				fld_rAttr[j][i].oColor = sfBlack;
				sfRectangleShape_setFillColor(fld[j][i], fld_rAttr[j][i].fColor); 		// FillColor
				sfRectangleShape_setOutlineColor(fld[j][i], fld_rAttr[j][i].oColor);	// Outline color
			}
		}
	}
}
