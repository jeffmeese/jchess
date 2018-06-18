#include "bitboard.h"
#include "fen.h"
#include "move.h"
#include "movelist.h"

#include <cassert>
#include <sstream>
#include <intrin.h>

#pragma intrinsic(_BitScanForward64)

// Helper macros for bitboards
#define C64(constantU64) constantU64##ULL
#define ONE C64(1)

// Macros for mapping (row,col)->index and vice-versa
#define getIndex(row,col) (((row)<<3)+(col))
#define getRow(index) ( (index >> 3) )
#define getCol(index) ( (index & 7) )

// Mask for full rank
static const int FullRank = 255;

// Pawn offsets
static const char NORTH =  8;
static const char SOUTH = -8;

// Square definition
enum Square
{
  A1, B1, C1, D1, E1, F1, G1, H1,
  A2, B2, C2, D2, E2, F2, G2, H2,
  A3, B3, C3, D3, E3, F3, G3, H3,
  A4, B4, C4, D4, E4, F4, G4, H4,
  A5, B5, C5, D5, E5, F5, G5, H5,
  A6, B6, C6, D6, E6, F6, G6, H6,
  A7, B7, C7, D7, E7, F7, G7, H7,
  A8, B8, C8, D8, E8, F8, G8, H8
};

// Rotation matrices

static const uint rotR90Matrix[] =
{
  A8, A7, A6, A5, A4, A3, A2, A1,
  B8, B7, B6, B5, B4, B3, B2, B1,
  C8, C7, C6, C5, C4, C3, C2, C1,
  D8, D7, D6, D5, D4, D3, D2, D1,
  E8, E7, E6, E5, E4, E3, E2, E1,
  F8, F7, F6, F5, F4, F3, F2, F1,
  G8, G7, G6, G5, G4, G3, G2, G1,
  H8, H7, H6, H5, H4, H3, H2, H1
};

static const uint rotL90Matrix[] =
{
  H8, H7, H6, H5, H4, H3, H2, H1,
  G8, G7, G6, G5, G4, G3, G2, G1,
  F8, F7, F6, F5, F4, F3, F2, F1,
  E8, E7, E6, E5, E4, E3, E2, E1,
  D8, D7, D6, D5, D4, D3, D2, D1,
  C8, C7, C6, C5, C4, C3, C2, C1,
  B8, B7, B6, B5, B4, B3, B2, B1,
  A8, A7, A6, A5, A4, A3, A2, A1
};

// Shifts for ranks
static const uint rankShifts[64] = {
  0, 0, 0, 0, 0, 0, 0, 0,
  8, 8, 8, 8, 8, 8, 8, 8,
  16, 16, 16, 16, 16, 16, 16, 16,
  24, 24, 24, 24, 24, 24, 24, 24,
  32, 32, 32, 32, 32, 32, 32, 32,
  40, 40, 40, 40, 40, 40, 40, 40,
  48, 48, 48, 48, 48, 48, 48, 48,
  56, 56, 56, 56, 56, 56, 56, 56
};

// Shifts for files
static const uint fileShifts[64] = {
  0, 8, 16, 24, 32, 40, 48, 56,
  0, 8, 16, 24, 32, 40, 48, 56,
  0, 8, 16, 24, 32, 40, 48, 56,
  0, 8, 16, 24, 32, 40, 48, 56,
  0, 8, 16, 24, 32, 40, 48, 56,
  0, 8, 16, 24, 32, 40, 48, 56,
  0, 8, 16, 24, 32, 40, 48, 56,
  0, 8, 16, 24, 32, 40, 48, 56
};

// Masks for ranks
static const uint rankMasks[64] = {
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255
};

// Masks for files
static const uint fileMasks[64] = {
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255
};

// Formatted for ease of visualization
static const uint rotateL45Matrix[] =
{
  H8,                                   // 0
  G8, H7,                               // 1
  F8, G7, H6,                           // 3
  E8, F7, G6, H5,                       // 6
  D8, E7, F6, G5, H4,                   // 10
  C8, D7, E6, F5, G4, H3,               // 15
  B8, C7, D6, E5, F4, G3, H2,           // 21
  A8, B7, C6, D5, E4, F3, G2, H1,       // 28
  A7, B6, C5, D4, E3, F2, G1,           // 36
  A6, B5, C4, D3, E2, F1,               // 43
  A5, B4, C3, D2, E1,                   // 49
  A4, B3, C2, D1,                       // 54
  A3, B2, C1,                           // 58
  A2, B1,                               // 61
  A1                                    // 63
};

// Formatted for ease of visualization
static const uint rotateR45Matrix[] =
{
  A8,                                   // 0
  A7, B8,                               // 1
  A6, B7, C8,                           // 3
  A5, B6, C7, D8,                       // 6
  A4, B5, C6, D7, E8,                   // 10
  A3, B4, C5, D6, E7, F8,               // 15
  A2, B3, C4, D5, E6, F7, G8,           // 21
  A1, B2, C3, D4, E5, F6, G7, H8,       // 28
  B1, C2, D3, E4, F5, G6, H7,           // 36
  C1, D2, E3, F4, G5, H6,               // 43
  D1, E2, F3, G4, H5,                   // 49
  E1, F2, G3, H4,                       // 54
  F1, G2, H3,                           // 58
  G1, H2,                               // 61
  H1                                    // 63
};

// Formatted for ease of visualization
static const uint invRotateL45Matrix[] =
{
  H8,                                   // 0
  G8, E8,                               // 1
  B8, F7, A7,                           // 3
  C6, D5, F8, D8,                       // 6
  A8, E7, H6, B6, C5,                   // 10
  D4, C8, H7, D7, G6, A6,               // 15
  B5, C4, E3, G7, C7, F6, H5,           // 21
  A5, B4, D3, G2, B7, E6, G5, H4,       // 28
  A4, C3, F2, B2, D6, F5, G4,           // 36
  H3, B3, E2, A2, F1, E5,               // 43
  F4, G3, A3, D2, H1,                   // 49
  E1, C1, E4, F3,                       // 54
  H2, C2, G1,                           // 58
  D1, B1,                               // 61
  A1                                    // 63
};

// Formatted for ease of visualization
static const uint invRotateR45Matrix[] =
{
  E4,                                   // 0
  E5, D6,                               // 1
  B7, G7, C8,                           // 3
  F8, H8, F3, F4,                       // 6
  F5, E6, C7, H7, D8,                   // 10
  G8, H2, G3, G4, G5, F6,               // 15
  D7, A8, E8, C2, A3, H3, H4,           // 21
  H5, G6, E7, B8, G1, D2, B3, A4,       // 28
  A5, A6, H6, F7, D1, H1, E2,           // 36
  C3, B4, B5, B6, A7, B1,               // 43
  E1, A2, F2, D3, C4,                   // 49
  C5, C6, A1, C1,                       // 54
  F1, B2, G2,                           // 58
  E3, D4,                               // 61
  D5                                    // 63
};

