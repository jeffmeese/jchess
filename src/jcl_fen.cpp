/*!
 * \file jcl_fen.cpp
 *
 * This file contains the implementation for the Fen object
 */

#include "jcl_fen.h"

#include <map>
#include <iostream>
#include <sstream>
#include <vector>

#include "jcl_board.h"
#include "jcl_util.h"

namespace jcl
{

std::vector<std::string> splitString(const std::string & s, const std::string & delim = " ")
{
  std::size_t startPos = 0;
  std::vector<std::string> tokens;
  while (true)
  {
    std::size_t endPos = s.find_first_of(delim, startPos);
    if (endPos == std::string::npos)
    {
      tokens.push_back(s.substr(startPos));
      break;
    }

    tokens.push_back(s.substr(startPos, endPos-startPos));
    startPos = endPos + 1;
  }

  return tokens;
}

Fen::Fen()
{
  init();
}

uint8_t Fen::getCastlingRights() const
{
  return mCastlingRights;
}

uint8_t Fen::getEnPassantColumn() const
{
  return mEnPassantColumn;
}

uint32_t Fen::getFullMoveCounter() const
{
  return mFullMoveCounter;
}

uint32_t Fen::getHalfMoveClock() const
{
  return mHalfMoveClock;
}

Color Fen::getSideToMove() const
{
  return mSideToMove;
}

void Fen::init()
{
  mCastlingRights = Board::CASTLE_BLACK_KING | Board::CASTLE_BLACK_QUEEN | Board::CASTLE_WHITE_KING | Board::CASTLE_BLACK_QUEEN;
  mSideToMove = Color::White;
  mEnPassantColumn = Board::INVALID_ENPASSANT_COLUMN;
  mHalfMoveClock = 0;
  mFullMoveCounter = 1;

  for (uint8_t i = 0; i < 8; i++)
  {
    for (uint8_t j = 0; j < 8; j++)
    {
      mPieceType[i][j] = PieceType::None;
    }
  }

  for (uint8_t i = 0; i < 8; i++)
  {
    mPieceType[1][i] = PieceType::WhitePawn;
    mPieceType[6][i] = PieceType::BlackPawn;
  }

  mPieceType[0][0] = mPieceType[0][7] = PieceType::WhiteRook;
  mPieceType[0][1] = mPieceType[0][6] = PieceType::WhiteKnight;
  mPieceType[0][2] = mPieceType[0][5] = PieceType::WhiteBishop;
  mPieceType[0][3] = PieceType::WhiteQueen;
  mPieceType[0][4] = PieceType::WhiteKing;

  mPieceType[7][0] = mPieceType[7][7] = PieceType::BlackRook;
  mPieceType[7][1] = mPieceType[7][6] = PieceType::BlackKnight;
  mPieceType[7][2] = mPieceType[7][5] = PieceType::BlackBishop;
  mPieceType[7][3] = PieceType::BlackQueen;
  mPieceType[7][4] = PieceType::BlackKing;
}

PieceType Fen::getPieceType(uint8_t row, uint8_t col) const
{
  return mPieceType[row][col];
}

void Fen::setCasltingRights(uint8_t value)
{
  mCastlingRights = value;
}

bool Fen::setFromBoard(const Board * board)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    for (uint8_t j = 0; j < 8; j++)
    {
      mPieceType[i][j] = board->getPieceType(i, j);
    }
  }

  mCastlingRights = board->getCastlingRights();
  mEnPassantColumn = board->getEnpassantColumn();
  mFullMoveCounter = board->getFullMoveNumber();
  mHalfMoveClock = board->getHalfMoveClock();
  mSideToMove = board->getSideToMove();

  return true;
}

