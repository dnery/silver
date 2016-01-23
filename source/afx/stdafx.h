/*==== Chess engine standard framework extension (AFX) =======================*/

#ifndef __STD0X88_H__
#define __STD0X88_H__

#include <stdint.h>
#include "llist.h"
#include "ilist.h"

/* GCC unsigned integer defs */
#define U32 uint32_t
#define U64 uint64_t
#define U8  uint8_t

/* GCC signed integer defs */
#define S32 int32_t
#define S64 int64_t
#define S8  int8_t

/* Color enum (Used as index) */
enum E_Color { WHITE, BLACK, NOCOLOR };

/* Piece enum (Used as index) */
enum E_Piece { KING, QUEEN, BISHOP, KNIGHT, ROOK, PAWN, NOPIECE };

/* Movement bits */
enum E_MFlags {

	MF_NOMOVE     = 0,
	MF_NORMAL     = 1,
	MF_CASTLE     = 2,
	MF_CAPTURE    = 4,
	MF_EPCAPTURE  = 8,
	MF_PROMOTION  = 16

};

/* Castling bits */
enum E_CFlags {

	NOCASTLE  = 0,
	CASTLE_WK = 1,
	CASTLE_WQ = 2,
	CASTLE_BK = 4,
	CASTLE_BQ = 8

};

/* End-game bits */
enum E_EGFlags {

	EG_CONINUE      = 0,
	EG_THREEREP     = 1,
	EG_FIFTYMOVE    = 2,
	EG_NOMATERIAL   = 4,
	EG_NOLEGALMOVES = 8,
	EG_WIN_WHITE    = 16,
	EG_WIN_BLACK    = 32

};

/* Attack vector bits*/
enum E_PAFlags {

	NOATTACK   = 0,
	PRECOMP_K  = 1,
	PRECOMP_Q  = 2,
	PRECOMP_B  = 4,
	PRECOMP_N  = 8,
	PRECOMP_R  = 16,
	PRECOMP_WP = 32,
	PRECOMP_BP = 64,

};

/* Board squares */
enum E_BoardSquares {

	A1 = 0,   B1, C1, D1, E1, F1, G1, H1,
	A2 = 16,  B2, C2, D2, E2, F2, G2, H2,
	A3 = 32,  B3, C3, D3, E3, F3, G3, H3,
	A4 = 48,  B4, C4, D4, E4, F4, G4, H4,
	A5 = 64,  B5, C5, D5, E5, F5, G5, H5,
	A6 = 80,  B6, C6, D6, E6, F6, G6, H6,
	A7 = 96,  B7, C7, D7, E7, F7, G7, H7,
	A8 = 112, B8, C8, D8, E8, F8, G8, H8,
	NOSQUARE = -1

};

/* "Eval Data" structure */
typedef struct S_EvalData {

	U32	val_piece[6];
	U32	val_capts[6];

	U32	material_start;
	U32	material_end;
	U8	endgame;

} EvalData;

/* "Zob Key" structure */
typedef struct S_Zobrist {

	U64 pcsq[6][2][128];
	U64 ep[128];
	U64 cs[16];
	U64 color;

} Zobrist;

/* "Board" structure */
typedef struct S_Board {

	U8		turn;              /* Current player */
	U8		castle;            /* Castling rights */
	U8		pieces[128];       /* Pieces array */
	U8		colors[128];       /* Colors array */
	U8		attacks[257];      /* Attack vectors */

	S8		ep_square;         /* En passant square (if any) */
	S8		king_square[2];    /* King locations (B and W) */

	int		material[2];       /* Material count (B and W) */
	int		ply_count;         /* Ply count (50 move) */
	int		ply_total;         /* Plys since beginning */
	int		piece_count[2][6]; /* Piece count (B and W) */

	U64		zhash_key;         /* Unique position hash key */

	IList		zhash_stack;       /* Position hash key stack */
	IList		player_moves;      /* Current player's legal moves */
	LList		player_pieces;     /* Current player's pieces */

	Zobrist		zob_container;     /* Zobrist container struct */

	EvalData	eval_data;         /* Eval data container struct */

} Board;

/* "Move" structure */
typedef struct S_Move {

	U8	flags;                  /* Definition flags */
	U8	castle;                 /* Castling rights */
	U8	piece_fr;               /* From: piece (making the move) */
	U8	piece_to;               /* To: piece (in case of attack) */

	S8	square_fr;              /* From: square */
	S8	square_to;              /* To: square */
	S8	ep_square;              /* EP square before the play, if any */

	int	ply_count;              /* Ply count (50 move) */
	int	ply_total;              /* Ply since beginning */
	int	score;                  /* Move scoring (used for ordering) */

} Move;

/* "Piece" structure */
typedef struct S_Piece {

	U8 piece;                                     /* Corresponding piece */
	U8 color;                                     /* Corresponding color */

	S8 square;                                    /* Location on board */

	void(*generate_moves)(Board *, const void *); /* Movegen function */

} Piece;

#endif // !__STD0X88_H__
