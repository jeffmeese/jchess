/*!
 * \file jcl_board8x8.cpp
 *
 * This file contains the interface for the Board8x8 object
 */

#include "jcl_board8x8.h"

#include <cstdint>

#include "jcl_fen.h"
#include "jcl_movelist.h"

// Define a few macros that make it easy to translate
// between a (row,col) pair and a board index
// and vice-versa
#define getIndex(row,col) (((row)<<3)+(col))
#define getRow(index) ( (index >> 3) )
#define getCol(index) ( (index % 8) )

namespace jcl
{

constexpr int8_t  NORTH          =  8;
constexpr int8_t  SOUTH          = -8;
constexpr int8_t  EAST           =  1;
constexpr int8_t  WEST           = -1;

enum Square
{
  A1=0 , B1, C1, D1, E1, F1, G1, H1,
  A2=8 , B2, C2, D2, E2, F2, G2, H2,
  A3=16, B3, C3, D3, E3, F3, G3, H3,
  A4=24, B4, C4, D4, E4, F4, G4, H4,
  A5=32, B5, C5, D5, E5, F5, G5, H5,
  A6=40, B6, C6, D6, E6, F6, G6, H6,
  A7=48, B7, C7, D7, E7, F7, G7, H7,
  A8=56, B8, C8, D8, E8, F8, G8, H8
};

Board8x8::Board8x8()
{
  initBoard();
}

bool Board8x8::doGenerateMoves(MoveList & moveList) const
{
  generateCastlingMoves(moveList);

  for (int8_t row = 0; row < 8; row++)
  {
    for (int8_t col = 0; col < 8; col++)
    {
      generateMoves(row, col, moveList, false);
    }
  }
  return true;
}

PieceType Board8x8::doGetPieceType(uint8_t row,
                                   uint8_t col) const
{
  Piece piece = mPieces[getIndex(row, col)];
  Color color = mColors[getIndex(row, col)];

  PieceType pieceType = PieceType::None;
  switch (piece)
  {
  case Piece::Pawn:
    pieceType = (color == Color::White) ? PieceType::WhitePawn : PieceType::BlackPawn;
    break;
  case Piece::Bishop:
    pieceType = (color == Color::White) ? PieceType::WhiteBishop : PieceType::BlackBishop;
    break;
  case Piece::Rook:
    pieceType = (color == Color::White) ? PieceType::WhiteRook : PieceType::BlackRook;
    break;
  case Piece::Knight:
    pieceType = (color == Color::White) ? PieceType::WhiteKnight : PieceType::BlackKnight;
    break;
  case Piece::Queen:
    pieceType = (color == Color::White) ? PieceType::WhiteQueen : PieceType::BlackQueen;
    break;
  case Piece::King:
    pieceType = (color == Color::White) ? PieceType::WhiteKing : PieceType::BlackKing;
    break;
  default:
    break;
  }

  return pieceType;
}

bool Board8x8::doGenerateMoves(uint8_t row,
                               uint8_t col,
                               MoveList & moveList) const
{
  return generateMoves(row, col, moveList, true);
}

bool Board8x8::doIsCellAttacked(uint8_t row,
                                uint8_t col,
                                Color attackColor) const
{
  return isCellAttacked(getIndex(row, col), attackColor);
}

bool Board8x8::doMakeMove(const Move * move)
{
  Color side = this->getSideToMove();
  uint8_t fromSquare = getIndex(move->getSourceRow(), move->getSourceColumn());
  uint8_t toSquare = getIndex(move->getDestinationRow(), move->getDestinationColumn());

  //std::cout << static_cast<int>(toSquare) << std::endl;

  // Update the color and piece arrays
  mColors[fromSquare] = Color::None;
  mPieces[fromSquare] = Piece::None;
  mColors[toSquare] = side;
  mPieces[toSquare] = move->getPiece();

  // Handle enPassant captures
  if (move->isEnPassantCapture())
  {
    char dir = (side == Color::White) ? SOUTH : NORTH;
    uint8_t sq = toSquare + dir;
    mPieces[sq] = Piece::None;
    mColors[sq] = Color::None;
  }

  setEnPassantColumn(INVALID_ENPASSANT_COLUMN);
  if (move->isDoublePush())
  {
    setEnPassantColumn(move->getDestinationColumn());
  }

  // Handle promotions
  if (move->isPromotion() || move->isPromotionCapture())
  {
    mPieces[toSquare] = move->getPromotedPiece();
  }

  // // Update castling rights
  // // If the square associated with a king or rook is
  // // modified in any way remove the castling ability
  // // for that piece
  // uint8_t castling = this->getCastlingRights();
  // switch (fromSquare)
  // {
  // case H1:
  //   castling &= ~CASTLE_WHITE_KING;
  //   break;
  // case E1:
  //   castling &= ~(CASTLE_WHITE_KING|CASTLE_WHITE_QUEEN);
  //   break;
  // case A1:
  //   castling &= ~CASTLE_WHITE_QUEEN;
  //   break;
  // case H8:
  //   castling &= ~CASTLE_BLACK_KING;
  //   break;
  // case E8:
  //   castling &= ~(CASTLE_BLACK_KING|CASTLE_BLACK_QUEEN);
  //   break;
  // case A8:
  //   castling &= ~CASTLE_BLACK_QUEEN;
  //   break;
  // }

  // switch (toSquare)
  // {
  // case H1:
  //   castling &= ~CASTLE_WHITE_KING;
  //   break;
  // case E1:
  //   castling &= ~(CASTLE_WHITE_KING|CASTLE_WHITE_QUEEN);
  //   break;
  // case A1:
  //   castling &= ~CASTLE_WHITE_QUEEN;
  //   break;
  // case H8:
  //   castling &= ~CASTLE_BLACK_KING;
  //   break;
  // case E8:
  //   castling &= ~(CASTLE_BLACK_KING|CASTLE_BLACK_QUEEN);
  //   break;
  // case A8:
  //   castling &= ~CASTLE_BLACK_QUEEN;
  //   break;
  // }

  // setCastlingRights(castling);

  // Handle castle moves
  if (move->isCastle())
  {
    switch (toSquare)
    {
    case C1:
      mPieces[D1] = Piece::Rook;
      mColors[D1] = Color::White;
      mPieces[A1] = Piece::None;
      mColors[A1] = Color::None;
      break;
    case G1:
      mPieces[F1] = Piece::Rook;
      mColors[F1] = Color::White;
      mPieces[H1] = Piece::None;
      mColors[H1] = Color::None;
      break;
    case C8:
      mPieces[D8] = Piece::Rook;
      mColors[D8] = Color::Black;
      mPieces[A8] = Piece::None;
      mColors[A8] = Color::None;
      break;
    case G8:
      mPieces[F8] = Piece::Rook;
      mColors[F8] = Color::Black;
      mPieces[H8] = Piece::None;
      mColors[H8] = Color::None;
      break;
    }
  }

  setHalfMoveClock(getHalfMoveClock()+1);
  if (move->getPiece() == Piece::Pawn || move->isCapture())
  {
    setHalfMoveClock(0);
  }

  return true;
}

void Board8x8::doReset()
{
  initBoard();
}

bool Board8x8::doSetPieceType(uint8_t row, uint8_t col, PieceType pieceType)
{
  uint8_t index = getIndex(row, col);

  switch (pieceType)
  {
  case PieceType::WhitePawn:
    mPieces[index] = Piece::Pawn;
    mColors[index] = Color::White;
    break;
  case PieceType::WhiteRook:
    mPieces[index] = Piece::Rook;
    mColors[index] = Color::White;
    break;
  case PieceType::WhiteBishop:
    mPieces[index] = Piece::Bishop;
    mColors[index] = Color::White;
    break;
  case PieceType::WhiteKnight:
    mPieces[index] = Piece::Knight;
    mColors[index] = Color::White;
    break;
  case PieceType::WhiteQueen:
    mPieces[index] = Piece::Queen;
    mColors[index] = Color::White;
    break;
  case PieceType::WhiteKing:
    mPieces[index] = Piece::King;
    mColors[index] = Color::White;
    break;
  case PieceType::BlackPawn:
    mPieces[index] = Piece::Pawn;
    mColors[index] = Color::Black;
    break;
  case PieceType::BlackRook:
    mPieces[index] = Piece::Rook;
    mColors[index] = Color::Black;
    break;
  case PieceType::BlackBishop:
    mPieces[index] = Piece::Bishop;
    mColors[index] = Color::Black;
    break;
  case PieceType::BlackKnight:
    mPieces[index] = Piece::Knight;
    mColors[index] = Color::Black;
    break;
  case PieceType::BlackQueen:
    mPieces[index] = Piece::Queen;
    mColors[index] = Color::Black;
    break;
  case PieceType::BlackKing:
    mPieces[index] = Piece::King;
    mColors[index] = Color::Black;
    break;
  case PieceType::None:
    mPieces[index] = Piece::None;
    mColors[index] = Color::None;
    break;
  }

  return true;
}

bool Board8x8::doSetPosition(const Fen & fen)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    for (uint8_t j = 0; j < 8; j++)
    {
      uint8_t index = getIndex(i, j);
      PieceType pieceType = fen.getPieceType(i, j);

      switch (pieceType)
      {
      case PieceType::WhitePawn:
        mPieces[index] = Piece::Pawn;
        mColors[index] = Color::White;
        break;
      case PieceType::WhiteRook:
        mPieces[index] = Piece::Rook;
        mColors[index] = Color::White;
        break;
      case PieceType::WhiteKnight:
        mPieces[index] = Piece::Knight;
        mColors[index] = Color::White;
        break;
      case PieceType::WhiteBishop:
        mPieces[index] = Piece::Bishop;
        mColors[index] = Color::White;
        break;
      case PieceType::WhiteQueen:
        mPieces[index] = Piece::Queen;
        mColors[index] = Color::White;
        break;
      case PieceType::WhiteKing:
        mPieces[index] = Piece::King;
        mColors[index] = Color::White;
        break;
      case PieceType::BlackPawn:
        mPieces[index] = Piece::Pawn;
        mColors[index] = Color::Black;
        break;
      case PieceType::BlackRook:
        mPieces[index] = Piece::Rook;
        mColors[index] = Color::Black;
        break;
      case PieceType::BlackKnight:
        mPieces[index] = Piece::Knight;
        mColors[index] = Color::Black;
        break;
      case PieceType::BlackBishop:
        mPieces[index] = Piece::Bishop;
        mColors[index] = Color::Black;
        break;
      case PieceType::BlackQueen:
        mPieces[index] = Piece::Queen;
        mColors[index] = Color::Black;
        break;
      case PieceType::BlackKing:
        mPieces[index] = Piece::King;
        mColors[index] = Color::Black;
        break;
      case PieceType::None:
        mPieces[index] = Piece::None;
        mColors[index] = Color::None;
        break;
      }
    }
  }

  return true;
}

