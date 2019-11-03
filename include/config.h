#define CFG_WIN_NAME            "Tetris CSFML" // Window name

// Ouline thickness of cells
#define CFG_OUT_THICK           1
// Field transparent background color
#define CFG_UI_TRANSPARENT      {0,   0,   0,   0  }
// Field background color
#define CFG_UI_BG_COLOR         {26,  26,  26,  255}
// Field inactive cell outline color
#define CFG_UI_FGINACTIVE_COLOR {55,  59,  65,  255}
// Field active cell outline color
#define CFG_UI_FGACTIVE_COLOR   {40,  42,  46,  255}

// Colors for shapes
#define CFG_SHAPE_L_COLOR       {222, 147, 95,  255}
#define CFG_SHAPE_RL_COLOR      {95,  129, 157, 255}
#define CFG_SHAPE_Z_COLOR       {165, 66,  66,  255}
#define CFG_SHAPE_S_COLOR       {140, 148, 64,  255}
#define CFG_SHAPE_B_COLOR       {197, 200, 198, 255}
#define CFG_SHAPE_I_COLOR       {94,  141, 135, 255}
#define CFG_SHAPE_T_COLOR       {133, 103, 143, 255}

// All latencies and clock periods are set in microseconds

// Time period used to unconditionally show you the "Game over" text
#define CFG_GAMEOVER_SHOWTIME_PERIOD    700000
// Latency between pushing left/right key and starting fast repeat
#define CFG_PREREPEAT_LATENCY           150000
// Latency for repeating action of left/right/down key
#define CFG_REPEAT_LATENCY              30000

// The clock period for menu animation
#define CFG_MENU_CLOCK_PERIOD    500000
// The minimum time between the shape has reached the floor and has merged, when
// player still can rotate and move it as he/she wish
#define CFG_PUT_CLOCK_PERIOD     300000

// The time used by shape for stepping downwards.
#define CFG_L00_CLOCK_PERIOD     800000
#define CFG_L01_CLOCK_PERIOD     716667
#define CFG_L02_CLOCK_PERIOD     633334
#define CFG_L03_CLOCK_PERIOD     550000
#define CFG_L04_CLOCK_PERIOD     466667
#define CFG_L05_CLOCK_PERIOD     383334
#define CFG_L06_CLOCK_PERIOD     300000
#define CFG_L07_CLOCK_PERIOD     216667
#define CFG_L08_CLOCK_PERIOD     133334
#define CFG_L09_CLOCK_PERIOD     100000
#define CFG_L10_CLOCK_PERIOD     83334
#define CFG_L11_CLOCK_PERIOD     CFG_L10_CLOCK_PERIOD
#define CFG_L12_CLOCK_PERIOD     CFG_L10_CLOCK_PERIOD
#define CFG_L13_CLOCK_PERIOD     66667
#define CFG_L14_CLOCK_PERIOD     CFG_L13_CLOCK_PERIOD
#define CFG_L15_CLOCK_PERIOD     CFG_L13_CLOCK_PERIOD
#define CFG_L16_CLOCK_PERIOD     50000
#define CFG_L17_CLOCK_PERIOD     CFG_L16_CLOCK_PERIOD
#define CFG_L18_CLOCK_PERIOD     CFG_L16_CLOCK_PERIOD
#define CFG_L19_CLOCK_PERIOD     33334
#define CFG_L20_CLOCK_PERIOD     CFG_L19_CLOCK_PERIOD
#define CFG_L21_CLOCK_PERIOD     CFG_L19_CLOCK_PERIOD
#define CFG_L22_CLOCK_PERIOD     CFG_L19_CLOCK_PERIOD
#define CFG_L23_CLOCK_PERIOD     CFG_L19_CLOCK_PERIOD
#define CFG_L24_CLOCK_PERIOD     CFG_L19_CLOCK_PERIOD
#define CFG_L25_CLOCK_PERIOD     CFG_L19_CLOCK_PERIOD
#define CFG_L26_CLOCK_PERIOD     CFG_L19_CLOCK_PERIOD
#define CFG_L27_CLOCK_PERIOD     CFG_L19_CLOCK_PERIOD
#define CFG_L28_CLOCK_PERIOD     CFG_L19_CLOCK_PERIOD
#define CFG_L29_CLOCK_PERIOD     16667

// Reward for 1, 2, 3 or 4 rows removing simultaneously
#define CFG_REWARD_FOR_1_ROW     40
#define CFG_REWARD_FOR_2_ROW     100
#define CFG_REWARD_FOR_3_ROW     300
#define CFG_REWARD_FOR_4_ROW     1200
// Number of rows required for remove to level up
#define CFG_ROWS_FOR_LEVELUP    10

// The main field size, in cells
#define CFG_FLD_SIZE_Y  22
#define CFG_FLD_SIZE_X  10
#define CFG_FLD_BOUND_Y (CFG_FLD_SIZE_Y + 4)
#define CFG_FLD_BOUND_X CFG_FLD_SIZE_X
// The main field position, in pixels
#define CFG_FLD_POS_X   10
#define CFG_FLD_POS_Y   (10 + 550 - 24)
#define CFG_FLD_POS     (struct vector2i){\
                            .x = CFG_FLD_POS_X,\
                            .y = CFG_FLD_POS_Y,}

/*
 * Field configuration:
 *
 * 22..........
 * 21..........
 * 20..........
 * 19..........
 * 18..........
 * 17..........
 * 16..........
 * 15..........
 * 14..........
 * 13..........
 * 12..........
 * 11..........
 * 10..........
 *  9..........
 *  8..........
 *  7..........
 *  6..........
 *  5.......#..
 *  4......##..
 *  3......#...
 *  2.#.##.....
 *  1.##.#..#..
 *  0..#.#.###.
 *   0123456789
 *
 */

// Any field cell dimensions, in pixels
#define CFG_CELL_SIZE_X 23
#define CFG_CELL_SIZE_Y 23
#define CFG_CELL_SIZE   (sfVector2f){\
                            .x = CFG_CELL_SIZE_X,\
                            .y = CFG_CELL_SIZE_Y,}

// "Next shape" field position, in pixels
#define CFG_NXT_POS_X   (250 + 10 + 30)
#define CFG_NXT_POS_Y   300
#define CFG_NXT_POS     (struct vector2i){\
                            .x = CFG_NXT_POS_X,\
                            .y = CFG_NXT_POS_Y,}
// "Next shape" field size, in cells
#define CFG_NXT_SIZE_Y  8
#define CFG_NXT_SIZE_X  4
#define CFG_NXT_SIZE    (struct vector2ui){\
                            .x = CFG_NXT_SIZE_X,\
                            .y = CFG_NXT_SIZE_Y,}

// Maximum bytes for player name
#define CFG_NAME_MAX            40
// Number of entries in highscores table
#define CFG_HS_TABLE_SIZE       15
// Name of file containing configuration parameters
#define CFG_CONFIG_FNAME        "config.json"
// Name of file containing highscores data
#define CFG_HIGHSCORES_FNAME    "dat/highscores.json"
// Name of file containing game texts
#define CFG_TEXTS_FNAME         "dat/texts.json"


_Static_assert(CFG_HS_TABLE_SIZE > 0, "CFG_HS_TABLE_SIZE must be > 0");
_Static_assert(CFG_NAME_MAX > 0, "CFG_NAME must be > 0");


struct config_keys {
    int rotate_left;
    int rotate_right;
    int move_left;
    int move_right;
    int move_down;
    int drop;
    int pause;
    int start;
};

struct config {
    struct config_keys keys;
};

extern struct config g_config;

int config_load_from_json_file(struct config *config, const char *fname);
