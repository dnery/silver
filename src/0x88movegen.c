#include "stdafx.h"
#include "sq0x88.h"
#include "0x88movegen.h"

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

/*
 * Gateway move generation function.
 *
 * No error checks. This will generate all possible castling moves.
 * It will also generate all possible moves for the current player pieces.
 */
void generate_moves(Board * board)
{
        if (board->turn == WHITE) {

                if (board->castle & CASTLE_WK) {
                        if (board->pieces[F1] == NOPIECE &&
                                        board->pieces[G1] == NOPIECE &&
                                        !exposed(board, E1, board->turn) &&
                                        !exposed(board, F1, board->turn) &&
                                        !exposed(board, G1, board->turn))
                                assert_move(board, E1, G1, KING, NOPIECE,
                                                MF_CASTLE);
                }
                if (board->castle & CASTLE_WQ) {
                        if (board->pieces[B1] == NOPIECE &&
                                        board->pieces[C1] == NOPIECE &&
                                        board->pieces[D1] == NOPIECE &&
                                        !exposed(board, E1, board->turn) &&
                                        !exposed(board, D1, board->turn) &&
                                        !exposed(board, C1, board->turn))
                                assert_move(board, E1, C1, KING, NOPIECE,
                                                MF_CASTLE);
                }
        } else {

                if (board->castle & CASTLE_BK) {
                        if (board->pieces[F8] == NOPIECE &&
                                        board->pieces[G8] == NOPIECE &&
                                        !exposed(board, E8, board->turn) &&
                                        !exposed(board, F8, board->turn) &&
                                        !exposed(board, G8, board->turn))
                                assert_move(board, E8, G8, KING, NOPIECE,
                                                MF_CASTLE);
                }
                if (board->castle & CASTLE_BQ) {
                        if (board->pieces[B8] == NOPIECE &&
                                        board->pieces[C8] == NOPIECE &&
                                        board->pieces[D8] == NOPIECE &&
                                        !exposed(board, E8, board->turn) &&
                                        !exposed(board, D8, board->turn) &&
                                        !exposed(board, C8, board->turn))
                                assert_move(board, E8, C8, KING, NOPIECE,
                                                MF_CASTLE);
                }
        }

        LNode *node = l_head(&board->player_pieces);
        while (node != NULL) {

                ((Piece *)l_data(node))->generate_moves(board, l_data(node));
                node = l_next(node);
        }

        return;
}

/*
 * Do remaining checks and store move only if legal.
 *
 * No error checks here. Allocate memory for move, make move on board;
 * if not in-check, keep it, otherwise discard it.
 */
void assert_move(Board *board, S8 from, S8 to, U8 pc_from, U8 pc_to, U8 flags)
{
        Move *move = malloc(sizeof(*move));

        move->piece_fr = pc_from;
        move->piece_to = pc_to;
        move->square_fr = from;
        move->square_to = to;
        move->flags = flags;

        move->castle = board->castle;
        move->ep_square = board->ep_square;
        move->ply_count = board->ply_count;
        move->ply_total = board->ply_total;

        /* Build score */
        move->score = pc_from;

        if (flags & MF_PROMOTION)
                move->score += 9000 + board->eval_data.val_piece[pc_from];

        if (board->pieces[to] != NOPIECE)
                move->score += 10000 + board->eval_data.val_capts[pc_to];

        if (pc_from == PAWN && to == board->ep_square) {
                move->score += 10000 + board->eval_data.val_capts[PAWN];
                move->flags |= MF_EPCAPTURE;
        }

        /* Make move, check, unmake */
        apply_move(board, move);

        int check = exposed(board, board->king_square[!board->turn],
                        !board->turn);

        clear_move(board, move);

        /* Store only if move is legal */
        if (!check)
                fl_insert(&board->player_moves, move);
        else
                free(move);

        return;
}

/*
 * Generate non-slider moves and captures.
 *
 * All vectors are checked until the always-possible '0' is found.
 */
