/*!
 * \file jcl_move.cpp
 *
 * This file contains the implementation for the Move object
 */

#include "jcl_move.h"

#include <sstream>

#include "jcl_board.h"

namespace jcl
{

Move::Move(uint8_t sourceRow, uint8_t sourceCol, uint8_t destRow, uint8_t destCol, Piece piece)
{
  mSourceRow = sourceRow;
  mSourceColumn = sourceCol;
  mDestinationRow = destRow;
  mDestinationColumn = destCol;
  mPiece = piece;
  mCapturedPiece = mPromotedPiece = Piece::None;
  mType = Type::Quiet;
  mEnPassantColumn = 8;
  mCastlingRights = Board::CASTLE_NONE;
}

bool Move::isValid() const
{
  if (mPiece == Piece::None)
  {
    return false;
  }

  if (mSourceColumn == mDestinationColumn && mSourceRow == mDestinationColumn)
  {
    return false;
  }

  return true;
}

std::string Move::toSmithNotation() const
{
  static const char colLetter[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
  std::stringstream oss;
  oss << colLetter[mSourceColumn] << (mSourceRow+1) << colLetter[mDestinationColumn] << (mDestinationRow+1);
  return oss.str();
}

}
