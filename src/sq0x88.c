#include "stdafx.h"
#include "sq0x88.h"
#include "zobkey.h"
#include "0x88movegen.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

/*
 * Transform coordinate: 0x88 to algebraic.
 *
 * a will be modified. Exits on error if square sq is invalid.
 */
int coordAfrom0x88(S8 * sq, char * a)
{
        if (*sq == NOSQUARE) {
                *a = '-';
                a++;
                return -1;
        }

        *a = (char)(row8x8(*sq) + 'a');
        a++;
        *a = (char)(col8x8(*sq) + '1');
        a++;

        return 0;
}

/*
 * Transofrm coordinate: algebraic to 0x88.
 *
 * sq will be modified. Exits on error if a states no square.
 */
int coord0x88fromA(S8 * sq, char * a)
{
        if (*a == '-') {
                *sq = NOSQUARE;
                return -1;
        }

        *sq = 0;
        *sq = (S8)(a[0] - 'a');
        *sq += (a[1] - '1') * 16;

        return 0;
}

/*
 * Free Piece struct.
 *
 * This is passed as parameter to a LList initialization.
 */
void freePiece(void *piece)
{
        free((Piece *)piece);
}

/*
 * Free Move struct.
 *
 * This is passed as parameter to a IList initialization.
 */
void freeMove(void * move)
{
        free((Move *)move);
}

/*
 * Free a hash key.
 *
 * This is passed as parameter to a IList initialization.
 */
void freeKey(void * hash)
{
        free((U64 *)hash);
}

/*
 * Initialize vector attacks array.
 *
 * No error checks here.
 * The array is static and filled with fixed values.
 */
void vectors_init(Board * board)
{
        /* Zero the array */
        memset(board->attacks, 0, (257 * sizeof(U8)));

        /* Slider attacks */
        int i;
        for (i = -119; i <= 119; i++) {

                if (i % 16 == 0)                        /* Verticals */
                        board->attacks[i + 128] |= (PRECOMP_Q | PRECOMP_R);
                if (i > -8 && i < 8)                    /* Horizontals */
                        board->attacks[i + 128] |= (PRECOMP_Q | PRECOMP_R);
                if (i % 15 == 0 || i % 17 == 0)         /* Both diagonals */
                        board->attacks[i + 128] |= (PRECOMP_Q | PRECOMP_B);
        }

        /* Pawns & kings */
        board->attacks[N + 128]  |= PRECOMP_K;
        board->attacks[S + 128]  |= PRECOMP_K;
        board->attacks[E + 128]  |= PRECOMP_K;
        board->attacks[W + 128]  |= PRECOMP_K;
        board->attacks[NE + 128] |= (PRECOMP_K | PRECOMP_WP);
        board->attacks[NW + 128] |= (PRECOMP_K | PRECOMP_WP);
        board->attacks[SE + 128] |= (PRECOMP_K | PRECOMP_BP);
        board->attacks[SW + 128] |= (PRECOMP_K | PRECOMP_BP);

        /* Leaper attacks */
        board->attacks[LEAP_NNW + 128] |= PRECOMP_N;
        board->attacks[LEAP_NNE + 128] |= PRECOMP_N;
        board->attacks[LEAP_SSW + 128] |= PRECOMP_N;
        board->attacks[LEAP_SSE + 128] |= PRECOMP_N;
        board->attacks[LEAP_ENE + 128] |= PRECOMP_N;
        board->attacks[LEAP_ESE + 128] |= PRECOMP_N;
        board->attacks[LEAP_WNW + 128] |= PRECOMP_N;
        board->attacks[LEAP_WSW + 128] |= PRECOMP_N;

        /* delta = 0 is invalid */
        board->attacks[0 + 128] = 0;

        return;
}

/*
 * Initialize Piece struct list.
 *
 * No error checks here.
 * Only the current player's pieces are put into the list.
 */
void members_init(Board * board)
{
        /* Movegen functions array */
        void(*movegens[6])(Board *, const void *) = {
                redir_to_k,
                redir_to_q,
                redir_to_b,
                redir_to_n,
                redir_to_r,
                redir_to_p
        };

        int i;
        for (i = 0; i < 128; i++) {

                /* Ignore empty squares */
                if (board->colors[i] != board->turn)
                        continue;

                /* Allocate a new piece */
                Piece *newPiece = malloc(sizeof(*newPiece));

                newPiece->square = (S8)i;
                newPiece->color = board->colors[i];
                newPiece->piece = board->pieces[i];
                newPiece->generate_moves = movegens[board->pieces[i]];

                /* Insert in piece list */
                l_ins_next(&board->player_pieces, NULL, newPiece);
        }

        return;
}

/*
 * Initialize Eval Data struct.
 *
 * No error checks here.
 * So far, all the eval data is static.
 */
