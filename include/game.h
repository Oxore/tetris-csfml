struct field;
struct idlist;
struct slist;
struct events_array;
typedef void media_window_t;

enum game_state {
    GS_MAIN_MENU,
    GS_STARTED,
    GS_GAME_OVER,
    GS_GAME_OVER_WAIT,
    GS_HIGHSCORES_INPUT,
    GS_HIGHSCORES_TABLE,
    GS_PAUSED
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

    media_window_t *window;

    struct config *config;

    struct field *fld;
    struct field *nxt;
    struct slist *texts;

    struct input input_name;
    struct hs_table highscores;
};

void transition_init(struct game *game);
void main_loop(struct game *game, const struct events_array *events);