// Lengths for north east diagonals
static const uint a1h8Lengths[64] = {
  8, 7, 6, 5, 4, 3, 2, 1,
  7, 8, 7, 6, 5, 4, 3, 2,
  6, 7, 8, 7, 6, 5, 4, 3,
  5, 6, 7, 8, 7, 6, 5, 4,
  4, 5, 6, 7, 8, 7, 6, 5,
  3, 4, 5, 6, 7, 8, 7, 6,
  2, 3, 4, 5, 6, 7, 8, 7,
  1, 2, 3, 4, 5, 6, 7, 8
};

// Masks for north east diagonals
static const uint a1h8Masks[64] = {
  255, 127,  63,  31,  15,   7,   3,   1,
  127, 255, 127,  63,  31,  15,   7,   3,
   63, 127, 255, 127,  63,  31,  15,   7,
   31,  63, 127, 255, 127,  63,  31,  15,
   15,  31,  63, 127, 255, 127,  63,  31,
    7,  15,  31,  63, 127, 255, 127,  63,
    3,   7,  15,  31,  63, 127, 255, 127,
    1,   3,   7,  15,  31,  63, 127, 255
};

// Shifts for north east diagonals
// 43
static const uint a1h8Shifts[64] = {
  28, 36, 43, 49, 54, 58, 61, 63,
  21, 28, 36, 43, 49, 54, 58, 61,
  15, 21, 28, 36, 43, 49, 54, 58,
  10, 15, 21, 28, 36, 43, 49, 54,
   6, 10, 15, 21, 28, 36, 43, 49,
   3,  6, 10, 15, 21, 28, 36, 43,
   1,  3,  6, 10, 15, 21, 28, 36,
   0,  1,  3,  6, 10, 15, 21, 28
};

// Lengths for north west diagonals
static const uint a8h1Lengths[64] = {
  1, 2, 3, 4, 5, 6, 7, 8,
  2, 3, 4, 5, 6, 7, 8, 7,
  3, 4, 5, 6, 7, 8, 7, 6,
  4, 5, 6, 7, 8, 7, 6, 5,
  5, 6, 7, 8, 7, 6, 5, 4,
  6, 7, 8, 7, 6, 5, 4, 3,
  7, 8, 7, 6, 5, 4, 3, 2,
  8, 7, 6, 5, 4, 3, 2, 1
};

// Masks for north west diagonals
static const uint a8h1Masks[64] = {
    1,   3,   7,  15,  31,  63, 127, 255,
    3,   7,  15,  31,  63, 127, 255, 127,
    7,  15,  31,  63, 127, 255, 127,  63,
   15,  31,  63, 127, 255, 127,  63,  31,
   31,  63, 127, 255, 127,  63,  31,  15,
   63, 127, 255, 127,  63,  31,  15,   7,
  127, 255, 127,  63,  31,  15,   7,   3,
  255, 127,  63,  31,  15,   7,   3,   1,
};

// Shifts for north west diagonals
static const uint a8h1Shifts[64] = {
  63, 61, 58, 54, 49, 43, 36, 28,
  61, 58, 54, 49, 43, 36, 28, 21,
  58, 54, 49, 43, 36, 28, 21, 15,
  54, 49, 43, 36, 28, 21, 15, 10,
  49, 43, 36, 28, 21, 15, 10,  6,
  43, 36, 28, 21, 15, 10,  6,  3,
  36, 28, 21, 15, 10,  6,  3,  1,
  28, 21, 15, 10,  6,  3,  1,  0
};

// Constants used for bitScanForward
const uchar index64[64] =
{
   63,  0, 58,  1, 59, 47, 53,  2,
   60, 39, 48, 27, 54, 33, 42,  3,
   61, 51, 37, 40, 49, 18, 28, 20,
   55, 30, 34, 11, 43, 14, 22,  4,
   62, 57, 46, 52, 38, 26, 32, 41,
   50, 36, 17, 19, 29, 10, 13, 21,
   56, 45, 25, 31, 35, 16,  9, 12,
   44, 24, 15,  8, 23,  7,  6,  5
};

BitBoard::BitBoard()
{
  initBoard();
  initBlackPawnAttacks();
  initWhitePawnAttacks();
  initKnightAttacks();
  initKingAttacks();
  initRookAttacks();
  initBishopAttacks();
}

/**
 * bitScanForward
 * @author Martin Läuter (1997)
 *         Charles E. Leiserson
 *         Harald Prokop
 *         Keith H. Randall
 * "Using de Bruijn Sequences to Index a 1 in a Computer Word"
 * @param bb bitboard to scan
 * @precondition bb != 0
 * @return index (0..63) of least significant one bit
 */
uchar BitBoard::bitScanForward(U64 bb) const
{
  //ulong index = 0;
  //_BitScanForward64(&index, bb);
  //return static_cast<uchar>(index);
   static const ulonglong debruijn64 = ulonglong(0x07EDD5E59A4E28C2);
   assert (bb != 0);
   return index64[((bb & -bb) * debruijn64) >> 58];
}

uchar BitBoard::bitScanForwardPopCount(U64 bb) const
{
  assert (bb != 0);
  return popCount( (bb & -bb) - 1 );
}

void BitBoard::generateBlackPawnMoves(MoveList &moveList) const
{
  U64 bbMove = 0;
  U64 promoMask = C64(255);

  // Promotion moves
  bbMove = (mPieceBB[BlackPawn] << 8) & promoMask & mEmptySquares;
  while (bbMove) {
    int toSq = bitScanForward(bbMove);
    int fromSq = toSq + NORTH;
    pushMove(fromSq, toSq, Type::BlackPawn, Type::None, Type::BlackQueen, Move::Type::Promotion, moveList);
    pushMove(fromSq, toSq, Type::BlackPawn, Type::None, Type::BlackRook, Move::Type::Promotion, moveList);
    pushMove(fromSq, toSq, Type::BlackPawn, Type::None, Type::BlackBishop, Move::Type::Promotion, moveList);
    pushMove(fromSq, toSq, Type::BlackPawn, Type::None, Type::BlackKnight, Move::Type::Promotion, moveList);
    bbMove &= bbMove - 1;
  }

  // Pawn pushes
  U64 pawnPushes = (mPieceBB[BlackPawn] >> 8) & mEmptySquares;
  bbMove = pawnPushes & ~promoMask;
  while (bbMove) {
    int toSq = bitScanForward(bbMove);
    int fromSq = toSq + NORTH;
    pushMove(fromSq, toSq, Type::BlackPawn, Type::None, Type::None, Move::Type::Quiet, moveList);
    bbMove &= bbMove - 1;
  }

  // Double pawn pushes
  U64 unmovedPawns = (mPieceBB[BlackPawn] & (C64(255) << 48));
  U64 pawnDoublePushes = ( (unmovedPawns & (~(mAllPieces << 8)) & (~(mAllPieces << 16)) ) ) >> 16;
  bbMove = pawnDoublePushes;
  while (bbMove) {
    int toSq = bitScanForward(bbMove);
    int fromSq = toSq + (NORTH << 1);
    pushMove(fromSq, toSq, Type::BlackPawn, Type::None, Type::None, Move::Type::EpPush, moveList);
    bbMove &= bbMove - 1;
  }
}

