struct field;
struct idlist;

enum game_state {
    GS_MAIN_MENU,
    GS_STARTED,
    GS_GAME_OVER,
    GS_GAME_OVER_WAIT,
    GS_PAUSED
};

// Bits in bitmap of keys
#define RIGHT     (1 << 0)
#define UP        (1 << 1)
#define DOWN      (1 << 2)
#define LEFT      (1 << 3)
#define RIGHTHOLD (1 << 4)
#define HARDDROP  (1 << 5)
#define PAUSE     (1 << 6)
#define LEFTHOLD  (1 << 7)
#define GAMEOVER  (1 << 8)

struct controls {
    uint32_t keys;         // bitmap of keys pressed
    sfClock *repPushDown;  // repeat latency when hold Down arrow
    sfClock *repKeyLeft;   // repeat latency when hold Left arrow
    sfClock *repKeyRight;  // repeat latency when hold Right arrow
};

struct game {
    enum game_state state;
    size_t      level;
    int         scoreCurrent;
    int         moveLatency;
    int         lines;
    sfClock    *gameTick;
    sfClock    *over_wait_tick;
    sfClock    *putTick;
    sfClock    *mTick;
    struct controls controls;

    struct field *fld;
    struct field *nxt;
    struct idlist *texts;
};
