#ifndef TFUNCTIONS_H
#define TFUNCTIONS_H

/* Prototypes of functions*/
void tKeyCtrl();
void initAll();
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
void scoreDisplay();
void colorizeFld();
void colorizeActiSh();
void drawFld(sfRenderWindow *window);
void menuTick();
void colorizeRandom();
void gameover();
void genNextShape();
void copyShape(struct shapeSt *localSh);
void drawNextShape();

#endif