bool Board8x8::doUnmakeMove(const Move * move)
{
  Color side = this->getSideToMove();
  uint8_t fromSquare = getIndex(move->getSourceRow(), move->getSourceColumn());
  uint8_t toSquare = getIndex(move->getDestinationRow(), move->getDestinationColumn());
  Piece piece = move->getPiece();

  // Update the piece and color arrays
  mPieces[fromSquare] = piece;
  mColors[fromSquare] = !side;
  mPieces[toSquare] = Piece::None;
  mColors[toSquare] = Color::None;

  // Handle captures
  if (move->isCapture())
  {
    uint8_t sq = toSquare;
    if (move->isEnPassantCapture())
    {
      char dir = (side == Color::White) ? NORTH : SOUTH;
      sq = toSquare + dir;
    }

    mPieces[sq] = move->getCapturedPiece();
    mColors[sq] = side;
  }

  // Handle promotions
  if (move->isPromotion() || move->isPromotionCapture())
  {
    mPieces[fromSquare] = Piece::Pawn;
    if (move->isPromotionCapture())
    {
      mPieces[toSquare] = move->getCapturedPiece();
    }
  }

  // Handle castling move
  if (move->isCastle())
  {
    switch (toSquare)
    {
    case C1:
      mPieces[A1] = Piece::Rook;
      mColors[A1] = Color::White;
      mPieces[D1] = Piece::None;
      mColors[D1] = Color::None;
      break;
    case G1:
      mPieces[H1] = Piece::Rook;
      mColors[H1] = Color::White;
      mPieces[F1] = Piece::None;
      mColors[F1] = Color::None;
      break;
    case C8:
      mPieces[A8] = Piece::Rook;
      mColors[A8] = Color::Black;
      mPieces[D8] = Piece::None;
      mColors[D8] = Color::None;
      break;
    case G8:
      mPieces[H8] = Piece::Rook;
      mColors[H8] = Color::Black;
      mPieces[F8] = Piece::None;
      mColors[F8] = Color::None;
      break;
    }
  }

  return true;
}

void Board8x8::generateCastlingMoves(MoveList & moveList) const
{
  Color sideToMove = this->getSideToMove();
  uint8_t castlingRights = this->getCastlingRights();

  if (castlingRights == CASTLE_NONE)
    return;

  if (sideToMove == Color::White)
  {
    bool b1Empty = mPieces[B1] == Piece::None;
    bool c1Empty = mPieces[C1] == Piece::None;
    bool d1Empty = mPieces[D1] == Piece::None;
    bool f1Empty = mPieces[F1] == Piece::None;
    bool g1Empty = mPieces[G1] == Piece::None;

    if (castlingRights & CASTLE_WHITE_KING)
    {
      bool empty = f1Empty && g1Empty;
      if (empty && !isCellAttacked(E1, Color::Black) && !isCellAttacked(F1, Color::Black) && !isCellAttacked(G1, Color::Black))
      {
        pushMove(getRow(E1), getCol(E1), getRow(G1), getCol(G1), Piece::King, Piece::None, Piece::None, Move::Type::Castle, moveList);
      }
    }

    if (castlingRights & CASTLE_WHITE_QUEEN)
    {
      bool empty = b1Empty && c1Empty && d1Empty;
      if (empty && !isCellAttacked(E1, Color::Black) && !isCellAttacked(D1, Color::Black) && !isCellAttacked(C1, Color::Black))
      {
        pushMove(getRow(E1), getCol(E1), getRow(C1), getCol(C1), Piece::King, Piece::None, Piece::None, Move::Type::Castle, moveList);
      }
    }
  }
  else
  {
    bool b8Empty = mPieces[B8] == Piece::None;
    bool c8Empty = mPieces[C8] == Piece::None;
    bool d8Empty = mPieces[D8] == Piece::None;
    bool f8Empty = mPieces[F8] == Piece::None;
    bool g8Empty = mPieces[G8] == Piece::None;

    if (castlingRights & CASTLE_BLACK_KING)
    {
      bool empty = f8Empty && g8Empty;
      if (empty && !isCellAttacked(E8, Color::White) && !isCellAttacked(F8, Color::White) && !isCellAttacked(G8, Color::White))
      {
        pushMove(getRow(E8), getCol(E8), getRow(G8), getCol(G8), Piece::King, Piece::None, Piece::None, Move::Type::Castle, moveList);
      }
    }

    if (castlingRights & CASTLE_BLACK_QUEEN)
    {
      bool empty = b8Empty && c8Empty && d8Empty;
      if (empty && !isCellAttacked(E8, Color::White) && !isCellAttacked(C8, Color::White) && !isCellAttacked(D8, Color::White))
      {
        pushMove(getRow(E8), getCol(E8), getRow(C8), getCol(C8), Piece::King, Piece::None, Piece::None, Move::Type::Castle, moveList);
      }
    }
  }
}



