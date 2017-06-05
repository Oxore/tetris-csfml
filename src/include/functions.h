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
void rotateShape();
void rotateS1();
void rotateS2();
void rotateS3();
void rotateS4();
int linesRmScore();
void scoreDisplay();
void menuTick();

#endif
