struct game {
    unsigned int level;
    int          isStarted;
    int          scoreCurrent;
    int          moveLatency;
    int          lines;
    sfClock     *gameTick;
    sfClock     *putTick;
    sfClock     *mTick;
    sfClock     *repPushDown;  // repeat latency when hold Down arrow
    sfClock     *repKeyLeft;   // repeat latency when hold Left arrow
    sfClock     *repKeyRight;  // repeat latency when hold Right arrow
};
