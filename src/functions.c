#include "common.h"
#include "functions.h"

/* Externs from main.c */

extern sfRectangleShape* ns[4][4];   // Array of next shape image rectangles
extern sfVector2f nsCSize; // Next shape rectangles size variable x/y
extern sfRectangleShape *fld[25][10]; // Array of field rectangles
extern sfVector2f fldCPos[25][10]; // Array of abs coords of field rectangles
extern sfVector2i fldSize;
extern sfVector2f fldPos;
extern sfVector2f fldCSize; // Field rectangles size variable x/y
extern int fldCOutThick; // Field rectangles outline thickness

extern sfVector2f textScore_pos;
extern sfText *textScore;
extern sfFont *fontScore;
extern int gameIsStarted;

extern uint8_t arrKeys;    // Arrow keys states byte container
/* arrKeys = ...n|7|6|5|4|3|2|1|0| (just a bit of so called "bit fucking")
 * 0 - Right arrow pushed and held
 * 1 - Down arrow pushed and held
 * 2 - N/A
 * 3 - Left arrow pushed and held
 * 4 - Right arrow short repeat activated (after once long repeat)
 * 5 - N/A
 * 6 - N/A
 * 7 - Right arrow short repeat activated (after once long repeat)
 */

extern sfClock *gameTick;
extern sfClock *mTick;
extern sfClock *repPushDown; // Clock for repeat latency when Down arrow long push
extern sfClock *repKeyLeft; // Clock for repeat latency when Left arrow long push
extern sfClock *repKeyRight; // Clock for repeat latency when Left arrow long push

extern int lvlLatency;
extern int scoreCurrent;
extern char *scoreDisp;
/* Shapes maps */
extern uint8_t arrShapeL[4][4];
extern uint8_t arrShapeRL[4][4];
extern uint8_t arrShapeZ[4][4];
extern uint8_t arrShapeS[4][4];
extern uint8_t arrShapeB[4][4];
extern uint8_t arrShapeI[4][4];
extern uint8_t arrShapeT[4][4];


