struct game {
    size_t      level;
    int         started;
    int         paused;
    int         over_wait;
    int         over;
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
};