bool Board8x8::generateMoves(uint8_t row,
                             uint8_t col,
                             MoveList & moveList,
                             bool generateCastling) const
{
  uint8_t index = getIndex(row, col);
  Piece piece = mPieces[index];
  Color color = mColors[index];
  Color sideToMove = this->getSideToMove();

  // Can't move a non-existent piece
  if (piece == Piece::None)
  {
    return true;
  }

  if (generateCastling)
  {
    generateCastlingMoves(moveList);
  }

  if (piece != Piece::None && color == sideToMove)
  {
    if (piece == Piece::Bishop)
    {
      generateSliderMoves(index, +1, +1, true, moveList);
      generateSliderMoves(index, +1, -1, true, moveList);
      generateSliderMoves(index, -1, +1, true, moveList);
      generateSliderMoves(index, -1, -1, true, moveList);
    }
    else if (piece == Piece::Rook)
    {
      generateSliderMoves(index, +1, +0, true, moveList);
      generateSliderMoves(index, -1, +0, true, moveList);
      generateSliderMoves(index, +0, +1, true, moveList);
      generateSliderMoves(index, +0, -1, true, moveList);
    }
    else if (piece == Piece::Queen)
    {
      generateSliderMoves(index, +1, +1, true, moveList);
      generateSliderMoves(index, +1, -1, true, moveList);
      generateSliderMoves(index, -1, +1, true, moveList);
      generateSliderMoves(index, -1, -1, true, moveList);
      generateSliderMoves(index, +1, +0, true, moveList);
      generateSliderMoves(index, -1, +0, true, moveList);
      generateSliderMoves(index, +0, +1, true, moveList);
      generateSliderMoves(index, +0, -1, true, moveList);
    }
    else if (piece == Piece::King)
    {
      generateSliderMoves(index, +1, +1, false, moveList);
      generateSliderMoves(index, +1, -1, false, moveList);
      generateSliderMoves(index, -1, +1, false, moveList);
      generateSliderMoves(index, -1, -1, false, moveList);
      generateSliderMoves(index, +1, +0, false, moveList);
      generateSliderMoves(index, -1, +0, false, moveList);
      generateSliderMoves(index, +0, +1, false, moveList);
      generateSliderMoves(index, +0, -1, false, moveList);
    }
    else if (piece == Piece::Knight)
    {
      generateSliderMoves(index, +2, +1, false, moveList);
      generateSliderMoves(index, +2, -1, false, moveList);
      generateSliderMoves(index, +1, -2, false, moveList);
      generateSliderMoves(index, -1, -2, false, moveList);
      generateSliderMoves(index, -2, -1, false, moveList);
      generateSliderMoves(index, -2, +1, false, moveList);
      generateSliderMoves(index, -1, +2, false, moveList);
      generateSliderMoves(index, +1, +2, false, moveList);
    }
    else if (piece == Piece::Pawn)
    {
      generatePawnMoves(index, sideToMove, moveList);
    }
  }

  return true;
}

void Board8x8::generateSliderMoves(uint8_t index,
                                   int8_t rowIncrement,
                                   int8_t colIncrement,
                                   bool slider,
                                   MoveList & moveList) const
{
  Piece piece = mPieces[index];
  Color sideToMove = this->getSideToMove();
  Color otherSide = (sideToMove == Color::White) ? Color::Black : Color::White;
  uint8_t row = getRow(index);
  uint8_t col = getCol(index);

  int8_t destRow = row + rowIncrement;
  int8_t destCol = col + colIncrement;
  while (true)
  {
    if ( (rowIncrement < 0 && destRow < 0) || (rowIncrement > 0 && destRow > 7) )
    {
      break;
    }

    if ( (colIncrement < 0 && destCol < 0) || (colIncrement > 0 && destCol > 7) )
    {
      break;
    }

    uint8_t destIndex = getIndex(destRow, destCol);
    Piece destPiece = mPieces[destIndex];
    Color destColor = mColors[destIndex];
    if (destPiece == Piece::None)
    {
      pushMove(row, col, destRow, destCol, piece, Piece::None, Piece::None, Move::Type::Quiet, moveList);
    }
    else if (destColor == otherSide)
    {
      pushMove(row, col, destRow, destCol, piece, destPiece, Piece::None, Move::Type::Capture, moveList);
      break;
    }
    else if (destColor == sideToMove)
    {
      break;
    }

    if (!slider)
    {
      break;
    }

    destRow += rowIncrement;
    destCol += colIncrement;
  }
}

void Board8x8::generatePawnMoves(uint8_t index,
                                 Color sideToMove,
                                 MoveList & moveList) const
{
  Color otherSide = (sideToMove == Color::White) ? Color::Black : Color::White;
  int8_t row = getRow(index);
  int8_t col = getCol(index);
  int8_t rowIncr = (sideToMove == Color::White) ? +1 : -1;
  int8_t startRow = (sideToMove == Color::White) ? 1 : 6;
  int8_t singlePushRow = row + rowIncr;
  int8_t doublePushRow = singlePushRow + rowIncr;

  // WTF?
  if (row == 0 || row == 8)
  {
    return;
  }

  // Generate standard moves
  uint8_t destIndex = getIndex(singlePushRow, col);
  uint8_t destRow = getRow(destIndex);
  uint8_t destCol = getCol(destIndex);
  if (mPieces[destIndex] == Piece::None)
  {
    if (singlePushRow == 0 || singlePushRow == 7)
    {
      pushMove(row, col, destRow, destCol, Piece::Pawn, Piece::None, Piece::Bishop, Move::Type::Promotion, moveList);
      pushMove(row, col, destRow, destCol, Piece::Pawn, Piece::None, Piece::Knight, Move::Type::Promotion, moveList);
      pushMove(row, col, destRow, destCol, Piece::Pawn, Piece::None, Piece::Rook, Move::Type::Promotion, moveList);
      pushMove(row, col, destRow, destCol, Piece::Pawn, Piece::None, Piece::Queen, Move::Type::Promotion, moveList);
    }
    else
    {
      pushMove(row, col, destRow, destCol, Piece::Pawn, Piece::None, Piece::None, Move::Type::Quiet, moveList);
    }

    if (row == startRow)
    {
      destIndex = getIndex(doublePushRow, col);
      destRow = getRow(destIndex);
      destCol = getCol(destIndex);
      if (mPieces[destIndex] == Piece::None)
      {
        pushMove(row, col, destRow, destCol, Piece::Pawn, Piece::None, Piece::None, Move::Type::DoublePush, moveList);
      }
    }
  }

  // Generate capture moves
  int8_t cols[] = {1, -1};
  for (int8_t i = 0; i < 2; i++)
  {
    int8_t destCol = col + cols[i];
    if (destCol >= 0 && destCol <= 7)
    {
      uint8_t destIndex = getIndex(singlePushRow, destCol);
      uint8_t destRow = getRow(destIndex);
      uint8_t destCol = getCol(destIndex);
      if (mPieces[destIndex] != Piece::None && mColors[destIndex] == otherSide)
      {
        if (singlePushRow == 0 || singlePushRow == 7)
        {
          pushMove(row, col, destRow, destCol, Piece::Pawn, mPieces[destIndex], Piece::Bishop, Move::Type::PromotionCapture, moveList);
          pushMove(row, col, destRow, destCol, Piece::Pawn, mPieces[destIndex], Piece::Knight, Move::Type::PromotionCapture, moveList);
          pushMove(row, col, destRow, destCol, Piece::Pawn, mPieces[destIndex], Piece::Rook, Move::Type::PromotionCapture, moveList);
          pushMove(row, col, destRow, destCol, Piece::Pawn, mPieces[destIndex], Piece::Queen, Move::Type::PromotionCapture, moveList);
        }
        else
        {
          pushMove(row, col, destRow, destCol, Piece::Pawn, mPieces[destIndex], Piece::None, Move::Type::Capture, moveList);
        }
      }
    }
  }

  // Generate enPassant captures
  int8_t enPassantColumn = getEnpassantColumn();
  if (enPassantColumn != INVALID_ENPASSANT_COLUMN)
  {
    constexpr int8_t dirs[] = {EAST, WEST};
    int8_t enPassantRow = (sideToMove == Color::White) ? 5 : 2;
    int8_t enPassantIndex = getIndex(enPassantRow, enPassantColumn);

    int8_t destRow = row + rowIncr;

    for (int8_t i = 0; i < 2; i++)
    {
      int8_t destCol = col + dirs[i];
      if (destRow >= 0 && destRow <= 7 && destCol >= 0 && destCol <= 7)
      {
        int8_t epCaptureIndex = getIndex(destRow, destCol);
        if (enPassantIndex == epCaptureIndex)
        {
          pushMove(row, col, destRow, destCol, Piece::Pawn, mPieces[destIndex - (8*rowIncr)], Piece::Pawn, Move::Type::EpCapture, moveList);
        }
      }
    }
  }
}

