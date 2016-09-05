#ifndef  _0X88MOVEGEN_INC
#define  _0X88MOVEGEN_INC

#include "stdafx.h"

/* Gateway/Manager function */
void generate_moves(Board *board);

/* Redirects to proper generators */
void redir_to_k(Board *board, const void *self);
void redir_to_q(Board *board, const void *self);
void redir_to_b(Board *board, const void *self);
void redir_to_n(Board *board, const void *self);
void redir_to_r(Board *board, const void *self);
void redir_to_p(Board *board, const void *self);

/* Generate moves for slider and non-slider pieces */
void generate_slider_moves(Board *board, Piece *piece, int *vectors);
void generate_nonsli_moves(Board *board, Piece *piece, int *vectors);

/* Panws: generate moves, captures and promotions */
void generate_pawn_moves(Board *board, Piece *piece);
void generate_pawn_capts(Board *board, Piece *piece);
void generate_promotions(Board *board, Piece *piece);

/* Threat verification functions */
int exposed(Board *board, S8 square, U8 player);
int diagonal_exposed(Board *board, S8 target, S8 origin); 	/* Helper */
int straight_exposed(Board *board, S8 target, S8 origin);	/* Helper */
int ray_clear(Board *board, S8 target, S8 origin, int vector);  /* Helper */

/* Allocate move, solve pendencies and check for legality */
void assert_move(Board *board, S8 from, S8 to, U8 pc_from, U8 pc_to, U8 flags);

/* Current line: Apply/Clear move */
void apply_move(Board *board, Move *move);
void clear_move(Board *board, Move *move);

#endif   /* ----- #ifndef 0X88MOVEGEN_INC  ----- */