bool Fen::setFromString(const std::string &fenString)
{
  init();

  std::string s(fenString);
  std::vector<std::string> tokens = splitString(trim(s));
  if (tokens.size() != 6)
  {
    std::cerr << "Invalid FEN string: " << fenString << std::endl;
    return false;
  }

  std::string pieceString = tokens[0];
  std::string sideString = tokens[1];
  std::string castleString = tokens[2];
  std::string epString = tokens[3];
  std::string halfMoveString = tokens[4];
  std::string fullMoveString = tokens[5];

  std::vector<std::string> pieceTokens = splitString(pieceString, "/");
  if (pieceTokens.size() != 8)
  {
    std::cerr << "Invalid number of tokens in piece data in FEN string: " << pieceString << std::endl;
    return false;
  }

  std::size_t pos = pieceString.find_first_not_of("rnbqkpRNBQKP12345678/");
  if (pos != std::string::npos)
  {
    std::cerr << "Invalid piece data in FEN string: " << pieceString << std::endl;
    return false;
  }

  pos = sideString.find_first_not_of("bw");
  if (pos != std::string::npos)
  {
    std::cerr << "Invalid side data in FEN string: " << sideString << std::endl;
    return false;
  }

  pos = castleString.find_first_not_of("KQkq-");
  if (pos != std::string::npos)
  {
    std::cerr << "Invalid castle data in FEN string: " << castleString << std::endl;
    return false;
  }

  pos = epString.find_first_not_of("abcdefgh36-");
  if (pos != std::string::npos)
  {
    std::cerr << "Invalid en-passant data in FEN string: " << epString << std::endl;
    return false;
  }

  for (uint8_t i = 0; i < 8; i++)
  {
    for (uint8_t j = 0; j < 8; j++)
    {
      mPieceType[i][j] = PieceType::None;
    }
  }

  for (int8_t i = 0; i < 8; i++)
  {
    uint8_t columnIndex = 0;
    std::string rowString = pieceTokens[i];
    for (std::size_t j = 0; j < rowString.size(); j++)
    {
      if (rowString[j] == 'P')
      {
        mPieceType[7-i][columnIndex] = PieceType::WhitePawn;
        columnIndex++;
      }
      else if (rowString[j] == 'R')
      {
        mPieceType[7-i][columnIndex] = PieceType::WhiteRook;
        columnIndex++;
      }
      else if (rowString[j] == 'N')
      {
        mPieceType[7-i][columnIndex] = PieceType::WhiteKnight;
        columnIndex++;
      }
      else if (rowString[j] == 'B')
      {
        mPieceType[7-i][columnIndex] = PieceType::WhiteBishop;
        columnIndex++;
      }
      else if (rowString[j] == 'Q')
      {
        mPieceType[7-i][columnIndex] = PieceType::WhiteQueen;
        columnIndex++;
      }
      else if (rowString[j] == 'K')
      {
        mPieceType[7-i][columnIndex] = PieceType::WhiteKing;
        columnIndex++;
      }
      else if (rowString[j] == 'p')
      {
        mPieceType[7-i][columnIndex] = PieceType::BlackPawn;
        columnIndex++;
      }
      else if (rowString[j] == 'r')
      {
        mPieceType[7-i][columnIndex] = PieceType::BlackRook;
        columnIndex++;
      }
      else if (rowString[j] == 'n')
      {
        mPieceType[7-i][columnIndex] = PieceType::BlackKnight;
        columnIndex++;
      }
      else if (rowString[j] == 'b')
      {
        mPieceType[7-i][columnIndex] = PieceType::BlackBishop;
        columnIndex++;
      }
      else if (rowString[j] == 'q')
      {
        mPieceType[7-i][columnIndex] = PieceType::BlackQueen;
        columnIndex++;
      }
      else if (rowString[j] == 'k')
      {
        mPieceType[7-i][columnIndex] = PieceType::BlackKing;
        columnIndex++;
      }
      else
      {
        int32_t value = rowString[j] - '0';
        columnIndex += value;
      }
    }
  }

  mSideToMove = (sideString == "w") ? Color::White : Color::Black;

  mCastlingRights = Board::CASTLE_NONE;
  if (castleString != "-")
  {
    if (castleString.find_first_of("K") != std::string::npos)
    {
      mCastlingRights |= Board::CASTLE_WHITE_KING;
    }

    if (castleString.find_first_of("Q") != std::string::npos)
    {
      mCastlingRights |= Board::CASTLE_WHITE_QUEEN;
    }

    if (castleString.find_first_of("k") != std::string::npos)
    {
      mCastlingRights |= Board::CASTLE_BLACK_KING;
    }

    if (castleString.find_first_of("q") != std::string::npos)
    {
      mCastlingRights |= Board::CASTLE_BLACK_QUEEN;
    }
  }

  mEnPassantColumn = Board::INVALID_ENPASSANT_COLUMN;
  if (epString != "-")
  {
    std::map<std::string, uint8_t> epMap;
    epMap["a"] = 0;
    epMap["b"] = 1;
    epMap["c"] = 2;
    epMap["d"] = 3;
    epMap["e"] = 4;
    epMap["f"] = 5;
    epMap["g"] = 6;
    epMap["h"] = 7;

    std::string epColumn = epString.substr(0,1);
    mEnPassantColumn = epMap[epColumn];
  }

  mHalfMoveClock = std::atoi(halfMoveString.c_str());
  mFullMoveCounter = std::atoi(fullMoveString.c_str());

  return true;
}