void Board8x8::initBoard()
{
  for (uint8_t i = 0; i < 64; i++)
  {
    mPieces[i] = Piece::None;
    mColors[i] = Color::None;
  }

  for (uint8_t i = A2; i <= H2; i++)
  {
    mPieces[i] = Piece::Pawn;
    mColors[i] = Color::White;
  }

  for (uint8_t i = A7; i <= H7; i++)
  {
    mPieces[i] = Piece::Pawn;
    mColors[i] = Color::Black;
  }

  for (uint8_t i = A1; i <= H1; i++)
  {
    mColors[i] = Color::White;
  }

  for (uint8_t i = A8; i <= H8; i++)
  {
    mColors[i] = Color::Black;
  }

  mPieces[A1] = mPieces[H1] = mPieces[A8] = mPieces[H8] = Piece::Rook;
  mPieces[B1] = mPieces[G1] = mPieces[B8] = mPieces[G8] = Piece::Knight;
  mPieces[C1] = mPieces[F1] = mPieces[C8] = mPieces[F8] = Piece::Bishop;
  mPieces[D1] = mPieces[D8] = Piece::Queen;
  mPieces[E1] = mPieces[E8] = Piece::King;
}

bool Board8x8::isCellAttacked(uint8_t index, Color attackColor) const
{
  enum Attack {Diag, Straight};
  static constexpr int8_t sliderRowIncrements[] = {+1, +1, -1, -1, +1, -1, +0, +0};
  static constexpr int8_t sliderColIncrements[] = {+1, -1, +1, -1, +0, +0, -1, +1};
  static constexpr int8_t kingRowIncrements[]   = {+1, +1, -1, -1, +1, -1, +0, +0}; // Same as slider but included for clarity
  static constexpr int8_t kingColIncrements[]   = {+1, -1, +1, -1, +0, +0, -1, +1}; // Same as slider but included for clarity
  static constexpr int8_t knightRowIncrements[] = {+2, +2, +1, -1, -2, -2, -1, +1};
  static constexpr int8_t knightColIncrements[] = {+1, -1, -2, -2, -1, +1, +2, +2};
  static constexpr Attack attacks[] = {Diag, Diag, Diag, Diag, Straight, Straight, Straight, Straight};

  int8_t row = getRow(index);
  int8_t col = getCol(index);

  // Check for pawn attacks
  // Pawns only attack in the forward direction so the row increment we
  // use we'll depend on the attacking color.
  // Its possible that we will find a different attacking enemy piece
  // here (e.g Queen, Bishop, King) but we will ignore that for now
  // to make the logic in the loops below easier to follow.
  int8_t rowIncr = (attackColor == Color::Black) ? +1 : -1;
  int8_t destRow = row + rowIncr;
  if (destRow >= 0 && destRow <= 7)
  {
    constexpr int8_t dirs[] = {EAST, WEST};

    for (int8_t i = 0; i < 2; i++)
    {
      int8_t destCol = col + dirs[i];
      if (destCol >= 0 && destCol <= 7)
      {
        int8_t destIndex = getIndex(destRow, destCol);
        if (mColors[destIndex] == attackColor && mPieces[destIndex] == Piece::Pawn)
        {
          return true;
        }
      }
    }
  }

  // Check for knight attacks
  for (int8_t i = 0; i < 8; i++)
  {
    int8_t rowIncr = knightRowIncrements[i];
    int8_t colIncr = knightColIncrements[i];

    int8_t destRow = row + rowIncr;
    int8_t destCol = col + colIncr;
    if (destRow >= 0 && destRow <= 7 && destCol >= 0 && destCol <= 7)
    {
      int8_t destIndex = getIndex(destRow, destCol);
      if (mColors[destIndex] == attackColor && mPieces[destIndex] == Piece::Knight)
      {
        return true;
      }
    }
  }

  // Check for king attacks
  // Kings can move the same directions as Queens but they can only
  // move a single square. As with the pawn loop we may find another
  // enenmy piece in this loop but will ignore it here as well.
  for (int8_t i = 0; i < 8; i++)
  {
    int8_t rowIncr = kingRowIncrements[i];
    int8_t colIncr = kingColIncrements[i];

    int8_t destRow = row + rowIncr;
    int8_t destCol = col + colIncr;
    if (destRow >= 0 && destRow <= 7 && destCol >= 0 && destCol <= 7)
    {
      int8_t destIndex = getIndex(destRow, destCol);
      if (mColors[destIndex] == attackColor && mPieces[destIndex] == Piece::King)
      {
        return true;
      }
    }
  }

  // Check for queen/rook/bishop attacks
  // Now that we've eliminated any attacks from pieces that can't move
  // across multiple squares (i.e. sliders) we turn our attention to
  // sliding attacks. We do this by continually moving away from our
  // square along a single ray (straight or diagonal) of the board. If we encounter a friendly
  // piece that means we are not being attacked along this ray. If we encounter
  // an attacking piece we determine if that piece can attack along that
  // ray. For instance, a rook cannot attack along a diagonal. If we do
  // run into an enemy piece that cannot attack along that ray we know
  // we are not being attacked along that ray.
  Color friendlyColor = (attackColor == Color::White) ? Color::Black : Color::White;
  for (int8_t i = 0; i < 8; i++)
  {
    int8_t rowIncr = sliderRowIncrements[i];
    int8_t colIncr = sliderColIncrements[i];
    Attack attack = attacks[i];

    int8_t destRow = row + rowIncr;
    int8_t destCol = col + colIncr;

    while (true)
    {
      if ( (rowIncr < 0 && destRow < 0) || (rowIncr > 0 && destRow > 7) )
      {
        break;
      }

      if ( (colIncr < 0 && destCol < 0) || (colIncr > 0 && destCol > 7) )
      {
        break;
      }

      int8_t destIndex = getIndex(destRow, destCol);
      Piece destPiece = mPieces[destIndex];
      Color destColor = mColors[destIndex];

      // Stop if we run into our own piece
      if (destColor == friendlyColor)
      {
        break;
      }

      if (destColor == attackColor)
      {
        if (attack == Diag && (destPiece == Piece::Bishop || destPiece == Piece::Queen))
        {
          return true;
        }
        else if (attack == Straight && (destPiece == Piece::Rook || destPiece == Piece::Queen))
        {
          return true;
        }
        else
        {
          // The enemy piece at this square cannot attack
          break;
        }
      }

      destRow += rowIncr;
      destCol += colIncr;
    }
  }

  return false;
}



// void Board8x8::pushMove(uint8_t from,
//                         uint8_t to,
//                         Piece piece,
//                         Piece capturedPiece,
//                         Piece promotedPiece,
//                         Move::Type type,
//                         MoveList & moveList) const
// {
//   uint8_t sourceRow = getRow(from);
//   uint8_t sourceColumn = getCol(from);
//   uint8_t destRow = getRow(to);
//   uint8_t destCol = getCol(to);

//   Move newMove(sourceRow, sourceColumn, destRow, destCol, piece, type, this, capturedPiece, promotedPiece);
//   moveList.addMove(newMove);
// }





// // Macros for mapping (row,col)->index and vice-versa
// #define getIndex(row,col) (((row)<<3)+(col))
// #define getRow(index) ( (index >> 3) )
// #define getCol(index) ( (index % 8) )

// // Constant for invalid square position
// static const uint8_t INVALID_SQUARE = 64;

// // Attack Offsets
// static const char NORTH =  8;
// static const char SOUTH = -8;
// static const char EAST  =  1;
// static const char WEST  = -1;
// static const char NW    =  7;
// static const char NE    =  9;
// static const char SW    = -9;
// static const char SE    = -7;
// static const char SSW   = -17;
// static const char SSE   = -15;
// static const char WWS   = -10;
// static const char EES   = -6;
// static const char EEN   =  10;
// static const char WWN   =  6;
// static const char NNE   =  17;
// static const char NNW   =  15;

// // Attack vectors for pieces
// // The king and queen attacks are identical but are both included for clarity
// static const char whitePawnAttacks[] = {NE, NW};
// static const char blackPawnAttacks[] = {SE, SW};
// static const char rookAttacks[]      = {NORTH, SOUTH, EAST, WEST};
// static const char bishopAttacks[]    = {NW, NE, SE, SW};
// static const char knightAttacks[]    = {SSW, SSE, WWS, EES, EEN, WWN, NNE, NNW};
// static const char queenAttacks[]     = {NORTH, SOUTH, EAST, WEST, NW, NE, SW, SE};
// static const char kingAttacks[]      = {NORTH, SOUTH, EAST, WEST, NW, NE, SW, SE};

