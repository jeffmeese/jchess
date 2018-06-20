#ifndef BOARDBASE_H
#define BOARDBASE_H

#include "types.h"

#include <iostream>
#include <string>

// Class forwards
class Move;
class MoveList;

class BoardBase
{
  // Public constants
public:
  static const uchar INVALID_EP;
  static const uchar BLACK_PROMOTION_ROW;
  static const uchar WHITE_PROMOTION_ROW;
  static const uchar CASTLE_NONE;
  static const uchar CASTLE_WHITE_KING;
  static const uchar CASTLE_WHITE_QUEEN;
  static const uchar CASTLE_BLACK_KING;
  static const uchar CASTLE_BLACK_QUEEN;

  // Properties
public:
  bool canBlackCastleKingSide() const;
  bool canBlackCastleQueenSide() const;
  bool canWhiteCastleKingSide() const;
  bool canWhiteCastleQueenSide() const;
  uchar castlingRights() const;
  uchar epColumn() const;
  uchar epRow() const;
  uint fullMoveCounter() const;
  uint halfMoveClock() const;
  bool isBlackToMove() const;
  bool isWhiteToMove() const;
  uchar kingColumn(Color color) const;
  uchar kingRow(Color color) const;
  Color sideToMove() const;
  void setBlackToMove();
  void setCastlingRights(uchar castling);
  void setEpColumn(uchar col);
  void setFullMoveCounter(uint value);
  void setHalfMoveClock(uint value);
  void setKingColumn(Color color, uchar col);
  void setKingRow(Color color, uchar row);
  void setSideToMove(Color color);
  void setWhiteToMove();

  // Methods
public:
  void clearEpCol();
  void decrementFullMoveCounter();
  void decrementHalfMoveClock();
  void incrementFullMoveCounter();
  void incrementHalfMoveClock();
  void print(std::ostream & output = std::cout) const;
  void toggleSideToMove();
  std::string writeFenString() const;

  // Overrideables
public:
  virtual PieceType pieceType(uchar row, uchar col) const = 0;

  // Construction
protected:
  BoardBase();

  // Implementation
private:
  void init();

  // Internal constants
private:
  static const uchar WHITE_INDEX;
  static const uchar BLACK_INDEX;

  // Members
private:
  uchar mCastlingRights;
  uchar mEpCol;
  uint mFullMoveCounter;
  uint mHalfMoveClock;
  uchar mKingCol[2];
  uchar mKingRow[2];
  Color mSideToMove;
};

inline bool BoardBase::canBlackCastleKingSide() const
{
  return (mCastlingRights & CASTLE_BLACK_KING) == CASTLE_BLACK_KING;
}

inline bool BoardBase::canBlackCastleQueenSide() const
{
  return (mCastlingRights & CASTLE_BLACK_QUEEN) == CASTLE_BLACK_QUEEN;
}

inline bool BoardBase::canWhiteCastleKingSide() const
{
  return (mCastlingRights & CASTLE_WHITE_KING) == CASTLE_WHITE_KING;
}

inline bool BoardBase::canWhiteCastleQueenSide() const
{
  return (mCastlingRights & CASTLE_WHITE_QUEEN) == CASTLE_WHITE_QUEEN;
}

inline uchar BoardBase::castlingRights() const
{
  return mCastlingRights;
}

inline void BoardBase::clearEpCol()
{
  mEpCol = INVALID_EP;
}

inline void BoardBase::decrementFullMoveCounter()
{
  mFullMoveCounter--;
}

inline void BoardBase::decrementHalfMoveClock()
{
  mHalfMoveClock--;
}

inline uchar BoardBase::epColumn() const
{
  return mEpCol;
}

inline uchar BoardBase::epRow() const
{
  return (mSideToMove == Color::Black) ? 2 : 5;
}

inline uint BoardBase::fullMoveCounter() const
{
  return mFullMoveCounter;
}

inline uint BoardBase::halfMoveClock() const
{
  return mHalfMoveClock;
}

inline void BoardBase::incrementFullMoveCounter()
{
  mFullMoveCounter++;
}

inline void BoardBase::incrementHalfMoveClock()
{
  mHalfMoveClock++;
}

inline bool BoardBase::isBlackToMove() const
{
  return mSideToMove == Color::Black;
}

inline bool BoardBase::isWhiteToMove() const
{
  return (mSideToMove == Color::White);
}

inline uchar BoardBase::kingColumn(Color color) const
{
  //return mKingCol[(color == Color::White) ? WHITE_INDEX : BLACK_INDEX];
  return mKingCol[static_cast<int>(color)];
}

inline uchar BoardBase::kingRow(Color color) const
{
  //return mKingRow[(color == Color::White) ? WHITE_INDEX : BLACK_INDEX];
  return mKingRow[static_cast<int>(color)];
}

inline void BoardBase::setBlackToMove()
{
  mSideToMove = Color::Black;
}

inline void BoardBase::setCastlingRights(uchar castling)
{
  mCastlingRights = castling;
}

inline void BoardBase::setEpColumn(uchar col)
{
  mEpCol = col;
}

inline void BoardBase::setFullMoveCounter(uint value)
{
  mFullMoveCounter = value;
}

inline void BoardBase::setHalfMoveClock(uint value)
{
  mHalfMoveClock = value;
}

inline void BoardBase::setKingColumn(Color color, uchar col)
{
  //mKingCol[(color == Color::White) ? WHITE_INDEX : BLACK_INDEX] = col;
  mKingCol[static_cast<int>(color)] = col;
}

inline void BoardBase::setKingRow(Color color, uchar row)
{
  //mKingRow[(color == Color::White) ? WHITE_INDEX : BLACK_INDEX] = row;
  mKingRow[static_cast<int>(color)] = row;
}

inline void BoardBase::setSideToMove(Color color)
{
  mSideToMove = color;
}

inline void BoardBase::setWhiteToMove()
{
  mSideToMove = Color::White;
}

inline Color BoardBase::sideToMove() const
{
  return mSideToMove;
}

inline void BoardBase::toggleSideToMove()
{
  mSideToMove = !mSideToMove;
}

#endif // #ifndef BOARDBASE_H
