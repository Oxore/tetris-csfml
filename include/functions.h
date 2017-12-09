#define RIGHT     0b0001
#define DOWN      0b0010
#define UP        0b0100
#define LEFT      0b1000
#define RIGHTHOLD 0b10000
#define LEFTHOLD  0b10000000

void tKeyCtrl();
void initFld();
void tTick();
void resetActiveShape(Shape *active);
void putShape();
int outOfFieldCheck(Field *fld, Shape *active);
void checkLevelUp(Game *game);
int cellCollisionCheckHere(Field *fld, Shape *active);
int cellCollisionCheck(int dir);
int wallCollisionCheck();
int cellRotCollisionCheck();
int wallRotCollisionCheck();
void rotateLeft();
void rotateRight();
void rotateShape();
int linesRmScore();
void valueAfterTextDisplay(int value, List *texts, char *type);
void colorizeFld();
void colorizeActive();
void drawFld(sfRenderWindow *window);
void colorizeRandom(Field *fld);
void gameover(Game *game);
void genNextShape();
void copyShape(Shape *localSh);
void drawNextShape();
void freeFld();