void BitBoard::generateCastlingMoves(MoveList & moveList) const
{
  if (sideToMove() == Color::White) {
    if (castlingRights() & CASTLE_WHITE_KING) {
      if (mType[Square::F1] == Type::None &&
          mType[Square::G1] == Type::None &&
          !isCellAttacked(Square::E1, Color::Black) &&
          !isCellAttacked(Square::F1, Color::Black) &&
          !isCellAttacked(Square::G1, Color::Black))
      {
        pushMove(E1, G1, Type::WhiteKing, Type::None, Type::None, Move::Type::Castle, moveList);
      }
    }
    if (castlingRights() & CASTLE_WHITE_QUEEN) {
      if (mType[Square::D1] == Type::None &&
          mType[Square::C1] == Type::None &&
          mType[Square::B1] == Type::None &&
          !isCellAttacked(Square::E1, Color::Black) &&
          !isCellAttacked(Square::D1, Color::Black) &&
          !isCellAttacked(Square::C1, Color::Black))
      {
        pushMove(E1, C1, Type::WhiteKing, Type::None, Type::None, Move::Type::Castle, moveList);
      }
    }
  }
  else {
    if (castlingRights() & CASTLE_BLACK_KING) {
      if (mType[Square::F8] == Type::None &&
          mType[Square::G8] == Type::None &&
          !isCellAttacked(Square::E8, Color::White) &&
          !isCellAttacked(Square::F8, Color::White) &&
          !isCellAttacked(Square::G8, Color::White))
      {
        pushMove(E8, G8, Type::BlackKing, Type::None, Type::None, Move::Type::Castle, moveList);
      }
    }
    if (castlingRights() & CASTLE_BLACK_QUEEN) {
      if (mType[Square::D8] == Type::None &&
          mType[Square::C8] == Type::None &&
          mType[Square::B8] == Type::None &&
          !isCellAttacked(Square::E8, Color::White) &&
          !isCellAttacked(Square::D8, Color::White) &&
          !isCellAttacked(Square::C8, Color::White))
      {
        pushMove(E8, C8, Type::BlackKing, Type::None, Type::None, Move::Type::Castle, moveList);
      }
    }
  }
}

void BitBoard::generateMoves(MoveList & moveList) const
{
  //TimerHolder holder(mGenMoveTimer);

  // Get the bitboards for the pieces on the side to move
  U64 pawns = mPieceBB[WhitePawn + mSide];
  U64 knights = mPieceBB[WhiteKnight + mSide];
  U64 kings = mPieceBB[WhiteKing + mSide];
  U64 rooks = mPieceBB[WhiteRook + mSide];
  U64 bishops = mPieceBB[WhiteBishop + mSide];
  U64 queens = mPieceBB[WhiteQueen + mSide];
  U64 friends = mPieceBB[mSide];
  U64 enemies = ~friends & mAllPieces;

  // Generate castling moves
  generateCastlingMoves(moveList);

  // Generate pawn moves
  if (mSide == White) {
    generateWhitePawnMoves(moveList);
    generatePawnAttacks(pawns, mPawnAttacks[0], enemies, moveList);
  }
  else {
    generateBlackPawnMoves(moveList);
    generatePawnAttacks(pawns, mPawnAttacks[1], enemies, moveList);
  }

  // Generate all other moves
  generateJumpAttacks(knights, mKnightAttacks, friends, moveList);
  generateJumpAttacks(kings, mKingAttacks, friends, moveList);
  generateSlideAttacks(rooks, mRankAttacks, friends, mAllPieces, rankShifts, rankMasks, moveList);
  generateSlideAttacks(rooks, mFileAttacks, friends, mRotateRight90Pieces, fileShifts, fileMasks, moveList);
  generateSlideAttacks(bishops, mNorthEastAttacks, friends, mRotateRight45Pieces, a1h8Shifts, a1h8Masks, moveList);
  generateSlideAttacks(bishops, mNorthWestAttacks, friends, mRotateLeft45Pieces, a8h1Shifts, a8h1Masks, moveList);
  generateSlideAttacks(queens, mRankAttacks, friends, mAllPieces, rankShifts, rankMasks, moveList);
  generateSlideAttacks(queens, mFileAttacks, friends, mRotateRight90Pieces, fileShifts, fileMasks, moveList);
  generateSlideAttacks(queens, mNorthEastAttacks, friends, mRotateRight45Pieces, a1h8Shifts, a1h8Masks, moveList);
  generateSlideAttacks(queens, mNorthWestAttacks, friends, mRotateLeft45Pieces, a8h1Shifts, a8h1Masks, moveList);
}

void BitBoard::generateJumpAttacks(U64 piece, const U64 * attacks, U64 friends, MoveList & moveList) const
{
  while (piece) {
    uchar fromSq = bitScanForward(piece);
    U64 attack = attacks[fromSq];
    U64 move = attack & ~friends;
    while (move) {
      uchar toSq = bitScanForward(move);
      Move::Type type = (mType[toSq] == Type::None) ? Move::Type::Quiet : Move::Type::Capture;
      pushMove(fromSq, toSq, mType[fromSq], mType[toSq], Type::None, type, moveList);
      move &= move-1;
    }
    piece &= piece-1;
  }
}

void BitBoard::generatePawnAttacks(U64 piece, const U64 *attacks, U64 enemies, MoveList &moveList) const
{
  while (piece) {
    uchar fromSq = bitScanForward(piece);
    U64 attack = attacks[fromSq];
    U64 move = attack & enemies;
    while (move) {
      uchar toSq = bitScanForward(move);
      uchar destRow = getRow(toSq);
      Type capturePiece = mType[toSq];
      if (destRow == WHITE_PROMOTION_ROW) { // Promotion capture
        pushMove(fromSq, toSq, mType[fromSq], capturePiece, Type::WhiteQueen, Move::Type::PromotionCapture, moveList);
        pushMove(fromSq, toSq, mType[fromSq], capturePiece, Type::WhiteRook, Move::Type::PromotionCapture, moveList);
        pushMove(fromSq, toSq, mType[fromSq], capturePiece, Type::WhiteBishop, Move::Type::PromotionCapture, moveList);
        pushMove(fromSq, toSq, mType[fromSq], capturePiece, Type::WhiteKnight, Move::Type::PromotionCapture, moveList);
      }
      else if (destRow == BLACK_PROMOTION_ROW) { // Promotion capture
        pushMove(fromSq, toSq, mType[fromSq], capturePiece, Type::BlackQueen, Move::Type::PromotionCapture, moveList);
        pushMove(fromSq, toSq, mType[fromSq], capturePiece, Type::BlackRook, Move::Type::PromotionCapture, moveList);
        pushMove(fromSq, toSq, mType[fromSq], capturePiece, Type::BlackBishop, Move::Type::PromotionCapture, moveList);
        pushMove(fromSq, toSq, mType[fromSq], capturePiece, Type::BlackKnight, Move::Type::PromotionCapture, moveList);
      }
      else { // Standard capture
        pushMove(fromSq, toSq, mType[fromSq], capturePiece, Type::None, Move::Type::Capture, moveList);
      }
      move &= move-1;
    }

    if (epColumn() != BoardBase::INVALID_EP) { // En-passant capture
      U64 bbMove = attack;
      while (bbMove) {
        uchar toSq = bitScanForward(bbMove);
        uchar epIndex = getIndex(epRow(), epColumn());
        char epDir = (sideToMove() == Color::White) ? SOUTH : NORTH;
        if (epIndex == toSq) {
          pushMove(fromSq, toSq, mType[fromSq], mType[toSq+epDir], Type::None, Move::Type::EpCapture, moveList);
        }
        bbMove &= bbMove-1;
      }
    }

    piece &= piece-1;
  }
}

