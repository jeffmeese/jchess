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

private:
  typedef unsigned long long U64;

  enum Side
  {
    White = 0,
    Black = 1
  };

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

  struct Undo
  {
    U64 rotate90;
    U64 rotateR45;
    U64 rotateL45;
  };

private:
  uchar bitScanForward(U64 bb) const;
  void generateBlackPawnMoves(MoveList & moveList) const;
  void generateCastlingMoves(MoveList & moveList) const;
  void generateJumpAttacks(U64 piece, const U64 * attacks, U64 friends, MoveList & moveList) const;
  void generatePawnAttacks(U64 piece, const U64 * attacks, U64 enemies, MoveList & moveList) const;
  void generateSlideAttacks(U64 piece, const U64 attacks[][256], U64 friends, U64 pieces, const uint * shifts, const uint * masks, MoveList & moveList) const;
  void generateWhitePawnMoves(MoveList & moveList) const;
  void init();
  void initAttacks();
  void initPawnAttacks(Side side, int rowIncr);
  void initBishopAttacks();
  void initKingAttacks();
  void initKnightAttacks();
  void initRookAttacks();
  U64 invRotateL45(U64 bb) const;
  U64 invRotateR45(U64 bb) const;
  bool isCellAttacked(uchar index, Color attackingColor) const;
  void pushBitBoardMoves(U64 bb, uchar fromSq, MoveList & moveList) const;
  void pushMove(uchar from, uchar to, Type piece, Type capture, Type promote, Move::Type type, MoveList & moveList) const;
  void reset();
  U64 rotateL45(U64 bb) const;
  U64 rotateL90(U64 bb) const;
  U64 rotateR45(U64 bb) const;
  U64 rotateR90(U64 bb) const;
  void updateAggregateBitBoards();
  void updateRotatedBitBoards();
  void writeBitBoard(U64 bb, std::ostream & output = std::cout) const;
  void writeOccupancy(uint occupancy, std::ostream & output = std::cout) const;

// Testing
  void testRotations();

private:
  uint mUndoPos;
  Side mSide;
  U64 mRotateL45;
  U64 mRotateR45;
  U64 mRotateR90;
  uint mNorthEastMasks[64];
  uint mNorthWestMasks[64];
  uint mFileMasks[64];
  uint mRankMasks[64];
  Piece mTypeToPiece[15];          //! Table of Type to Piece
  U64 mPiece[16];
  U64 mMask[64];
  U64 mInvMask[64];
  U64 mInvRotR45[64];
  U64 mInvRotL45[64];
  U64 mInvRotR90[64];
  Type mType[64];
  U64 mKingAttacks[64];            //! Attack bitboards for kings for each square
  U64 mKnightAttacks[64];          //! Attack bitboards for knights for each square
  U64 mPawnAttacks[2][64];         //! Attack bitboards for pawns on each square
  U64 mFileAttacks[64][256];       //! Attack bitboards along files for all occupanies
  U64 mNorthEastAttacks[64][256];  //! Attack bitboards along a1h8 diagonal for all occupanies
  U64 mNorthWestAttacks[64][256];  //! Attack bitboards along a8h1 diagonal for all occupanies
  U64 mRankAttacks[64][256];       //! Attack bitboards along ranks for all occupanies
  Undo mUndoStack[1000];
};














//#include "boardbase.h"
//#include "move.h"
//#include "movelist.h"

//class BitBoard
//    : public BoardBase
//{
//  // Construction
//public:
//  BitBoard();

//  // Methods
//public:
//  void generateMoves(MoveList & moveList) const;
//  bool isCellAttacked(uchar row, uchar col, Color attackingColor) const;
//  void makeMove(const Move & move);
//  PieceType pieceType(uchar row, uchar col) const;
//  void setPosition(const std::string & fenString);
//  void unmakeMove(const Move & move);

//  // Internal definitions
//private:
//  typedef unsigned long long u64;

//  // Side to move
//  enum Side
//  {
//    White = 0,
//    Black = 1
//  };

//  // Enumeration so that we can hold the bit boards in arrays
//  enum Type
//  {
//    WhitePieces = 0,
//    BlackPieces = 1,
//    WhitePawn = 2,
//    BlackPawn = 3,
//    WhiteRook = 4,
//    BlackRook = 5,
//    WhiteBishop = 6,
//    BlackBishop = 7,
//    WhiteKnight = 8,
//    BlackKnight = 9,
//    WhiteQueen = 10,
//    BlackQueen = 11,
//    WhiteKing = 12,
//    BlackKing = 13,
//    None = 14,
//    All = 15
//  };

//  // Undo struct for easy retrieval of rotated bitboards
//  struct Undo
//  {
//    u64 rotate90;
//    u64 rotateR45;
//    u64 rotateL45;
//  };

//  // Implementation
//private:
//  uchar bitScanForward(u64 bb) const;
//  void generateBlackPawnMoves(MoveList & moveList) const;
//  void generateCastlingMoves(MoveList & moveList) const;
//  void generateJumpAttacks(u64 piece, const u64 * attacks, u64 friends, MoveList & moveList) const;
//  void generatePawnAttacks(u64 piece, const u64 * attacks, u64 enemies, MoveList & moveList) const;
//  void generateSlideAttacks(u64 piece, const u64 attacks[][256], u64 friends, u64 pieces, const uint * shifts, const uint * masks, MoveList & moveList) const;
//  void generateWhitePawnMoves(MoveList & moveList) const;
//  void init();
//  void initBoard();
//  void initAttacks();
//  void initBishopAttacks();
//  void initKingAttacks();
//  void initKnightAttacks();
//  void initPawnAttacks(Side side, char rowIncr);
//  void initRookAttacks();
//  u64 invRotateL45(u64 bb) const;
//  u64 invRotateR45(u64 bb) const;
//  bool isCellAttacked(uchar index, Color attackingColor) const;
//  void pushBitBoardMoves(u64 bb, uchar fromSq, MoveList & moveList) const;
//  void pushMove(uchar from, uchar to, Type piece, Type capture, Type promote, Move::Type type, MoveList & moveList) const;
//  u64 rotateL90(u64 bb) const;
//  u64 rotateR90(u64 bb) const;
//  u64 rotateL45(u64 bb) const;
//  u64 rotateR45(u64 bb) const;
//  void updateAggregateBitBoards();
//  void updateRotatedBitBoards();
//  void writeBitBoard(u64 bb, std::ostream & output = std::cout) const;
//  void writeOccupancy(uint occupancy, bool flip = false) const;

//private:
//  Side mSide;
//  uint mUndoPos;
//  u64 mRotateL45;
//  u64 mRotateR45;
//  u64 mRotateR90;
//  u64 mPiece[16];
//  Type mType[64];
//  Piece mTypeToPiece[15];          //! Table of Type to Piece
//  u64 mKingAttacks[64];
//  u64 mKnightAttacks[64];
//  u64 mMask[64];
//  u64 mInvMask[64];
//  u64 mPawnAttacks[2][64];
//  u64 mFileAttacks[64][256];
//  u64 mNorthEastAttacks[64][256];
//  u64 mNorthWestAttacks[64][256];
//  u64 mRankAttacks[64][256];
//  Undo mUndoStack[1000];
//};

#endif // BITBOARD_H