void Fen::setFullMoveCounter(uint32_t value)
{
  mFullMoveCounter = value;
}

void Fen::setHalfMoveClock(uint32_t value)
{
  mHalfMoveClock = value;
}

void Fen::setPieceType(uint8_t row, uint8_t col, PieceType pieceType)
{
  mPieceType[row][col] = pieceType;
}

void Fen::setSideToMove(Color value)
{
  mSideToMove = value;
}

std::string Fen::toString() const
{
  static std::map<PieceType, std::string> pieceToString;
  static std::map<uint8_t, std::string> enPassantMap;
  if (pieceToString.size() == 0)
  {
    pieceToString[PieceType::WhitePawn] = "P";
    pieceToString[PieceType::WhiteRook] = "R";
    pieceToString[PieceType::WhiteKnight] = "N";
    pieceToString[PieceType::WhiteBishop] = "B";
    pieceToString[PieceType::WhiteQueen] = "Q";
    pieceToString[PieceType::WhiteKing] = "K";
    pieceToString[PieceType::BlackPawn] = "p";
    pieceToString[PieceType::BlackRook] = "r";
    pieceToString[PieceType::BlackKnight] = "n";
    pieceToString[PieceType::BlackBishop] = "b";
    pieceToString[PieceType::BlackQueen] = "q";
    pieceToString[PieceType::BlackKing] = "k";

    enPassantMap[0] = "a";
    enPassantMap[1] = "b";
    enPassantMap[2] = "c";
    enPassantMap[3] = "d";
    enPassantMap[4] = "e";
    enPassantMap[5] = "f";
    enPassantMap[6] = "g";
    enPassantMap[7] = "h";
  }

  std::ostringstream oss;
  for (int8_t i = 7; i >= 0; i--)
  {
    uint32_t spaces = 0;
    for (int8_t j = 0; j < 8; j++)
    {
      PieceType pieceType = mPieceType[i][j];
      if (pieceType == PieceType::None)
      {
        spaces++;
      }
      else
      {
        std::string s = pieceToString[pieceType];
        if (spaces > 0)
        {
          oss << spaces;
        }

        oss << s;
        spaces = 0;
      }
    }

    if (spaces > 0)
    {
      oss << spaces;
    }

    if (i > 0)
    {
      oss << "/";
    }
  }

  oss << ((mSideToMove == Color::White) ? " w " : " b ");

  std::string castleString;
  if (mCastlingRights & Board::CASTLE_WHITE_KING)
  {
    castleString += "K";
  }

  if (mCastlingRights & Board::CASTLE_WHITE_QUEEN)
  {
    castleString += "Q";
  }

  if (mCastlingRights & Board::CASTLE_BLACK_KING)
  {
    castleString += "k";
  }

  if (mCastlingRights & Board::CASTLE_BLACK_QUEEN)
  {
    castleString += "q";
  }

  if (castleString.empty())
  {
    castleString = "-";
  }
  oss << castleString << " ";

  std::string enPassantString;
  if (mEnPassantColumn != Board::INVALID_ENPASSANT_COLUMN)
  {
    std::string s = enPassantMap[mEnPassantColumn];
    std::ostringstream ossEp;
    if (mSideToMove == Color::White)
    {
      ossEp << s << "6";
    }
    else
    {
      ossEp << s << "3";
    }

    enPassantString = ossEp.str();
  }

  if (enPassantString.empty())
  {
    enPassantString = "-";
  }
  oss << enPassantString << " ";

  oss << mHalfMoveClock << " ";
  oss << mFullMoveCounter;

  return oss.str();
}

}