// // Square index
// enum Square
// {
//   A1=0  , B1, C1, D1, E1, F1, G1, H1,
//   A2=8  , B2, C2, D2, E2, F2, G2, H2,
//   A3=16 , B3, C3, D3, E3, F3, G3, H3,
//   A4=24 , B4, C4, D4, E4, F4, G4, H4,
//   A5=32 , B5, C5, D5, E5, F5, G5, H5,
//   A6=40 , B6, C6, D6, E6, F6, G6, H6,
//   A7=48 , B7, C7, D7, E7, F7, G7, H7,
//   A8=56 , B8, C8, D8, E8, F8, G8, H8
// };

// // Constructor
// Board8x8::Board8x8()
// {
//   init();
//   initAttacks();
// }

// Board8x8::~Board8x8()
// {

// }

// bool Board8x8::checkNonSliderAttack(uint8_t index, const uint8_t attackVector[][8], Color attackingColor, Piece piece) const
// {
//   for (int i = 0; i < 8; i++) {
//     uint8_t destIndex = attackVector[index][i];
//     if (destIndex != INVALID_SQUARE && mColors[destIndex] == attackingColor && mPieces[destIndex] == piece) {
//       return true;
//     }
//   }
//   return false;
// }

// bool Board8x8::checkPawnAttack(uint8_t index, const uint8_t attackVector[][2], Color attackingColor) const
// {
//   for (int i = 0; i < 2; i++) {
//     uint8_t destIndex = attackVector[index][i];
//     if (destIndex != INVALID_SQUARE && mColors[destIndex] == attackingColor && mPieces[destIndex] == Piece::Pawn) {
//       return true;
//     }
//   }
//   return false;
// }

// bool Board8x8::checkSliderAttack(char row, char col, char rowIncr, char colIncr, Color attackingColor, Piece piece1, Piece piece2) const
// {
//   for (int i = 0; i < 8; i++) {
//     char destRow = row + rowIncr*(i+1);
//     char destCol = col + colIncr*(i+1);
//     if (destRow < 0 || destRow > 7 || destCol < 0 || destCol > 7) {
//       break;
//     }

//     uint8_t destIndex = getIndex(destRow, destCol);

//     // Piece of the same color
//     if (mColors[destIndex] == !attackingColor) {
//       break;
//     }

//     // Found an attacking piece
//     if (mColors[destIndex] == attackingColor) {
//       if (mPieces[destIndex] == piece1 || mPieces[destIndex] == piece2) {
//         return true;
//       }
//       else {
//         // This type of piece cannot attack the square from its current position
//         break;
//       }
//     }
//   }
//   return false;
// }

// void Board8x8::generateCastlingMoves(MoveList & moveList) const
// {
//   if (sideToMove() == Color::White) {
//     if (castlingRights() & CASTLE_WHITE_KING) {
//       if (mPieces[Square::F1] == Piece::None &&
//           mPieces[Square::G1] == Piece::None &&
//           !isCellAttacked(Square::E1, Color::Black) &&
//           !isCellAttacked(Square::F1, Color::Black) &&
//           !isCellAttacked(Square::G1, Color::Black))
//       {
//         pushMove(E1, G1, Piece::King, Piece::None, Piece::None, Move::Type::Castle, moveList);
//       }
//     }
//     if (castlingRights() & CASTLE_WHITE_QUEEN) {
//       if (mPieces[Square::D1] == Piece::None &&
//           mPieces[Square::C1] == Piece::None &&
//           mPieces[Square::B1] == Piece::None &&
//           !isCellAttacked(Square::E1, Color::Black) &&
//           !isCellAttacked(Square::D1, Color::Black) &&
//           !isCellAttacked(Square::C1, Color::Black))
//       {
//         pushMove(E1, C1, Piece::King, Piece::None, Piece::None, Move::Type::Castle, moveList);
//       }
//     }
//   }
//   else {
//     if (castlingRights() & CASTLE_BLACK_KING) {
//       if (mPieces[Square::F8] == Piece::None &&
//           mPieces[Square::G8] == Piece::None &&
//           !isCellAttacked(Square::E8, Color::White) &&
//           !isCellAttacked(Square::F8, Color::White) &&
//           !isCellAttacked(Square::G8, Color::White))
//       {
//         pushMove(E8, G8, Piece::King, Piece::None, Piece::None, Move::Type::Castle, moveList);
//       }
//     }
//     if (castlingRights() & CASTLE_BLACK_QUEEN) {
//       if (mPieces[Square::D8] == Piece::None &&
//           mPieces[Square::C8] == Piece::None &&
//           mPieces[Square::B8] == Piece::None &&
//           !isCellAttacked(Square::E8, Color::White) &&
//           !isCellAttacked(Square::D8, Color::White) &&
//           !isCellAttacked(Square::C8, Color::White))
//       {
//         pushMove(E8, C8, Piece::King, Piece::None, Piece::None, Move::Type::Castle, moveList);
//       }
//     }
//   }
// }

// void Board8x8::generateMoves(MoveList & moveList) const
// {
//   Color side = sideToMove();

//   // Generate castling moves
//   generateCastlingMoves(moveList);

//   // Generate all other moves
//   for (uint8_t row = 0; row < 8; row++) {
//     for (uint8_t col = 0; col < 8; col++) {
//       uint8_t sq = getIndex(row, col);
//       if (mColors[sq] == side) {
//         Piece piece = mPieces[sq];
//         switch (piece) {
//         case Piece::Pawn:
//           generatePawnMoves(sq, moveList);
//           if (side == Color::White)
//             generatePawnCaptures(sq, mWhitePawnAttacks, moveList);
//           else
//             generatePawnCaptures(sq, mBlackPawnAttacks, moveList);
//           break;
//         case Piece::Bishop:
//           generateSliderMoves(sq, piece, mSliderAttacksNorthEast, moveList);
//           generateSliderMoves(sq, piece, mSliderAttacksNorthWest, moveList);
//           generateSliderMoves(sq, piece, mSliderAttacksSouthEast, moveList);
//           generateSliderMoves(sq, piece, mSliderAttacksSouthWest, moveList);
//           break;
//         case Piece::Rook:
//           generateSliderMoves(sq, piece, mSliderAttacksNorth, moveList);
//           generateSliderMoves(sq, piece, mSliderAttacksSouth, moveList);
//           generateSliderMoves(sq, piece, mSliderAttacksEast, moveList);
//           generateSliderMoves(sq, piece, mSliderAttacksWest, moveList);
//           break;
//         case Piece::Queen:
//           generateSliderMoves(sq, piece, mSliderAttacksNorthEast, moveList);
//           generateSliderMoves(sq, piece, mSliderAttacksNorthWest, moveList);
//           generateSliderMoves(sq, piece, mSliderAttacksSouthEast, moveList);
//           generateSliderMoves(sq, piece, mSliderAttacksSouthWest, moveList);
//           generateSliderMoves(sq, piece, mSliderAttacksNorth, moveList);
//           generateSliderMoves(sq, piece, mSliderAttacksSouth, moveList);
//           generateSliderMoves(sq, piece, mSliderAttacksEast, moveList);
//           generateSliderMoves(sq, piece, mSliderAttacksWest, moveList);
//           break;
//         case Piece::Knight:
//           generateNonSliderMoves(sq, piece, mKnightAttacks, moveList);
//           break;
//         case Piece::King:
//           generateNonSliderMoves(sq, piece, mKingAttacks, moveList);
//           break;
//         }
//       }
//     }
//   }
// }

// void Board8x8::generateNonSliderMoves(uint8_t index, Piece piece, const uint8_t attackVector[][8], MoveList &moveList) const
// {
//   Color side = sideToMove();

//   for (uint8_t i = 0; i < 8; i++) {
//     uint8_t destIndex = attackVector[index][i];

//     // Check for valid destination
//     if (destIndex != INVALID_SQUARE) {
//       if (mPieces[destIndex] == Piece::None) { // Standard move
//         pushMove(index, destIndex, piece, Piece::None, Piece::None, Move::Type::Quiet, moveList);
//       }
//       else if (mColors[destIndex] == !side) { // Capture move
//         pushMove(index, destIndex, piece, mPieces[destIndex], Piece::None, Move::Type::Capture, moveList);
//       }
//     }
//   }
// }

// void Board8x8::generatePawnCaptures(uint8_t index, const uint8_t attackVector[][2], MoveList & moveList) const
// {
//   Color side = sideToMove();

//   for (uint8_t i = 0; i < 2; i++) {
//     uint8_t sq = attackVector[index][i];
//     uint8_t row = getRow(sq);

//     // Check for valid destination
//     if (sq != INVALID_SQUARE) {