void evalues_init(Board *board)
{
        /* Set piece values */
        board->eval_data.val_piece[KING] = 0;
        board->eval_data.val_piece[QUEEN] = 1000;
        board->eval_data.val_piece[BISHOP] = 325;
        board->eval_data.val_piece[KNIGHT] = 325;
        board->eval_data.val_piece[ROOK] = 550;
        board->eval_data.val_piece[PAWN] = 100;

        /* Set capture values */
        board->eval_data.val_capts[KING] = 50000;
        board->eval_data.val_capts[QUEEN] = 1000;
        board->eval_data.val_capts[BISHOP] = 325;
        board->eval_data.val_capts[KNIGHT] = 325;
        board->eval_data.val_capts[ROOK] = 550;
        board->eval_data.val_capts[PAWN] = 100;

        /* Set referential data */
        board->eval_data.material_start = 2300;
        board->eval_data.material_end = 1300;
        board->eval_data.endgame = 0;
}

/*
 * Initialize Board struct.
 *
 * No error checks here, just zero the values.
 */
void sq0x88_board_init(Board * board)
{
        int i;
        for (i = 0; i < 128; i++) {
                board->pieces[i] = NOPIECE;
                board->colors[i] = NOCOLOR;
        }

        board->turn = WHITE;
        board->castle = NOCASTLE;
        board->ep_square = NOSQUARE;
        board->king_square[WHITE] = NOSQUARE;
        board->king_square[BLACK] = NOSQUARE;

        board->material[0] = 0;
        board->material[1] = 0;
        board->ply_count = 0;
        board->ply_total = 0;

        for (i = 0; i < 6; i++) {
                board->piece_count[WHITE][i] = 0;
                board->piece_count[BLACK][i] = 0;
        }

        board->zhash_key = 0;

        /* Initiate other structures */
        fl_init(&board->zhash_stack, freeKey);
        fl_init(&board->player_moves, freeMove);
        l_init(&board->player_pieces, freePiece);

        vectors_init(board);
        evalues_init(board);
        zobrist_init(&board->zob_container);

        return;
}

/*
 * Finalize Board struct.
 *
 * No error checks. Also finalize the lists.
 */
void sq0x88_board_drop(Board * board)
{
        fl_drop(&board->zhash_stack);
        fl_drop(&board->player_moves);
        l_drop(&board->player_pieces);

        memset(board, 0, sizeof(Board)); /* DO NOT Dyn-alloc this */

        return;
}

/*
 * Fill square with a piece.
 *
 * No error checks. All incremental data must be taken care of here.
 */
void sq0x88_square_fill(Board * board, S8 square, U8 piece, U8 color)
{
        board->pieces[square] = piece;
        board->colors[square] = color;

        if (piece == KING)
                board->king_square[color] = square;

        /* Unless a promotion happens, this must be succeeded by a decrement */
        board->piece_count[color][piece]++;

        board->material[color] += board->eval_data.val_piece[piece];

        board->zhash_key ^= board->zob_container.pcsq[piece][color][square];

        return;
}

/*
 * Clear piece from a square.
 *
 * No error checks. All incremental data must be taken care of here.
 */
void sq0x88_square_clear(Board * board, S8 square)
{
        U8 piece = board->pieces[square];
        U8 color = board->colors[square];

        board->pieces[square] = NOPIECE;
        board->colors[square] = NOCOLOR;

        /* Unless a capture happens, this must be preceded by an increment */
        board->piece_count[color][piece]--;

        board->material[color] -= board->eval_data.val_piece[piece];

        board->zhash_key ^= board->zob_container.pcsq[piece][color][square];

        return;
}

/*
 * Build the board from a FEN-compliant string.
 *
 * TODO exit on error if fen_line doesn't match a certain regex.
 */
