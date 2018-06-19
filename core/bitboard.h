#ifndef BITBOARD_H
#define BITBOARD_H

#include "boardbase.h"
#include "move.h"
#include "timer.h"

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
  void resetTimers();
  void setPosition(const std::string & fenString);
  void unmakeMove(const Move & move);
  void writeTimers() const;

  // Internal definitions
private:
  typedef ulonglong U64;

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
    None = 14
  };

  // Implementation
private:
  uchar bitScanForward(U64 bb) const;
  uchar bitScanForwardPopCount(U64 bb) const;
  void generateJumpAttacks(U64 piece, const U64 * attacks, U64 friends, MoveList & moveList) const;
  void generateSlideAttacks(U64 piece, const U64 attacks[][256], U64 friends, U64 pieces, const uint * shifts, const uint * masks, MoveList & moveList) const;
  void generatePawnAttacks(U64 piece, const U64 * attacks, U64 enemies, MoveList & moveList) const;
  void generateBlackPawnMoves(MoveList & moveList) const;
  void generateCastlingMoves(MoveList & moveList) const;
  void generateWhitePawnMoves(MoveList & moveList) const;
  void pushBitBoardMoves(U64 bb, uchar fromSq, MoveList & moveList) const;
  void initBlackPawnAttacks();
  void initBoard();
  void initKingAttacks();
  void initKnightAttacks();
  void initBishopAttacks();
  void initRookAttacks();
  void initWhitePawnAttacks();
  bool isCellAttacked(uchar index, Color attackingColor) const;
  U64 invRotateL45(U64 bb) const;
  U64 invRotateR45(U64 bb) const;
  uchar popCount(U64 x) const;
  void pushMove(uchar from, uchar to, Type piece, Type capture, Type promote, Move::Type type, MoveList & moveList) const;
  U64 rotateL90(U64 bb) const;
  U64 rotateR90(U64 bb) const;
  U64 rotateL45(U64 bb) const;
  U64 rotateR45(U64 bb) const;
  void updateAggregateBitBoards();
  void writeBitBoard(U64 bitboard, std::ostream & output) const;
  void writeOccupancy(uint occupancy, bool flip = false) const;

  // Members
private:
  U64 mAllPieces;                  //! Bitboard for all pieces
  U64 mEmptySquares;               //! Bitboard for empty squares
  U64 mRotateLeft45Pieces;         //! Bitboard of pieces rotated along a8h1 diagonal
  U64 mRotateRight45Pieces;        //! Bitboard of pieces rotated along a1h8 diagonal
  U64 mRotateRight90Pieces;        //! Bitboard of pieces rotated along files
  Side mSide;                      //! Side to move
  U64 mPieceBB[15];                //! Bitboard for each type of piece indexed by Type enum
  Piece mTypeToPiece[15];          //! Table of Type to Piece
  Type mType[64];                  //! Piece types for quick lookup
  U64 mKingAttacks[64];            //! Attack bitboards for kings for each square
  U64 mKnightAttacks[64];          //! Attack bitboards for knights for each square
  U64 mPawnAttacks[2][64];         //! Attack bitboards for pawns on each square
  U64 mFileAttacks[64][256];       //! Attack bitboards along files for all occupanies
  U64 mNorthEastAttacks[64][256];  //! Attack bitboards along a1h8 diagonal for all occupanies
  U64 mNorthWestAttacks[64][256];  //! Attack bitboards along a8h1 diagonal for all occupanies
  U64 mRankAttacks[64][256];       //! Attack bitboards along ranks for all occupanies
  mutable Timer mMakeMoveTimer;
  mutable Timer mGenMoveTimer;
  mutable Timer mUnmakeMoveTimer;
  mutable Timer mCellAttackTimer;
  mutable Timer mRotateTimer;
};

#endif // BITBOARD_H