void generate_nonsli_moves(Board *board, Piece *piece, int *vectors)
{
        for (; *vectors; vectors++) {

                S8 target = (S8)(piece->square + *vectors);
                if (!is0x88(target) || board->colors[target] == board->turn)
                        continue;

                if (board->pieces[target] == NOPIECE)		/* Regular */
                        assert_move(board, piece->square, target, piece->piece,
                                        board->pieces[target], MF_NORMAL);
                else 						/* Capture */
                        assert_move(board, piece->square, target, piece->piece,
                                        board->pieces[target], MF_CAPTURE);
        }

        return;
}

/*
 * Generate slider moves and captures.
 *
 * No error checks here, same logic as 'generate_nonsli_moves'.
 * The only difference is that here, 'vectors' is in fact an array of
 * vector increments, thus a whole ray is checked at a time instead
 * of a single position. 'Slider' pieces 'slide' through a ray.
 */
void generate_slider_moves(Board * board, Piece * piece, int * vectors)
{
        for (; *vectors; vectors++) {

                int i = piece->square + *vectors;
                for (; is0x88(i); i += *vectors) {

                        if (board->colors[i] == board->turn)
                                break;

                        if (board->pieces[i] == NOPIECE) {	/* Regular */
                                assert_move(board, piece->square, (S8)i,
                                                piece->piece, board->pieces[i],
                                                MF_NORMAL);
                        } else {				/* Capture */
                                assert_move(board, piece->square, (S8)i,
                                                piece->piece, board->pieces[i],
                                                MF_CAPTURE);
                                break;
                        }
                }
        }

        return;
}

/*
 * Check if a square can be moved into by a pawn.
 *
 * This is a helper function with all calls
 * integrated into 'generate_pawn_moves'.
 */
static inline int navigable_by_pawn(Board *board, S8 target)
{
        return is0x88(target) && board->pieces[target] == NOPIECE;
}

/*
 * Generate pawn normal moves.
 *
 * No error checks, captures and promotions are not generated here.
 */
void generate_pawn_moves(Board *board, Piece *piece)
{
        S8 target;

        if (board->turn == WHITE) {

                target = piece->square + N;
                if (navigable_by_pawn(board, target) &&
                                row8x8(piece->square) < ROW_7) {

                        assert_move(board, piece->square, target, piece->piece,
                                        NOPIECE, MF_NORMAL);

                        if (navigable_by_pawn(board, target + N) &&
                                        row8x8(piece->square) == ROW_2)
                                assert_move(board, piece->square, target + N,
                                                piece->piece, NOPIECE,
                                                MF_NORMAL);
                }

        } else {

                target = piece->square + S;
                if (navigable_by_pawn(board, target) &&
                                row8x8(piece->square) > ROW_2) {

                        assert_move(board, piece->square, target, piece->piece,
                                        NOPIECE, MF_NORMAL);

                        if (navigable_by_pawn(board, target + S)
                                        && row8x8(piece->square) == ROW_7)
                                assert_move(board, piece->square, target + S,
                                                piece->piece, NOPIECE,
                                                MF_NORMAL);
                }
        }

        return;
}

/*
 * Check if a square can be captured by a pawn.
 *
 * This is a helper function with all calls
 * integrated into 'generate_pawn_capts'.
 */
static inline int capturable_by_pawn(Board *board, S8 target)
{
        return is0x88(target) && (board->colors[target] == !board->turn ||
                        board->ep_square == target);
}

/*
 * Generate pawn captures.
 *
 * No error checks here, normal moves and promotions are not generated here.
 */
void generate_pawn_capts(Board *board, Piece *piece)
{
        S8 target;

        if (board->turn == WHITE) {

                target = (S8)(piece->square + NW);
                if (capturable_by_pawn(board, target))
                        assert_move(board, piece->square, target, piece->piece,
                                        board->pieces[target], MF_CAPTURE);

                target = (S8)(piece->square + NE);
                if (capturable_by_pawn(board, target))
                        assert_move(board, piece->square, target, piece->piece,
                                        board->pieces[target], MF_CAPTURE);

        } else {

                target = (S8)(piece->square + SW);
                if (capturable_by_pawn(board, target))
                        assert_move(board, piece->square, target, piece->piece,
                                        board->pieces[target], MF_CAPTURE);

                target = (S8)(piece->square + SE);
                if (capturable_by_pawn(board, target))
                        assert_move(board, piece->square, target, piece->piece,
                                        board->pieces[target], MF_CAPTURE);
        }

        return;
}

