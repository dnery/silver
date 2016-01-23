// pt1: Score & Order moves, write algebraically
// pt2: Improved stalemate checking
// pt3: AI & stuff

#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>

#include "afx/stdafx.h"
#include "afx/sq0x88.h"
#include "afx/algebraic.h"
#include "afx/0x88movegen.h"

void draw_board(Board * board)
{
	/* This is not pretty. I don't give a sh*t. */

	char p_array[3][7] = {
		{'K','Q','B','N','R','P','.'},
		{'k','q','b','n','r','p','.'},
		{'.','.','.','.','.','.','.'}
	};

	printf("\n    A B C D E F G H\n");
	printf("  *-----------------*\n");

	S8 row = 7;
	for (; row >= 0; row--) {

		printf("%" PRId8 " | ", row + 1);

		S8 col = 0;
		for (; col < 8; col++) {

			S8 sq = (S8)sq0x88(row, col);
			printf("%c ", p_array
					[board->colors[sq]]
					[board->pieces[sq]]);
		}

		printf("| %" PRId8 "\n", row + 1);
	}

	printf("  *-----------------*\n");
	printf("    A B C D E F G H\n\n");
	printf("Zobkey for position: %" PRIu64 "\n\n", board->zhash_key);
}

int main(int argc, char *argv[])
{
	Board board;

	char FEN[1024];

	fgets(FEN, 1024, stdin);

	/* Engage structures */
	sq0x88_board_init(&board);
	sq0x88_FEN_read(&board, FEN);

	/* Generate and sort moves */
	generate_moves(&board);
	algebraic_sort(&board);

	/* Write readable output */
	draw_board(&board);
	algebraic_write(&board);

	/* Free allocated memory */
	sq0x88_board_drop(&board);

	return 0;
}
