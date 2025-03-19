#include "jcl_board.h"

#include "jcl_consts.h"

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