/*
 * Redirects to move generator for non sliders, with corrected arguments.
 */
void redir_to_k(Board *board, const void *self)
{
        int moveArgs[] = { N, S, E, W, NE, SE, NW, SW, 0 };

        generate_nonsli_moves(board, (Piece *)self, moveArgs);
}

/*
 * Redirects to move generator for slider rays, with corrected arguments.
 */
void redir_to_q(Board *board, const void *self)
{
        int moveArgs[] = { N, S, E, W, NE, SE, NW, SW, 0 };

        generate_slider_moves(board, (Piece *)self, moveArgs);
}

/*
 * Redirects to move generator for slider rays, with corrected arguments.
 */
void redir_to_b(Board *board, const void *self)
{
        int moveArgs[] = { NE, SE, NW, SW, 0 };

        generate_slider_moves(board, (Piece *)self, moveArgs);
}

/*
 * Redirects to move generator for non sliders, with corrected arguments.
 */
void redir_to_n(Board *board, const void *self)
{
        int moveArgs[] = { LEAP_NNE, LEAP_NNW, LEAP_SSE, LEAP_SSW,
                LEAP_ENE, LEAP_ESE, LEAP_WNW, LEAP_WSW, 0 };

        generate_nonsli_moves(board, (Piece *)self, moveArgs);
}

/*
 * Redirects to move generator for slider rays, with corrected arguments.
 */
void redir_to_r(Board *board, const void *self)
{
        int moveArgs[] = { N, S, E, W, 0 };

        generate_slider_moves(board, (Piece *)self, moveArgs);
}

/*
 * Redirects to move generators for pawns only, with corrected arguments.
 */
void redir_to_p(Board *board, const void *self)
{
        generate_pawn_moves(board, ((Piece *)self));
        generate_pawn_capts(board, ((Piece *)self));
}

/*
 * Apply move to board state.
 *
 * No error checks here, incrementally updated stuff is also taken care of.
 */
void apply_move(Board *board, Move *move)
{
        board->turn = !board->turn;
        board->zhash_key ^= board->zob_container.color;

        board->ply_count++;
        board->ply_total++;

        if (move->piece_fr == PAWN)
                board->ply_count = 0;

        /* Take piece from origin square */
        sq0x88_square_clear(board, move->square_fr);

        /* Re-update in case of capture */
        if (move->flags & MF_CAPTURE) {
                sq0x88_square_clear(board, move->square_to);
                board->ply_count = 0;
        }

        /* Insert piece in dest square */
        sq0x88_square_fill(board, move->square_to, move->piece_fr,
                        !board->turn);

        /* Update castling */
        switch (move->square_fr) {

                case H1: board->castle &= ~CASTLE_WK; break;
                case A1: board->castle &= ~CASTLE_WQ; break;
                case H8: board->castle &= ~CASTLE_BK; break;
                case A8: board->castle &= ~CASTLE_BQ; break;
                case E1: board->castle &= ~(CASTLE_WK | CASTLE_WQ); break;
                case E8: board->castle &= ~(CASTLE_BK | CASTLE_BQ); break;
                default: break;

        }
        switch (move->square_fr) {

                case H1: board->castle &= ~CASTLE_WK; break;
                case A1: board->castle &= ~CASTLE_WQ; break;
                case H8: board->castle &= ~CASTLE_BK; break;
                case A8: board->castle &= ~CASTLE_BQ; break;
                case E1: board->castle &= ~(CASTLE_WK | CASTLE_WQ); break;
                case E8: board->castle &= ~(CASTLE_BK | CASTLE_BQ); break;
                default: break;

        }

        board->zhash_key ^= board->zob_container.cs[move->castle];
        board->zhash_key ^= board->zob_container.cs[board->castle];

        /* Actually move the rook */
        if (move->flags & MF_CASTLE) {

                if (move->square_to == G1) {
                        sq0x88_square_clear(board, H1);
                        sq0x88_square_fill(board, F1, ROOK, WHITE);
                } else if (move->square_to == C1) {
                        sq0x88_square_clear(board, A1);
                        sq0x88_square_fill(board, D1, ROOK, WHITE);
                } else if (move->square_to == G8) {
                        sq0x88_square_clear(board, H8);
                        sq0x88_square_fill(board, F8, ROOK, BLACK);
                } else if (move->square_to == C8) {
                        sq0x88_square_clear(board, A8);
                        sq0x88_square_fill(board, D8, ROOK, BLACK);
                }
        }

        /* Reset EP square & update if necessary */
        if (board->ep_square != NOSQUARE) {
                board->zhash_key ^= board->zob_container.ep[board->ep_square];
                board->ep_square  = NOSQUARE;
        }

        int dist = move->square_to - move->square_fr;
        if (move->piece_fr == PAWN && (dist == 32 || dist == -32)) {
                board->ep_square  = (move->square_fr + move->square_to) / 2;
                board->zhash_key ^= board->zob_container.ep[board->ep_square];
        }

        /* If EP capture, pawn has to be manually removed*/
        if (move->flags & MF_EPCAPTURE) {
                if ((!board->turn) == WHITE)
                        sq0x88_square_clear(board, (S8)(move->square_to - 16));
                else
                        sq0x88_square_clear(board, (S8)(move->square_to + 16));
        }

        /* Stack hash key */
        void *data = malloc(sizeof(U64));
        memcpy(data, &board->zhash_key, sizeof(U64));
        fl_insert(&board->zhash_stack, data);

        return;
}

