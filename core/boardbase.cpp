#include "boardbase.h"

#include <map>
#include <sstream>
#include <string>

const uchar BoardBase::INVALID_EP = 8;
const uchar BoardBase::WHITE_INDEX = 0;
const uchar BoardBase::BLACK_INDEX = 1;
const uchar BoardBase::BLACK_PROMOTION_ROW = 0;
const uchar BoardBase::WHITE_PROMOTION_ROW = 7;

const uchar BoardBase::CASTLE_NONE        = 0x00;
const uchar BoardBase::CASTLE_WHITE_KING  = 0x01;
const uchar BoardBase::CASTLE_WHITE_QUEEN = 0x02;
const uchar BoardBase::CASTLE_BLACK_KING  = 0x04;
const uchar BoardBase::CASTLE_BLACK_QUEEN = 0x08;

BoardBase::BoardBase()
{
  init();
}

void BoardBase::init()
{
  mCastlingRights = CASTLE_BLACK_KING | CASTLE_BLACK_QUEEN | CASTLE_WHITE_KING | CASTLE_WHITE_QUEEN;
  mFullMoveCounter = 0;
  mHalfMoveClock = 0;
  mSideToMove = Color::White;
  mEpCol = INVALID_EP;
  mKingRow[WHITE_INDEX] = 0;
  mKingCol[WHITE_INDEX] = 4;
  mKingRow[BLACK_INDEX] = 7;
  mKingCol[BLACK_INDEX] = 3;
}

void BoardBase::print(std::ostream & output) const
{
  std::map<PieceType, std::string> pieceMap;
  pieceMap[PieceType::None] = std::string("   ");
  pieceMap[PieceType::WhitePawn] = std::string(" P ");
  pieceMap[PieceType::WhiteRook] = std::string(" R ");
  pieceMap[PieceType::WhiteKnight] = std::string(" N ");
  pieceMap[PieceType::WhiteBishop] = std::string(" B ");
  pieceMap[PieceType::WhiteQueen] = std::string(" Q ");
  pieceMap[PieceType::WhiteKing] = std::string("[K]");
  pieceMap[PieceType::BlackPawn] = std::string(" p ");
  pieceMap[PieceType::BlackRook] = std::string(" r ");
  pieceMap[PieceType::BlackKnight] = std::string(" n ");
  pieceMap[PieceType::BlackBishop] = std::string(" b ");
  pieceMap[PieceType::BlackQueen] = std::string(" q ");
  pieceMap[PieceType::BlackKing] = std::string("[k]");

  output << "\n";
  for (int row = 7; row >= 0; row--) {
    output << " +---+---+---+---+---+---+---+---+\n";
    output << row + 1 << "|";

    for (int col = 0; col < 8; col++) {
      PieceType type = pieceType(row, col);
      output << pieceMap[type];
      output << "|";

      if (col == 7) {
        if (row == 7)
          output << " To Move: " << ( sideToMove() == Color::White ? "White" : "Black");

        if (row == 6) {
          uchar castling = castlingRights();
          output << " Castling: ";
          if (castling & CASTLE_WHITE_KING)
            output << "K";
          if (castling & CASTLE_WHITE_QUEEN)
            output << "Q";
          if (castling & CASTLE_BLACK_KING)
            output << "k";
          if (castling & CASTLE_BLACK_QUEEN)
            output << "q";
        }
      }
    }
    output << "\n";
  }

  output << " +---+---+---+---+---+---+---+---+\n";
  output << "   A   B   C   D   E   F   G   H  \n";
  output << "\n";

  output << "FEN: " << writeFenString() << "\n";
}

std::string BoardBase::writeFenString() const
{
  std::map<PieceType, std::string> pieceMap;
  pieceMap[PieceType::WhitePawn] = std::string("P");
  pieceMap[PieceType::WhiteRook] = std::string("R");
  pieceMap[PieceType::WhiteKnight] = std::string("N");
  pieceMap[PieceType::WhiteBishop] = std::string("B");
  pieceMap[PieceType::WhiteQueen] = std::string("Q");
  pieceMap[PieceType::WhiteKing] = std::string("K");
  pieceMap[PieceType::BlackPawn] = std::string("p");
  pieceMap[PieceType::BlackRook] = std::string("r");
  pieceMap[PieceType::BlackKnight] = std::string("n");
  pieceMap[PieceType::BlackBishop] = std::string("b");
  pieceMap[PieceType::BlackQueen] = std::string("q");
  pieceMap[PieceType::BlackKing] = std::string("k");

  std::ostringstream oss;

  for (uint i = 0; i < 8; i++) {
    uint row = 7-i;
    uint col = 0;
    while (col < 8) {
      PieceType pieceType = this->pieceType(row,col);
      if (pieceType != PieceType::None) {
        oss << pieceMap[pieceType];
        ++col;
      }
      else {
        uint totalBlanks = 0;
        while (pieceType == PieceType::None && col < 8) {
          ++totalBlanks;
          ++col;
          pieceType = this->pieceType(row,col);
        }
        oss << totalBlanks;
      }
    }
    if (row != 0)
      oss << "/";
  }

  oss << " " << (this->sideToMove() == Color::White ? "w" : "b");

  std::string castleString;
  uchar castlingRights = this->castlingRights();
  if (castlingRights & CASTLE_WHITE_KING)
    castleString += "K";
  if (castlingRights & CASTLE_WHITE_QUEEN)
    castleString += "Q";
  if (castlingRights & CASTLE_BLACK_KING)
    castleString += "k";
  if (castlingRights & CASTLE_BLACK_QUEEN)
    castleString += "q";
  if (castleString.empty())
    castleString += "-";

  oss << " " << castleString;

  std::ostringstream epString;
  int epRow = this->epRow();
  int epCol = this->epColumn();
  if (epCol != INVALID_EP) {
    if (epRow >= 0 && epCol >= 0) {
      switch (epCol)
      {
      case 0:
        epString << "a";
        break;
      case 1:
        epString << "b";
        break;
      case 2:
        epString << "c";
        break;
      case 3:
        epString << "d";
        break;
      case 4:
        epString << "e";
        break;
      case 5:
        epString << "f";
        break;
      case 6:
        epString << "g";
        break;
      case 7:
        epString << "h";
        break;
      }
      epString << (epRow + 1);
    }
  }

  if (epString.str().empty())
    epString << "-";

  oss << " " << epString.str();
  oss << " " << halfMoveClock();
  oss << " " << fullMoveCounter();

  return oss.str();
}
