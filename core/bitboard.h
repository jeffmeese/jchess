#ifndef BITBOARD_H
#define BITBOARD_H

#include "boardbase.h"
#include "move.h"
#include "movelist.h"

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
  typedef unsigned long long u64;

  // Side to move
  enum Side
  {
    White = 0,
    Black = 1
  };

  // Enumeration so that we can hold the bit boards in arrays
  enum Type
  {
    WhitePieces = 0,
    BlackPieces = 1,
    WhitePawn = 2,
    BlackPawn = 3,
    WhiteRook = 4,
    BlackRook = 5,
    WhiteBishop = 6,
    BlackBishop = 7,
    WhiteKnight = 8,
    BlackKnight = 9,
    WhiteQueen = 10,
    BlackQueen = 11,
    WhiteKing = 12,
    BlackKing = 13,
    None = 14,
    All = 15
  };

  // Undo struct for easy retrieval of rotated bitboards
  struct Undo
  {
    u64 rotate90;
    u64 rotateR45;
    u64 rotateL45;
  };

  // Implementation
private:
  uchar bitScanForward(u64 bb) const;
  void generateBlackPawnMoves(MoveList & moveList) const;
  void generateCastlingMoves(MoveList & moveList) const;
  void generateJumpAttacks(u64 piece, const u64 * attacks, u64 friends, MoveList & moveList) const;
  void generatePawnAttacks(u64 piece, const u64 * attacks, u64 enemies, MoveList & moveList) const;
  void generateSlideAttacks(u64 piece, const u64 attacks[][256], u64 friends, u64 pieces, const uint * shifts, const uint * masks, MoveList & moveList) const;
  void generateWhitePawnMoves(MoveList & moveList) const;
  void init();
  void initBoard();
  void initAttacks();
  void initBishopAttacks();
  void initKingAttacks();
  void initKnightAttacks();
  void initPawnAttacks(Side side, char rowIncr);
  void initRookAttacks();
  u64 invRotateL45(u64 bb) const;
  u64 invRotateR45(u64 bb) const;
  bool isCellAttacked(uchar index, Color attackingColor) const;
  void pushBitBoardMoves(u64 bb, uchar fromSq, MoveList & moveList) const;
  void pushMove(uchar from, uchar to, Type piece, Type capture, Type promote, Move::Type type, MoveList & moveList) const;
  u64 rotateL90(u64 bb) const;
  u64 rotateR90(u64 bb) const;
  u64 rotateL45(u64 bb) const;
  u64 rotateR45(u64 bb) const;
  void updateAggregateBitBoards();
  void updateRotatedBitBoards();
  void writeBitBoard(u64 bb, std::ostream & output = std::cout) const;
  void writeOccupancy(uint occupancy, bool flip = false) const;

private:
  Side mSide;
  uint mUndoPos;
  u64 mRotateL45;
  u64 mRotateR45;
  u64 mRotateR90;
  u64 mPiece[16];
  Type mType[64];
  Piece mTypeToPiece[15];          //! Table of Type to Piece
  u64 mKingAttacks[64];
  u64 mKnightAttacks[64];
  u64 mMask[64];
  u64 mInvMask[64];
  u64 mPawnAttacks[2][64];
  u64 mFileAttacks[64][256];
  u64 mNorthEastAttacks[64][256];
  u64 mNorthWestAttacks[64][256];
  u64 mRankAttacks[64][256];
  Undo mUndoStack[1000];
};

#endif // BITBOARD_H