//       // Check for enemy piece
//       if (mColors[sq] == !side) {
//         if (row == BLACK_PROMOTION_ROW || row == WHITE_PROMOTION_ROW) { // Promotion captures
//           pushMove(index, sq, Piece::Pawn, mPieces[sq], Piece::Queen, Move::Type::PromotionCapture, moveList);
//           pushMove(index, sq, Piece::Pawn, mPieces[sq], Piece::Rook, Move::Type::PromotionCapture, moveList);
//           pushMove(index, sq, Piece::Pawn, mPieces[sq], Piece::Bishop, Move::Type::PromotionCapture, moveList);
//           pushMove(index, sq, Piece::Pawn, mPieces[sq], Piece::Knight, Move::Type::PromotionCapture, moveList);
//         }
//         else { // Standard capture
//           pushMove(index, sq, Piece::Pawn, mPieces[sq], Piece::None, Move::Type::Capture, moveList);
//         }
//       }

//       if (epColumn() != BoardBase::INVALID_EP) { // En-passant capture
//         uint8_t epIndex = getIndex(epRow(), epColumn());
//         char epDir = (side == Color::White) ? SOUTH : NORTH;
//         if (epIndex == sq) {
//           pushMove(index, sq, Piece::Pawn, mPieces[sq+epDir], Piece::None, Move::Type::EpCapture, moveList);
//         }
//       }
//     }
//   }
// }

// void Board8x8::generatePawnMoves(uint8_t index, MoveList & moveList) const
// {
//   Color side = sideToMove();
//   char dir = (side == Color::White) ? NORTH : SOUTH;
//   char startRow = (side == Color::White) ? 1 : 6;

//   uint8_t sq = index+dir;
//   uint8_t sq2 = index+dir+dir;

//   // Check to see if we're blocked
//   if (mPieces[sq] == Piece::None) {

//     // Single pawn push
//     uint8_t destRow = getRow(sq);
//     if (destRow == BLACK_PROMOTION_ROW || destRow == WHITE_PROMOTION_ROW) { // Promotion moves
//       pushMove(index, sq, Piece::Pawn, Piece::None, Piece::Queen, Move::Type::Promotion, moveList);
//       pushMove(index, sq, Piece::Pawn, Piece::None, Piece::Rook, Move::Type::Promotion, moveList);
//       pushMove(index, sq, Piece::Pawn, Piece::None, Piece::Bishop, Move::Type::Promotion, moveList);
//       pushMove(index, sq, Piece::Pawn, Piece::None, Piece::Knight, Move::Type::Promotion, moveList);
//     }
//     else { // Standard move
//       pushMove(index, sq, Piece::Pawn, Piece::None, Piece::None, Move::Type::Quiet, moveList);
//     }

//     if (getRow(index) == startRow && mPieces[sq2] == Piece::None) { // Double pawn push
//       pushMove(index, sq2, Piece::Pawn, Piece::None, Piece::None, Move::Type::EpPush, moveList);
//     }
//   }
// }

// void Board8x8::generateSliderMoves(uint8_t index, Piece piece, const uint8_t attackVector[][8], MoveList & moveList) const
// {
//   Color color = sideToMove();

//   for (uint8_t i = 0; i < 8; i++) {
//     uint8_t destIndex = attackVector[index][i];

//     // Valid slider attacks are always at the beginning of the array
//     if (destIndex == INVALID_SQUARE)
//       break;

//     // We hit our own piece
//     if (mPieces[destIndex] != Piece::None && mColors[destIndex] == color)
//       break;

//     if (mPieces[destIndex] == Piece::None) { // Standard move
//       pushMove(index, destIndex, piece, Piece::None, Piece::None, Move::Type::Quiet, moveList);
//     }
//     else if (mColors[destIndex] == !color) { // Capture move, stop moving
//       pushMove(index, destIndex, piece, mPieces[destIndex], Piece::None, Move::Type::Capture, moveList);
//       break;
//     }
//   }
// }

// void Board8x8::init()
// {
//   static const Piece pieces[] = { Piece::Rook, Piece::Knight, Piece::Bishop, Piece::Queen,
//                                   Piece::King, Piece::Bishop, Piece::Knight, Piece::Rook };

//   for (uint8_t sq = 0; sq < 64; sq++) {
//     mPieces[sq] = Piece::None;
//     mColors[sq] = Color::None;
//   }

//   for (uint8_t col = 0; col < 8; col++) {
//     mPieces[getIndex(1, col)] = Piece::Pawn;
//     mColors[getIndex(1, col)] = Color::White;

//     mPieces[getIndex(0, col)] = pieces[col];
//     mColors[getIndex(0, col)] = Color::White;

//     mPieces[getIndex(6, col)] = Piece::Pawn;
//     mColors[getIndex(6, col)] = Color::Black;

//     mPieces[getIndex(7, col)] = pieces[col];
//     mColors[getIndex(7, col)] = Color::Black;
//   }
// }

// void Board8x8::initAttacks()
// {
//   for (uint8_t row = 0; row < 8; row++) {
//     for (uint8_t col = 0; col < 8; col++) {
//       uint8_t index = getIndex(row, col);

//       for (uint8_t i = 0; i < 2; i++) {
//         mWhitePawnAttacks[index][i] = INVALID_SQUARE;
//         mBlackPawnAttacks[index][i] = INVALID_SQUARE;
//       }

//       for (uint8_t i = 0; i < 8; i++) {
//         mSliderAttacksEast[index][i] = INVALID_SQUARE;
//         mSliderAttacksNorth[index][i] = INVALID_SQUARE;
//         mSliderAttacksSouth[index][i] = INVALID_SQUARE;
//         mSliderAttacksWest[index][i] = INVALID_SQUARE;
//         mSliderAttacksNorthEast[index][i] = INVALID_SQUARE;
//         mSliderAttacksNorthWest[index][i] = INVALID_SQUARE;
//         mSliderAttacksSouthEast[index][i] = INVALID_SQUARE;
//         mSliderAttacksSouthWest[index][i] = INVALID_SQUARE;
//         mKingAttacks[index][i] = INVALID_SQUARE;
//         mKnightAttacks[index][i] = INVALID_SQUARE;
//       }
//     }
//   }

//   // The loops below could probably be functionalized
//   for (char sourceRow = 0; sourceRow < 8; sourceRow++) {
//     for (char sourceCol = 0; sourceCol < 8; sourceCol++) {
//       uint8_t index = getIndex(sourceRow, sourceCol);

//       // Knight attacks
//       for (char i = 0; i < 8; i++) {
//         uint8_t destIndex = index + knightAttacks[i];
//         uint8_t destRow = getRow(destIndex);
//         uint8_t destCol = getCol(destIndex);
//         if (abs(destRow-sourceRow) <= 2 && abs(destCol-sourceCol) <= 2) {
//           if (destRow >= 0 && destRow < 8 && destCol >= 0 && destCol < 8) {
//             mKnightAttacks[index][i] = destIndex;
//           }
//         }
//       }

//       // King attacks
//       for (char i = 0; i < 8; i++) {
//         uint8_t destIndex = index + kingAttacks[i];
//         uint8_t destRow = getRow(destIndex);
//         uint8_t destCol = getCol(destIndex);

//         if (abs(destRow-sourceRow) <= 1 && abs(destCol-sourceCol) <= 1) {
//           if (destRow >= 0 && destRow < 8 && destCol >= 0 && destCol < 8) {
//             mKingAttacks[index][i] = destIndex;
//           }
//         }
//       }

//       // White pawn attacks
//       for (char i = 0; i < 2; i++) {
//         uint8_t destIndex = index + whitePawnAttacks[i];
//         uint8_t destRow = getRow(destIndex);
//         uint8_t destCol = getCol(destIndex);
//         if (abs(destRow-sourceRow) <= 1 && abs(destCol-sourceCol) <= 1) {
//           if (destRow >= 0 && destRow < 8 && destCol >= 0 && destCol < 8) {
//             mWhitePawnAttacks[index][i] = destIndex;
//           }
//         }
//       }

//       // Black pawn attacks
//       for (char i = 0; i < 2; i++) {
//         uint8_t destIndex = index + blackPawnAttacks[i];
//         uint8_t destRow = getRow(destIndex);
//         uint8_t destCol = getCol(destIndex);
//         if (abs(destRow-sourceRow) <= 1 && abs(destCol-sourceCol) <= 1) {
//           if (destRow >= 0 && destRow < 8 && destCol >= 0 && destCol < 8) {
//             mBlackPawnAttacks[index][i] = destIndex;
//           }
//         }
//       }

