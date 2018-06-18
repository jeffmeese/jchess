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
  ulong index = 0;
  _BitScanForward64(&index, bb);
  return static_cast<uchar>(index);
   //static const ulonglong debruijn64 = ulonglong(0x07EDD5E59A4E28C2);
   //assert (bb != 0);
   //return index64[((bb & -bb) * debruijn64) >> 58];
}

uchar BitBoard::bitScanForwardPopCount(U64 bb) const
{
  assert (bb != 0);
  return popCount( (bb & -bb) - 1 );
}

void BitBoard::generateAttackMoves(U64 pieces, MoveList &moveList) const
{
  U64 bbFriends = pieces;

  Color side = sideToMove();
  const U64 * pawnAttacks = (side == Color::White) ? mWhitePawnAttacks : mBlackPawnAttacks;
  U64 oppPieces = (side == Color::White) ? mBlackPieces : mWhitePieces;

  while (pieces) {
    uchar fromSq = bitScanForward(pieces);

    PieceType type = mPieceType[fromSq];
    switch (type) {
    case PieceType::WhitePawn:
    case PieceType::BlackPawn:
      generatePawnAttacks(fromSq, pawnAttacks[fromSq], oppPieces, moveList);
      break;
    case PieceType::WhiteRook:
    case PieceType::BlackRook:
      generateSliderAttacks(fromSq, bbFriends, true, false, moveList);
      break;
    case PieceType::WhiteKnight:
    case PieceType::BlackKnight:
      generateNonSliderAttacks(fromSq, mKnightAttacks[fromSq], bbFriends, moveList);
      break;
    case PieceType::WhiteBishop:
    case PieceType::BlackBishop:
      generateSliderAttacks(fromSq, bbFriends, false, true, moveList);
      break;
    case PieceType::WhiteQueen:
    case PieceType::BlackQueen:
      generateSliderAttacks(fromSq, bbFriends, true, true, moveList);
      break;
    case PieceType::WhiteKing:
    case PieceType::BlackKing:
      generateNonSliderAttacks(fromSq, mKingAttacks[fromSq], bbFriends, moveList);
      break;
    }

    pieces &= pieces-1;
  }
}

void BitBoard::generateBlackPawnMoves(MoveList &moveList) const
{
  U64 bbMove = 0;
  U64 promoMask = C64(255);

  // Promotion moves
  bbMove = (mBlackPawns << 8) & promoMask & mEmptySquares;
  while (bbMove) {
    int toSq = bitScanForward(bbMove);
    int fromSq = toSq + NORTH;
    pushMove(fromSq, toSq, PieceType::BlackPawn, PieceType::None, PieceType::BlackQueen, Move::Type::Promotion, moveList);
    pushMove(fromSq, toSq, PieceType::BlackPawn, PieceType::None, PieceType::BlackRook, Move::Type::Promotion, moveList);
    pushMove(fromSq, toSq, PieceType::BlackPawn, PieceType::None, PieceType::BlackBishop, Move::Type::Promotion, moveList);
    pushMove(fromSq, toSq, PieceType::BlackPawn, PieceType::None, PieceType::BlackKnight, Move::Type::Promotion, moveList);
    bbMove &= bbMove - 1;
  }

  // Pawn pushes
  U64 pawnPushes = (mBlackPawns >> 8) & mEmptySquares;
  bbMove = pawnPushes & ~promoMask;
  while (bbMove) {
    int toSq = bitScanForward(bbMove);
    int fromSq = toSq + NORTH;
    pushMove(fromSq, toSq, PieceType::BlackPawn, PieceType::None, PieceType::None, Move::Type::Quiet, moveList);
    bbMove &= bbMove - 1;
  }

  // Double pawn pushes
  U64 unmovedPawns = (mBlackPawns & (C64(255) << 48));
  U64 pawnDoublePushes = ( (unmovedPawns & (~(mAllPieces << 8)) & (~(mAllPieces << 16)) ) ) >> 16;
  bbMove = pawnDoublePushes;
  while (bbMove) {
    int toSq = bitScanForward(bbMove);
    int fromSq = toSq + (NORTH << 1);
    pushMove(fromSq, toSq, PieceType::BlackPawn, PieceType::None, PieceType::None, Move::Type::EpPush, moveList);
    bbMove &= bbMove - 1;
  }
}

