/*==== Public interface for 0x88 board move generation API ===================*/

#ifndef __0X88MOVEGEN_H__
#define __0X88MOVEGEN_H__

#include "stdafx.h"

/* Movegenerator: generic move allocator */
void movegenType_any(Board *board, S8 sq_fr,
                                   S8 sq_to,
                                   S8 pc_fr,
                                   S8 pc_to,
                                   U8 flags);

/* Movegenerators: single move and slider pieces */
void movegenType_single(Board *board, Piece *piece, int *delta);
void movegenType_slider(Board *board, Piece *piece, int *delta);

/* Movegenerators: pawns only moves and captures */
void movegenType_pawn_moves(Board *board, Piece *piece);
void movegenType_pawn_capts(Board *board, Piece *piece);

/* Function redirs for movegenerators */
void movegen_k(Board * board, const void * self);
void movegen_q(Board *board, const void *self);
void movegen_b(Board *board, const void *self);
void movegen_n(Board *board, const void *self);
void movegen_r(Board *board, const void *self);
void movegen_p(Board *board, const void *self);

/* Main functions */
void movegenL(Board *board);        /* Generate all moves and keep legal ones */
void move_make(Board *board, Move *move);   /* Apply move to the current line */
void move_umake(Board *board, Move *move);    /* Clear move from current line */

/* Problem solvers */
int sqExposed(Board *board, S8 square, U8 player);
int rgExposed(Board *board, S8 target, S8 origin, int vector);

#endif // !__8X88MOVEGEN_H__