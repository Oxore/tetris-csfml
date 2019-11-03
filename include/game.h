struct field;
struct idlist;

enum game_state {
    GS_MAIN_MENU,
    GS_STARTED,
    GS_GAME_OVER,
    GS_GAME_OVER_WAIT,
    GS_HIGHSCORES_INPUT,
    GS_HIGHSCORES_TABLE,
    GS_PAUSED
};

// Bits in bitmap of keys
#define ROTRIGHT  (1 << 0)
#define ROTLEFT   (1 << 1)
#define RIGHT     (1 << 2)
#define LEFT      (1 << 3)
#define DOWN      (1 << 4)
#define RIGHTHOLD (1 << 5)
#define HARDDROP  (1 << 6)
#define PAUSE     (1 << 7)
#define LEFTHOLD  (1 << 8)
#define GAMEOVER  (1 << 9)

struct controls {
    uint32_t keys;         // bitmap of keys pressed
    sfClock *down_repeat_clock;  // repeat latency when hold Down arrow
    sfClock *left_repeat_clock;   // repeat latency when hold Left arrow
    sfClock *right_repeat_clock;  // repeat latency when hold Right arrow
};

struct game {
    enum game_state state;

    size_t      level;
    unsigned    score;
    int         tick_period;
    size_t      rows;
    sfClock    *game_clock;
    sfClock    *game_over_wait_clock;
    sfClock    *put_clock;
    sfClock    *menu_clock;

    struct controls controls;

    struct config *config;

    struct field *fld;
    struct field *nxt;
    struct idlist *texts;

    struct input input_name;
    struct hs_table highscores;
};

void transition_init(struct game *game);
void main_loop(struct game *game, const struct idlist *events);
