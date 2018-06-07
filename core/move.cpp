#include "move.h"

#include <sstream>

//Move::Move()
//  : mSourceRow(0)
//  , mSourceCol(0)
//  , mDestRow(0)
//  , mDestCol(0)
//  , mPiece(Piece::None)
//  , mCapturedPiece(Piece::None)
//  , mPromotedPiece(Piece::None)
//  , mType(Type::Quiet)
//  , mEnPassantColumn(INVALID_EP)
//  , mCastlingRights(CASTLE_NONE)
//{
//}

//Move::Move(uchar sourceRow, uchar sourceCol, uchar destRow, uchar destCol, Piece piece)
//  : mSourceRow(sourceRow)
//  , mSourceCol(sourceCol)
//  , mDestRow(destRow)
//  , mDestCol(destCol)
//  , mPiece(piece)
//  , mCapturedPiece(Piece::None)
//  , mPromotedPiece(Piece::None)
//  , mType(Type::Quiet)
//  , mEnPassantColumn(INVALID_EP)
//  , mCastlingRights(CASTLE_NONE)
//{
//}

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
