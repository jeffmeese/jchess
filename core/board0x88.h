#ifndef BOARD0X88_H
#define BOARD0X88_H

//#include "board.h"
#include "boardbase.h"
#include "move.h"
#include "types.h"

#include <stack>

class MoveList;

class Board0x88
    : public BoardBase
{
public:
  Board0x88();

public:
  void generateMoves(MoveList & moveList) const;
  bool isCellAttacked(uchar row, uchar col, Color attackingColor) const;
  void makeMove(const Move & move);
  PieceType pieceType(uchar row, uchar col) const;
  void setPosition(const std::string & fenString);
  void unmakeMove(const Move & move);

private:
  void generateCastlingMoves(MoveList & moveList) const;
  void generatePawnCaptures(uchar index, const char * attackVector, uchar vectorSize, MoveList & moveList) const;
  void generatePawnMoves(uchar index, MoveList & moveList) const;
  void generateNonPawnMoves(Piece piece, uchar index, const char * attackVector, uchar vectorSize, bool slider, MoveList & moveList) const;
  void init();
  bool isCellAttacked(uchar index, Color attackingColor) const;
  void pushMove(uchar from, uchar to, Piece piece, Piece capture, Piece promote, Move::Type type, MoveList & moveList) const;

private:
  struct PieceEntry
  {
    Piece piece;
    uchar sq;
  };

private:
  Piece mPieces[128];
  Color mColors[128];
  PieceEntry mWhitePieces[16];
  PieceEntry mBlackPieces[16];
  uchar mPieceIndex[128];
  std::stack<uchar> mIndexList;
};

#endif // #ifndef BOARD0X88_H
