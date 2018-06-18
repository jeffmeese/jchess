#include "move.h"

#include <sstream>

bool Move::isValid() const
{
  if (mPiece == Piece::None)
    return false;

  if (mSourceCol == mDestCol && mSourceRow == mDestCol)
    return false;

  return true;
}

std::string Move::toSmithNotation() const
{
  static const char colLetter[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
  std::stringstream oss;
  oss << colLetter[mSourceCol] << (mSourceRow+1) << colLetter[mDestCol] << (mDestRow+1);
  return oss.str();
}
