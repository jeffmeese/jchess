#ifndef BITBOARD_H
#define BITBOARD_H

#include <iostream>

#include "jcl_board.h"
#include "jcl_move.h"
#include "jcl_movelist.h"

namespace jcl
{

class BitBoard
    : public Board
{
  // Construction
public:
  BitBoard();
  virtual ~BitBoard();

  // Methods
public:
  bool generateMoves(MoveList & moveList) const override;
  bool generateMoves(uint8_t row, uint8_t col, MoveList & moveList) const override;
  uint8_t getKingColumn(Color color) const override;
  uint8_t getKingRow(Color color) const override;
  bool isCellAttacked(uint8_t row, uint8_t col, Color attackingColor) const override;
  bool makeMove(const Move * move) override;
  PieceType getPieceType(uint8_t row, uint8_t col) const override;
  bool setPosition(const std::string & fenString) override;
  bool unmakeMove(const Move * move) override;
  void printPerformanceMetrics() const override;

  // Internal definitions
private:
  typedef uint64_t U64;

  // // Side to move
  // enum Side
  // {
  //   White = 0,
  //   Black = 1
  // };

  // // Piece types for bit boards
  // enum Type
  // {
  //   WhitePieces = 0,
  //   BlackPieces = 1,
  //   WhitePawn = 2,
  //   BlackPawn = 3,
  //   WhiteRook = 4,
  //   BlackRook = 5,
  //   WhiteBishop = 6,
  //   BlackBishop = 7,
  //   WhiteKnight = 8,
  //   BlackKnight = 9,
  //   WhiteQueen = 10,
  //   BlackQueen = 11,
  //   WhiteKing = 12,
  //   BlackKing = 13,
  //   None = 14,
  //   All = 15
  // };

  // Undo structure to save rotation matrices
  struct Undo
  {
    U64 rotate90;
    U64 rotateR45;
    U64 rotateL45;
  };

  // Implementation
private:
  uint8_t bitScanForward(U64 bb) const;
  void generateCastlingMoves(MoveList & moveList) const;
  void generateJumpAttacks(U64 piece, const U64 * attacks, U64 friends, MoveList & moveList) const;
  void generatePawnAttacks(U64 piece, const U64 * attacks, U64 enemies, char dir, uint8_t promotionRow, MoveList & moveList) const;
  void generatePawnMoves(char dir, U64 pawnPushes, U64 pawnDoublePushes, U64 promoMask, MoveList & moveList) const;
  void generateSlideAttacks(U64 piece, const U64 attacks[][256], U64 friends, U64 pieces, const uint32_t * shifts, const uint32_t * masks, MoveList & moveList) const;
  void init();
  void initAttacks();
  void initBishopAttacks();
  void initKingAttacks();
  void initKnightAttacks();
  void initPawnAttacks(Color side, int rowIncr);
  void initRookAttacks();
  U64 invRotateL45(U64 bb) const;
  U64 invRotateR45(U64 bb) const;
  bool isCellAttacked(uint8_t index, Color attackingColor) const;
  void pushBitBoardMoves(U64 bb, uint8_t fromSq, MoveList & moveList) const;
  void pushMove(uint8_t from, uint8_t to, PieceType piece, PieceType capture, PieceType promote, Move::Type type, MoveList & moveList) const;
  void reset();
  U64 rotateL45(U64 bb) const;
  U64 rotateL90(U64 bb) const;
  U64 rotateR45(U64 bb) const;
  U64 rotateR90(U64 bb) const;
  void updateAggregateBitBoards();
  void updateRotatedBitBoards();
  void writeBitBoard(U64 bb, std::ostream & output = std::cout) const;
  void writeOccupancy(uint32_t occupancy, int length = 8, std::ostream & output = std::cout) const;

// Testing
  void testRotations();

private:
  uint32_t mUndoPos;                   //! Position of undo stack
  //Side mSide;                      //! Side to move
  U64 mRotateL45;                  //! Bitboard of all pieces rotated 90 degrees CCW
  U64 mRotateR45;                  //! Bitboard of all pieces rotated 45 degrees CW
  U64 mRotateR90;                  //! Bitboard of all pieces rotated 90 degrees CW
  uint32_t mNorthEastMasks[64];        //! Masks for north east (a1h8) diagonals
  uint32_t mNorthWestMasks[64];        //! Masks for north west (a8h1) diagonals
  uint32_t mFileMasks[64];             //! Masks for files
  uint32_t mRankMasks[64];             //! Masks for ranks
  Piece mTypeToPiece[15];          //! Table of Type to Piece
  Piece mType[64];                  //! Type of piece on each square
  U64 mPiece[16];                  //! Bitboards for each type of piece
  U64 mMask[64];                   //! Masks for each square
  U64 mInvMask[64];                //! Inverse masks for each square
  U64 mInvRotR45[64];              //! Inverse masks for each square in the CW 45 degree rotated bitboard
  U64 mInvRotL45[64];              //! Inverse masks for each square in the CCW 45 degree rotated bitboard
  U64 mInvRotR90[64];              //! Inverse masks for each square in the CW 90 degree rotated bitboard
  U64 mKingAttacks[64];            //! Attack bitboards for kings for each square
  U64 mKnightAttacks[64];          //! Attack bitboards for knights for each square
  U64 mPawnAttacks[2][64];         //! Attack bitboards for pawns on each square
  U64 mFileAttacks[64][256];       //! Attack bitboards along files for all occupanies
  U64 mNorthEastAttacks[64][256];  //! Attack bitboards along a1h8 diagonal for all occupanies
  U64 mNorthWestAttacks[64][256];  //! Attack bitboards along a8h1 diagonal for all occupanies
  U64 mRankAttacks[64][256];       //! Attack bitboards along ranks for all occupanies
  Undo mUndoStack[1000];           //! Undo stack used in unmakeMove
};

}

#endif // BITBOARD_H