/* Field init routine */
void initFld()
{
    /* Create field */
    for (int j = 0; j < fldSize.y; j++) {
        for(int i = 0; i < fldSize.x; i++) {
            fldCAtt[j][i].a = 0;    // Inactive = empty
            fldCPos[j][i].x = fldPos.x
                + (i * (fldCSize.x + 2 * fldCOutThick));
            fldCPos[j][i].y = fldPos.y -
                (j * (fldCSize.y + 2 * fldCOutThick));
            fld[j][i] = sfRectangleShape_create();
            sfRectangleShape_setFillColor(fld[j][i], uiColor1);
            sfRectangleShape_setSize(fld[j][i], fldCSize);
            sfRectangleShape_setPosition(fld[j][i], fldCPos[j][i]);
            sfRectangleShape_setOutlineColor(fld[j][i], uiColor3);
            sfRectangleShape_setOutlineThickness(fld[j][i],
                fldCOutThick);
        }
    }
    /* Create next shape field */
    sfVector2f nsPos;
    for (int j = 0; j < 4; j++) {
        for(int i = 0; i < 4; i++) {
            nsPos.x = nxtShape.x+i*(nsCSize.x+2*fldCOutThick);
            nsPos.y = nxtShape.y-j*(nsCSize.y+2*fldCOutThick);
            ns[j][i] = sfRectangleShape_create();
            sfRectangleShape_setSize(ns[j][i], nsCSize);
            sfRectangleShape_setPosition(ns[j][i], nsPos);
            sfRectangleShape_setOutlineThickness(ns[j][i],
                                 fldCOutThick);
        }
    }
    genNextShape();
    resetActiveShape();
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
        if (k >= 10) {    // If line is full
            s++;    // Give scores for line
            for (int n = j; n < 20; n++) {    // Drop all lines down
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
            j--;     // Do not let loop to go to next line because
                // next line go down by itself =)
        }
        k = 0;    // Clear line fill indicator
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
    scoreCurrent += linesRmScore()*100;     // Remove filled lines and
                        // get score;
    for (int i = 0; i < 10; i++)
        if (fldCAtt[20][i].a) {
            gameover();
            return;
        }
    resetActiveShape();
}

void resetActiveShape()
{
    genNextShape();
    copyShape(&actiSh);
    actiSh.x = 3;
    if (actiSh.t == 6)
        actiSh.y = 17;
    else
        actiSh.y = 16;
    for (;;) {
        if (cellCollisionCheck(0b0010))
            actiSh.y++;
        else
            break;
    }
}

/*
 * Game tick
 *
 */
void tTick()
{     // If tick exceeds current level tick latency
    if (sfClock_getElapsedTime(gameTick).microseconds >= lvlLatency) {
        sfClock_restart(gameTick); // Restart gameTick
        /* if bottom not reached */
        if ((wallCollisionCheck(0b0010) == 0)
        && (cellCollisionCheck(0b0010) == 0))
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
    for (int j = 0; j < 4; j++)
        for (int i = 0; i < 4; i++)
            if (actiSh.c[j][i]
            && fldCAtt[j+actiSh.y][i+actiSh.x].a)
                return 1; // Collision happens
    return 0; // Else it do not
}


int wallRotCollisionCheck()
{
    if(actiSh.y < 0) //If shape has crossed Bottom border
        for(int i = 0; i < 4; i++)
            if (actiSh.c[-1-actiSh.y][i])
                return 1;    // Collision happens
    if(actiSh.x < 0) //If shape has crossed Left border
        for(int i = 0; i < 4; i++)
            if (actiSh.c[i][-1-actiSh.x])
                return 1;    // Collision happens
    if(actiSh.x > 6) //If shape has crossed Right border
        for(int i = 0; i < 4; i++)
            if (actiSh.c[i][3-(actiSh.x-7)])
                return 1;    // Collision happens
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
    if (sfKeyboard_isKeyPressed(sfKeyUp)) {
        if (!(arrKeys & 0b0100)) {
            arrKeys = arrKeys | 0b0100;
            rotateShape();
        }
    } else {
        if ((arrKeys & 0b0100)) {
            arrKeys = arrKeys & ~0b0100;
        }
    }

    /* Down Arrow Key 'hold' handler */
    if (sfKeyboard_isKeyPressed(sfKeyDown)) {
        if (!(arrKeys & 0b0010)) {
            arrKeys = arrKeys | 0b0010;
            if (!wallCollisionCheck(0b0010)
            && !cellCollisionCheck(0b0010)) {
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
        if ((arrKeys & 0b0010)) {
            arrKeys = arrKeys & ~0b0010;
            arrKeys = arrKeys & ~0b100000;
        }
    }

    /* Left Arrow Key 'hold' handler */
    if (sfKeyboard_isKeyPressed(sfKeyLeft)
    && !sfKeyboard_isKeyPressed(sfKeyRight)) {
        if (!(arrKeys & 0b1000)) {
            arrKeys = arrKeys | 0b1000;
            if (!wallCollisionCheck(0b1000)
            && !cellCollisionCheck(0b1000))
                actiSh.x--;
            repKeyLeft = sfClock_create();
        } else {
            if (!(arrKeys & 0b10000000)) {
                if (sfClock_getElapsedTime(repKeyLeft)
                                .microseconds
                >=  moveRepeatLatency1) {
                    arrKeys = arrKeys | 0b10000000;
                    arrKeys = arrKeys & ~0b1000;
                }
            } else {
                if (sfClock_getElapsedTime(repKeyLeft)
                                .microseconds
                >=  moveRepeatLatency2)
                    arrKeys = arrKeys & ~0b1000;
            }
        }
    } else if (!sfKeyboard_isKeyPressed(sfKeyLeft)) {
        if ((arrKeys & 0b1000)){
            arrKeys = arrKeys & ~0b1000;
            arrKeys = arrKeys & ~0b10000000;
        }
    }

    /* Right Arrow Key 'hold' handler */
    if (sfKeyboard_isKeyPressed(sfKeyRight)
    && !sfKeyboard_isKeyPressed(sfKeyLeft)) {
        if (!(arrKeys & 0b0001)){
            arrKeys = arrKeys | 0b0001;
            if (!wallCollisionCheck(0b0001)
            && !cellCollisionCheck(0b0001))
                actiSh.x++;
            repKeyRight = sfClock_create();
        } else {
            if (!(arrKeys & 0b10000)) {
                if (sfClock_getElapsedTime(repKeyRight)
                                .microseconds
                >=  moveRepeatLatency1) {
                    arrKeys = arrKeys | 0b10000;
                    arrKeys = arrKeys & ~0b0001;
                }
            } else if (!sfKeyboard_isKeyPressed(sfKeyLeft)) {
                if (sfClock_getElapsedTime(repKeyRight)
                                .microseconds
                >=  moveRepeatLatency2) // Wait short time
                    arrKeys = arrKeys & ~0b0001;
            }
        }
    } else {
        if ((arrKeys & 0b0001)) {
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
    for(int j = 0; j < fldSize.y-5; j++) {
        for(int i = 0; i < fldSize.x; i++) {
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
            if (actiSh.c[j][i] && j+actiSh.y < 20) {
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


/*
 * Draw all fld cells
 *
 */
void drawFld(sfRenderWindow *window)
{
    for (int j = 0; j < fldSize.y; j++){
        for(int i = 0; i < fldSize.x; i++){
            sfRenderWindow_drawRectangleShape(window,
                              fld[j][i],
                              NULL);
        }
    }
}


void menuTick()
{
    if(sfClock_getElapsedTime(mTick).microseconds >= lvlLatency) {
        sfClock_restart(mTick);
        colorizeRandom();
    }
}


void colorizeRandom()
{
    int a;
    for (int j = 0; j < fldSize.y-5; j++) {
        for (int i = 0; i < fldSize.x; i++) {
            a = rand()%7+1;
            switch (a) {
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


void gameover()
{
    gameIsStarted = 0;
    scoreCurrent = 0;
}


void genNextShape()
{
    actiSh.t = nxtShape.t;
    nxtShape.t = (rand()%7)+1; // Insert new random shape of 7 variants
    copyShape(&nxtShape);
    if (nxtShape.t == 5)
        for (int j = 0; j < 3; j++)
            for (int i = 0; i < 4; i++)
                nxtShape.c[i][j] = nxtShape.c[i][j+1];
}


void copyShape(struct shapeSt *localSh)
{
    switch (localSh->t) { // Copy cell active/inactive state
        case 1 :
            memcpy(&localSh->c[0][0],
                &arrShapeL[0][0],
                sizeof(uint8_t)*4*4);
            localSh->fColor = tOrange;
            break;
        case 2 :
            memcpy(&localSh->c[0][0],
                &arrShapeRL[0][0],
                sizeof(uint8_t)*4*4);
            localSh->fColor = tBlue;
            break;
        case 3 :
            memcpy(&localSh->c[0][0],
                &arrShapeZ[0][0],
                sizeof(uint8_t)*4*4);
            localSh->fColor = tRed;
            break;
        case 4 :
            memcpy(&localSh->c[0][0],
                &arrShapeS[0][0],
                sizeof(uint8_t)*4*4);
            localSh->fColor = tGreen;
            break;
        case 5 :
            memcpy(&localSh->c[0][0],
                &arrShapeB[0][0],
                sizeof(uint8_t)*4*4);
            localSh->fColor = tYellow;
            break;
        case 6 :
            memcpy(&localSh->c[0][0],
                &arrShapeI[0][0],
                sizeof(uint8_t)*4*4);
            localSh->fColor = tCyan;
            break;
        case 7 :
            memcpy(&localSh->c[0][0],
                &arrShapeT[0][0],
                sizeof(uint8_t)*4*4);
            localSh->fColor = tMagneta;
            break;
    }
}


void drawNextShape(sfRenderWindow *window)
{
    static sfText *textNextShape;
    sfVector2f textNextShapePos;
    textNextShapePos.x = 250+10+10;
    textNextShapePos.y = 80;
    if (!textNextShape)
        textNextShape = sfText_create();
    sfText_setFont(textNextShape, fontScore);
    sfText_setCharacterSize(textNextShape, 20);
    sfText_setPosition(textNextShape, textNextShapePos);
    char a[13];
    sprintf(a, "Next Shape: ");
    sfText_setString(textNextShape, (char *)&a);
    sfRenderWindow_drawText(window, textNextShape, NULL);
    for(int j = 0; j < 4; j++)
        for(int i = 0; i < 4; i++)
            if (nxtShape.c[j][i]) {
                sfRectangleShape_setFillColor(ns[j][i],
                                  nxtShape.fColor);
                sfRectangleShape_setOutlineColor(ns[j][i],
                                 uiColor3);
                sfRenderWindow_drawRectangleShape(window,
                                  ns[j][i],
                                  NULL);
            }
}

void cleanupFld() {
    for (int j = 0; j < fldSize.y; j++)
        for(int i = 0; i < fldSize.x; i++)
            sfRectangleShape_destroy(fld[j][i]);
    for (int j = 0; j < 4; j++)
        for(int i = 0; i < 4; i++)
            sfRectangleShape_destroy(ns[j][i]);
}
