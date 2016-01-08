#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#include "afx/stdafx.h"
#include "afx/sq0x88.h"
#include "afx/0x88movegen.h"

void movegenType_any(Board *board, S8 sq_fr,
                     S8 sq_to,
                     S8 pc_fr,
                     S8 pc_to,
                     U8 flags) {

    Move *move = malloc(sizeof(*move));

    move->flags = flags;
    move->piece_fr = (U8)pc_fr;
    move->piece_to = (U8)pc_to;
    move->square_fr = sq_fr;
    move->square_to = sq_to;

    move->castle = board->castle;
    move->ep_square = board->ep_square;
    move->ply_count = board->ply_count;
    move->ply_total = board->ply_total;

    /* Build score */
    move->score = pc_fr;

    /* Bonus for capture */
    if(pc_to != NOPIECE)
        move->score += 10000 + board->eval_data.val_capts[pc_to];

    /* Bonus for EP capture */
    if (pc_fr == PAWN && sq_to == board->ep_square) {

        move->score += 10000 + board->eval_data.val_capts[PAWN];
        move->flags |= MF_EPCAPTURE;
    }

    /* Bonus for pawn promotion */
    if(move->flags & MF_PROMOTION)
        move->score += 9000 + board->eval_data.val_capts[pc_to];

    /* Apply move */
    move_make(board, move);

    /* Check test */
    int check = sqExposed(board, board->king_square[!board->turn],
                          (U8)!board->turn);
    /* Clear move */
    move_umake(board, move);

    /* Eval check */
    if (check)
        free(move);                                      /* Discard if ilegal */
    else
        fl_insert(&board->player_moves, move);           /* Otherwise keep it */

    return;
}

void movegenType_single(Board * board, Piece * piece, int * delta) {

    for (; *delta; delta++) {

        S8 target = (S8)(piece->square + *delta);

        /* Do nothing if friendly is found */
        if (target & 0x88 || board->colors[target] == board->turn) continue;

        /* Regular move */
        if(board->pieces[target] == NOPIECE)
        movegenType_any(board, piece->square, target,
            piece->piece, board->pieces[target], MF_NORMAL);

        /* Capture move */
        else 
        movegenType_any(board, piece->square, target,
            piece->piece, board->pieces[target], MF_CAPTURE);
    }

    return;
}

void movegenType_slider(Board * board, Piece * piece, int * delta) {

    for (; *delta; delta++) {

        int i;
        for (i = (piece->square + *delta); !(i & 0x88); i += *delta) {

            /* Do nothing if friendly is found */
            if (i & 0x88 || board->colors[i] == board->turn) {
                break;
            }
            /* Regular move */
            if (board->pieces[i] == NOPIECE) {
                movegenType_any(board, piece->square, (S8)i,
                                piece->piece, board->pieces[i], MF_NORMAL);
            }
            /* Capture move */
            else {
                movegenType_any(board, piece->square, (S8)i,
                                piece->piece, board->pieces[i], MF_CAPTURE);
                break;
            }
        }
    }

    return;
}

