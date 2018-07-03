#define RIGHT     (1 << 0)
#define DOWN      (1 << 1)
#define UP        (1 << 2)
#define LEFT      (1 << 3)
#define RIGHTHOLD (1 << 4)
#define LEFTHOLD  (1 << 5)

void tKeyCtrl();
void tTick();
void checkLevelUp(struct game *game);
int  getMoveLatencyOfLevel(unsigned int level);
void valueAfterTextDisplay(int value, List *texts, char *type);
void gameover(struct game *game);
