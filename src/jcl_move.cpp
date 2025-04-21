/*!
 * \file jcl_move.cpp
 *
 * This file contains the implementation for the Move object
 */

#include "jcl_move.h"

#include <sstream>

namespace jcl
{

Move::Move(uint8_t sourceRow,
           uint8_t sourceCol,
           uint8_t destRow,
           uint8_t destCol,
           uint8_t castlingRights,
           uint8_t enPassantColumn,
           uint32_t halfMoveClock,
           uint32_t fullMoveCounter,
           Piece piece,
           Type type,
           Piece capturePiece,
           Piece promotionPiece)
{
  mSourceRow = sourceRow;
  mSourceColumn = sourceCol;
  mDestinationRow = destRow;
  mDestinationColumn = destCol;
  mCastlingRights = castlingRights;
  mEnPassantColumn = enPassantColumn;
  mHalfMoveClock = halfMoveClock;
  mFullMoveCounter = fullMoveCounter;
  mPiece = piece;
  mCapturedPiece = capturePiece;
  mPromotedPiece = promotionPiece;
  mType = type;
}

// Move::Move(const Position & sourcePosition,
//            const Position & destinationPosition,
//            const Pieces & pieces,
//            const BoardState & boardState,
//            Type type)
// {
//   mSourceRow = sourcePosition.row;
//   mSourceColumn = sourcePosition.col;
//   mDestinationRow = destinationPosition.row;
//   mDestinationColumn = destinationPosition.col;
//   mPiece = pieces.piece;
//   mCapturedPiece = pieces.capturePiece;
//   mPromotedPiece = pieces.promotionPiece;
//   mCastlingRights = boardState.castlingRights;
//   mEnPassantColumn = boardState.enPassantColumn;
//   mHalfMoveClock = boardState.halfMoveClock;
//   mFullMoveCounter = boardState.fullMoveCounter;
//   mType = type;
// }

// Move::Move(uint8_t sourceRow,
//            uint8_t sourceCol,
//            uint8_t destRow,
//            uint8_t destCol,
//            Piece piece,
//            Type type,
//            uint8_t castlingRights,
//            uint8_t enPassantColumn,
//            uint32_t halfMoveClock,
//            uint32_t fullMoveCounter,
//            Piece capturePiece,
//            Piece promotionPiece)
// {
//   mSourceRow = sourceRow;
//   mSourceColumn = sourceCol;
//   mDestinationRow = destRow;
//   mDestinationColumn = destCol;
//   mPiece = piece;
//   mCapturedPiece = capturePiece;
//   mPromotedPiece = promotionPiece;
//   mType = type;
//   //mHalfMoveClock = board->getHalfMoveClock();
//   //mFullMoveCounter = board->getFullMoveNumber();
//   //mCastlingRights = board->getCastlingRights();
//   //mEnPassantColumn = board->getEnpassantColumn();
// }

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