void BitBoard::generateCastlingMoves(MoveList & moveList) const
{
  if (sideToMove() == Color::White) {
    if (castlingRights() & CASTLE_WHITE_KING) {
      if (mPieceType[Square::F1] == PieceType::None &&
          mPieceType[Square::G1] == PieceType::None &&
          !isCellAttacked(Square::E1, Color::Black) &&
          !isCellAttacked(Square::F1, Color::Black) &&
          !isCellAttacked(Square::G1, Color::Black))
      {
        pushMove(E1, G1, PieceType::WhiteKing, PieceType::None, PieceType::None, Move::Type::Castle, moveList);
      }
    }
    if (castlingRights() & CASTLE_WHITE_QUEEN) {
      if (mPieceType[Square::D1] == PieceType::None &&
          mPieceType[Square::C1] == PieceType::None &&
          mPieceType[Square::B1] == PieceType::None &&
          !isCellAttacked(Square::E1, Color::Black) &&
          !isCellAttacked(Square::D1, Color::Black) &&
          !isCellAttacked(Square::C1, Color::Black))
      {
        pushMove(E1, C1, PieceType::WhiteKing, PieceType::None, PieceType::None, Move::Type::Castle, moveList);
      }
    }
  }
  else {
    if (castlingRights() & CASTLE_BLACK_KING) {
      if (mPieceType[Square::F8] == PieceType::None &&
          mPieceType[Square::G8] == PieceType::None &&
          !isCellAttacked(Square::E8, Color::White) &&
          !isCellAttacked(Square::F8, Color::White) &&
          !isCellAttacked(Square::G8, Color::White))
      {
        pushMove(E8, G8, PieceType::BlackKing, PieceType::None, PieceType::None, Move::Type::Castle, moveList);
      }
    }
    if (castlingRights() & CASTLE_BLACK_QUEEN) {
      if (mPieceType[Square::D8] == PieceType::None &&
          mPieceType[Square::C8] == PieceType::None &&
          mPieceType[Square::B8] == PieceType::None &&
          !isCellAttacked(Square::E8, Color::White) &&
          !isCellAttacked(Square::D8, Color::White) &&
          !isCellAttacked(Square::C8, Color::White))
      {
        pushMove(E8, C8, PieceType::BlackKing, PieceType::None, PieceType::None, Move::Type::Castle, moveList);
      }
    }
  }
}

void BitBoard::generateMoves(MoveList & moveList) const
{
  // Generate the castling moves
  generateCastlingMoves(moveList);

  // Generate the pawn non capture moves
  if (sideToMove() == Color::White) {
    generateWhitePawnMoves(moveList);
  }
  else {
    generateBlackPawnMoves(moveList);
  }

  // Generate all attack moves
  U64 friends = (sideToMove() == Color::White) ? mWhitePieces : mBlackPieces;
  generateAttackMoves(friends, moveList);
}

void BitBoard::generateNonSliderAttacks(uchar fromSq, U64 bbAttack, U64 bbFriendly, MoveList & moveList) const
{
  U64 bbMove = bbAttack & ~bbFriendly;
  while (bbMove) {
    uchar toSq = bitScanForward(bbMove);
    Move::Type type = (mPieceType[toSq] == PieceType::None) ? Move::Type::Quiet : Move::Type::Capture;
    pushMove(fromSq, toSq, mPieceType[fromSq], mPieceType[toSq], PieceType::None, type, moveList);
    bbMove &= bbMove-1;
  }
}

void BitBoard::generatePawnAttacks(uchar fromSq, U64 bbAttack, U64 bbOpp, MoveList &moveList) const
{
  U64 bbMove = bbAttack & bbOpp;
  while (bbMove) {
    uchar toSq = bitScanForward(bbMove);
    uchar destRow = getRow(toSq);
    PieceType capturePiece = mPieceType[toSq];
    if (destRow == WHITE_PROMOTION_ROW) { // Promotion capture
      pushMove(fromSq, toSq, mPieceType[fromSq], capturePiece, PieceType::WhiteQueen, Move::Type::PromotionCapture, moveList);
      pushMove(fromSq, toSq, mPieceType[fromSq], capturePiece, PieceType::WhiteRook, Move::Type::PromotionCapture, moveList);
      pushMove(fromSq, toSq, mPieceType[fromSq], capturePiece, PieceType::WhiteBishop, Move::Type::PromotionCapture, moveList);
      pushMove(fromSq, toSq, mPieceType[fromSq], capturePiece, PieceType::WhiteKnight, Move::Type::PromotionCapture, moveList);
    }
    else if (destRow == BLACK_PROMOTION_ROW) { // Promotion capture
      pushMove(fromSq, toSq, mPieceType[fromSq], capturePiece, PieceType::BlackQueen, Move::Type::PromotionCapture, moveList);
      pushMove(fromSq, toSq, mPieceType[fromSq], capturePiece, PieceType::BlackRook, Move::Type::PromotionCapture, moveList);
      pushMove(fromSq, toSq, mPieceType[fromSq], capturePiece, PieceType::BlackBishop, Move::Type::PromotionCapture, moveList);
      pushMove(fromSq, toSq, mPieceType[fromSq], capturePiece, PieceType::BlackKnight, Move::Type::PromotionCapture, moveList);
    }
    else { // Standard capture
      pushMove(fromSq, toSq, mPieceType[fromSq], capturePiece, PieceType::None, Move::Type::Capture, moveList);
    }
    bbMove &= bbMove-1;
  }

  if (epColumn() != BoardBase::INVALID_EP) { // En-passant capture
    U64 bbMove = bbAttack;
    while (bbMove) {
      uchar toSq = bitScanForward(bbMove);
      uchar epIndex = getIndex(epRow(), epColumn());
      char epDir = (sideToMove() == Color::White) ? SOUTH : NORTH;
      if (epIndex == toSq) {
        pushMove(fromSq, toSq, mPieceType[fromSq], mPieceType[toSq+epDir], PieceType::None, Move::Type::EpCapture, moveList);
      }
      bbMove &= bbMove-1;
    }
  }
}

