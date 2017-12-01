#ifndef TFUNCTIONS_H
#define TFUNCTIONS_H

/* Prototypes of functions*/
void tKeyCtrl();
void initFld();
void tTick();
void resetActiveShape();
void putShape();
int cellCollisionCheck(int dir);
int wallCollisionCheck();
int cellRotCollisionCheck();
int wallRotCollisionCheck();
void rotateLeft();
void rotateRight();
void rotateShape();
int linesRmScore();
void scoreDisplay(int s, Text *textScore);
void colorizeFld();
void colorizeActiSh();
void drawFld(sfRenderWindow *window);
void menuTick();
void colorizeRandom();
void gameover();
void genNextShape();
void copyShape(Shape *localSh);
void drawNextShape();
void cleanupFld();

#endif
