struct field;
struct idlist;

enum game_state {
    GS_MAIN_MENU,
    GS_STARTED,
    GS_GAME_OVER,
    GS_GAME_OVER_WAIT,
    GS_PAUSED
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
    sfClock    *repPushDown;  // repeat latency when hold Down arrow
    sfClock    *repKeyLeft;   // repeat latency when hold Left arrow
    sfClock    *repKeyRight;  // repeat latency when hold Right arrow

    struct field *fld;
    struct field *nxt;
    struct idlist *texts;
};