void BitBoard::generateSlideAttacks(U64 piece, const U64 attacks[][256], U64 friends, U64 pieces, const uint * shifts, const uint * masks, MoveList & moveList) const
{
  while (piece) {
    uchar fromSq = bitScanForward(piece);
    uint shift = shifts[fromSq];
    uint occupancy = (pieces >> shift) & masks[fromSq];
    U64 move = attacks[fromSq][occupancy] & ~friends;
    pushBitBoardMoves(move, fromSq, moveList);
    piece &= piece-1;
  }
}

void BitBoard::generateWhitePawnMoves(MoveList &moveList) const
{
  U64 bbMove = 0;
  U64 promoMask = C64(255) << 56;

  // Promotion moves
  bbMove = (mPieceBB[WhitePawn] << 8) & promoMask & mEmptySquares;
  while (bbMove) {
    int toSq = bitScanForward(bbMove);
    int fromSq = toSq + SOUTH;
    pushMove(fromSq, toSq, Type::WhitePawn, Type::None, Type::WhiteQueen, Move::Type::Promotion, moveList);
    pushMove(fromSq, toSq, Type::WhitePawn, Type::None, Type::WhiteRook, Move::Type::Promotion, moveList);
    pushMove(fromSq, toSq, Type::WhitePawn, Type::None, Type::WhiteBishop, Move::Type::Promotion, moveList);
    pushMove(fromSq, toSq, Type::WhitePawn, Type::None, Type::WhiteKnight, Move::Type::Promotion, moveList);
    bbMove &= bbMove - 1;
  }

  // Pawn pushes
  U64 pawnPushes = (mPieceBB[WhitePawn] << 8) & mEmptySquares;
  bbMove = pawnPushes & ~promoMask;
  while (bbMove) {
    int toSq = bitScanForward(bbMove);
    int fromSq = toSq + SOUTH;
    pushMove(fromSq, toSq, Type::WhitePawn, Type::None, Type::None, Move::Type::Quiet, moveList);
    bbMove &= bbMove - 1;
  }

  // Pawn double pushes
  U64 unmovedPawns = (mPieceBB[WhitePawn] & (255 << 8));
  U64 pawnDoublePushes = ( (unmovedPawns & (~(mAllPieces >> 8)) & (~(mAllPieces >> 16)) ) ) << 16;
  bbMove = pawnDoublePushes;
  while (bbMove) {
    int toSq = bitScanForward(bbMove);
    int fromSq = toSq + (SOUTH << 1);
    pushMove(fromSq, toSq, Type::WhitePawn, Type::None, Type::None, Move::Type::EpPush, moveList);
    bbMove &= bbMove - 1;
  }
}

void BitBoard::initBishopAttacks()
{
  // North east attacks
  for (uchar index = 0; index < 64; index++) {
    uchar col = getCol(index);
    uchar row = getRow(index);
    uchar shift = a1h8Shifts[index];
    uchar length = a1h8Lengths[index];
    uchar mask = a1h8Masks[index];

    for (uint occupancy = 0; occupancy <= mask; occupancy++) {
      U64 bb = 0;

      int start = (col > row) ? row : col;
      for (int i = (start)+1; i < length; i++) {
        bb += (ONE << i);
        if (occupancy & (1 << i)) break;
      }

      for (int i = (start)-1; i >= 0; i--) {
        bb += (ONE << i);
        if (occupancy & (1 << i)) break;
      }

      U64 bbRot = invRotateR45(bb << shift);
      mNorthEastAttacks[index][occupancy] = bbRot;
    }
  }

  // North west attacks
  for (uchar index = 0; index < 64; index++) {
    uchar col = getCol(index);
    uchar row = getRow(index);
    uchar shift = a8h1Shifts[index];
    uchar length = a8h1Lengths[index];
    uchar mask = a8h1Masks[index];

    for (uint occupancy = 0; occupancy <= mask; occupancy++) {
      U64 bb = 0;

      int start = (row > (7-col)) ? (7-row) : col;
      for (int i = start+1; i < length; i++) {
        bb += (ONE << i);
        if (occupancy & (1 << i)) break;
      }

      for (int i = start-1; i >= 0; i--) {
        bb += (ONE << i);
        if (occupancy & (1 << i)) break;
      }

      U64 bbRot = invRotateL45(bb << shift);
      mNorthWestAttacks[index][occupancy] = bbRot;
    }
  }
}

void BitBoard::initBlackPawnAttacks()
{
  static const int blackPawnRowIncr[] = {-1, -1};
  static const int blackPawnColIncr[] = {-1, 1};

  // Precompute attacks for each square
  for (char sourceRow = 0; sourceRow < 8; sourceRow++) {
    for (char sourceCol = 0; sourceCol < 8; sourceCol++) {
      uchar index = getIndex(sourceRow, sourceCol);
      mPawnAttacks[1][index] = 0ULL;

      for (int k = 0; k < 2; k++) {
        char destRow = sourceRow + blackPawnRowIncr[k];
        char destCol = sourceCol + blackPawnColIncr[k];
        if (destRow >= 0 && destRow < 8 && destCol >= 0 && destCol < 8) {
          uchar destIndex = getIndex(destRow, destCol);
          mPawnAttacks[1][index] |= (ONE << destIndex);
        }
      }
    }
  }
}