void movegenType_pawn_moves(Board * board, Piece * piece) {

    if (board->turn == WHITE) {

        if (is0x88(piece->square + N) &&
            board->pieces[piece->square + N] == NOPIECE) {

            if (row8x8(piece->square) < ROW_7) {

                movegenType_any(board, piece->square, (S8)(piece->square + N),
                                piece->piece, NOPIECE, MF_NORMAL);

            }
            else if (row8x8(piece->square) == ROW_7) {

                movegenType_any(board, piece->square, (S8)(piece->square + N),
                                piece->piece, QUEEN, MF_PROMOTION);
                movegenType_any(board, piece->square, (S8)(piece->square + N),
                                piece->piece, BISHOP, MF_PROMOTION);
                movegenType_any(board, piece->square, (S8)(piece->square + N),
                                piece->piece, KNIGHT, MF_PROMOTION);
                movegenType_any(board, piece->square, (S8)(piece->square + N),
                                piece->piece, ROOK, MF_PROMOTION);
            }

            if (is0x88(piece->square + NN) &&
                row8x8(piece->square) == ROW_2 &&
                board->pieces[piece->square + NN] == NOPIECE) {

                movegenType_any(board, piece->square, (S8)(piece->square + NN),
                                piece->piece, NOPIECE, MF_NORMAL);
            }
        }
    }
    else {

        if (is0x88(piece->square + S) &&
            board->pieces[piece->square + S] == NOPIECE) {

            if (row8x8(piece->square) > ROW_2) {

                movegenType_any(board, piece->square, (S8)(piece->square + S),
                                piece->piece, NOPIECE, MF_NORMAL);
            }
            else if (row8x8(piece->square) == ROW_2) {

                movegenType_any(board, piece->square, (S8)(piece->square + S),
                                piece->piece, QUEEN, MF_PROMOTION);
                movegenType_any(board, piece->square, (S8)(piece->square + S),
                                piece->piece, BISHOP, MF_PROMOTION);
                movegenType_any(board, piece->square, (S8)(piece->square + S),
                                piece->piece, KNIGHT, MF_PROMOTION);
                movegenType_any(board, piece->square, (S8)(piece->square + S),
                                piece->piece, ROOK, MF_PROMOTION);
            }

            if (is0x88(piece->square + SS) && row8x8(piece->square) == ROW_7 &&
                board->pieces[piece->square + SS] == NOPIECE) {

                movegenType_any(board, piece->square, (S8)(piece->square + SS),
                                piece->piece, NOPIECE, MF_NORMAL);
            }
        }
    }

    return;
}

void movegenType_pawn_capts(Board * board, Piece * piece) {

    S8 target;

    if (board->turn == WHITE) {

        target = (S8)(piece->square + NW);
        if (is0x88(target) &&
           (board->ep_square == target || board->colors[target]==!board->turn))
            movegenType_any(board, piece->square, target,
                piece->piece, board->pieces[target], MF_CAPTURE);

        target = (S8)(piece->square + NE);
        if (is0x88(target) &&
           (board->ep_square == target || board->colors[target]==!board->turn))
            movegenType_any(board, piece->square, target,
                piece->piece, board->pieces[target], MF_CAPTURE);
    }
    else {

        target = (S8)(piece->square + SW);
        if (is0x88(target) &&
           (board->ep_square == target || board->colors[target]==!board->turn))
            movegenType_any(board, piece->square, target,
                piece->piece, board->pieces[target], MF_CAPTURE);

        target = (S8)(piece->square + SE);
        if (is0x88(target) &&
           (board->ep_square == target || board->colors[target]==!board->turn))
            movegenType_any(board, piece->square, target,
                piece->piece, board->pieces[target], MF_CAPTURE);
    }

    return;
}

/*
 * Redirects to movegenerator for non sliders, with correct arguments
 */
void movegen_k(Board * board, const void * self) {

    int moveArgs[] = { N, S, E, W, NE, SE, NW, SW, 0 };

    movegenType_single(board, (Piece *)self, moveArgs);
}

/*
 * Redirects to movegenerator for slider rays, with correct arguments
 */
void movegen_q(Board * board, const void * self) {

    int moveArgs[] = { N, S, E, W, NE, SE, NW, SW, 0 };

    movegenType_slider(board, (Piece *)self, moveArgs);
}

/*
 * Redirects to movegenerator for slider rays, with correct arguments
 */
void movegen_b(Board * board, const void * self) {

    int moveArgs[] = { NE, SE, NW, SW, 0 };

    movegenType_slider(board, (Piece *)self, moveArgs);
}

/*
 * Redirects to movegenerator for non sliders, with correct arguments
 */
void movegen_n(Board * board, const void * self) {

    int moveArgs[] = { LEAP_NNE, LEAP_NNW,
                       LEAP_SSE, LEAP_SSW,
                       LEAP_ENE, LEAP_ESE,
                       LEAP_WNW, LEAP_WSW, 0 };

    movegenType_single(board, (Piece *)self, moveArgs);
}

