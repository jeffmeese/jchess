#ifndef BITBOARD_H
#define BITBOARD_H

#include "boardbase.h"
#include "move.h"

#include <iostream>

class MoveList;

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
  uchar bitScanForward(U64 bb) const;
  uchar bitScanForwardPopCount(U64 bb) const;
  void generateAttackMoves(U64 bb, MoveList & moveList) const;
  void generateBlackPawnMoves(MoveList & moveList) const;
  void generateCastlingMoves(MoveList & moveList) const;
  void generateNonSliderAttacks(uchar fromSq, U64 bbAttack, U64 bbFriendly, MoveList & moveList) const;
  void generatePawnAttacks(uchar fromSq, U64 bbAttack, U64 bbOpp, MoveList & moveList) const;
  void generateSliderAttacks(uchar fromSq, U64 bbFriends, bool straight, bool diag, MoveList &moveList) const;
  void generateWhitePawnMoves(MoveList & moveList) const;
  void pushBitBoardMoves(U64 bb, uchar fromSq, MoveList & moveList) const;
  void initBoard();
  void initAttacks();
  void initDiagAttacks();
  void initStraightAttacks();
  bool isCellAttacked(uchar index, Color attackingColor) const;
  uchar popCount (U64 x) const;
  void pushMove(uchar from, uchar to, Piece piece, Piece capture, Piece promote, Move::Type type, MoveList & moveList) const;
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
  U64 mRotate90AllPieces;
  U64 mRotate45LeftPieces;
  U64 mRotate45RightPieces;
  U64 mWhiteBishops;
  U64 mWhiteKings;
  U64 mWhiteKnights;
  U64 mWhiteQueens;
  U64 mWhitePawns;
  U64 mWhitePieces;
  U64 mWhiteRooks;
  U64 mBlackPawnAttacks[64];
  U64 mKingAttacks[64];
  U64 mKnightAttacks[64];
  U64 mWhitePawnAttacks[64];
  U64 mRankAttacks[64][256];
  U64 mFileAttacks[64][256];
  U64 mDiagAttacksNorthEast[64][256];
  U64 mDiagAttacksNorthWest[64][256];
  uint mA1H8Shifts[64];
  uint mA1H8Mask[64];
  uint mA8H1Shifts[64];
  uint mA8H1Mask[64];
  Piece mPieces[64];     // Redundant piece matrix for quick lookup
  Color mColors[64];     // Redundant color matrix for quick lookup
};

#endif // BITBOARD_H
