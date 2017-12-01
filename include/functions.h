#ifndef TFUNCTIONS_H
#define TFUNCTIONS_H

/* Prototypes of functions*/
void tKeyCtrl();
void initFld();
void tTick();
void resetActiveShape();
void putShape();
void checkLevelUp(Game *game);
int cellCollisionCheck(int dir);
int wallCollisionCheck();
int cellRotCollisionCheck();
int wallRotCollisionCheck();
void rotateLeft();
void rotateRight();
void rotateShape();
int linesRmScore();
void scoreDisplay(int s, Text *textScore);
void levelDisplay(int s, Text *textLevel);
void colorizeFld();
void colorizeActive();
void drawFld(sfRenderWindow *window);
void colorizeRandom(Field *fld);
void gameover(Game *game);
void genNextShape();
void copyShape(Shape *localSh);
void drawNextShape();
void freeFld();

#endif
