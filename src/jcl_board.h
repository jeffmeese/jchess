#ifndef JCL_BOARD_H
#define JCL_BOARD_H

#include "jcl_move.h"
#include "jcl_movelist.h"
#include "jcl_types.h"

namespace jcl
{

class Board
{
public:
  Board();

  uint8_t getCastlingRights() const;
  uint8_t getEnpassantColumn() const;
  uint32_t getFullMoveNumber() const;
  uint32_t getHalfMoveClock() const;
  Color getSideToMove() const;
  void setCastlingRights(uint8_t value);
  void setEnPassantColumn(uint8_t value);
  void setFullMoveCounter(uint32_t value);
  void setHalfMoveClock(uint32_t value);
  void setSideToMove(Color value);

  void reset();

  virtual bool generateMoves(MoveList & moveList) const = 0;
  virtual bool generateMoves(uint8_t row, uint8_t col, MoveList & moveList) const = 0;
  virtual uint8_t getKingColumn(Color color) const = 0;
  virtual uint8_t getKingRow(Color color) const = 0;
  virtual PieceType getPieceType(uint8_t row, uint8_t col) const = 0;
  virtual bool isCellAttacked(uint8_t row, uint8_t col, Color attackColor) const = 0;
  virtual bool makeMove(const Move * move) = 0;
  virtual bool setPosition(const std::string & fenString) = 0;
  virtual bool unmakeMove(const Move * move) = 0;

protected:
  virtual void doReset() = 0;

private:
  void init();

  uint8_t mCastlingRights;
  uint8_t mEnPassantColumn;
  uint32_t mFullMoveCounter;
  uint32_t mHalfMoveClock;
  Color mSideToMove;
};

inline uint8_t Board::getCastlingRights() const
{
  return mCastlingRights;
}

inline uint8_t Board::getEnpassantColumn() const
{
  return mEnPassantColumn;
}

inline uint32_t Board::getFullMoveNumber() const
{
  return mFullMoveCounter;
}

inline uint32_t Board::getHalfMoveClock() const
{
  return mHalfMoveClock;
}

inline Color Board::getSideToMove() const
{
  return mSideToMove;
}

inline void Board::setCastlingRights(uint8_t value)
{
  mCastlingRights = value;
}

inline void Board::setEnPassantColumn(uint8_t value)
{
  mEnPassantColumn = value;
}

inline void Board::setFullMoveCounter(uint32_t value)
{
  mFullMoveCounter = value;
}

inline void Board::setHalfMoveClock(uint32_t value)
{
  mHalfMoveClock = value;
}

inline void Board::setSideToMove(Color value)
{
  mSideToMove = value;
}

}

#endif // #ifndef JCL_BOARD_H
