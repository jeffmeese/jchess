#ifndef BOARD_H
#define BOARD_H

//#define USE_BOARD8x8
#define USE_BITBOARD

#if defined(USE_BOARD8x8)
#include "board8x8.h"
typedef Board8x8 Board;
#elif defined(USE_BITBOARD)
#include "bitboard.h"
typedef BitBoard Board;
#else
#include "board0x88.h"
typedef Board0x88 Board;
#endif

#endif // #ifndef BOARD_H