/*
 * Clear move from board state.
 *
 * No error checks here, incrementally updated stuff is also taken care of.
 */
void clear_move(Board *board, Move *move)
{
        board->turn = !board->turn;
        board->zhash_key ^= board->zob_container.color;

        board->ply_count = move->ply_count;
        board->ply_total = move->ply_total;

        if (board->ep_square != NOSQUARE)
                board->zhash_key ^= board->zob_container.ep[board->ep_square];

        if (move->ep_square  != NOSQUARE)
                board->zhash_key ^=  board->zob_container.ep[move->ep_square];

        board->ep_square = move->ep_square;

        /* Take piece from target square */
        sq0x88_square_clear(board, move->square_to);

        /* And put it back in origin square */
        sq0x88_square_fill(board, move->square_fr, move->piece_fr, board->turn);

        /* Restore piece in case of caputre */
        if (move->flags & MF_CAPTURE)
                sq0x88_square_fill(board, move->square_to, move->piece_to,
                                !board->turn);

        /* Replace rook if it was castled */
        if (move->flags & MF_CASTLE) {
                if (move->square_to == G1) {
                        sq0x88_square_clear(board, F1);
                        sq0x88_square_fill(board, H1, ROOK, WHITE);
                } else if (move->square_to == C1) {
                        sq0x88_square_clear(board, D1);
                        sq0x88_square_fill(board, A1, ROOK, WHITE);
                } else if (move->square_to == G8) {
                        sq0x88_square_clear(board, F8);
                        sq0x88_square_fill(board, H8, ROOK, BLACK);
                } else if (move->square_to == C8) {
                        sq0x88_square_clear(board, D8);
                        sq0x88_square_fill(board, A8, ROOK, BLACK);
                }
        }

        /* Readjust the flags also*/
        board->zhash_key ^= board->zob_container.cs[move->castle];
        board->zhash_key ^= board->zob_container.cs[board->castle];
        board->castle = move->castle;

        /* if EP un-capture, pawn needs to be replaced */
        if (move->flags & MF_EPCAPTURE) {
                if (board->turn == WHITE)
                        sq0x88_square_fill(board, move->square_to - 16, PAWN,
                                        BLACK);
                else
                        sq0x88_square_fill(board, move->square_to + 16, PAWN,
                                        WHITE);
        }

        /* Unstack previous hash */
        void *data;
        fl_remove(&board->zhash_stack, &data);
        free(data);

        return;
}

/*
 * Check if a given square is threatened in any way.
 *
 * Checks a given square agains all squares on the board; if it's unoccupied,
 * it's ignored. Otherwise, checks are done for pawns, knights, kings,
 * diagonal sliding pieces and straight sliding pieces.
 */