void BitBoard::initBoard()
{
  mSide = White;

  // Initialize the type to piece array
  mTypeToPiece[None] = Piece::None;
  mTypeToPiece[WhitePawn] = Piece::Pawn;
  mTypeToPiece[BlackPawn] = Piece::Pawn;
  mTypeToPiece[WhiteRook] = Piece::Rook;
  mTypeToPiece[BlackRook] = Piece::Rook;
  mTypeToPiece[WhiteBishop] = Piece::Bishop;
  mTypeToPiece[BlackBishop] = Piece::Bishop;
  mTypeToPiece[WhiteKnight] = Piece::Knight;
  mTypeToPiece[BlackKnight] = Piece::Knight;
  mTypeToPiece[WhiteQueen] = Piece::Queen;
  mTypeToPiece[BlackQueen] = Piece::Queen;
  mTypeToPiece[WhiteKing] = Piece::King;
  mTypeToPiece[BlackKing] = Piece::King;

  for (uint index = 0; index < 64; index++) {
    mType[index] = Type::None;
  }

  // Initialize the pawn piece types
  for (uint col = 0; col < 8; col++) {
    mType[getIndex(1, col)] = Type::WhitePawn;
    mType[getIndex(6, col)] = Type::BlackPawn;
  }

  // Initialize the white piece types
  mType[getIndex(0, 0)] = mType[getIndex(0, 7)] = Type::WhiteRook;
  mType[getIndex(0, 1)] = mType[getIndex(0, 6)] = Type::WhiteKnight;
  mType[getIndex(0, 2)] = mType[getIndex(0, 5)] = Type::WhiteBishop;
  mType[getIndex(0, 3)] = Type::WhiteQueen;
  mType[getIndex(0, 4)] = Type::WhiteKing;

  // Initialize black piece types
  mType[getIndex(7, 0)] = mType[getIndex(7, 7)] = Type::BlackRook;
  mType[getIndex(7, 1)] = mType[getIndex(7, 6)] = Type::BlackKnight;
  mType[getIndex(7, 2)] = mType[getIndex(7, 5)] = Type::BlackBishop;
  mType[getIndex(7, 3)] = Type::BlackQueen;
  mType[getIndex(7, 4)] = Type::BlackKing;

  // Initialize the bit boards
  for (uchar i = 0; i < Black; i++) {
    mPieceBB[i] = 0;
    mPieceBB[i+Black] = 0;
  }

  // Fill the white bit boards
  mPieceBB[WhiteBishop] = 0x24ULL;
  mPieceBB[WhiteKing] = 0x10ULL;
  mPieceBB[WhiteKnight] = 0x42ULL;
  mPieceBB[WhitePawn] = (0xffULL) << 8ULL;
  mPieceBB[WhiteQueen] = 0x08ULL;
  mPieceBB[WhiteRook] = 0x81ULL;

  // Fill the black bit boards
  mPieceBB[BlackBishop] = (0x24ULL << C64(56));
  mPieceBB[BlackKing] = (0x10ULL << C64(56));
  mPieceBB[BlackKnight] = (0x42ULL << C64(56));
  mPieceBB[BlackPawn] = (0xffULL << C64(48));
  mPieceBB[BlackQueen] = (0x08ULL << C64(56));
  mPieceBB[BlackRook] = (0x81ULL << C64(56));

  // Update aggregate bit boards
  updateAggregateBitBoards();
}

void BitBoard::initKingAttacks()
{
  static const int kingRowIncr[]      = {1, 1, 1, 0, -1, -1, -1, 0};
  static const int kingColIncr[]      = {-1, 0, 1, 1, 1, 0, -1, -1};

  // Precompute attacks for each square
  for (char sourceRow = 0; sourceRow < 8; sourceRow++) {
    for (char sourceCol = 0; sourceCol < 8; sourceCol++) {
      uchar index = getIndex(sourceRow, sourceCol);
      mKingAttacks[index] = 0ULL;

      for (char k = 0; k < 8; k++) {
        char destRow = sourceRow + kingRowIncr[k];
        char destCol = sourceCol + kingColIncr[k];
        if (destRow >= 0 && destRow < 8 && destCol >= 0 && destCol < 8) {
          uchar destIndex = getIndex(destRow, destCol);
          mKingAttacks[index] |= (ONE << destIndex);
        }
      }
    }
  }
}

void BitBoard::initKnightAttacks()
{
  static const int knightRowIncr[]    = {1, 2, 2, 1, -1, -2, -2, -1};
  static const int knightColIncr[]    = {-2, -1, 1, 2, 2, 1, -1, -2};

  // Precompute attacks for each square
  for (char sourceRow = 0; sourceRow < 8; sourceRow++) {
    for (char sourceCol = 0; sourceCol < 8; sourceCol++) {
      uchar index = getIndex(sourceRow, sourceCol);
      mKnightAttacks[index] = 0ULL;

      for (char k = 0; k < 8; k++) {
        char destRow = sourceRow + knightRowIncr[k];
        char destCol = sourceCol + knightColIncr[k];
        if (destRow >= 0 && destRow < 8 && destCol >= 0 && destCol < 8) {
          uchar destIndex = getIndex(destRow, destCol);
          mKnightAttacks[index] |= (ONE << destIndex);
        }
      }
    }
  }
}

void BitBoard::initRookAttacks()
{
  // Rank attacks
  for (int col = 0; col < 8; col++) {
    for (int occupancy = 0; occupancy < 256; occupancy++) {
      U64 bb = 0;

      for (int x = col-1; x >=0; x--) {
        bb += (ONE << x);
        if (occupancy & (1<<x)) {
          break;
        }
      }

      for (int x = col+1; x < 8; x++) {
        bb += (ONE << x);
        if (occupancy & (1<<x)) {
          break;
        }
      }

      for (int row = 0; row < 8; row++) {
        uint shift = row*8;
        mRankAttacks[(row*8)+col][occupancy] = (bb << shift);
      }
    }
  }

  // File attacks
  for (int row = 0; row < 8; row++) {
    for (uint occupancy = 0; occupancy < 256; occupancy++) {
      U64 bb = 0;

      for (int i = (7-row)+1; i < 8; i++) {
        bb += (ONE << i);
        if (occupancy & (1 << i)) break;
      }

      for (int i = (7-row)-1; i >= 0; i--) {
        bb += (ONE << i);
        if (occupancy & (1 << i)) break;
      }

      for (int col = 0; col < 8; col++) {
        uint shift = (7-col)*8;
        mFileAttacks[(row*8)+col][occupancy] = rotateL90(bb << shift);
      }
    }
  }
}

void BitBoard::initWhitePawnAttacks()
{
  static const int whitePawnRowIncr[] = {1, 1};
  static const int whitePawnColIncr[] = {-1, 1};

  // Precompute attacks for each square
  for (char sourceRow = 0; sourceRow < 8; sourceRow++) {
    for (char sourceCol = 0; sourceCol < 8; sourceCol++) {
      uchar index = getIndex(sourceRow, sourceCol);
      mPawnAttacks[0][index] = 0ULL;

      for (int k = 0; k < 2; k++) {
        char destRow = sourceRow + whitePawnRowIncr[k];
        char destCol = sourceCol + whitePawnColIncr[k];
        if (destRow >= 0 && destRow < 8 && destCol >= 0 && destCol < 8) {
          uchar destIndex = getIndex(destRow, destCol);
          mPawnAttacks[0][index] |= (ONE << destIndex);
        }
      }
    }
  }
}

bool BitBoard::isCellAttacked(uchar row, uchar col, Color attackingColor) const
{
  return isCellAttacked(getIndex(row, col), attackingColor);
}

