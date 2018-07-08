#define windowName_conf   "Tetris CSFML" // Window name
#define OUT_THICK         1
#define UITRANSPARENT     {0,   0,   0,   0  } // Field background
#define UIBGCOLOR         {26,  26,  26,  255} // Field background
#define UIFGINACTIVECOLOR {55,  59,  65,  255} // Field inactive cell outline
#define UIFGACTIVECOLOR   {40,  42,  46,  255} // Field active cell outline
#define LCOLOR            {222, 147, 95,  255} // Orange color of L shape
#define RLCOLOR           {95,  129, 157, 255}
#define ZCOLOR            {165, 66,  66,  255}
#define SCOLOR            {140, 148, 64,  255}
#define BCOLOR            {197, 200, 198, 255}
#define ICOLOR            {94,  141, 135, 255}
#define TCOLOR            {133, 103, 143, 255}

#define moveRepeatLatency1 150000 // microseconds, only for left-right arrows,
                                  // first repeat move when long push
#define moveRepeatLatency2  30000 // microseconds, for Left, Right and Down
                                  // arrows, the rest repeat move when long push
#define basicLatency      500000
#define L00LATENCY        800000
#define L01LATENCY        716667
#define L02LATENCY        633334
#define L03LATENCY        550000
#define L04LATENCY        466667
#define L05LATENCY        383334
#define L06LATENCY        300000
#define L07LATENCY        216667
#define L08LATENCY        133334
#define L09LATENCY        100000
#define L10LATENCY         83334
#define L11LATENCY    L10LATENCY
#define L12LATENCY    L10LATENCY
#define L13LATENCY         66667
#define L14LATENCY    L13LATENCY
#define L15LATENCY    L13LATENCY
#define L16LATENCY         50000
#define L17LATENCY    L16LATENCY
#define L18LATENCY    L16LATENCY
#define L19LATENCY         33334
#define L20LATENCY    L19LATENCY
#define L21LATENCY    L19LATENCY
#define L22LATENCY    L19LATENCY
#define L23LATENCY    L19LATENCY
#define L24LATENCY    L19LATENCY
#define L25LATENCY    L19LATENCY
#define L26LATENCY    L19LATENCY
#define L27LATENCY    L19LATENCY
#define L28LATENCY    L19LATENCY
#define L29LATENCY         16667
#define PUT_LATENCY       300000
#define RM_1LINES_SCORE 40
#define RM_2LINES_SCORE 100
#define RM_3LINES_SCORE 300
#define RM_4LINES_SCORE 1200
#define LEVELUP_LINES 10

#define FLD_SIZE_Y  22
#define FLD_SIZE_X  10
#define FLD_BOUND_Y FLD_SIZE_Y + 4
#define FLD_BOUND_X FLD_SIZE_X
#define FLD_POS     (struct vector2i){.x = 10, .y = 10+550-24}
#define CELL_SIZE   (sfVector2f){.x = 23, .y = 23}

#define NXT_SIZE_Y  8
#define NXT_SIZE_X  4
#define NXT_POS     (struct vector2i){.x = 250 + 10 + 30, .y = 300}
#define NXT_SIZE    (struct vector2ui){.x = NXT_SIZE_X, .y = NXT_SIZE_Y}

/*
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
 *  L
 *  ..#. .#.. .... ##..
 *  ###. .#.. ###. .#..
 *  .... .##. #... .#..
 *  .... .... .... ....
 *
 *  RL
 *  #... .##. .... .#..
 *  ###. .#.. ###. .#..
 *  .... .#.. ..#. ##..
 *  .... .... .... ....
 *
 *  Z
 *  ##.. ..#. .... .#..
 *  .##. .##. ##.. ##..
 *  .... .#.. .##. #...
 *  .... .... .... ....
 *
 *  S
 *  .##. .#.. .... #...
 *  ##.. .##. .##. ##..
 *  .... ..#. ##.. .#..
 *  .... .... .... ....
 *
 *  Brick
 *  ##.. ##.. ##.. ##..
 *  ##.. ##.. ##.. ##..
 *  .... .... .... ....
 *  .... .... .... ....
 *
 *  I
 *  .#.. .... .#.. ....
 *  .#.. #### .#.. ####
 *  .#.. .... .#.. ....
 *  .#.. .... .#.. ....
 *
 *  T
 *  .#.. .#.. .... .#..
 *  ###. .##. ###. ##..
 *  .... .#.. .#.. .#..
 *  .... .... .... ....
 */