//       // East attacks
//       for (char i = 0; i < 8; i++) {
//         char destCol = sourceCol + (i+1);
//         if (destCol > 7)
//           break;

//         uint8_t destIndex = getIndex(sourceRow, destCol);
//         mSliderAttacksEast[index][i] = destIndex;
//       }

//       // West attacks
//       for (char i = 0; i < 8; i++) {
//         char destCol = sourceCol - (i+1);
//         if (destCol < 0)
//           break;

//         uint8_t destIndex = getIndex(sourceRow, destCol);
//         mSliderAttacksWest[index][i] = destIndex;
//       }

//       // North attacks
//       for (char i = 0; i < 8; i++) {
//         char destRow = sourceRow + (i+1);
//         if (destRow > 7)
//           break;

//         uint8_t destIndex = getIndex(destRow, sourceCol);
//         mSliderAttacksNorth[index][i] = destIndex;
//       }

//       // South attacks
//       for (char i = 0; i < 8; i++) {
//         char destRow = sourceRow - (i+1);
//         if (destRow < 0)
//           break;

//         uint8_t destIndex = getIndex(destRow, sourceCol);
//         mSliderAttacksSouth[index][i] = destIndex;
//       }

//       // North east attacks
//       for (char i = 0; i < 8; i++) {
//         char destRow = sourceRow + (i+1);
//         char destCol = sourceCol + (i+1);
//         if (destRow > 7 || destCol > 7) {
//           break;
//         }
//         uint8_t destIndex = getIndex(destRow, destCol);
//         mSliderAttacksNorthEast[index][i] = destIndex;
//       }

//       // North west attacks
//       for (char i = 0; i < 8; i++) {
//         char destRow = sourceRow + (i+1);
//         char destCol = sourceCol - (i+1);
//         if (destRow > 7 || destCol < 0) {
//           break;
//         }
//         uint8_t destIndex = getIndex(destRow, destCol);
//         mSliderAttacksNorthWest[index][i] = destIndex;
//       }

//       // South west attacks
//       for (char i = 0; i < 8; i++) {
//         char destRow = sourceRow - (i+1);
//         char destCol = sourceCol - (i+1);
//         if (destRow < 0 || destCol < 0) {
//           break;
//         }
//         uint8_t destIndex = getIndex(destRow, destCol);
//         mSliderAttacksSouthWest[index][i] = destIndex;
//       }

//       // South east attacks
//       for (char i = 0; i < 8; i++) {
//         char destRow = sourceRow - (i+1);
//         char destCol = sourceCol + (i+1);
//         if (destRow < 0 || destCol > 7) {
//           break;
//         }
//         uint8_t destIndex = getIndex(destRow, destCol);
//         mSliderAttacksSouthEast[index][i] = destIndex;
//       }
//     }
//   }
// }

// bool Board8x8::isCellAttacked(uint8_t row, uint8_t col, Color attackingColor) const
// {
//   return isCellAttacked(getIndex(row, col), attackingColor);
// }

// bool Board8x8::isCellAttacked(uint8_t index, Color attackingColor) const
// {
//   char row = getRow(index);
//   char col = getCol(index);

//   // Attacks from the north
//   if (checkSliderAttack(row, col, 1, 0, attackingColor, Piece::Rook, Piece::Queen))
//     return true;

//   // Attacks from the south
//   if (checkSliderAttack(row, col, -1, 0, attackingColor, Piece::Rook, Piece::Queen))
//     return true;

//   // Attacks from the east
//   if (checkSliderAttack(row, col, 0, 1, attackingColor, Piece::Rook, Piece::Queen))
//     return true;

//   // Attacks from the west
//   if (checkSliderAttack(row, col, 0, -1, attackingColor, Piece::Rook, Piece::Queen))
//     return true;

//   // Attacks from the north east
//   if (checkSliderAttack(row, col, 1, 1, attackingColor, Piece::Bishop, Piece::Queen))
//     return true;

//   // Attacks from the north west
//   if (checkSliderAttack(row, col, 1, -1, attackingColor, Piece::Bishop, Piece::Queen))
//     return true;

//   // Attacks from the south east
//   if (checkSliderAttack(row, col, -1, 1, attackingColor, Piece::Bishop, Piece::Queen))
//     return true;

//   // Attacks from the south west
//   if (checkSliderAttack(row, col, -1, -1, attackingColor, Piece::Bishop, Piece::Queen))
//     return true;

//   // Knight attacks
//   if (checkNonSliderAttack(index, mKnightAttacks, attackingColor, Piece::Knight))
//     return true;

//   // King attacks
//   if (checkNonSliderAttack(index, mKingAttacks, attackingColor, Piece::King))
//     return true;

//   // Pawn attacks
//   // The attack vectors are flipped since we're looking from
//   // the point of view of the attacked square
//   // For example, a black pawn attacking this square is the same
//   // as a white pawn of this square attacking the black pawns' square
//   if (attackingColor == Color::Black) {
//     if (checkPawnAttack(index, mWhitePawnAttacks, attackingColor))
//       return true;
//   }
//   else {
//     if (checkPawnAttack(index, mBlackPawnAttacks, attackingColor))
//       return true;
//   }

//   return false;
// }

// void Board8x8::makeMove(const Move & move)
// {
//   Color side = sideToMove();
//   uint8_t fromSquare = getIndex(move.sourceRow(), move.sourceCol());
//   uint8_t toSquare = getIndex(move.destRow(), move.destCol());

//   // Update the color and piece arrays
//   mColors[fromSquare] = Color::None;
//   mPieces[fromSquare] = Piece::None;
//   mColors[toSquare] = side;
//   mPieces[toSquare] = move.piece();

//   // Handle enPassant captures
//   if (move.isEpCapture()) {
//     char dir = (side == Color::White) ? SOUTH : NORTH;
//     uint8_t sq = toSquare + dir;
//     mPieces[sq] = Piece::None;
//     mColors[sq] = Color::None;
//   }

//   // Handle promotions
//   if (move.isPromotion() || move.isPromotionCapture()) {
//     mPieces[toSquare] = move.promotedPiece();
//   }

//   // Handle king move
//   if (move.piece() == Piece::King) {
//     setKingColumn(side, move.destCol());
//     setKingRow(side, move.destRow());
//   }

//   // Update the en-passant index
//   clearEpCol();
//   if (move.isEpPush()) {
//     setEpColumn(move.destCol());
//   }

//   // Update castling rights
//   // If the square associated with a king or rook is
//   // modified in any way remove the castling ability
//   // for that piece
//   uint8_t castling = castlingRights();
//   switch (fromSquare) {
//   case H1:
//     castling &= ~CASTLE_WHITE_KING;
//     break;
//   case E1:
//     castling &= ~(CASTLE_WHITE_KING|CASTLE_WHITE_QUEEN);
//     break;
//   case A1:
//     castling &= ~CASTLE_WHITE_QUEEN;
//     break;
//   case H8:
//     castling &= ~CASTLE_BLACK_KING;
//     break;
//   case E8:
//     castling &= ~(CASTLE_BLACK_KING|CASTLE_BLACK_QUEEN);
//     break;
//   case A8:
//     castling &= ~CASTLE_BLACK_QUEEN;
//     break;
//   }

//   switch (toSquare) {
//   case H1:
//     castling &= ~CASTLE_WHITE_KING;
//     break;
//   case E1:
//     castling &= ~(CASTLE_WHITE_KING|CASTLE_WHITE_QUEEN);
//     break;
//   case A1:
//     castling &= ~CASTLE_WHITE_QUEEN;
//     break;
//   case H8:
//     castling &= ~CASTLE_BLACK_KING;
//     break;
//   case E8:
//     castling &= ~(CASTLE_BLACK_KING|CASTLE_BLACK_QUEEN);
//     break;
//   case A8:
//     castling &= ~CASTLE_BLACK_QUEEN;
//     break;
//   }

//   setCastlingRights(castling);

