/*!
 * \file jcl_board.cpp
 *
 * This file contains the implementation for the Board object
 */

#include "jcl_board.h"

namespace jcl
{

Board::Board()
{
  init();
}

void Board::init()
{
  mCastlingRights = CASTLE_BLACK_KING | CASTLE_BLACK_QUEEN | CASTLE_WHITE_KING | CASTLE_WHITE_QUEEN;
  mEnPassantColumn = INVALID_ENPASSANT_COLUMN;
  mHalfMoveClock = 0;
  mFullMoveCounter = 1;
  mSideToMove = Color::White;
}

void Board::reset()
{
  init();
  doReset();
}

}
