#define RIGHT     1
#define DOWN      2
#define UP        4
#define LEFT      8
#define RIGHTHOLD 16
#define LEFTHOLD  128

void tKeyCtrl();
void tTick();
void checkLevelUp(struct game *game);
int getMoveLatencyOfLevel(int level);
int rmLines();
void valueAfterTextDisplay(int value, List *texts, char *type);
void colorizeActive();
void drawFld(sfRenderWindow *window);
void gameover(struct game *game);
void genNextShape();
void drawNextShape();
void freeFld();
