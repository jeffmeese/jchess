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
  void pushMove(uchar from, uchar to, PieceType piece, PieceType capture, PieceType promote, Move::Type type, MoveList & moveList) const;
  U64 rotateL90(U64 bb) const;
  U64 rotateR90(U64 bb) const;
  U64 rotateL45(U64 bb) const;
  U64 rotateR45(U64 bb) const;
  Piece typeToPiece(PieceType type) const;
  void updateAggregateBitBoards();
  void writeBitBoard(U64 bitboard, std::ostream & output) const;
  void writeBoards() const;
  void writeOccupancy(uint occupancy, bool flip = false) const;

  // Members
private:
  U64 mAllPieces;                  //! Bitboard for all pieces
  U64 mBlackBishops;               //! Bitboard for black bishops
  U64 mBlackKings;                 //! Bitboard for black kings
  U64 mBlackKnights;               //! Bitboard for black knights
  U64 mBlackQueens;                //! Bitboard for black queens
  U64 mBlackPawns;                 //! Bitboard for black pawns
  U64 mBlackPieces;                //! Bitboard for black pieces
  U64 mBlackRooks;                 //! Bitboard for black rooks
  U64 mEmptySquares;               //! Bitboard for empty squares
  U64 mRotateRight90Pieces;        //! Bitboard of pieces rotated along files
  U64 mRotateLeft45Pieces;         //! Bitboard of pieces rotated along a8h1 diagonal
  U64 mRotateRight45Pieces;        //! Bitboard of pieces rotated along a1h8 diagonal
  U64 mWhiteBishops;               //! Bitboard for white bishops
  U64 mWhiteKings;                 //! Bitboard for white kings
  U64 mWhiteKnights;               //! Bitboard for white knights
  U64 mWhiteQueens;                //! Bitboard for white queens
  U64 mWhitePawns;                 //! Bitboard for white pawns
  U64 mWhitePieces;                //! Bitboard for white pieces
  U64 mWhiteRooks;                 //! Bitboard for white rooks
  U64 mBlackPawnAttacks[64];       //! Attack bitboards for black pawns for each square
  U64 mKingAttacks[64];            //! Attack bitboards for kings for each square
  U64 mKnightAttacks[64];          //! Attack bitboards for knights for each square
  U64 mWhitePawnAttacks[64];       //! Attack bitboards for white pawns for each square
  U64 mFileAttacks[64][256];       //! Attack bitboards along files for all occupanies
  U64 mNorthEastAttacks[64][256];  //! Attack bitboards along a1h8 diagonal for all occupanies
  U64 mNorthWestAttacks[64][256];  //! Attack bitboards along a8h1 diagonal for all occupanies
  U64 mRankAttacks[64][256];       //! Attack bitboards along ranks for all occupanies
  PieceType mPieceType[64];        //! Redundant piece matrix for quick lookup
};

#endif // BITBOARD_H
