#ifndef JCL_FEN_H
#define JCL_FEN_H

#include <string>

#include "jcl_board.h"
#include "jcl_types.h"

namespace jcl
{

class Fen
{
public:
  Fen();

public:
  uint8_t getCastlingRights() const;
  uint8_t getEnPassantColumn() const;
  uint32_t getFullMoveCounter() const;
  uint32_t getHalfMoveClock() const;
  PieceType getPieceType(uint8_t row, uint8_t col) const;
  Color getSideToMove() const;
  void setEnPassantColumn(uint8_t value);
  bool setFromBoard(const Board * board);
  bool setFromString(const std::string & fenString);
  void setFullMoveCounter(uint32_t value);
  void setHalfMoveClock(uint32_t value);
  void setPieceType(uint8_t row, uint8_t col, PieceType pieceType);
  void setSideToMove(Color value);
  std::string toString() const;

private:
  void init();

private:
  uint8_t mCastlingRights;
  uint8_t mEnPassantColumn;
  uint32_t mHalfMoveClock;
  uint32_t mFullMoveCounter;
  Color mSideToMove;
  PieceType mPieceType[8][8];
};

}

#endif // #ifndef JCL_FEN_H
