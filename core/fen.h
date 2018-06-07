#ifndef FEN_H
#define FEN_H

#include "types.h"

#include <iostream>
#include <string>

class Fen
{
public:
  Fen();

public:
  bool canBlackCastleKingSide() const;
  bool canBlackCastleQueenSide() const;
  bool canWhiteCastleKingSide() const;
  bool canWhiteCastleQueenSide() const;
  int epColumn() const;
  uint fullMoveCounter() const;
  uint halfMoveClock() const;
  PieceType pieceType(uchar row, uchar col) const;
  bool whiteToMove() const;
  void setCanBlackCastleKingSide(bool castle);
  void setCanBlackCastleQueenSide(bool castle);
  void setCanWhiteCastleKingSide(bool castle);
  void setCanWhiteCastleQueenSide(bool castle);
  void setEpColumn(int column);
  void setFullMoveCounter(uint value);
  void setHalfMoveClock(uint value);
  void setPieceType(uchar row, uchar col, PieceType pieceType);
  void setWhiteToMove(bool whiteToMove);

public:
  void readString(const std::string & fenString);

private:
  void init();

private:
  bool mBlackCastleKingSide;
  bool mBlackCastleQueenSide;
  bool mWhiteCastleKingSide;
  bool mWhiteCastleQueenSide;
  bool mWhiteToMove;
  uint mEnPassantCol;
  uint mHalfMoveClock;
  uint mFullMoveCounter;
  PieceType mPieceType[8][8];
};

#endif // FEN_H