bool BitBoard::isCellAttacked(uchar index, Color attackingColor) const
{
  Side bySide = (attackingColor == Color::White) ? White : Black;

  U64 pawns = mPieceBB[WhitePawn + bySide];
  if (mPawnAttacks[bySide^1][index] & pawns)
    return true;

  U64 knights = mPieceBB[WhiteKnight + bySide];
  if (mKnightAttacks[index] & knights)
    return true;

  U64 kings = mPieceBB[WhiteKing + bySide];
  if (mKingAttacks[index] & kings)
    return true;

  uint shift = 0;
  uint occupancy = 0;
  //U64 rooks = mPieceBB[WhiteRook + bySide];
  //U64 bishops = mPieceBB[WhiteBishop + bySide];
  //U64 queens = mPieceBB[WhiteQueen + bySide];
  U64 bishopsQueens = mPieceBB[WhiteBishop + bySide] | mPieceBB[WhiteQueen + bySide];
  U64 rooksQueens = mPieceBB[WhiteRook + bySide] | mPieceBB[WhiteQueen + bySide];

  shift = rankShifts[index];
  occupancy = (mAllPieces >> shift) & rankMasks[index];
  if (mRankAttacks[index][occupancy] & rooksQueens)
    return true;

  shift = fileShifts[index];
  occupancy = (mRotateRight90Pieces >> shift) & fileMasks[index];
  if (mFileAttacks[index][occupancy] & rooksQueens)
    return true;

  shift = a1h8Shifts[index];
  occupancy = (mRotateRight45Pieces >> shift) & a1h8Masks[index];
  if (mNorthEastAttacks[index][occupancy] & bishopsQueens)
    return true;

  shift = a8h1Shifts[index];
  occupancy = (mRotateLeft45Pieces >> shift) & a8h1Masks[index];
  if (mNorthWestAttacks[index][occupancy] & bishopsQueens)
    return true;

  //shift = rankShifts[index];
  //occupancy = (mAllPieces >> shift) & rankMasks[index];
  //if (mRankAttacks[index][occupancy] & queens)
  //  return true;

  //shift = fileShifts[index];
  //occupancy = (mRotateRight90Pieces >> shift) & fileMasks[index];
  //if (mFileAttacks[index][occupancy] & queens)
  //  return true;

  //shift = a1h8Shifts[index];
  //occupancy = (mRotateRight45Pieces >> shift) & a1h8Masks[index];
  //if (mNorthEastAttacks[index][occupancy] & queens)
  //  return true;

  //shift = a8h1Shifts[index];
  //occupancy = (mRotateLeft45Pieces >> shift) & a8h1Masks[index];
  //if (mNorthWestAttacks[index][occupancy] & queens)
  //  return true;

  return false;
}

void BitBoard::makeMove(const Move & move)
{
  //TimerHolder holder(mMakeMoveTimer);

  // Get move information
  Color side = sideToMove();
  uchar fromSquare = getIndex(move.sourceRow(), move.sourceCol());
  uchar toSquare = getIndex(move.destRow(), move.destCol());
  Type fromPiece = mType[fromSquare];
  Type toPiece = mType[toSquare];

  // Calculate required bitboards
  U64 bbFrom = (ONE << fromSquare);
  U64 bbTo = (ONE << toSquare);
  U64 bbFromTo = bbFrom ^ bbTo;

  // Update the type array
  mType[fromSquare] = Type::None;
  mType[toSquare] = fromPiece;

  // Handle non promotion moves
  if (!move.isPromotion() && !move.isPromotionCapture()) {
    mPieceBB[fromPiece] ^= bbFromTo;
  }

  // Handle capture
  if (move.isCapture() && !move.isEpCapture()) {
    mPieceBB[toPiece] ^= (ONE << toSquare);
  }

  // Handle enPassant captures
  if (move.isEpCapture()) {
    char dir = (side == Color::White) ? SOUTH : NORTH;
    uchar sq = toSquare + dir;
    mType[sq] = Type::None;
    mPieceBB[!mSide] ^= (ONE << sq);
  }

  // Handle king move
  if (move.piece() == Piece::King) {
    setKingColumn(side, move.destCol());
    setKingRow(side, move.destRow());
  }

  // Update the en-passant index
  clearEpCol();
  if (move.isEpPush()) {
    setEpColumn(move.destCol());
  }

  // Handle promotions
  if (move.isPromotion() || move.isPromotionCapture()) {
    Piece promotedPiece = move.promotedPiece();

    mPieceBB[WhitePawn + mSide] ^= (ONE << fromSquare);
    switch (promotedPiece) {
    case Piece::Queen:
      mPieceBB[WhiteQueen + mSide] ^= (ONE << toSquare);
      mType[toSquare] = static_cast<Type>(WhiteQueen + mSide);
      break;
    case Piece::Rook:
      mPieceBB[WhiteRook + mSide] ^= (ONE << toSquare);
      mType[toSquare] = static_cast<Type>(WhiteRook + mSide);
      break;
    case Piece::Bishop:
      mPieceBB[WhiteBishop + mSide] ^= (ONE << toSquare);
      mType[toSquare] = static_cast<Type>(WhiteBishop + mSide);
      break;
    case Piece::Knight:
      mPieceBB[WhiteKnight + mSide] ^= (ONE << toSquare);
      mType[toSquare] = static_cast<Type>(WhiteKnight + mSide);
      break;
    }
  }

  // Update castling rights
  // If the square associated with a king or rook is
  // modified in any way remove the castling ability
  // for that piece
  uchar castling = castlingRights();
  switch (fromSquare) {
  case H1:
    castling &= ~CASTLE_WHITE_KING;
    break;
  case E1:
    castling &= ~(CASTLE_WHITE_KING|CASTLE_WHITE_QUEEN);
    break;
  case A1:
    castling &= ~CASTLE_WHITE_QUEEN;
    break;
  case H8:
    castling &= ~CASTLE_BLACK_KING;
    break;
  case E8:
    castling &= ~(CASTLE_BLACK_KING|CASTLE_BLACK_QUEEN);
    break;
  case A8:
    castling &= ~CASTLE_BLACK_QUEEN;
    break;
  }

  switch (toSquare) {
  case H1:
    castling &= ~CASTLE_WHITE_KING;
    break;
  case E1:
    castling &= ~(CASTLE_WHITE_KING|CASTLE_WHITE_QUEEN);
    break;
  case A1:
    castling &= ~CASTLE_WHITE_QUEEN;
    break;
  case H8:
    castling &= ~CASTLE_BLACK_KING;
    break;
  case E8:
    castling &= ~(CASTLE_BLACK_KING|CASTLE_BLACK_QUEEN);
    break;
  case A8:
    castling &= ~CASTLE_BLACK_QUEEN;
    break;
  }

  // Handle castle move
  if (move.isCastle()) {
    switch (toSquare) {
    case C1:
      mType[D1] = Type::WhiteRook;
      mType[A1] = Type::None;
      mPieceBB[WhiteRook] ^= (ONE << D1);
      mPieceBB[WhiteRook] ^= (ONE << A1);
      break;
    case G1:
      mType[F1] = Type::WhiteRook;
      mType[H1] = Type::None;
      mPieceBB[WhiteRook] ^= (ONE << F1);
      mPieceBB[WhiteRook] ^= (ONE << H1);
      break;
    case C8:
      mType[D8] = Type::BlackRook;
      mType[A8] = Type::None;
      mPieceBB[BlackRook] ^= (ONE << D8);
      mPieceBB[BlackRook] ^= (ONE << A8);
      break;
    case G8:
      mType[F8] = Type::BlackRook;
      mType[H8] = Type::None;
      mPieceBB[BlackRook] ^= (ONE << F8);
      mPieceBB[BlackRook] ^= (ONE << H8);
      break;
    }
  }

  // Set the castling rights
  setCastlingRights(castling);

  // Update board state
  incrementHalfMoveClock();
  if (move.piece() == Piece::Pawn || move.isCapture())
    setHalfMoveClock(0);

  // Update the aggregate bitboards
  updateAggregateBitBoards();

  // Switch sides
  mSide = static_cast<Side>(!mSide);
  toggleSideToMove();
}

