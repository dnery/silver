/*==== Public interface for Algebraic conversions API ========================*/

#ifndef __ALGEBRAIC_H__
#define __ALGEBRAIC_H__

#include "stdafx.h"

/* Check for algebraic format redundancies */
int redundancy_scan(Board * board, Move * move);

/* Write algebraic format */
void algebraic_write(Board * board);

/* Sort moves algeb-style */
void algebraic_sort(Board * board);

#endif // !__ALGEBRAIC_H__