/*
 * Redirects to movegenerator for slider rays, with correct arguments
 */
void movegen_r(Board * board, const void * self) {

    int moveArgs[] = { N, S, E, W, 0 };

    movegenType_slider(board, (Piece *)self, moveArgs);
}

/*
 * Redirects to movegenerators for pawns only, with correct arguments
 */
void movegen_p(Board * board, const void * self) {

    movegenType_pawn_moves(board, ((Piece *)self));
    movegenType_pawn_capts(board, ((Piece *)self));
}

void movegenL(Board * board) {

    /* Generate castling moves */
    if (board->turn == WHITE) {

        if (board->castle & CASTLE_WK) {
            if (board->pieces[F1] == NOPIECE &&
                board->pieces[G1] == NOPIECE &&
                !sqExposed(board, E1, board->turn) &&
                !sqExposed(board, F1, board->turn) &&
                !sqExposed(board, G1, board->turn))
                movegenType_any(board, E1, G1, KING, NOPIECE, MF_CASTLE);
        }
        if (board->castle & CASTLE_WQ) {
            if (board->pieces[B1] == NOPIECE &&
                board->pieces[C1] == NOPIECE &&
                board->pieces[D1] == NOPIECE &&
                !sqExposed(board, E1, board->turn) &&
                !sqExposed(board, D1, board->turn) &&
                !sqExposed(board, C1, board->turn))
                movegenType_any(board, E1, C1, KING, NOPIECE, MF_CASTLE);
        }
    }
    else {

        if (board->castle & CASTLE_BK) {
            if (board->pieces[F8] == NOPIECE &&
                board->pieces[G8] == NOPIECE &&
                !sqExposed(board, E8, board->turn) &&
                !sqExposed(board, F8, board->turn) &&
                !sqExposed(board, G8, board->turn))
                movegenType_any(board, E8, G8, KING, NOPIECE, MF_CASTLE);
        }
        if (board->castle & CASTLE_BQ) {
            if (board->pieces[B8] == NOPIECE &&
                board->pieces[C8] == NOPIECE &&
                board->pieces[D8] == NOPIECE &&
                !sqExposed(board, E8, board->turn) &&
                !sqExposed(board, D8, board->turn) &&
                !sqExposed(board, C8, board->turn))
                movegenType_any(board, E8, C8, KING, NOPIECE, MF_CASTLE);
        }
    }

    /* Generate moves for all player pieces */
    LNode *node = l_head(&board->player_pieces);
    while (node != NULL) {

        ((Piece *)l_data(node))->move_gen(board, l_data(node)); 

        node = l_next(node);
    }

    return;
}