uchar BitBoard::popCount (U64 x) const
{
  uchar count = 0;
  while (x) {
    count++;
    x &= x - 1; // reset LS1B
  }
  return count;
}

void BitBoard::pushBitBoardMoves(U64 bb, uchar fromSq, MoveList & moveList) const
{
  while (bb) {
    int toSq = bitScanForward(bb);
    Type capturePiece = mType[toSq];
    Move::Type type = (capturePiece == Type::None) ? Move::Type::Quiet : Move::Type::Capture;
    pushMove(fromSq, toSq, mType[fromSq], capturePiece, Type::None, type, moveList);
    bb &= bb-1;
  }
}

void BitBoard::pushMove(uchar from, uchar to, Type piece, Type capture, Type promote, Move::Type type, MoveList & moveList) const
{
  Move move(getRow(from), getCol(from), getRow(to), getCol(to), mTypeToPiece[piece]);
  move.setCastlingRights(castlingRights());
  move.setHalfMoveClock(halfMoveClock());
  move.setFullMoveCounter(fullMoveCounter());
  move.setType(type);
  move.setCapturedPiece(mTypeToPiece[capture]);
  move.setPromotedPiece(mTypeToPiece[promote]);
  move.setEnpassantColumn(epColumn());

  moveList.addMove(move);
}

PieceType BitBoard::pieceType(uchar row, uchar col) const
{
  uchar index = getIndex(row, col);
  Type type = mType[index];
  switch (type) {
  case WhitePawn:
    return PieceType::WhitePawn;
  case WhiteBishop:
    return PieceType::WhiteBishop;
  case WhiteRook:
    return PieceType::WhiteRook;
  case WhiteQueen:
    return PieceType::WhiteQueen;
  case WhiteKing:
    return PieceType::WhiteKing;
  case WhiteKnight:
    return PieceType::WhiteKnight;
  case BlackPawn:
    return PieceType::BlackPawn;
  case BlackBishop:
    return PieceType::BlackBishop;
  case BlackRook:
    return PieceType::BlackRook;
  case BlackQueen:
    return PieceType::BlackQueen;
  case BlackKing:
    return PieceType::BlackKing;
  case BlackKnight:
    return PieceType::BlackKnight;
  }

  return PieceType::None;
}

BitBoard::U64 BitBoard::invRotateL45(U64 bb) const
{
  U64 symmBB = 0;
  for (uint index = 0; index < 64; index++)
    if (bb & (ONE << invRotateL45Matrix[index]))
      symmBB |= (ONE << index);

  return symmBB;
}

BitBoard::U64 BitBoard::invRotateR45(U64 bb) const
{
  U64 symmBB = 0;
  for (uint index = 0; index < 64; index++)
    if (bb & (ONE << invRotateR45Matrix[index]))
      symmBB |= (ONE << index);

  return symmBB;
}

BitBoard::U64 BitBoard::rotateR90(U64 bb) const
{
  U64 symmBB = 0;
  for (uint index = 0; index < 64; index++)
    if (bb & (ONE << rotR90Matrix[index]))
      symmBB |= (ONE << index);

  return symmBB;
}

BitBoard::U64 BitBoard::rotateL90(U64 bb) const
{
  U64 symmBB = 0;
  for (uint index = 0; index < 64; index++)
    if (bb & (ONE << rotL90Matrix[index]))
      symmBB |= (ONE << index);

  return symmBB;
}

BitBoard::U64 BitBoard::rotateL45(U64 bb) const
{
  U64 symmBB = 0;
  for (uint index = 0; index < 64; index++)
    if (bb & (ONE << rotateL45Matrix[index]))
      symmBB += (ONE << index);

  return symmBB;
}

BitBoard::U64 BitBoard::rotateR45(U64 bb) const
{
  U64 symmBB = 0;
  for (uint index = 0; index < 64; index++)
    if (bb & (ONE << rotateR45Matrix[index]))
      symmBB += (ONE << index);

  return symmBB;
}

void BitBoard::setPosition(const std::string & fenString)
{
  initBoard();

  Fen fenData;
  fenData.readString(fenString);

  uchar castling = CASTLE_NONE;
  if (fenData.canBlackCastleKingSide())
    castling |= CASTLE_BLACK_KING;
  if (fenData.canBlackCastleQueenSide())
    castling |= CASTLE_BLACK_QUEEN;
  if (fenData.canWhiteCastleKingSide())
    castling |= CASTLE_WHITE_KING;
  if (fenData.canWhiteCastleQueenSide())
    castling |= CASTLE_WHITE_QUEEN;

  setCastlingRights(castling);
  setHalfMoveClock(fenData.halfMoveClock());
  setFullMoveCounter(fenData.fullMoveCounter());

  Color color = (fenData.whiteToMove()) ? Color::White : Color::Black;
  mSide = (color == Color::White) ? White : Black;
  setSideToMove(color);

  clearEpCol();
  if (fenData.epColumn() >= 0) {
    setEpColumn(fenData.epColumn());
  }

  for (uint i = WhitePawn; i <= BlackKing; i++) {
    mPieceBB[i] = 0;
  }

  for (uchar row = 0; row < 8; row++) {
    for (uchar col = 0; col < 8; col++) {
      PieceType type = fenData.pieceType(row, col);
      uchar sq = getIndex(row, col);
      ulonglong bb = (ONE << sq);

      mType[sq] = None;
      switch (type) {
      case PieceType::WhitePawn:
        mPieceBB[WhitePawn] |= bb;
        mType[sq] = WhitePawn;
        break;
      case PieceType::WhiteRook:
        mPieceBB[WhiteRook] |= bb;
        mType[sq] = WhiteRook;
        break;
      case PieceType::WhiteKnight:
        mPieceBB[WhiteKnight] |= bb;
        mType[sq] = WhiteKnight;
        break;
      case PieceType::WhiteBishop:
        mPieceBB[WhiteBishop] |= bb;
        mType[sq] = WhiteBishop;
        break;
      case PieceType::WhiteQueen:
        mPieceBB[WhiteQueen] |= bb;
        mType[sq] = WhiteQueen;
        break;
      case PieceType::WhiteKing:
        mPieceBB[WhiteKing] |= bb;
        mType[sq] = WhiteKing;
        setKingRow(Color::White, row);
        setKingColumn(Color::White, col);
        break;
      case PieceType::BlackPawn:
        mPieceBB[BlackPawn] |= bb;
        mType[sq] = BlackPawn;
        break;
      case PieceType::BlackRook:
        mPieceBB[BlackRook] |= bb;
        mType[sq] = BlackRook;
        break;
      case PieceType::BlackKnight:
        mPieceBB[BlackKnight] |= bb;
        mType[sq] = BlackKnight;
        break;
      case PieceType::BlackBishop:
        mPieceBB[BlackBishop] |= bb;
        mType[sq] = BlackBishop;
        break;
      case PieceType::BlackQueen:
        mPieceBB[BlackQueen] |= bb;
        mType[sq] = BlackQueen;
        break;
      case PieceType::BlackKing:
        mPieceBB[BlackKing] |= bb;
        mType[sq] = BlackKing;
        setKingRow(Color::Black, row);
        setKingColumn(Color::Black, col);
        break;
      default:
        break;
      }
    }
  }

  // Update aggregate bitboards
  updateAggregateBitBoards();
}