int exposed(Board *board, S8 square, U8 player)
{
        int i;
        for (i = 0; i < 128; i++) {

                if (board->colors[i] != !player)
                        continue;

                int delta = (square - i + 128);

                if (board->pieces[i] == PAWN) {

                        if (board->attacks[delta] == PRECOMP_BP &&
                                        player == WHITE)
                                return 1;

                        if (board->attacks[delta] == PRECOMP_WP &&
                                        player == BLACK)
                                return 1;

                } else if (board->pieces[i] == KNIGHT &&
                                board->attacks[delta] & PRECOMP_N) {
                        return 1;

                } else if (board->pieces[i] == KING &&
                                board->attacks[delta] & PRECOMP_K) {
                        return 1;

                } else if (board->pieces[i] == QUEEN ||
                                board->pieces[i] == BISHOP) {
                        return diagonal_exposed(board, square, i);

                } else if (board->pieces[i] == QUEEN ||
                                board->pieces[i] == ROOK) {
                        return straight_exposed(board, square, i);
                }
        }

        return 0;
}

/*
 * Check if a square is threatened by attackers in a diagonal angle.
 *
 * The attack vector is calculated; if the vector is possible by the
 * correspondently competent piece (queen or bishop), the ray is
 * checked for obstructions. Otherwise it's done.
 *
 * This is a helper function with all calls integrated into 'exposed'.
 */
int diagonal_exposed(Board *board, S8 target, S8 origin)
{
        int delta = (target - origin + 128);	/* Distance (attack) vector */
        int trow = row8x8(target);		/* Target square rank */
        int tcol = col8x8(target);		/* Target square file */
        int arow = row8x8(origin);		/* Attacker square rank */
        int acol = col8x8(origin);		/* Attacker square file */

        if (board->attacks[delta] & (PRECOMP_Q | PRECOMP_B)) {

                if (trow > arow && tcol > acol)
                        return ray_clear(board, target, (S8)origin, NE);
                if (trow > arow && tcol < acol)
                        return ray_clear(board, target, (S8)origin, NW);
                if (trow < arow && tcol > acol)
                        return ray_clear(board, target, (S8)origin, SE);
                if (trow < arow && tcol < acol)
                        return ray_clear(board, target, (S8)origin, SW);
        }

        return 0;
}

/*
 * Check if a square is threatened by attackers in a straight angle.
 *
 * The attack vector is calculated; if the vector is possible by the
 * correspondently competent piece (queen or rook), the ray is
 * checked for obstructions. Otherwise, it's done.
 *
 * This is a helper function with all calls integrated into 'exposed'.
 */
int straight_exposed(Board *board, S8 target, S8 origin)
{
        int delta = (target - origin + 128);	/* Distance (attack) vector */
        int trow = row8x8(target);		/* Target square rank */
        int tcol = col8x8(target);		/* Target square file */
        int arow = row8x8(origin);		/* Attacker square rank */
        int acol = col8x8(origin);		/* Attacker square file */

        if (board->attacks[delta] & (PRECOMP_Q | PRECOMP_R)) {

                if (trow < arow && tcol == acol)
                        return ray_clear(board, target, (S8)origin, S);
                if (trow > arow && tcol == acol)
                        return ray_clear(board, target, (S8)origin, N);
                if (trow == arow && tcol < acol)
                        return ray_clear(board, target, (S8)origin, W);
                if (trow == arow && tcol > acol)
                        return ray_clear(board, target, (S8)origin, E);
        }

        return 0;
}

/*
 * Check if a given ray is obstructed.
 *
 * Ray is given via origin square, target square and a vector increment.
 * Returns true if unobstructed, false otherwise.
 *
 * This is a helper function, with calls integrated into 'diagonal_exposed'
 * and 'straight_exposed'.
 */
int ray_clear(Board *board, S8 target, S8 origin, int vector)
{
        int i;
        for (i = origin + vector; i != target; i += vector)
                if (board->colors[i] != NOCOLOR)
                        return 0;

        return 1;
}