void move_make(Board * board, Move * move) {

    /* Update some info */
    board->turn = (U8)!board->turn;
    board->zhash_key ^= board->zob_container.color;

    board->ply_count++;
    board->ply_total++;

    if (move->piece_fr == PAWN)
        board->ply_count = 0;

    /* Take piece from origin square */
    sq0x88_square_clear(board, move->square_fr);

    /* Re-update in case of capture */
    if (move->flags & MF_CAPTURE) {

        sq0x88_square_clear(board, move->square_to); board->ply_count = 0;
    }

    /* Insert piece in dest square */
    sq0x88_square_fill(board, move->square_to, move->piece_fr,
                       (U8)!board->turn);

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
        }
        else if (move->square_to == C1) {
            sq0x88_square_clear(board, A1);
            sq0x88_square_fill(board, D1, ROOK, WHITE);
        }
        else if (move->square_to == G8) {
            sq0x88_square_clear(board, H8);
            sq0x88_square_fill(board, F8, ROOK, BLACK);
        }
        else if (move->square_to == C8) {
            sq0x88_square_clear(board, A8);
            sq0x88_square_fill(board, D8, ROOK, BLACK);
        }
    }

    /* Reset EP square & update if necessary */
    if (board->ep_square != NOSQUARE) {
        board->zhash_key ^= board->zob_container.ep[board->ep_square];
        board->ep_square  = NOSQUARE;
    }

    int distance = move->square_to - move->square_fr;
    if (move->piece_fr == PAWN && (distance == 32 || distance == -32)) {
        board->ep_square  = (S8)((move->square_fr + move->square_to) / 2);
        board->zhash_key ^= board->zob_container.ep[board->ep_square];
    }

    /* If EP capture, pawn has to be manually removed*/
    if (move->flags & MF_EPCAPTURE) {
        if (!board->turn == WHITE)
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

void move_umake(Board * board, Move * move) {

    /* Update some info */
    board->turn = (U8)!board->turn;
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
                           (U8)!board->turn);

    /* Replace rook if it was castled */
    if (move->flags & MF_CASTLE) {
        if (move->square_to == G1) {
            sq0x88_square_clear(board, F1);
            sq0x88_square_fill(board, H1, ROOK, WHITE);
        }
        else if (move->square_to == C1) {
            sq0x88_square_clear(board, D1);
            sq0x88_square_fill(board, A1, ROOK, WHITE);
        }
        else if (move->square_to == G8) {
            sq0x88_square_clear(board, F8);
            sq0x88_square_fill(board, H8, ROOK, BLACK);
        }
        else if (move->square_to == C8) {
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
            sq0x88_square_fill(board, (S8)(move->square_to - 16), PAWN, BLACK);
        else
            sq0x88_square_fill(board, (S8)(move->square_to + 16), PAWN, WHITE);
    }

    void *data;

    /* Unstack previous hash */
    fl_remove(&board->zhash_stack, &data);  free(data);

    return;
}

int sqExposed(Board * board, S8 square, U8 player) {

    int delta, trow, tcol, arow, acol;

    int i;
    for (i = 0; i < 128; i++) {

        /* Assuming player as attacked */
        if (board->colors[i] != !player) continue;

        /* Precalculations */
        delta = (square - i + 128);
        trow  = row8x8(square); arow = row8x8(i);
        tcol  = col8x8(square); acol = col8x8(i);

        /* Pawns */
        if (board->pieces[i] == PAWN) { 

            if (board->attacks[delta] == PRECOMP_BP && player == WHITE)
                return 1;
            if (board->attacks[delta] == PRECOMP_WP && player == BLACK)
                return 1;
        }

        /* Kings */
        else if (board->pieces[i] == KING && board->attacks[delta] & PRECOMP_K)
            return 1;

        /* Knights */
        else if (board->pieces[i]==KNIGHT && board->attacks[delta] & PRECOMP_N)
            return 1;

        /* Diagonal sliders */
        else if (board->pieces[i] == QUEEN || board->pieces[i] == BISHOP) {

            if (board->attacks[delta] & (PRECOMP_Q | PRECOMP_B)) {

                if (trow > arow && tcol > acol)
                    return rgExposed(board, square, (S8)i,  17);
                if (trow > arow && tcol < acol)
                    return rgExposed(board, square, (S8)i,  15);
                if (trow < arow && tcol > acol)
                    return rgExposed(board, square, (S8)i, -15);
                if (trow < arow && tcol < acol)
                    return rgExposed(board, square, (S8)i, -17);
            }
        }

        /* Vertical sliders */
        else if (board->pieces[i] == QUEEN  ||  board->pieces[i] == ROOK) {

            if (board->attacks[delta] & (PRECOMP_Q | PRECOMP_R)) {

                if (trow < arow && tcol == acol)
                    return rgExposed(board, square, (S8)i, -16);
                if (trow > arow && tcol == acol)
                    return rgExposed(board, square, (S8)i,  16);
                if (trow == arow && tcol < acol)
                    return rgExposed(board, square, (S8)i,  -1);
                if (trow == arow && tcol > acol)
                    return rgExposed(board, square, (S8)i,   1);
            }
        }
    }

    return 0;
}

int rgExposed(Board *board, S8 target, S8 origin, int vector) {

    int i;
    for (i = origin + vector; i != target; i += vector)
        if (board->colors[i] != NOCOLOR) return 0;

    return 1;
}

