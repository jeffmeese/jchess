/*!
 * \file jcl_board.cpp
 *
 * This file contains the implementation for the Board object
 */

#include "jcl_board.h"

#include <cstdint>
#include <iostream>
#include <string>

#include "jcl_fen.h"

// Macros for mapping (row,col)->index and vice-versa
#define getIndex(row,col) (((row)<<3)+(col))

enum Square
{
  A1 =  0, B1, C1, D1, E1, F1, G1, H1,
  A2 =  8, B2, C2, D2, E2, F2, G2, H2,
  A3 = 16, B3, C3, D3, E3, F3, G3, H3,
  A4 = 24, B4, C4, D4, E4, F4, G4, H4,
  A5 = 32, B5, C5, D5, E5, F5, G5, H5,
  A6 = 40, B6, C6, D6, E6, F6, G6, H6,
  A7 = 48, B7, C7, D7, E7, F7, G7, H7,
  A8 = 56, B8, C8, D8, E8, F8, G8, H8
};

namespace jcl
{

Board::Board()
{
  init();
}

bool Board::generateMoves(MoveList & moveList) const
{
  return doGenerateMoves(moveList);
}

bool Board::generateMoves(uint8_t row, uint8_t col, MoveList & moveList) const
{
  return doGenerateMoves(row, col, moveList);
}

uint8_t Board::getKingColumn(Color color) const
{
  return mKingColumn.find(color)->second;
}

uint8_t Board::getKingRow(Color color) const
{
  return mKingRow.find(color)->second;
}

PieceType Board::getPieceType(uint8_t row, uint8_t col) const
{
  return doGetPieceType(row, col);
}

void Board::init()
{
  mCastlingRights = CASTLE_BLACK_KING | CASTLE_BLACK_QUEEN | CASTLE_WHITE_KING | CASTLE_WHITE_QUEEN;
  mEnPassantColumn = INVALID_ENPASSANT_COLUMN;
  mHalfMoveClock = 0;
  mFullMoveCounter = 1;
  mSideToMove = Color::White;
  mKingColumn[Color::White] = 4;
  mKingRow[Color::White] = 0;
  mKingColumn[Color::Black] = 4;
  mKingRow[Color::Black] = 7;
}

bool Board::isCellAttacked(uint8_t row, uint8_t col, Color attackColor) const
{
  return doIsCellAttacked(row, col, attackColor);
}

bool Board::makeMove(const Move * move)
{
  Color sideToMove = this->getSideToMove();
  Color otherSide = (mSideToMove == Color::White) ? Color::Black : Color::White;

  // Update for king move
  if (move->getPiece() == Piece::King)
  {
    mKingColumn[sideToMove] = move->getDestinationColumn();
    mKingRow[sideToMove] = move->getDestinationRow();
  }

  // Let subclasses update their state
  doMakeMove(move);

  // Handle double pawn pushes
  mEnPassantColumn = INVALID_ENPASSANT_COLUMN;
  if (move->isDoublePush())
  {
    mEnPassantColumn = move->getSourceColumn();
  }

  // Update board state
  updateCastlingRights(move);
  updateMoveClocks(move);
  setSideToMove(otherSide);

  return true;
}

void Board::pushMove(uint8_t sourceRow,
                     uint8_t sourceCol,
                     uint8_t destRow,
                     uint8_t destCol,
                     Piece piece,
                     Piece capturedPiece,
                     Piece promotedPiece,
                     Move::Type type,
                     MoveList & moveList) const
{
  Move newMove(sourceRow, sourceCol, destRow, destCol, mCastlingRights, mEnPassantColumn, mHalfMoveClock, mFullMoveCounter,
               piece, type, capturedPiece, promotedPiece);

  moveList.addMove(newMove);
}

void Board::reset()
{
  init();
  doReset();
}

bool Board::setPieceType(uint8_t row, uint8_t col, PieceType pieceType)
{
  if (pieceType == PieceType::WhiteKing)
  {
    mKingRow[Color::White] = row;
    mKingColumn[Color::White] = col;
  }

  if (pieceType == PieceType::BlackKing)
  {
    mKingRow[Color::Black] = row;
    mKingColumn[Color::Black] = col;
  }

  return doSetPieceType(row, col, pieceType);
}

bool Board::setPosition(const std::string & fenString)
{
  Fen fen;
  if (!fen.setFromString(fenString))
  {
    std::cout << "Invalid FEN string" << std::endl;
    return false;
  }

  setCastlingRights(fen.getCastlingRights());
  setEnPassantColumn(fen.getEnPassantColumn());
  setFullMoveCounter(fen.getFullMoveCounter());
  setHalfMoveClock(fen.getHalfMoveClock());
  setSideToMove(fen.getSideToMove());

  return doSetPosition(fen);
}

bool Board::unmakeMove(const Move * move)
{
  Color otherSide = (mSideToMove == Color::White) ? Color::Black : Color::White;

  // Update for king move
  if (move->getPiece() == Piece::King)
  {
    mKingColumn[otherSide] = move->getSourceColumn();
    mKingRow[otherSide] = move->getSourceRow();
  }

  // Let subclasses update their state
  doUnmakeMove(move);

  // Reset the board state
  setFullMoveCounter(move->getFullMoveCounter());
  setHalfMoveClock(move->getHalfMoveClock());
  setCastlingRights(move->getCastlingRights());
  setEnPassantColumn(move->getEnPassantColumn());

  setSideToMove(otherSide);

  return true;
}

void Board::updateCastlingRights(const Move * move)
{
  uint8_t fromSquare = getIndex(move->getSourceRow(), move->getSourceColumn());
  uint8_t toSquare = getIndex(move->getDestinationRow(), move->getDestinationColumn());

  // Update castling rights
  // If the square associated with a king or rook is
  // modified in any way remove the castling ability
  // for that piece
  switch (fromSquare)
  {
  case H1:
    mCastlingRights &= ~CASTLE_WHITE_KING;
    break;
  case E1:
    mCastlingRights &= ~(CASTLE_WHITE_KING|CASTLE_WHITE_QUEEN);
    break;
  case A1:
    mCastlingRights &= ~CASTLE_WHITE_QUEEN;
    break;
  case H8:
    mCastlingRights &= ~CASTLE_BLACK_KING;
    break;
  case E8:
    mCastlingRights &= ~(CASTLE_BLACK_KING|CASTLE_BLACK_QUEEN);
    break;
  case A8:
    mCastlingRights &= ~CASTLE_BLACK_QUEEN;
    break;
  }

  switch (toSquare)
  {
  case H1:
    mCastlingRights &= ~CASTLE_WHITE_KING;
    break;
  case E1:
    mCastlingRights &= ~(CASTLE_WHITE_KING|CASTLE_WHITE_QUEEN);
    break;
  case A1:
    mCastlingRights &= ~CASTLE_WHITE_QUEEN;
    break;
  case H8:
    mCastlingRights &= ~CASTLE_BLACK_KING;
    break;
  case E8:
    mCastlingRights &= ~(CASTLE_BLACK_KING|CASTLE_BLACK_QUEEN);
    break;
  case A8:
    mCastlingRights &= ~CASTLE_BLACK_QUEEN;
    break;
  }
}

void Board::updateMoveClocks(const Move * move)
{
  setHalfMoveClock(getHalfMoveClock() + 1);
  if (move->getPiece() == Piece::Pawn || move->isCapture())
  {
    setHalfMoveClock(0);
  }

  if (getSideToMove() == Color::Black)
  {
    setFullMoveCounter(getFullMoveNumber() + 1);
  }
}

}