void BitBoard::generateSliderAttacks(uchar fromSq, U64 bbFriends, bool straight, bool diag, MoveList &moveList) const
{
  uint occupancy = 0;
  uint shift = 0;
  U64 bbMove = 0;

  if (straight) {

    // Rank attacks
    shift = getRow(fromSq) << 3;
    occupancy = (mAllPieces >> shift) & FullRank;
    bbMove = (mRankAttacks[fromSq][occupancy]) & ~bbFriends;
    pushBitBoardMoves(bbMove, fromSq, moveList);

    // File attacks
    shift = getCol(fromSq) << 3;
    occupancy = (mRotateRight90Pieces >> shift) & FullRank;
    bbMove = (mFileAttacks[fromSq][occupancy]) & ~bbFriends;
    pushBitBoardMoves(bbMove, fromSq, moveList);
  }

  if (diag) {

    // North east attacks
    shift = a1h8Shifts[fromSq];
    occupancy = (mRotateRight45Pieces >> shift) & a1h8Masks[fromSq];
    bbMove = mNorthEastAttacks[fromSq][occupancy] & ~bbFriends;
    pushBitBoardMoves(bbMove, fromSq, moveList);

    // North west attacks
    shift = a8h1Shifts[fromSq];
    occupancy = (mRotateLeft45Pieces >> shift) & a8h1Masks[fromSq];
    bbMove = mNorthWestAttacks[fromSq][occupancy] & ~bbFriends;
    pushBitBoardMoves(bbMove, fromSq, moveList);
  }
}

void BitBoard::generateWhitePawnMoves(MoveList &moveList) const
{
  U64 bbMove = 0;
  U64 promoMask = C64(255) << 56;

  // Promotion moves
  bbMove = (mWhitePawns << 8) & promoMask & mEmptySquares;
  while (bbMove) {
    int toSq = bitScanForward(bbMove);
    int fromSq = toSq + SOUTH;
    pushMove(fromSq, toSq, PieceType::WhitePawn, PieceType::None, PieceType::WhiteQueen, Move::Type::Promotion, moveList);
    pushMove(fromSq, toSq, PieceType::WhitePawn, PieceType::None, PieceType::WhiteRook, Move::Type::Promotion, moveList);
    pushMove(fromSq, toSq, PieceType::WhitePawn, PieceType::None, PieceType::WhiteBishop, Move::Type::Promotion, moveList);
    pushMove(fromSq, toSq, PieceType::WhitePawn, PieceType::None, PieceType::WhiteKnight, Move::Type::Promotion, moveList);
    bbMove &= bbMove - 1;
  }

  // Pawn pushes
  U64 pawnPushes = (mWhitePawns << 8) & mEmptySquares;
  bbMove = pawnPushes & ~promoMask;
  while (bbMove) {
    int toSq = bitScanForward(bbMove);
    int fromSq = toSq + SOUTH;
    pushMove(fromSq, toSq, PieceType::WhitePawn, PieceType::None, PieceType::None, Move::Type::Quiet, moveList);
    bbMove &= bbMove - 1;
  }

  // Pawn double pushes
  U64 unmovedPawns = (mWhitePawns & (255 << 8));
  U64 pawnDoublePushes = ( (unmovedPawns & (~(mAllPieces >> 8)) & (~(mAllPieces >> 16)) ) ) << 16;
  bbMove = pawnDoublePushes;
  while (bbMove) {
    int toSq = bitScanForward(bbMove);
    int fromSq = toSq + (SOUTH << 1);
    pushMove(fromSq, toSq, PieceType::WhitePawn, PieceType::None, PieceType::None, Move::Type::EpPush, moveList);
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
      mBlackPawnAttacks[index] = 0ULL;

      for (int k = 0; k < 2; k++) {
        char destRow = sourceRow + blackPawnRowIncr[k];
        char destCol = sourceCol + blackPawnColIncr[k];
        if (destRow >= 0 && destRow < 8 && destCol >= 0 && destCol < 8) {
          uchar destIndex = getIndex(destRow, destCol);
          mBlackPawnAttacks[index] |= (ONE << destIndex);
        }
      }
    }
  }
}