//   // Handle castle moves
//   if (move.isCastle()) {
//     switch (toSquare) {
//     case C1:
//       mPieces[D1] = Piece::Rook;
//       mColors[D1] = Color::White;
//       mPieces[A1] = Piece::None;
//       mColors[A1] = Color::None;
//       break;
//     case G1:
//       mPieces[F1] = Piece::Rook;
//       mColors[F1] = Color::White;
//       mPieces[H1] = Piece::None;
//       mColors[H1] = Color::None;
//       break;
//     case C8:
//       mPieces[D8] = Piece::Rook;
//       mColors[D8] = Color::Black;
//       mPieces[A8] = Piece::None;
//       mColors[A8] = Color::None;
//       break;
//     case G8:
//       mPieces[F8] = Piece::Rook;
//       mColors[F8] = Color::Black;
//       mPieces[H8] = Piece::None;
//       mColors[H8] = Color::None;
//       break;
//     }
//   }

//   // Update board state
//   incrementHalfMoveClock();
//   if (move.piece() == Piece::Pawn || move.isCapture())
//     setHalfMoveClock(0);

//   toggleSideToMove();
// }

// PieceType Board8x8::pieceType(uint8_t row, uint8_t col) const
// {
//   uint8_t index = getIndex(row, col);
//   Piece piece = mPieces[index];
//   Color color = mColors[index];

//   PieceType type = PieceType::None;
//   switch (piece) {
//   case Piece::Pawn:
//     type = (color == Color::White) ? PieceType::WhitePawn : PieceType::BlackPawn;
//     break;
//   case Piece::Rook:
//     type = (color == Color::White) ? PieceType::WhiteRook : PieceType::BlackRook;
//     break;
//   case Piece::Knight:
//     type = (color == Color::White) ? PieceType::WhiteKnight : PieceType::BlackKnight;
//     break;
//   case Piece::Bishop:
//     type = (color == Color::White) ? PieceType::WhiteBishop : PieceType::BlackBishop;
//     break;
//   case Piece::Queen:
//     type = (color == Color::White) ? PieceType::WhiteQueen : PieceType::BlackQueen;
//     break;
//   case Piece::King:
//     type = (color == Color::White) ? PieceType::WhiteKing : PieceType::BlackKing;
//     break;
//   default:
//     break;
//   }

//   return type;
// }

// void Board8x8::pushMove(uint8_t from, uint8_t to, Piece piece, Piece capture, Piece promote, Move::Type type, MoveList & moveList) const
// {
//   Move move(getRow(from), getCol(from), getRow(to), getCol(to), piece);
//   move.setCastlingRights(castlingRights());
//   move.setHalfMoveClock(halfMoveClock());
//   move.setFullMoveCounter(fullMoveCounter());
//   move.setType(type);
//   move.setCapturedPiece(capture);
//   move.setPromotedPiece(promote);
//   move.setEnpassantColumn(epColumn());

//   moveList.addMove(move);
// }

// void Board8x8::setPosition(const std::string & fenString)
// {
//   Fen fen;
//   fen.readString(fenString);

//   Color sideMoving = (fen.whiteToMove()) ? Color::White : Color::Black;

//   // Set the castling rights
//   uint8_t castling = CASTLE_NONE;
//   if (fen.canBlackCastleKingSide())
//     castling |= CASTLE_BLACK_KING;
//   if (fen.canBlackCastleQueenSide())
//     castling |= CASTLE_BLACK_QUEEN;
//   if (fen.canWhiteCastleKingSide())
//     castling |= CASTLE_WHITE_KING;
//   if (fen.canWhiteCastleQueenSide())
//     castling |= CASTLE_WHITE_QUEEN;

//   setCastlingRights(castling);

//   // Set board state
//   setHalfMoveClock(fen.halfMoveClock());
//   setFullMoveCounter(fen.fullMoveCounter());
//   setSideToMove(sideMoving);

//   // Set the enPassant data
//   clearEpCol();
//   if (fen.epColumn() >= 0) {
//     setEpColumn(fen.epColumn());
//   }

//   // Set the pieces
//   for (uint8_t row = 0; row < 8; row++) {
//     for (uint8_t col = 0; col < 8; col++) {
//       PieceType type = fen.pieceType(row, col);
//       uint8_t sq = getIndex(row, col);
//       switch (type) {
//       case PieceType::WhitePawn:
//         mPieces[sq] = Piece::Pawn;
//         mColors[sq] = Color::White;
//         break;
//       case PieceType::WhiteRook:
//         mPieces[sq] = Piece::Rook;
//         mColors[sq] = Color::White;
//         break;
//       case PieceType::WhiteKnight:
//         mPieces[sq] = Piece::Knight;
//         mColors[sq] = Color::White;
//         break;
//       case PieceType::WhiteBishop:
//         mPieces[sq] = Piece::Bishop;
//         mColors[sq] = Color::White;
//         break;
//       case PieceType::WhiteQueen:
//         mPieces[sq] = Piece::Queen;
//         mColors[sq] = Color::White;
//         break;
//       case PieceType::WhiteKing:
//         mPieces[sq] = Piece::King;
//         mColors[sq] = Color::White;
//         setKingRow(Color::White, row);
//         setKingColumn(Color::White, col);
//         break;
//       case PieceType::BlackPawn:
//         mPieces[sq] = Piece::Pawn;
//         mColors[sq] = Color::Black;
//         break;
//       case PieceType::BlackRook:
//         mPieces[sq] = Piece::Rook;
//         mColors[sq] = Color::Black;
//         break;
//       case PieceType::BlackKnight:
//         mPieces[sq] = Piece::Knight;
//         mColors[sq] = Color::Black;
//         break;
//       case PieceType::BlackBishop:
//         mPieces[sq] = Piece::Bishop;
//         mColors[sq] = Color::Black;
//         break;
//       case PieceType::BlackQueen:
//         mPieces[sq] = Piece::Queen;
//         mColors[sq] = Color::Black;
//         break;
//       case PieceType::BlackKing:
//         mPieces[sq] = Piece::King;
//         mColors[sq] = Color::Black;
//         setKingRow(Color::Black, row);
//         setKingColumn(Color::Black, col);
//         break;
//       default:
//         mPieces[sq] = Piece::None;
//         mColors[sq] = Color::None;
//         break;
//       }
//     }
//   }
// }

// void Board8x8::unmakeMove(const Move & move)
// {
//   Color side = sideToMove();
//   uint8_t fromSquare = getIndex(move.sourceRow(), move.sourceCol());
//   uint8_t toSquare = getIndex(move.destRow(), move.destCol());
//   Piece piece = move.piece();

//   // Reset the board state from the move
//   setFullMoveCounter(move.fullMoveCounter());
//   setHalfMoveClock(move.halfMoveClock());
//   setCastlingRights(move.castlingRights());
//   setEpColumn(move.enPassantCol());

//   // Update the piece and color arrays
//   mPieces[fromSquare] = piece;
//   mColors[fromSquare] = !side;
//   mPieces[toSquare] = Piece::None;
//   mColors[toSquare] = Color::None;

//   // Handle captures
//   if (move.isCapture()) {
//     uint8_t sq = toSquare;
//     if (move.isEpCapture()) {
//       char dir = (side == Color::White) ? NORTH : SOUTH;
//       sq = toSquare + dir;
//     }

//     mPieces[sq] = move.capturePiece();
//     mColors[sq] = side;
//   }

//   // Handle promotions
//   if (move.isPromotion() || move.isPromotionCapture()) {
//     mPieces[fromSquare] = Piece::Pawn;
//   }

//   // Handle king moves
//   if (piece == Piece::King) {
//     setKingRow(!side, move.sourceRow());
//     setKingColumn(!side, move.sourceCol());
//   }

//   // Handle castling move
//   if (move.isCastle()) {
//     switch (toSquare) {
//     case C1:
//       mPieces[A1] = Piece::Rook;
//       mColors[A1] = Color::White;
//       mPieces[D1] = Piece::None;
//       mColors[D1] = Color::None;
//       break;
//     case G1:
//       mPieces[H1] = Piece::Rook;
//       mColors[H1] = Color::White;
//       mPieces[F1] = Piece::None;
//       mColors[F1] = Color::None;
//       break;
//     case C8:
//       mPieces[A8] = Piece::Rook;
//       mColors[A8] = Color::Black;
//       mPieces[D8] = Piece::None;
//       mColors[D8] = Color::None;
//       break;
//     case G8:
//       mPieces[H8] = Piece::Rook;
//       mColors[H8] = Color::Black;
//       mPieces[F8] = Piece::None;
//       mColors[F8] = Color::None;
//       break;
//     }
//   }

//   toggleSideToMove();
// }

}
