struct window {
    sfVideoMode mode;
    sfRenderWindow *window;
    sfEvent event;
};

struct game {
    int isStarted;
    int scoreCurrent;
    int level;
    int moveLatency;
    int lines;
};