void BitBoard::initBoard()
{
  // Initialize the pieces and colors
  for (uint index = 0; index < 64; index++) {
    mPieceType[index] = PieceType::None;
  }

  for (uint col = 0; col < 8; col++) {
    mPieceType[getIndex(1, col)] = PieceType::WhitePawn;
    mPieceType[getIndex(6, col)] = PieceType::BlackPawn;
  }

  // Initialize white piece types
  mPieceType[getIndex(0, 0)] = mPieceType[getIndex(0, 7)] = PieceType::WhiteRook;
  mPieceType[getIndex(0, 1)] = mPieceType[getIndex(0, 6)] = PieceType::WhiteKnight;
  mPieceType[getIndex(0, 2)] = mPieceType[getIndex(0, 5)] = PieceType::WhiteBishop;
  mPieceType[getIndex(0, 3)] = PieceType::WhiteQueen;
  mPieceType[getIndex(0, 4)] = PieceType::WhiteKing;

  // Initialize black piece types
  mPieceType[getIndex(7, 0)] = mPieceType[getIndex(7, 7)] = PieceType::BlackRook;
  mPieceType[getIndex(7, 1)] = mPieceType[getIndex(7, 6)] = PieceType::BlackKnight;
  mPieceType[getIndex(7, 2)] = mPieceType[getIndex(7, 5)] = PieceType::BlackBishop;
  mPieceType[getIndex(7, 3)] = PieceType::BlackQueen;
  mPieceType[getIndex(7, 4)] = PieceType::BlackKing;

  // Initialize the white bit boards
  mWhiteBishops = mWhiteKings = mWhiteKnights = mWhiteQueens = mWhiteRooks = mWhitePawns = 0;
  mWhiteBishops = 0x24ULL;
  mWhiteKings = 0x10ULL;
  mWhiteKnights = 0x42ULL;
  mWhitePawns = (0xffULL) << 8ULL;
  mWhiteQueens = 0x08ULL;
  mWhiteRooks = 0x81ULL;

  // Initialize the black bit boards
  mBlackBishops = mBlackKings = mBlackKnights = mBlackQueens = mBlackRooks = mBlackPawns = 0;
  mBlackBishops = (0x24ULL << C64(56));
  mBlackKings = (0x10ULL << C64(56));
  mBlackKnights = (0x42ULL << C64(56));
  mBlackPawns = (0xffULL << C64(48));
  mBlackQueens = (0x08ULL << C64(56));
  mBlackRooks = (0x81ULL << C64(56));

  // Update the aggregate bit boards
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
      mWhitePawnAttacks[index] = 0ULL;

      for (int k = 0; k < 2; k++) {
        char destRow = sourceRow + whitePawnRowIncr[k];
        char destCol = sourceCol + whitePawnColIncr[k];
        if (destRow >= 0 && destRow < 8 && destCol >= 0 && destCol < 8) {
          uchar destIndex = getIndex(destRow, destCol);
          mWhitePawnAttacks[index] |= (ONE << destIndex);
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
  return false;
}

void BitBoard::makeMove(const Move & move)
{
  // Get move information
  Color side = sideToMove();
  uchar fromSquare = getIndex(move.sourceRow(), move.sourceCol());
  uchar toSquare = getIndex(move.destRow(), move.destCol());
  PieceType fromPiece = mPieceType[fromSquare];
  PieceType toPiece = mPieceType[toSquare];

  // Calculate required bitboards
  U64 bbFrom = (ONE << fromSquare);
  U64 bbTo = (ONE << toSquare);
  U64 bbFromTo = bbFrom ^ bbTo;

  // Update the color and piece arrays
  mPieceType[fromSquare] = PieceType::None;
  mPieceType[toSquare] = fromPiece;

  // Handle non promotion moves
  if (!move.isPromotion() && !move.isPromotionCapture()) {
    switch (fromPiece) {
      case PieceType::WhitePawn:
        mWhitePawns ^= bbFromTo;
        break;
      case PieceType::WhiteBishop:
        mWhiteBishops ^= bbFromTo;
        break;
      case PieceType::WhiteQueen:
        mWhiteQueens ^= bbFromTo;
        break;
      case PieceType::WhiteKnight:
        mWhiteKnights ^= bbFromTo;
        break;
      case PieceType::WhiteKing:
        mWhiteKings ^= bbFromTo;
        break;
      case PieceType::WhiteRook:
        mWhiteRooks ^= bbFromTo;
        break;
      case PieceType::BlackPawn:
        mBlackPawns ^= bbFromTo;
        break;
      case PieceType::BlackBishop:
        mBlackBishops ^= bbFromTo;
        break;
      case PieceType::BlackQueen:
        mBlackQueens ^= bbFromTo;
        break;
      case PieceType::BlackKnight:
        mBlackKnights ^= bbFromTo;
        break;
      case PieceType::BlackKing:
        mBlackKings ^= bbFromTo;
        break;
      case PieceType::BlackRook:
        mBlackRooks ^= bbFromTo;
        break;
    }
  }

  // Handle captures
  if (move.isCapture() && !move.isEpCapture()) {
    switch (toPiece) {
      case PieceType::WhitePawn:
        mWhitePawns ^= (ONE << toSquare);
        break;
      case PieceType::WhiteBishop:
        mWhiteBishops ^= (ONE << toSquare);
        break;
      case PieceType::WhiteQueen:
        mWhiteQueens ^= (ONE << toSquare);
        break;
      case PieceType::WhiteKnight:
        mWhiteKnights ^= (ONE << toSquare);
        break;
      case PieceType::WhiteKing:
        mWhiteKings ^= (ONE << toSquare);
        break;
      case PieceType::WhiteRook:
        mWhiteRooks ^= (ONE << toSquare);
        break;
      case PieceType::BlackPawn:
        mBlackPawns ^= (ONE << toSquare);
        break;
      case PieceType::BlackBishop:
        mBlackBishops ^= (ONE << toSquare);
        break;
      case PieceType::BlackQueen:
        mBlackQueens ^= (ONE << toSquare);
        break;
      case PieceType::BlackKnight:
        mBlackKnights ^= (ONE << toSquare);
        break;
      case PieceType::BlackKing:
        mBlackKings ^= (ONE << toSquare);
        break;
      case PieceType::BlackRook:
        mBlackRooks ^= (ONE << toSquare);
        break;
    }
  }

  // Handle enPassant captures
  if (move.isEpCapture()) {
    char dir = (side == Color::White) ? SOUTH : NORTH;
    U64 & oppPawns = (side == Color::White) ? mBlackPawns : mWhitePawns;
    uchar sq = toSquare + dir;
    mPieceType[sq] = PieceType::None;
    oppPawns ^= (ONE << sq);
  }

  // Handle promotions
  if (move.isPromotion() || move.isPromotionCapture()) {

    // Get bit boards
    U64 & pawns = (side == Color::White) ? mWhitePawns : mBlackPawns;
    U64 & queens = (side == Color::White) ? mWhiteQueens : mBlackQueens;
    U64 & rooks = (side == Color::White) ? mWhiteRooks : mBlackRooks;
    U64 & knights = (side == Color::White) ? mWhiteKnights : mBlackKnights;
    U64 & bishops = (side == Color::White) ? mWhiteBishops : mBlackBishops;

    // Remove the pawn
    pawns ^= (ONE << fromSquare);

    // Update the piece type array and the bit boards
    switch (move.promotedPiece()) {
      case Piece::Queen:
        mPieceType[toSquare] = (side == Color::White) ? PieceType::WhiteQueen : PieceType::BlackQueen;
        queens ^= (ONE << toSquare);
        break;
      case Piece::Rook:
        mPieceType[toSquare] = (side == Color::White) ? PieceType::WhiteRook : PieceType::BlackRook;
        rooks ^= (ONE << toSquare);
        break;
      case Piece::Bishop:
        mPieceType[toSquare] = (side == Color::White) ? PieceType::WhiteBishop : PieceType::BlackBishop;
        bishops ^= (ONE << toSquare);
        break;
      case Piece::Knight:
        mPieceType[toSquare] = (side == Color::White) ? PieceType::WhiteKnight : PieceType::BlackKnight;
        knights ^= (ONE << toSquare);
        break;
    }
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
      mPieceType[D1] = PieceType::WhiteRook;
      mPieceType[A1] = PieceType::None;
      mWhiteRooks ^= (ONE << D1);
      mWhiteRooks ^= (ONE << A1);
      break;
    case G1:
      mPieceType[F1] = PieceType::WhiteRook;
      mPieceType[H1] = PieceType::None;
      mWhiteRooks ^= (ONE << F1);
      mWhiteRooks ^= (ONE << H1);
      break;
    case C8:
      mPieceType[D8] = PieceType::BlackRook;
      mPieceType[A8] = PieceType::None;
      mBlackRooks ^= (ONE << D8);
      mBlackRooks ^= (ONE << A8);
      break;
    case G8:
      mPieceType[F8] = PieceType::BlackRook;
      mPieceType[H8] = PieceType::None;
      mBlackRooks ^= (ONE << F8);
      mBlackRooks ^= (ONE << H8);
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
    PieceType capturePiece = mPieceType[toSq];
    Move::Type type = (capturePiece == PieceType::None) ? Move::Type::Quiet : Move::Type::Capture;
    pushMove(fromSq, toSq, mPieceType[fromSq], capturePiece, PieceType::None, type, moveList);
    bb &= bb-1;
  }
}

void BitBoard::pushMove(uchar from, uchar to, PieceType piece, PieceType capture, PieceType promote, Move::Type type, MoveList & moveList) const
{
  Move move(getRow(from), getCol(from), getRow(to), getCol(to), typeToPiece(piece));
  move.setCastlingRights(castlingRights());
  move.setHalfMoveClock(halfMoveClock());
  move.setFullMoveCounter(fullMoveCounter());
  move.setType(type);
  move.setCapturedPiece(typeToPiece(capture));
  move.setPromotedPiece(typeToPiece(promote));
  move.setEnpassantColumn(epColumn());

  moveList.addMove(move);
}

PieceType BitBoard::pieceType(uchar row, uchar col) const
{
  return mPieceType[getIndex(row, col)];
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
  setSideToMove(color);

  clearEpCol();
  if (fenData.epColumn() >= 0) {
    setEpColumn(fenData.epColumn());
  }

  mWhiteBishops = mWhiteKings = mWhiteKnights = mWhitePawns = mWhiteQueens = mWhiteRooks = 0;
  mBlackBishops = mBlackKings = mBlackKnights = mBlackPawns = mBlackQueens = mBlackRooks = 0;
  for (uchar row = 0; row < 8; row++) {
    for (uchar col = 0; col < 8; col++) {
      PieceType type = fenData.pieceType(row, col);
      uchar sq = getIndex(row, col);
      ulonglong bb = (ONE << sq);

      mPieceType[sq] = type;
      switch (type) {
      case PieceType::WhitePawn:
        mWhitePawns |= bb;
        break;
      case PieceType::WhiteRook:
        mWhiteRooks |= bb;
        break;
      case PieceType::WhiteKnight:
        mWhiteKnights |= bb;
        break;
      case PieceType::WhiteBishop:
        mWhiteBishops |= bb;
        break;
      case PieceType::WhiteQueen:
        mWhiteQueens |= bb;
        break;
      case PieceType::WhiteKing:
        mWhiteKings |= bb;
        setKingRow(Color::White, row);
        setKingColumn(Color::White, col);
        break;
      case PieceType::BlackPawn:
        mBlackPawns |= bb;
        break;
      case PieceType::BlackRook:
        mBlackRooks |= bb;
        break;
      case PieceType::BlackKnight:
        mBlackKnights |= bb;
        break;
      case PieceType::BlackBishop:
        mBlackBishops |= bb;
        break;
      case PieceType::BlackQueen:
        mBlackQueens |= bb;
        break;
      case PieceType::BlackKing:
        setKingRow(Color::Black, row);
        setKingColumn(Color::Black, col);
        mBlackKings |= bb;
        break;
      default:
        break;
      }
    }
  }

  // Update aggregate bitboards
  updateAggregateBitBoards();
}

Piece BitBoard::typeToPiece(PieceType type) const
{
  Piece piece = Piece::None;

  switch (type) {
    case PieceType::WhiteBishop:
    case PieceType::BlackBishop:
      piece = Piece::Bishop;
      break;
    case PieceType::WhiteKing:
    case PieceType::BlackKing:
      piece = Piece::King;
      break;
    case PieceType::WhiteKnight:
    case PieceType::BlackKnight:
      piece = Piece::Knight;
      break;
    case PieceType::WhitePawn:
    case PieceType::BlackPawn:
      piece = Piece::Pawn;
      break;
    case PieceType::WhiteQueen:
    case PieceType::BlackQueen:
      piece = Piece::Queen;
      break;
    case PieceType::WhiteRook:
    case PieceType::BlackRook:
      piece = Piece::Rook;
      break;
  }

  return piece;
}

void BitBoard::unmakeMove(const Move & move)
{
  // Get move information
  Color side = sideToMove();
  Piece piece = move.piece();
  uchar fromSquare = getIndex(move.sourceRow(), move.sourceCol());
  uchar toSquare = getIndex(move.destRow(), move.destCol());
  PieceType toPiece = mPieceType[toSquare];

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
  mPieceType[fromSquare] = toPiece;
  mPieceType[toSquare] = PieceType::None;

  if (!move.isPromotion() && !move.isPromotionCapture()) {
    switch (toPiece) {
      case PieceType::WhitePawn:
        mWhitePawns ^= bbFromTo;
        break;
      case PieceType::WhiteBishop:
        mWhiteBishops ^= bbFromTo;
        break;
      case PieceType::WhiteQueen:
        mWhiteQueens ^= bbFromTo;
        break;
      case PieceType::WhiteKnight:
        mWhiteKnights ^= bbFromTo;
        break;
      case PieceType::WhiteKing:
        mWhiteKings ^= bbFromTo;
        break;
      case PieceType::WhiteRook:
        mWhiteRooks ^= bbFromTo;
        break;
      case PieceType::BlackPawn:
        mBlackPawns ^= bbFromTo;
        break;
      case PieceType::BlackBishop:
        mBlackBishops ^= bbFromTo;
        break;
      case PieceType::BlackQueen:
        mBlackQueens ^= bbFromTo;
        break;
      case PieceType::BlackKnight:
        mBlackKnights ^= bbFromTo;
        break;
      case PieceType::BlackKing:
        mBlackKings ^= bbFromTo;
        break;
      case PieceType::BlackRook:
        mBlackRooks ^= bbFromTo;
        break;
    }
  }

  // Handle captures
  if (move.isCapture()) {
    uchar sq = toSquare;
    if (move.isEpCapture()) {
      char dir = (side == Color::White) ? NORTH : SOUTH;
      sq = toSquare + dir;
    }

    U64 & pawns = (side == Color::White) ? mWhitePawns : mBlackPawns;
    U64 & rooks = (side == Color::White) ? mWhiteRooks : mBlackRooks;
    U64 & queens = (side == Color::White) ? mWhiteQueens : mBlackQueens;
    U64 & kings = (side == Color::White) ? mWhiteKings : mBlackKings;
    U64 & knights = (side == Color::White) ? mWhiteKnights : mBlackKnights;
    U64 & bishops = (side == Color::White) ? mWhiteBishops : mBlackBishops;
    switch (move.capturePiece()) {
      case Piece::Pawn:
        mPieceType[sq] = (side == Color::White) ? PieceType::WhitePawn : PieceType::BlackPawn;
        pawns ^= (ONE << sq);
        break;
      case Piece::Bishop:
        mPieceType[sq] = (side == Color::White) ? PieceType::WhiteBishop : PieceType::BlackBishop;
        bishops ^= (ONE << sq);
        break;
      case Piece::Queen:
        mPieceType[sq] = (side == Color::White) ? PieceType::WhiteQueen : PieceType::BlackQueen;
        queens ^= (ONE << sq);
        break;
      case Piece::King:
        mPieceType[sq] = (side == Color::White) ? PieceType::WhiteKing : PieceType::BlackKing;
        kings ^= (ONE << sq);
        break;
      case Piece::Rook:
        mPieceType[sq] = (side == Color::White) ? PieceType::WhiteRook : PieceType::BlackRook;
        rooks ^= (ONE << sq);
        break;
      case Piece::Knight:
        mPieceType[sq] = (side == Color::White) ? PieceType::WhiteKnight : PieceType::BlackKnight;
        knights ^= (ONE << sq);
        break;
    }
  }

  // Handle promotions
  if (move.isPromotion() || move.isPromotionCapture()) {
    mPieceType[fromSquare] = (side == Color::White) ? PieceType::BlackPawn : PieceType::WhitePawn;

    U64 & pawns = (side == Color::Black) ? mWhitePawns : mBlackPawns;
    U64 & rooks = (side == Color::Black) ? mWhiteRooks : mBlackRooks;
    U64 & queens = (side == Color::Black) ? mWhiteQueens : mBlackQueens;
    U64 & knights = (side == Color::Black) ? mWhiteKnights : mBlackKnights;
    U64 & bishops = (side == Color::Black) ? mWhiteBishops : mBlackBishops;

    pawns ^= (ONE << fromSquare);
    switch (move.promotedPiece()) {
      case Piece::Queen:
        queens ^= (ONE << toSquare);
        break;
      case Piece::Bishop:
        bishops ^= (ONE << toSquare);
        break;
      case Piece::Rook:
        rooks ^= (ONE << toSquare);
        break;
      case Piece::Knight:
        knights ^= (ONE << toSquare);
        break;
    }
  }

  // Handle king moves
  if (piece == Piece::King) {
    setKingRow(!side, move.sourceRow());
    setKingColumn(!side, move.sourceCol());
  }

  // Handle castling move
  if (move.isCastle()) {
    switch (toSquare) {
    case C1:
      mPieceType[A1] = PieceType::WhiteRook;
      mPieceType[D1] = PieceType::None;
      mWhiteRooks ^= (ONE << A1);
      mWhiteRooks ^= (ONE << D1);
      break;
    case G1:
      mPieceType[H1] = PieceType::WhiteRook;
      mPieceType[F1] = PieceType::None;
      mWhiteRooks ^= (ONE << H1);
      mWhiteRooks ^= (ONE << F1);
      break;
    case C8:
      mPieceType[A8] = PieceType::BlackRook;
      mPieceType[D8] = PieceType::None;
      mBlackRooks ^= (ONE << A8);
      mBlackRooks ^= (ONE << D8);
      break;
    case G8:
      mPieceType[H8] = PieceType::BlackRook;
      mPieceType[F8] = PieceType::None;
      mBlackRooks ^= (ONE << H8);
      mBlackRooks ^= (ONE << F8);
      break;
    }
  }

  // Update the aggregate bitboards
  updateAggregateBitBoards();

  // Switch sides
  toggleSideToMove();
}

void BitBoard::updateAggregateBitBoards()
{
  // Initialize aggregate bitboards
  mWhitePieces = mWhiteBishops | mWhiteKings | mWhiteKnights | mWhitePawns | mWhiteQueens | mWhiteRooks;
  mBlackPieces = mBlackBishops | mBlackKings | mBlackKnights | mBlackPawns | mBlackQueens | mBlackRooks;
  mAllPieces = mWhitePieces | mBlackPieces;
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

void BitBoard::writeBoards() const
{
  //writeBitBoard(mWhitePawns, std::cout);
  //writeBitBoard(mWhiteRooks, std::cout);
  //writeBitBoard(mWhiteKnights, std::cout);
  //writeBitBoard(mWhiteBishops, std::cout);
  //writeBitBoard(mWhiteQueens, std::cout);
  //writeBitBoard(mWhiteKings, std::cout);
  //writeBitBoard(mBlackPawns, std::cout);
  //writeBitBoard(mBlackRooks, std::cout);
  //writeBitBoard(mBlackKnights, std::cout);
  //writeBitBoard(mBlackBishops, std::cout);
  //writeBitBoard(mBlackQueens, std::cout);
  //writeBitBoard(mBlackKings, std::cout);
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

//uchar index = 2;
//U64 friends = mWhitePieces;
//U64 friends = mBlackPieces;
//  U64 friends = 0;
//  friends |= (ONE << A2);
//  friends |= (ONE << B3);
//friends |= (ONE << C4);
//  friends |= (ONE << D5);
//  friends |= (ONE << E6);
//  friends |= (ONE << F7);
//friends |= (ONE << G8);
//friends |= (ONE << H8);

//  friends |= (ONE << G1);
//  friends |= (ONE << H1);
//  friends |= (ONE << A2);
//  friends |= (ONE << B2);
//  friends |= (ONE << C2);
//  friends |= (ONE << D2);
//  friends |= (ONE << E2);
//  friends |= (ONE << F2);
//  friends |= (ONE << G2);
//  friends |= (ONE << H2);

//  uchar row = getRow(index);
//  uchar col = getCol(index);

//  //uchar shift = a8h1Shifts[index];
//  //uchar mask = a8h1Masks[index];
//  uchar shift = a1h8Shifts[index];
//  uchar mask = a1h8Masks[index];

//  std::cout << "Pieces\n";
//  writeBitBoard(friends, std::cout);

//  std::cout << "Rotate 45\n";
//  U64 bbRot = rotateR45(friends);
//  writeBitBoard(bbRot, std::cout);

//  std::cout << "Shifted\n";
//  U64 bbShift = bbRot >> shift;
//  writeBitBoard(bbShift, std::cout);

//  uint occupancy = bbShift & mask;
//  writeOccupancy(occupancy); std::cout << "\n";

//  std::cout << "Attacks\n";
//  U64 attackBB = mNorthEastAttacks[index][occupancy] & ~friends;
//  writeBitBoard(attackBB, std::cout);

//MoveList m;
//generateSliderAttacks(5, friends, false, true, m);
//m.print(std::cout);
//exit(1);

//  U64 attacks[64][256];

//  uchar index = 18;
//  uchar row = getRow(index);
//  uchar col = getCol(index);

//  uchar shift = a1h8Shifts[index];
//  uchar mask = a1h8Masks[index];
//  uchar length = a1h8Lengths[index];

//  uint occupancy = 0;
//  occupancy += (1 << B1);
//  occupancy += (1 << F1);

//  std::cout << "Occupancy\n";
//  writeOccupancy(occupancy); std::cout << "\n";

//  U64 bb = 0;
//  for (int i = col+1; i < length; i++) {
//    bb += (ONE << i);
//    if (occupancy & (1 << i)) break;
//  }
//  for (int i = col-1; i >= 0; i--) {
//    bb += (ONE << i);
//    if (occupancy & (1 << i)) break;
//  }

//  std::cout << "Occupancy BB\n";
//  writeBitBoard(bb, std::cout);

//  std::cout << "Occupancy BB Shifted\n";
//  U64 bbShift = bb << shift;
//  writeBitBoard(bbShift, std::cout);

//  std::cout << "Occupancy BB Rotate\n";
//  U64 bbRot = rotateL45(bbShift);
//  writeBitBoard(bbRot, std::cout);
//  attacks[index][occupancy] = bbRot;

//  U64 pieces = 0;
//  pieces += (ONE << B2);
//  pieces += (ONE << F6);

//  std::cout << "Pieces\n";
//  writeBitBoard(pieces, std::cout);

//  std::cout << "Pieces Rotated\n";
//  U64 piecesRot = rotateR45(pieces);
//  writeBitBoard(piecesRot, std::cout);

//  std::cout << "Pieces Shifted\n";
//  U64 piecesShift = piecesRot >> shift;
//  writeBitBoard(piecesShift, std::cout);

//  uint occ = piecesShift & mask;
//  U64 attackBB = attacks[index][occ];
//  std::cout << "Attacks\n";
//  writeBitBoard(attackBB, std::cout);
