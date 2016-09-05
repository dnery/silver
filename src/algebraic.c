#include <stdlib.h>
#include <stdio.h>

#include "stdafx.h"
#include "sq0x88.h"
#include "algebraic.h"

int redundancy_scan(Board * board, Move * move) {

        U64 i;
        for (i = 0; i < fl_size(&board->player_moves); i++) {

                Move *qmove = (Move *)fl_at(&board->player_moves, i);

                /* Ignore if start square is the same */
                if (move->square_fr == qmove->square_fr) continue;

                /* 1: In case piece types are the same */
                if (move->square_to == qmove->square_to &&
                                move->piece_fr  == qmove->piece_fr) {

                        /* 2: In case columns are also the same */
                        if (col8x8(move->square_fr) == col8x8(qmove->square_fr))
                                return 2;

                        // TODO pawn promotion 3-equal-piece scenario

                        return 1;
                }
        }

        return 0;
}

int sort_scores(const void *a, const void *b) {

        return ((Move *)fl_data(*((INode **)b)))->score -
                ((Move *)fl_data(*((INode **)a)))->score;
}

int sort_files(const void *a, const void *b) {

        return col8x8(((Move *)fl_data(*((INode **)a)))->square_fr) -
                col8x8(((Move *)fl_data(*((INode **)b)))->square_fr);
}

int sort_ranks(const void *a, const void *b) {

        return row8x8(((Move *)fl_data(*((INode **)a)))->square_fr) -
                row8x8(((Move *)fl_data(*((INode **)b)))->square_fr);
}

void algebraic_write(Board * board) {

        U64 i;
        for (i = 0; i < fl_size(&board->player_moves); i++) {

                Move * qmove = (Move *)fl_at(&board->player_moves, i);

                /* Write castling exclusively */
                if (qmove->flags & MF_CASTLE){

                        if(qmove->castle & CASTLE_WK || qmove->castle & CASTLE_BK)
                                printf("0-0\n");

                        if(qmove->castle & CASTLE_WQ || qmove->castle & CASTLE_BQ)
                                printf("0-0-0\n");

                        continue;
                }

                /* Ignore pawns on piece char */
                switch (qmove->piece_fr) {
                        case KING:   printf("K"); break;
                        case QUEEN:  printf("Q"); break;
                        case BISHOP: printf("B"); break;
                        case KNIGHT: printf("N"); break;
                        case ROOK:   printf("R"); break;
                        default: break;
                }

                /* Redundancy prevention chart */
                switch (redundancy_scan(board, qmove)) {
                        case 1: printf("%c", col8x8(qmove->square_fr) + 'a'); break;
                        case 2: printf("%d", row8x8(qmove->square_fr) +  1 ); break;
                        default: break;
                }

                /* Write char for capture */
                if (qmove->flags & MF_CAPTURE) {

                        /* Column if pawn capture */
                        if (qmove->piece_fr == PAWN)
                                printf("%c", col8x8(qmove->square_fr) + 'a');

                        printf("x");
                }
                /* Special case for EP captures */
                else if (qmove->flags & MF_EPCAPTURE) {
                        printf("%cx", col8x8(qmove->square_fr) + 'a');
                }

                /* Write target square */
                printf("%c", col8x8(qmove->square_to) + 'a');
                printf("%d", row8x8(qmove->square_to) +  1 );

                /* Promotion, if any */
                if (qmove->flags & MF_PROMOTION)
                        switch (qmove->piece_to) {
                                case KING:   printf("K"); break;
                                case QUEEN:  printf("Q"); break;
                                case BISHOP: printf("B"); break;
                                case KNIGHT: printf("N"); break;
                                case ROOK:   printf("R"); break;
                                default: break;
                        }

                printf("\n");
        }

        return;
}

void algebraic_sort(Board * board) {

        /* qsort((&board->player_moves)->array, fl_size(&board->player_moves),
           sizeof(INode), sort_ranks); */
        /*qsort((&board->player_moves)->array, fl_size(&board->player_moves),
          sizeof(INode), sort_files); */
        qsort((&board->player_moves)->array, fl_size(&board->player_moves),
                        sizeof(INode), sort_scores);

        return;
}