void BitBoard::unmakeMove(const Move & move)
{
  //TimerHolder holder(mUnmakeMoveTimer);

  // Get move information
  Color side = sideToMove();
  Piece piece = move.piece();
  uchar fromSquare = getIndex(move.sourceRow(), move.sourceCol());
  uchar toSquare = getIndex(move.destRow(), move.destCol());
  Type toPiece = mType[toSquare];

  // Calculate required bitboards
  U64 bbFrom = (ONE << fromSquare);
  U64 bbTo = (ONE << toSquare);
  U64 bbFromTo = bbFrom ^ bbTo;

  // Reset the board state
  setFullMoveCounter(move.fullMoveCounter());
  setHalfMoveClock(move.halfMoveClock());
  setCastlingRights(move.castlingRights());
  setEpColumn(move.enPassantCol());

  // Update the piece and color tables
  mType[fromSquare] = toPiece;
  mType[toSquare] = Type::None;

  // Handle non promotion moves
  if (!move.isPromotion() && !move.isPromotionCapture()) {
    mPieceBB[toPiece] ^= bbFromTo;
  }

  // Handle captures
  if (move.isCapture()) {
    uchar sq = toSquare;
    if (move.isEpCapture()) {
      char dir = (side == Color::White) ? NORTH : SOUTH;
      sq = toSquare + dir;
    }

    // Update the relevant bit board
    Piece capturePiece = move.capturePiece();
    int index = (mSide == Black) ? BlackPawn : WhitePawn;

    for (int i = index; i < None; i += 2) {
      if (mTypeToPiece[i] == capturePiece) {
        //std::cout << i << "\n";
        mType[sq] = static_cast<Type>(i);
        mPieceBB[i] |= (ONE << sq);
        break;
      }
    }
  }

  // Handle king moves
  if (piece == Piece::King) {
    setKingRow(!side, move.sourceRow());
    setKingColumn(!side, move.sourceCol());
  }

  // Handle promotions
  if (move.isPromotion() || move.isPromotionCapture()) {
    Piece promotedPiece = move.promotedPiece();

    mPieceBB[WhitePawn + !mSide] ^= (ONE << fromSquare);
    mType[fromSquare] = static_cast<Type>(WhitePawn + !mSide);
    switch (promotedPiece) {
    case Piece::Queen:
      mPieceBB[WhiteQueen + !mSide] ^= (ONE << toSquare);
      break;
    case Piece::Rook:
      mPieceBB[WhiteRook + !mSide] ^= (ONE << toSquare);
      break;
    case Piece::Bishop:
      mPieceBB[WhiteBishop + !mSide] ^= (ONE << toSquare);
      break;
    case Piece::Knight:
      mPieceBB[WhiteKnight + !mSide] ^= (ONE << toSquare);
      break;
    }
  }

  // Handle castling move
  if (move.isCastle()) {
    switch (toSquare) {
    case C1:
      mType[A1] = Type::WhiteRook;
      mType[D1] = Type::None;
      mPieceBB[WhiteRook] ^= (ONE << A1);
      mPieceBB[WhiteRook] ^= (ONE << D1);
      break;
    case G1:
      mType[H1] = Type::WhiteRook;
      mType[F1] = Type::None;
      mPieceBB[WhiteRook] ^= (ONE << H1);
      mPieceBB[WhiteRook] ^= (ONE << F1);
      break;
    case C8:
      mType[A8] = Type::BlackRook;
      mType[D8] = Type::None;
      mPieceBB[BlackRook] ^= (ONE << A8);
      mPieceBB[BlackRook] ^= (ONE << D8);
      break;
    case G8:
      mType[H8] = Type::BlackRook;
      mType[F8] = Type::None;
      mPieceBB[BlackRook] ^= (ONE << H8);
      mPieceBB[BlackRook] ^= (ONE << F8);
      break;
    }
  }

  // Update the aggregate bitboards
  updateAggregateBitBoards();

  // Switch sides
  mSide = static_cast<Side>(!mSide);
  toggleSideToMove();

  //writeBitBoard(mPieceBB[WhitePawn], std::cout);
  //writeBitBoard(mPieceBB[BlackPawn], std::cout);
}

void BitBoard::updateAggregateBitBoards()
{
  // Initialize aggregate bitboards
  mPieceBB[0] = mPieceBB[WhitePawn] | mPieceBB[WhiteRook] | mPieceBB[WhiteBishop] | mPieceBB[WhiteKnight] | mPieceBB[WhiteQueen] | mPieceBB[WhiteKing];
  mPieceBB[1] = mPieceBB[BlackPawn] | mPieceBB[BlackRook] | mPieceBB[BlackBishop] | mPieceBB[BlackKnight] | mPieceBB[BlackQueen] | mPieceBB[BlackKing];

  mAllPieces = mPieceBB[0] | mPieceBB[1];
  mEmptySquares = ~mAllPieces;
  mRotateRight90Pieces = rotateR90(mAllPieces);
  mRotateRight45Pieces = rotateR45(mAllPieces);
  mRotateLeft45Pieces = rotateL45(mAllPieces);
}

void BitBoard::writeBitBoard(U64 bitboard, std::ostream & output) const
{
  for (int i = 7; i >= 0; i--) {
    std::cout << i+1 << " ";
    for (int j = 0; j < 8; j++) {
      int shift = 8*i + j ;
      if ( (bitboard >> shift) & 0x1)
        std::cout << "X ";
      else
        std::cout << "- ";
    }
    std::cout << "\n";
  }
  std::cout << "  ";
  for (int j = 0; j < 8; j++) {
    switch (j) {
      case 0:
        std::cout << "A ";
        break;
      case 1:
        std::cout << "B ";
        break;
      case 2:
        std::cout << "C ";
        break;
      case 3:
        std::cout << "D ";
        break;
      case 4:
        std::cout << "E ";
        break;
      case 5:
        std::cout << "F ";
        break;
      case 6:
        std::cout << "G ";
        break;
      case 7:
        std::cout << "H";
        break;
    }
  }
  std::cout << "\n\n";
}

void BitBoard::writeOccupancy(uint occupancy, bool flip) const
{
	std::ostringstream oss;
  if (flip) {
    for (int i = 7; i >=0; i--) {
      if ( (occupancy >> i) & 0x01)
        oss << 'X';
      else
        oss << '-';
    }
  }
  else {
    for (int i = 0; i < 8; i++) {
      if ( (occupancy >> i) & 0x01)
        oss << 'X';
      else
        oss << '-';
    }
  }

	std::cout << oss.str() << "\n";
}
