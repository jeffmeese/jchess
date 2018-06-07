#ifndef BITBOARD_H
#define BITBOARD_H

#include "boardbase.h"

class BitBoard
    : public BoardBase
{
  // Construction
public:
  BitBoard();

  // Methods
public:
  void generateMoves(MoveList & moveList);
  bool isCellAttacked(uchar row, uchar col, Color attackingColor) const;
  void makeMove(const Move & move);
  PieceType pieceType(uchar row, uchar col) const;
  void print(std::ostream & ouput) const;
  void setPosition(const std::string & fenString);
  void unmakeMove(const Move & move);

  // Implementation
private:
  void init();

  // Members
private:
  ulonglong mWhitePawns;
  ulonglong mBlackPawns;
  ulonglong mWhitePieces;
  ulonglong mBlackPieces;
  ulonglong mEmptySquares;
};

#endif // BITBOARD_H
