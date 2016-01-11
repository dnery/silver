/*==== Public interface for Zobrist hash keys API ============================*/

#ifndef __ZOBKEY_H__
#define __ZOBKEY_H__

#include "stdafx.h"

/* Return a 64 bit random integer */
U64 zobrist_U64rand();

/* Initiate zobrist key structure */
void zobrist_init(Zobrist * zob);

#endif // !__ZOBKEY_H__
