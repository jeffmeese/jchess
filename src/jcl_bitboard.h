#ifndef BITBOARD_H
#define BITBOARD_H

#include <iostream>
#include <map>

#include "jcl_board.h"
#include "jcl_move.h"
#include "jcl_movelist.h"

namespace jcl
{

class BitBoard
: public Board
{
public:

  /*!
   * \brief Constructor
   *
   * Constructs a default BitBoard instance.
   */
  BitBoard();

  uint64_t getAll() const;
  uint64_t getAll(Color color) const;
  uint64_t getBishops(Color color) const;
  uint64_t getKings(Color color) const;
  uint64_t getKnights(Color color) const;
  uint64_t getNone() const;
  uint64_t getPawns(Color color) const;
  uint64_t getPieces(Color color) const;
  uint64_t getQueens(Color color) const;
  uint64_t getRooks(Color color) const;

protected:

  // Override
  bool doGenerateMoves(MoveList & moveList) const override;
  bool doGenerateMoves(uint8_t row, uint8_t col, MoveList & moveList) const override;
  PieceType doGetPieceType(uint8_t row, uint8_t col) const override;
  bool doIsCellAttacked(uint8_t row, uint8_t col, Color attackingColor) const override;
  bool doMakeMove(const Move * move) override;
  void doReset() override;
  bool doSetPieceType(uint8_t row, uint8_t col, PieceType pieceType) override;
  bool doSetPosition(const Fen & fen) override;
  bool doUnmakeMove(const Move * move) override;

private:
  enum BitBoardPiece
  {
    WhitePawn = 0,
    WhiteRook,
    WhiteKnight,
    WhiteBishop,
    WhiteQueen,
    WhiteKing,
    BlackPawn,
    BlackRook,
    BlackKnight,
    BlackBishop,
    BlackQueen,
    BlackKing,
    None,
  };


  void generateBishopAttacks(uint64_t rooks, uint64_t friendly, uint64_t enemy, Piece piece, MoveList & moveList) const;
  /*!
   * \brief Generates the castling moves
   *
   * This function generates the possible castling moves
   * based on the current board position. Castling moves
   * are only dependent of the current state of the board
   * and the castling rights available so they only need
   * to be generated once when all moves are being generated.
   *
   * \param moveList The move list to hold the moves
   */
  void generateCastlingMoves(MoveList & moveList) const;
  void generateLeapAttacks(uint64_t pieceBitBoard, Piece piece, const uint64_t * moves, uint64_t friendly, uint64_t enemy, MoveList & moveList) const;
  void generatePawnAttacks(uint64_t pawns, const uint64_t * pawnAttacks, uint64_t enemy, uint64_t promoRank, MoveList & moveList) const;
  void generatePawnPushesBlack(uint64_t pawns, uint64_t friendly, MoveList & moveList) const;
  void generatePawnPushesWhite(uint64_t pawns, uint64_t friendly, MoveList & moveList) const;
  void generateRookAttacks(uint64_t rooks, uint64_t friendly, uint64_t enemy, Piece piece, MoveList & moveList) const;

  void init();

  void initBoard();

  void initBishopAttacks();

  void initKingMoves();

  void initKnightMoves();

  void initPawnAttacks();

  void initRookAttacks();

  /*!
   * \brief Determines if a cell is attacked
   *
   * This function determines if the cell at the specified
   * index is attacked by a piece of the specified color.
   * This is primarily used by engines to determine if particular
   * move is valid. Any moves that put the king in check are
   * considered invalid moves.
   *
   * \param index The index of the square to check for attacks
   * \param attackColor The color of the pieces that are attacking.
   *
   * \return true if the cell at the index is attacked, false otherwise
   */
  bool isCellAttacked(uint8_t index, Color attackColor) const;

  void extractMoves(uint64_t bitBoard, MoveList & moveList) const;

  void pushMove(uint8_t from,
                uint8_t to,
                Piece piece,
                Piece capture,
                Piece promote,
                Move::Type type,
                MoveList & moveList) const;

  BitBoardPiece translatePiece(Piece piece, Color color) const;
  void updateAggregateBitBoards();
  void writeBitBoard(uint64_t bb, std::ostream & output) const;

  // Members
  uint64_t mAllPieceBitBoard;                            // Bit board for all pieces
  uint64_t mBlackPieceBitboard;                          // Bit board for all black pieces
  uint64_t mNoPieceBitboard;                             // Bit board for unoccupied squares
  uint64_t mWhitePieceBitboard;                          // Bit board for all white piece
  uint64_t mMask[64];                                    // Bit board masks for each square
  uint64_t mBitboards[12];                               // Bitboard for each type of piece
  Piece mPieces[64];                                     // Piece type on each square
  uint64_t mBishopAttacks[64];
  uint64_t mKingMoves[64];
  uint64_t mKnightMoves[64];
  uint64_t mPawnAttacksBlack[64];
  uint64_t mPawnAttacksWhite[64];
  uint64_t mRookAttacks[64];
  Color mColors[64];                                     // Color on each square
  std::map<BitBoardPiece, jcl::PieceType> mPieceToType;  // Map of BitBoardPiece type to PieceType
};

inline uint64_t BitBoard::getAll() const
{
  return mAllPieceBitBoard;
}

inline uint64_t BitBoard::getAll(Color color) const
{
  return (color == Color::White) ? mWhitePieceBitboard : mBlackPieceBitboard;
}

inline uint64_t BitBoard::getBishops(Color color) const
{
  return (color == Color::White) ? mBitboards[WhiteBishop] : mBitboards[BlackBishop];
}

inline uint64_t BitBoard::getKings(Color color) const
{
  return (color == Color::White) ? mBitboards[WhiteKing] : mBitboards[BlackKing];
}

inline uint64_t BitBoard::getKnights(Color color) const
{
  return (color == Color::White) ? mBitboards[WhiteKnight] : mBitboards[BlackKnight];
}

inline uint64_t BitBoard::getNone() const
{
  return mNoPieceBitboard;
}

inline uint64_t BitBoard::getPawns(Color color) const
{
  return (color == Color::White) ? mBitboards[WhitePawn] : mBitboards[BlackPawn];
}

inline uint64_t BitBoard::getPieces(Color color) const
{
  return (color == Color::White) ? mWhitePieceBitboard : mBlackPieceBitboard;
}

inline uint64_t BitBoard::getQueens(Color color) const
{
  return (color == Color::White) ? mBitboards[WhiteQueen] : mBitboards[BlackQueen];
}

inline uint64_t BitBoard::getRooks(Color color) const
{
  return (color == Color::White) ? mBitboards[WhiteRook] : mBitboards[BlackRook];
}

// class BitBoard
//     : public Board
// {
//   // Construction
// public:
//   BitBoard();
//   virtual ~BitBoard();

//   // Methods
// public:
//   bool generateMoves(MoveList & moveList) const override;
//   bool generateMoves(uint8_t row, uint8_t col, MoveList & moveList) const override;
//   uint8_t getKingColumn(Color color) const override;
//   uint8_t getKingRow(Color color) const override;
//   bool isCellAttacked(uint8_t row, uint8_t col, Color attackingColor) const override;
//   bool makeMove(const Move * move) override;
//   PieceType getPieceType(uint8_t row, uint8_t col) const override;
//   bool setPosition(const std::string & fenString) override;
//   bool unmakeMove(const Move * move) override;
//   void printPerformanceMetrics() const override;

//   // Internal definitions
// private:
//   typedef uint64_t U64;

//   // // Side to move
//   // enum Side
//   // {
//   //   White = 0,
//   //   Black = 1
//   // };

//   // // Piece types for bit boards
//   // enum Type
//   // {
//   //   WhitePieces = 0,
//   //   BlackPieces = 1,
//   //   WhitePawn = 2,
//   //   BlackPawn = 3,
//   //   WhiteRook = 4,
//   //   BlackRook = 5,
//   //   WhiteBishop = 6,
//   //   BlackBishop = 7,
//   //   WhiteKnight = 8,
//   //   BlackKnight = 9,
//   //   WhiteQueen = 10,
//   //   BlackQueen = 11,
//   //   WhiteKing = 12,
//   //   BlackKing = 13,
//   //   None = 14,
//   //   All = 15
//   // };

//   // Undo structure to save rotation matrices
//   struct Undo
//   {
//     U64 rotate90;
//     U64 rotateR45;
//     U64 rotateL45;
//   };

//   // Implementation
// private:
//   uint8_t bitScanForward(U64 bb) const;
//   void generateCastlingMoves(MoveList & moveList) const;
//   void generateJumpAttacks(U64 piece, const U64 * attacks, U64 friends, MoveList & moveList) const;
//   void generatePawnAttacks(U64 piece, const U64 * attacks, U64 enemies, char dir, uint8_t promotionRow, MoveList & moveList) const;
//   void generatePawnMoves(char dir, U64 pawnPushes, U64 pawnDoublePushes, U64 promoMask, MoveList & moveList) const;
//   void generateSlideAttacks(U64 piece, const U64 attacks[][256], U64 friends, U64 pieces, const uint32_t * shifts, const uint32_t * masks, MoveList & moveList) const;
//   void init();
//   void initAttacks();
//   void initBishopAttacks();
//   void initKingAttacks();
//   void initKnightAttacks();
//   void initPawnAttacks(Color side, int rowIncr);
//   void initRookAttacks();
//   U64 invRotateL45(U64 bb) const;
//   U64 invRotateR45(U64 bb) const;
//   bool isCellAttacked(uint8_t index, Color attackingColor) const;
//   void pushBitBoardMoves(U64 bb, uint8_t fromSq, MoveList & moveList) const;
//   void pushMove(uint8_t from, uint8_t to, PieceType piece, PieceType capture, PieceType promote, Move::Type type, MoveList & moveList) const;
//   void reset();
//   U64 rotateL45(U64 bb) const;
//   U64 rotateL90(U64 bb) const;
//   U64 rotateR45(U64 bb) const;
//   U64 rotateR90(U64 bb) const;
//   void updateAggregateBitBoards();
//   void updateRotatedBitBoards();
//   void writeBitBoard(U64 bb, std::ostream & output = std::cout) const;
//   void writeOccupancy(uint32_t occupancy, int length = 8, std::ostream & output = std::cout) const;

// // Testing
//   void testRotations();

// private:
//   uint32_t mUndoPos;                   //! Position of undo stack
//   //Side mSide;                      //! Side to move
//   U64 mRotateL45;                  //! Bitboard of all pieces rotated 90 degrees CCW
//   U64 mRotateR45;                  //! Bitboard of all pieces rotated 45 degrees CW
//   U64 mRotateR90;                  //! Bitboard of all pieces rotated 90 degrees CW
//   uint32_t mNorthEastMasks[64];        //! Masks for north east (a1h8) diagonals
//   uint32_t mNorthWestMasks[64];        //! Masks for north west (a8h1) diagonals
//   uint32_t mFileMasks[64];             //! Masks for files
//   uint32_t mRankMasks[64];             //! Masks for ranks
//   Piece mTypeToPiece[15];          //! Table of Type to Piece
//   Piece mType[64];                  //! Type of piece on each square
//   U64 mPiece[16];                  //! Bitboards for each type of piece
//   U64 mMask[64];                   //! Masks for each square
//   U64 mInvMask[64];                //! Inverse masks for each square
//   U64 mInvRotR45[64];              //! Inverse masks for each square in the CW 45 degree rotated bitboard
//   U64 mInvRotL45[64];              //! Inverse masks for each square in the CCW 45 degree rotated bitboard
//   U64 mInvRotR90[64];              //! Inverse masks for each square in the CW 90 degree rotated bitboard
//   U64 mKingAttacks[64];            //! Attack bitboards for kings for each square
//   U64 mKnightAttacks[64];          //! Attack bitboards for knights for each square
//   U64 mPawnAttacks[2][64];         //! Attack bitboards for pawns on each square
//   U64 mFileAttacks[64][256];       //! Attack bitboards along files for all occupanies
//   U64 mNorthEastAttacks[64][256];  //! Attack bitboards along a1h8 diagonal for all occupanies
//   U64 mNorthWestAttacks[64][256];  //! Attack bitboards along a8h1 diagonal for all occupanies
//   U64 mRankAttacks[64][256];       //! Attack bitboards along ranks for all occupanies
//   Undo mUndoStack[1000];           //! Undo stack used in unmakeMove
// };

}

#endif // BITBOARD_H
