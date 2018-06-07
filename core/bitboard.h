#ifndef BITBOARD_H
#define BITBOARD_H

#include "boardbase.h"

#include <iostream>

class BitBoard
    : public BoardBase
{
  // Construction
public:
  BitBoard();

  // Methods
public:
  void generateMoves(MoveList & moveList) const;
  bool isCellAttacked(uchar row, uchar col, Color attackingColor) const;
  void makeMove(const Move & move);
  PieceType pieceType(uchar row, uchar col) const;
  void setPosition(const std::string & fenString);
  void unmakeMove(const Move & move);

  // Internal definitions
private:
  typedef ulonglong U64;

  // Implementation
private:
  int bitScanForward(U64 bb) const;
  int bitScanForwardPopCount(U64 bb) const;
  void initBoard();
  void initAttacks();
  int popCount (U64 x) const;
  U64 rotate90(U64 bb) const;
  U64 rotate45(U64 bb, const uint * rotMatrix) const;
  U64 rotateNeg45(U64 bb, const uint * rotMatrix) const;
  void writeBitBoard(U64 bitboard, std::ostream & output) const;

  // Members
private:
  U64 mAllPieces;
  U64 mBlackBishops;
  U64 mBlackKings;
  U64 mBlackKnights;
  U64 mBlackQueens;
  U64 mBlackPawns;
  U64 mBlackPieces;
  U64 mBlackRooks;
  U64 mEmptySquares;
  U64 mWhiteBishops;
  U64 mWhiteKings;
  U64 mWhiteKnights;
  U64 mWhiteQueens;
  U64 mWhitePawns;
  U64 mWhitePieces;
  U64 mWhiteRooks;
  U64 mRotate90AllPieces;
  U64 mRotate45LeftPieces;
  U64 mRotate45RightPieces;
  U64 mRotate90[64];
  U64 mRotate45Right[64];
  U64 mRotate45Left[64];
  U64 mBishopAttacks[64];
  U64 mBlackPawnAttacks[64];
  U64 mKingAttacks[64];
  U64 mKnightAttacks[64];
  U64 mRookAttacks[64];
  U64 mWhitePawnAttacks[64];
  Piece mPieces[64];     // Redundant piece matrix for quick lookup
  Color mColors[64];     // Redundant color matrix for quick lookup
};

#endif // BITBOARD_H
