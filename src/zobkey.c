#include "stdafx.h"
#include "zobkey.h"

#include <stdlib.h>

U64 zobrist_U64rand() {

        return rand() ^ ((U64)rand() << 15) ^ ((U64)rand() << 30)
                ^ ((U64)rand() << 45) ^ ((U64)rand() << 60);
}

void zobrist_init(Zobrist * zob) {

        int pc_idx;
        int cl_idx;
        int sq_idx;

        /* All pieces, colors and squares */
        for (pc_idx = 0; pc_idx < 6; pc_idx++)
                for (cl_idx = 0; cl_idx < 2; cl_idx++)
                        for (sq_idx = 0; sq_idx < 128; sq_idx++)
                                zob->pcsq[pc_idx][cl_idx][sq_idx] = zobrist_U64rand();

        zob->color = zobrist_U64rand();

        int cs_idx;
        for (cs_idx = 0; cs_idx < 16; cs_idx++)
                zob->cs[cs_idx] = zobrist_U64rand();


        int ep_idx;
        for (ep_idx = 0; ep_idx < 128; ep_idx++)
                zob->ep[ep_idx] = zobrist_U64rand();

        return;
}
