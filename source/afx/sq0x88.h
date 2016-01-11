/*==== Public interface for 0x88 square board API ============================*/

#ifndef __SQ0X88_H__
#define __SQ0X88_H__

#include "stdafx.h"

/* Row macros */
#define ROW_1 (A1 >> 4)
#define ROW_2 (A2 >> 4)
#define ROW_3 (A3 >> 4)
#define ROW_4 (A4 >> 4)
#define ROW_5 (A5 >> 4)
#define ROW_6 (A6 >> 4)
#define ROW_7 (A7 >> 4)
#define ROW_8 (A8 >> 4)

/* Col macros */
#define COL_A (A1 & 7)
#define COL_B (B1 & 7)
#define COL_C (C1 & 7)
#define COL_D (D1 & 7)
#define COL_E (E1 & 7)
#define COL_F (F1 & 7)
#define COL_G (G1 & 7)
#define COL_H (H1 & 7)

/* Movement vectors */
#define N   16
#define NN  (N + N)
#define S  -16
#define SS  (S + S)
#define E   1
#define EE  (E + E)
#define W  -1
#define WW  (W + W)
#define NE  17
#define SW -17
#define NW  15
#define SE -15

/* Leaper movements */
#define LEAP_NNE (NN + E)
#define LEAP_NNW (NN + W)
#define LEAP_SSE (SS + E)
#define LEAP_SSW (SS + W)
#define LEAP_ENE (EE + N)
#define LEAP_ESE (EE + S)
#define LEAP_WNW (WW + N)
#define LEAP_WSW (WW + S)

/* Row & col macros */
#define col8x8(index)        ((index) & 7)
#define row8x8(index)        ((index) >> 4)

/* Square index macros */
#define sq8x8(row, col)      ((row) * 8 + (col))
#define sq0x88(row, col)     ((row) * 16 + (col))

/* 0x88 square check macro*/
#define is0x88(index)        (((index) & 0x88) ? 0 : 1)

/* Index conversion macros */
#define sq0x88from8x8(index) ((index) + ((index) & ~7))
#define sq8x8from0x88(index) (((index) + ((index) & 7)) >> 1)

/* Square index relation macros */
#define sqRelV(sq1, sq2)     ((col0x88(sq1) == col0x88(sq2)) ? 1 : 0)
#define sqRelH(sq1, sq2)     ((row0x88(sq1) == row0x88(sq2)) ? 1 : 0)

/* Total material on board */
#define totalMat(board)      ((board)->material[0] + (board)->material[1])

/* Utils operation prototypes */
int  coordAfrom0x88(S8 * sq, char * a);
int  coord0x88fromA(S8 * sq, char * a);

/* Engager prototypes */
void vectors_init(Board *board);
void members_init(Board *board);
void evalues_init(Board *board);

/* Board operation prototypes */
void sq0x88_board_init(Board *board);
void sq0x88_board_drop(Board *board);

/* Square operation prototypes */
void sq0x88_square_fill(Board *board, S8 square, U8 piece, U8 color);
void sq0x88_square_clear(Board *board, S8 square);

/* FEN string operation prototypes */
int  sq0x88_FEN_read(Board *board, const char *fen_line);
int  sq0x88_FEN_write(Board *board, char **fen_line);

#endif // !__SQ0X88_H__
