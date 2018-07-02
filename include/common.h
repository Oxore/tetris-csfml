struct game {
    int      isStarted;
    int      scoreCurrent;
    int      level;
    int      moveLatency;
    int      lines;
    sfClock *gameTick;
    sfClock *putTick;
    sfClock *mTick;
    sfClock *repPushDown;    // Clock for repeat latency when Down arrow long push
    sfClock *repKeyLeft;    // Clock for repeat latency when Left arrow long push
    sfClock *repKeyRight;    // Clock for repeat latency when Left arrow long push
};
