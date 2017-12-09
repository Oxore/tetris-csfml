#define windowName_conf "Tetris CSFML" // Window name
#define UIBGCOLOR sfColor_fromRGB(26, 26, 26) // Field background
#define UIFGINACTIVECOLOR sfColor_fromRGB(55, 59, 65) // Field inactive cell outline
#define UIFGACTIVECOLOR sfColor_fromRGB(40, 42, 46) // Field active cell outline
#define LCOLOR sfColor_fromRGB(222, 147, 95) // Orange color of L shape
#define RLCOLOR sfColor_fromRGB(95, 129, 157)
#define SCOLOR sfColor_fromRGB(140, 148, 64)
#define ZCOLOR sfColor_fromRGB(165, 66, 66)
#define BCOLOR sfColor_fromRGB(197, 200, 198)
#define ICOLOR sfColor_fromRGB(94, 141, 135)
#define TCOLOR sfColor_fromRGB(133, 103, 143)
#define moveRepeatLatency1 150000 // microseconds, only for left-right arrows,
                  // first repeat move when long push
#define moveRepeatLatency2  30000 // microseconds, for Left, Right and Down
                  // arrows, the rest repeat move when long push
#define basicLatency 500000
#define L00LATENCY 800000
#define L01LATENCY 716667
#define L02LATENCY 633334
#define L03LATENCY 550000
#define L04LATENCY 466667
#define L05LATENCY 383334
#define L06LATENCY 300000
#define L07LATENCY 216667
#define L08LATENCY 133334
#define L09LATENCY 100000
#define L10LATENCY  83334
#define L13LATENCY  66667
#define L16LATENCY  50000
#define L19LATENCY  33334
#define L29LATENCY  16667
#define PUT_LATENCY 300000
#define RM_1LINES_SCORE 40
#define RM_2LINES_SCORE 100
#define RM_3LINES_SCORE 300
#define RM_4LINES_SCORE 1200
#define LEVELUP_LINES 10


/*
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