int sq0x88_FEN_read(Board * board, const char * fen_line)
{
        char *string = (char *)fen_line;

        U8 row = 7; /* 0 to 7 is 8 */
        U8 col = 0; /* 0 to 7 is 8 */

        while (*string != ' ') {

                switch (*string) {

                        case 'K': sq0x88_square_fill(board, (S8)sq0x88(row, col), KING, WHITE); col++; break;
                        case 'Q': sq0x88_square_fill(board, (S8)sq0x88(row, col), QUEEN, WHITE); col++; break;
                        case 'B': sq0x88_square_fill(board, (S8)sq0x88(row, col), BISHOP, WHITE); col++; break;
                        case 'N': sq0x88_square_fill(board, (S8)sq0x88(row, col), KNIGHT, WHITE); col++; break;
                        case 'R': sq0x88_square_fill(board, (S8)sq0x88(row, col), ROOK, WHITE); col++; break;
                        case 'P': sq0x88_square_fill(board, (S8)sq0x88(row, col), PAWN, WHITE); col++; break;
                        case 'k': sq0x88_square_fill(board, (S8)sq0x88(row, col), KING, BLACK); col++; break;
                        case 'q': sq0x88_square_fill(board, (S8)sq0x88(row, col), QUEEN, BLACK); col++; break;
                        case 'b': sq0x88_square_fill(board, (S8)sq0x88(row, col), BISHOP, BLACK); col++; break;
                        case 'n': sq0x88_square_fill(board, (S8)sq0x88(row, col), KNIGHT, BLACK); col++; break;
                        case 'r': sq0x88_square_fill(board, (S8)sq0x88(row, col), ROOK, BLACK); col++; break;
                        case 'p': sq0x88_square_fill(board, (S8)sq0x88(row, col), PAWN, BLACK); col++; break;
                        case '/': col = 0; row--; break;
                        default: col += *string - '0'; break;

                }
                string++;
        }
        string++;

        if (*string == 'w') {
                board->turn = WHITE;
        } else {
                board->turn = BLACK;
                board->zhash_key ^= board->zob_container.color;
        }
        string += 2;

        while (*string != ' ') {

                switch (*string) {

                        case 'K': board->castle |= CASTLE_WK; break;
                        case 'Q': board->castle |= CASTLE_WQ; break;
                        case 'k': board->castle |= CASTLE_BK; break;
                        case 'q': board->castle |= CASTLE_BQ; break;
                        default: break;

                }
                string++;
        }
        string++;

        board->zhash_key ^= board->zob_container.cs[board->castle];

        if (!coord0x88fromA(&board->ep_square, string))
                board->zhash_key ^= board->zob_container.ep[board->ep_square];

        while (*string != ' ')
                string++;

        /* Scan the rest directly with sscanf */
        sscanf(string, " %d %d", &board->ply_count, &board->ply_total);

        /* Initiate player's pieces */
        members_init(board);

        return 0;
}

/*
 * Dump current board state to a FEN-compliant string.
 *
 * TODO change the return type.
 * The board never holds an impossible state, so int is unecessary.
 */
int sq0x88_FEN_write(Board * board, char ** fen_line)
{
        char *string = *fen_line;

        memset(string, ' ', 1024);

        S8  row = 7; /* 0 to 7 is 8 */
        S8  col = 0; /* 0 to 7 is 8 */

        int blanks = 0;

        /* while: inside vertical bounds */
        while (!(sq0x88(row, col) & 0x80)) {

                S8 sq = (S8)sq0x88(row, col);
                col++;

                if (!is0x88(sq)) {

                        if (row > ROW_1) {
                                *string = '/';
                                string++;
                        }
                        row--;
                        col = 0;
                        continue;

                } else if (board->colors[sq] == WHITE) {

                        switch (board->pieces[sq]) {

                                case KING: *string = 'K'; string++; break;
                                case QUEEN: *string = 'Q'; string++; break;
                                case BISHOP: *string = 'B'; string++; break;
                                case KNIGHT: *string = 'N'; string++; break;
                                case ROOK: *string = 'R'; string++; break;
                                case PAWN: *string = 'P'; string++; break;
                                default: break;

                        }

                } else if (board->colors[sq] == BLACK) {

                        switch (board->pieces[sq]) {

                                case KING: *string = 'k'; string++; break;
                                case QUEEN: *string = 'q'; string++; break;
                                case BISHOP: *string = 'b'; string++; break;
                                case KNIGHT: *string = 'n'; string++; break;
                                case ROOK: *string = 'r'; string++; break;
                                case PAWN: *string = 'p'; string++; break;
                                default: break;

                        }

                } else {
                        blanks++;

                        if (col == COL_H || board->colors[sq + 1] != NOCOLOR) {

                                *string = (char)(blanks + '0');
                                blanks = 0;
                                string++;
                        }
                }
        }
        string++;

        if (board->turn == WHITE)
                *string = 'w';
        else
                *string = 'b';
        string += 2;

        if (board->castle == NOCASTLE) {
                *string = '-';
                string++;
        } else {
                if (board->castle & CASTLE_WK) {
                        *string = 'K';
                        string++;
                }
                if (board->castle & CASTLE_WQ) {
                        *string = 'Q';
                        string++;
                }
                if (board->castle & CASTLE_BK) {
                        *string = 'k';
                        string++;
                }
                if (board->castle & CASTLE_BQ) {
                        *string = 'q';
                        string++;
                }
        }
        string++;

        coordAfrom0x88(&board->ep_square, string);
        string++;

        /* Write the rest of the state to string with sprintf */
        sprintf(string, " %d %d", board->ply_count, board->ply_total);

        return 0;
}
