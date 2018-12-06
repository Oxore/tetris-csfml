struct game {
    size_t      level;
    int         started;
    int         paused;
    int         scoreCurrent;
    int         moveLatency;
    int         lines;
    sfClock    *gameTick;
    sfClock    *putTick;
    sfClock    *mTick;
    sfClock    *repPushDown;  // repeat latency when hold Down arrow
    sfClock    *repKeyLeft;   // repeat latency when hold Left arrow
    sfClock    *repKeyRight;  // repeat latency when hold Right arrow
};
