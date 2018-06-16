#include "bitboard.h"
#include "fen.h"
#include "move.h"
#include "movelist.h"

#include <cassert>
#include <sstream>

// Macros for mapping (row,col)->index and vice-versa and valid squares
#define C64(constantU64) constantU64##ULL
#define ONE C64(1)
#define getIndex(row,col) (((row)<<3)+(col))
#define getRow(index) ( (index >> 3) )
#define getCol(index) ( (index & 7) )

static const int FullRank = 255;

// Attack offsets
static const char NORTH =  8;
static const char SOUTH = -8;
static const char EAST  =  1;
static const char WEST  = -1;
static const char NW    =  7;
static const char NE    =  9;
static const char SW    = -9;
static const char SE    = -7;
static const char SSW   = -17;
static const char SSE   = -15;
static const char WWS   = -10;
static const char EES   = -6;
static const char EEN   =  10;
static const char WWN   =  6;
static const char NNE   =  17;
static const char NNW   =  15;

// Attack vectors for pieces
// The king and queen attacks are identical but are both included for clarity
static const char whitePawnAttacks[] = {NE, NW};
static const char blackPawnAttacks[] = {SE, SW};
static const char rookAttacks[]      = {NORTH, SOUTH, EAST, WEST};
static const char bishopAttacks[]    = {NW, NE, SE, SW};
static const char knightAttacks[]    = {SSW, SSE, WWS, EES, EEN, WWN, NNE, NNW};
static const char queenAttacks[]     = {NORTH, SOUTH, EAST, WEST, NW, NE, SW, SE};
static const char kingAttacks[]      = {NORTH, SOUTH, EAST, WEST, NW, NE, SW, SE};

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
  initNonSliderAttacks();
  initStraightAttacks();
  initDiagonalAttacks();

  for (uint i = 0; i < 64; i++) {
    mInvRotR45[rotateR45Matrix[i]] = i;
  }
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
   static const ulonglong debruijn64 = ulonglong(0x07EDD5E59A4E28C2);
   assert (bb != 0);
   return index64[((bb & -bb) * debruijn64) >> 58];
}

uchar BitBoard::bitScanForwardPopCount(U64 bb) const
{
  assert (bb != 0);
  return popCount( (bb & -bb) - 1 );
}

void BitBoard::generateAttackMoves(U64 bb, MoveList &moveList) const
{
  U64 bbFriends = bb;

  Color side = sideToMove();
  const U64 * pawnAttacks = (side == Color::White) ? mWhitePawnAttacks : mBlackPawnAttacks;
  U64 oppPieces = (side == Color::White) ? mBlackPieces : mWhitePieces;

  while (bb) {
    uchar fromSq = bitScanForward(bb);

    Piece piece = mPieces[fromSq];
    switch (piece) {
    case Piece::Pawn:
      generatePawnAttacks(fromSq, pawnAttacks[fromSq], oppPieces, moveList);
      break;
    case Piece::Rook:
      generateSliderAttacks(fromSq, bbFriends, true, false, moveList);
      break;
    case Piece::Knight:
      generateNonSliderAttacks(fromSq, mKnightAttacks[fromSq], bbFriends, moveList);
      break;
    case Piece::Bishop:
      generateSliderAttacks(fromSq, bbFriends, false, true, moveList);
      break;
    case Piece::Queen:
      generateSliderAttacks(fromSq, bbFriends, true, true, moveList);
      break;
    case Piece::King:
      generateNonSliderAttacks(fromSq, mKingAttacks[fromSq], bbFriends, moveList);
      break;
    }
    bb &= bb-1;
  }
}

void BitBoard::generateCastlingMoves(MoveList & moveList) const
{
  if (sideToMove() == Color::White) {
    if (castlingRights() & CASTLE_WHITE_KING) {
      if (mPieces[Square::F1] == Piece::None &&
          mPieces[Square::G1] == Piece::None &&
          !isCellAttacked(Square::E1, Color::Black) &&
          !isCellAttacked(Square::F1, Color::Black) &&
          !isCellAttacked(Square::G1, Color::Black))
      {
        pushMove(E1, G1, Piece::King, Piece::None, Piece::None, Move::Type::Castle, moveList);
      }
    }
    if (castlingRights() & CASTLE_WHITE_QUEEN) {
      if (mPieces[Square::D1] == Piece::None &&
          mPieces[Square::C1] == Piece::None &&
          mPieces[Square::B1] == Piece::None &&
          !isCellAttacked(Square::E1, Color::Black) &&
          !isCellAttacked(Square::D1, Color::Black) &&
          !isCellAttacked(Square::C1, Color::Black))
      {
        pushMove(E1, C1, Piece::King, Piece::None, Piece::None, Move::Type::Castle, moveList);
      }
    }
  }
  else {
    if (castlingRights() & CASTLE_BLACK_KING) {
      if (mPieces[Square::F8] == Piece::None &&
          mPieces[Square::G8] == Piece::None &&
          !isCellAttacked(Square::E8, Color::White) &&
          !isCellAttacked(Square::F8, Color::White) &&
          !isCellAttacked(Square::G8, Color::White))
      {
        pushMove(E8, G8, Piece::King, Piece::None, Piece::None, Move::Type::Castle, moveList);
      }
    }
    if (castlingRights() & CASTLE_BLACK_QUEEN) {
      if (mPieces[Square::D8] == Piece::None &&
          mPieces[Square::C8] == Piece::None &&
          mPieces[Square::B8] == Piece::None &&
          !isCellAttacked(Square::E8, Color::White) &&
          !isCellAttacked(Square::D8, Color::White) &&
          !isCellAttacked(Square::C8, Color::White))
      {
        pushMove(E8, C8, Piece::King, Piece::None, Piece::None, Move::Type::Castle, moveList);
      }
    }
  }
}

void BitBoard::generateMoves(MoveList & moveList) const
{
  // Generate the castling moves
  generateCastlingMoves(moveList);

  // Generate the pawn non capture moves
  U64 pieces = (sideToMove() == Color::White) ? mWhitePieces : mBlackPieces;
  if (sideToMove() == Color::White) {
    generateWhitePawnMoves(moveList);
  }
  else {
    generateBlackPawnMoves(moveList);
  }

  // Generate all attack moves
  generateAttackMoves(pieces, moveList);
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
    pushMove(fromSq, toSq, mPieces[fromSq], mPieces[toSq], Piece::Queen, Move::Type::Promotion, moveList);
    pushMove(fromSq, toSq, mPieces[fromSq], mPieces[toSq], Piece::Rook, Move::Type::Promotion, moveList);
    pushMove(fromSq, toSq, mPieces[fromSq], mPieces[toSq], Piece::Bishop, Move::Type::Promotion, moveList);
    pushMove(fromSq, toSq, mPieces[fromSq], mPieces[toSq], Piece::Knight, Move::Type::Promotion, moveList);

    bbMove &= bbMove - 1;
  }

  // Pawn pushes
  U64 pawnPushes = (mBlackPawns >> 8) & mEmptySquares;
  bbMove = pawnPushes & ~promoMask;
  while (bbMove) {
    int toSq = bitScanForward(bbMove);
    int fromSq = toSq + NORTH;
    pushMove(fromSq, toSq, mPieces[fromSq], Piece::None, Piece::None, Move::Type::Quiet, moveList);

    bbMove &= bbMove - 1;
  }

  // Double pawn pushes
  U64 unmovedPawns = (mBlackPawns & (C64(255) << 48));
  U64 pawnDoublePushes = ( (unmovedPawns & (~(mAllPieces << 8)) & (~(mAllPieces << 16)) ) ) >> 16;
  bbMove = pawnDoublePushes;
  while (bbMove) {
    int toSq = bitScanForward(bbMove);
    int fromSq = toSq + (NORTH << 1);
    pushMove(fromSq, toSq, mPieces[fromSq], Piece::None, Piece::None, Move::Type::EpPush, moveList);

    bbMove &= bbMove - 1;
  }
}

void BitBoard::generateNonSliderAttacks(uchar fromSq, U64 bbAttack, U64 bbFriendly, MoveList & moveList) const
{
  U64 bbMove = bbAttack & ~bbFriendly;
  while (bbMove) {
    uchar toSq = bitScanForward(bbMove);
    Move::Type type = (mPieces[toSq] == Piece::None) ? Move::Type::Quiet : Move::Type::Capture;
    pushMove(fromSq, toSq, mPieces[fromSq], mPieces[toSq], Piece::None, type, moveList);
    bbMove &= bbMove-1;
  }
}

void BitBoard::generatePawnAttacks(uchar fromSq, U64 bbAttack, U64 bbOpp, MoveList &moveList) const
{
  U64 bbMove = bbAttack & bbOpp;
  while (bbMove) {
    uchar toSq = bitScanForward(bbMove);
    uchar destRow = getRow(toSq);
    Piece capturePiece = mPieces[toSq];
    if (destRow == BLACK_PROMOTION_ROW || destRow == WHITE_PROMOTION_ROW) { // Promotion capture
      pushMove(fromSq, toSq, mPieces[fromSq], capturePiece, Piece::Queen, Move::Type::PromotionCapture, moveList);
      pushMove(fromSq, toSq, mPieces[fromSq], capturePiece, Piece::Rook, Move::Type::PromotionCapture, moveList);
      pushMove(fromSq, toSq, mPieces[fromSq], capturePiece, Piece::Bishop, Move::Type::PromotionCapture, moveList);
      pushMove(fromSq, toSq, mPieces[fromSq], capturePiece, Piece::Knight, Move::Type::PromotionCapture, moveList);
    }
    else { // Standard capture
      pushMove(fromSq, toSq, mPieces[fromSq], capturePiece, Piece::None, Move::Type::Capture, moveList);
    }

    if (epColumn() != BoardBase::INVALID_EP) { // En-passant capture
      uchar epIndex = getIndex(epRow(), epColumn());
      char epDir = (sideToMove() == Color::White) ? SOUTH : NORTH;
      if (epIndex == toSq) {
        pushMove(fromSq, toSq, Piece::Pawn, mPieces[toSq+epDir], Piece::None, Move::Type::EpCapture, moveList);
      }
    }
    bbMove &= bbMove-1;
  }
}

void BitBoard::generateSliderAttacks(uchar fromSq, U64 bbFriends, bool straight, bool diag, MoveList &moveList) const
{
  uint occupancy = 0;
  uint shift = 0;
  U64 bbMove = 0;

  if (straight) {

    // Generate rank attacks
    shift = getRow(fromSq) << 3;
    occupancy = (mAllPieces >> shift) & FullRank;
    bbMove = (mRankAttacks[fromSq][occupancy]) & ~bbFriends;
    pushBitBoardMoves(bbMove, fromSq, moveList);

    // Generate file attacks
    shift = getCol(fromSq) << 3;
    occupancy = (mRotateRight90Pieces >> shift) & FullRank;
    bbMove = (mFileAttacks[fromSq][occupancy]) & ~bbFriends;
    pushBitBoardMoves(bbMove, fromSq, moveList);
  }

  if (diag) {

    // Generate north east attacks
    shift = a1h8Shifts[fromSq];
    occupancy = (mRotateRight45Pieces >> shift) & a1h8Masks[fromSq];
    bbMove = mDiagAttacksNorthEast[fromSq][occupancy] & ~bbFriends;
    pushBitBoardMoves(bbMove, fromSq, moveList);

    // Generate north west attacks
    shift = a8h1Shifts[fromSq];
    occupancy = (mRotateLeft45Pieces >> shift) & a8h1Masks[fromSq];
    bbMove = mDiagAttacksNorthWest[fromSq][occupancy] & ~bbFriends;
    pushBitBoardMoves(bbMove, fromSq, moveList);

    if (fromSq == 65) {
      writeBitBoard(mAllPieces, std::cout);
      writeBitBoard(mRotateRight45Pieces, std::cout);
      writeBitBoard((mRotateRight45Pieces >> shift), std::cout);
      writeBitBoard(bbMove, std::cout);
      writeOccupancy(occupancy);
    }
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
    pushMove(fromSq, toSq, mPieces[fromSq], mPieces[toSq], Piece::Queen, Move::Type::Promotion, moveList);
    pushMove(fromSq, toSq, mPieces[fromSq], mPieces[toSq], Piece::Rook, Move::Type::Promotion, moveList);
    pushMove(fromSq, toSq, mPieces[fromSq], mPieces[toSq], Piece::Bishop, Move::Type::Promotion, moveList);
    pushMove(fromSq, toSq, mPieces[fromSq], mPieces[toSq], Piece::Knight, Move::Type::Promotion, moveList);

    bbMove &= bbMove - 1;
  }

  // Pawn pushes
  U64 pawnPushes = (mWhitePawns << 8) & mEmptySquares;
  bbMove = pawnPushes & ~promoMask;
  while (bbMove) {
    int toSq = bitScanForward(bbMove);
    int fromSq = toSq + SOUTH;
    pushMove(fromSq, toSq, mPieces[fromSq], Piece::None, Piece::None, Move::Type::Quiet, moveList);

    bbMove &= bbMove - 1;
  }

  // Pawn double pushes
  U64 unmovedPawns = (mWhitePawns & (255 << 8));
  U64 pawnDoublePushes = ( (unmovedPawns & (~(mAllPieces >> 8)) & (~(mAllPieces >> 16)) ) ) << 16;
  bbMove = pawnDoublePushes;
  while (bbMove) {
    int toSq = bitScanForward(bbMove);
    int fromSq = toSq + (SOUTH << 1);
    pushMove(fromSq, toSq, mPieces[fromSq], Piece::None, Piece::None, Move::Type::EpPush, moveList);

    bbMove &= bbMove - 1;
  }
}

void BitBoard::initNonSliderAttacks()
{
  static const int knightRowIncr[]    = {1, 2, 2, 1, -1, -2, -2, -1};
  static const int knightColIncr[]    = {-2, -1, 1, 2, 2, 1, -1, -2};
  static const int kingRowIncr[]      = {1, 1, 1, 0, -1, -1, -1, 0};
  static const int kingColIncr[]      = {-1, 0, 1, 1, 1, 0, -1, -1};
  static const int whitePawnRowIncr[] = {1, 1};
  static const int whitePawnColIncr[] = {-1, 1};
  static const int blackPawnRowIncr[] = {-1, -1};
  static const int blackPawnColIncr[] = {-1, 1};

  for (int i = 0; i < 64; i++) {
    mKingAttacks[i] = 0ULL;
    mKnightAttacks[i] = 0ULL;
    mWhitePawnAttacks[i] = 0ULL;
    mBlackPawnAttacks[i] = 0ULL;
  }

  // Precompute attacks for each square
  for (char sourceRow = 0; sourceRow < 8; sourceRow++) {
    for (char sourceCol = 0; sourceCol < 8; sourceCol++) {
      uchar index = getIndex(sourceRow, sourceCol);

      // White pawn attacks
      for (int k = 0; k < 2; k++) {
        char destRow = sourceRow + whitePawnRowIncr[k];
        char destCol = sourceCol + whitePawnColIncr[k];
        if (destRow >= 0 && destRow < 8 && destCol >= 0 && destCol < 8) {
          uchar destIndex = getIndex(destRow, destCol);
          mWhitePawnAttacks[index] |= (ONE << destIndex);
        }
      }

      // Black pawn attacks
      for (int k = 0; k < 2; k++) {
        char destRow = sourceRow + blackPawnRowIncr[k];
        char destCol = sourceCol + blackPawnColIncr[k];
        if (destRow >= 0 && destRow < 8 && destCol >= 0 && destCol < 8) {
          uchar destIndex = getIndex(destRow, destCol);
          mBlackPawnAttacks[index] |= (ONE << destIndex);
        }
      }

      // Knight attacks
      for (char k = 0; k < 8; k++) {
        char destRow = sourceRow + knightRowIncr[k];
        char destCol = sourceCol + knightColIncr[k];
        if (destRow >= 0 && destRow < 8 && destCol >= 0 && destCol < 8) {
          uchar destIndex = getIndex(destRow, destCol);
          mKnightAttacks[index] |= (ONE << destIndex);
        }
      }

      // King attacks
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

void BitBoard::initBoard()
{
  static const Piece pieces[] = { Piece::Rook, Piece::Knight, Piece::Bishop, Piece::Queen,
                                  Piece::King, Piece::Bishop, Piece::Knight, Piece::Rook };


  // Initialize the pieces and colors
  for (uint index = 0; index < 64; index++) {
    mPieces[index] = Piece::None;
    mColors[index] = Color::None;
  }

  for (uint col = 0; col < 8; col++) {
    mColors[getIndex(0, col)] = Color::White;
    mColors[getIndex(1, col)] = Color::White;
    mColors[getIndex(6, col)] = Color::Black;
    mColors[getIndex(7, col)] = Color::Black;

    mPieces[getIndex(0, col)] = pieces[col];
    mPieces[getIndex(1, col)] = Piece::Pawn;
    mPieces[getIndex(6, col)] = Piece::Pawn;
    mPieces[getIndex(7, col)] = pieces[col];
  }

  mWhiteBishops = mWhiteKings = mWhiteKnights = mWhiteQueens = mWhiteRooks = mWhitePawns = 0;
  mBlackBishops = mBlackKings = mBlackKnights = mBlackQueens = mBlackRooks = mBlackPawns = 0;

  // Initialize the white bit boards
  mWhiteBishops = 0x24ULL;
  mWhiteKings = 0x10ULL;
  mWhiteKnights = 0x42ULL;
  mWhitePawns = (0xffULL) << 8ULL;
  mWhiteQueens = 0x08ULL;
  mWhiteRooks = 0x81ULL;

  // Initialize the black bit boards
  mBlackBishops = (0x24ULL << C64(56));
  mBlackKings = (0x10ULL << C64(56));
  mBlackKnights = (0x42ULL << C64(56));
  mBlackPawns = (0xffULL << C64(48));
  mBlackQueens = (0x08ULL << C64(56));
  mBlackRooks = (0x81ULL << C64(56));

  updateAggregateBitBoards();
}

void BitBoard::initDiagonalAttacks()
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

      for (int i = row+1; i < length; i++) {
        bb += (ONE << i);
        if (occupancy & (1 << i)) break;
      }

      for (int i = row-1; i >= 0; i--) {
        bb += (ONE << i);
        if (occupancy & (1 << i)) break;
      }

      U64 bbRot = invRotateR45(bb << shift);
      mDiagAttacksNorthEast[index][occupancy] = bbRot;
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

      for (int i = (col)+1; i < length; i++) {
        bb += (ONE << i);
        if (occupancy & (1 << i)) break;
      }

      for (int i = (col)-1; i >= 0; i--) {
        bb += (ONE << i);
        if (occupancy & (1 << i)) break;
      }

      U64 bbRot = invRotateL45(bb << shift);
      mDiagAttacksNorthWest[index][occupancy] = bbRot;
    }
  }


  uchar index = 58;
  //U64 friends = mWhitePieces;
  //U64 friends = mBlackPieces;
  U64 friends = 0;
  friends |= (ONE << C8);
  friends |= (ONE << D7);
  friends |= (ONE << E6);
  friends |= (ONE << F5);
  friends |= (ONE << G4);
  friends |= (ONE << H3);
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

  uchar row = getRow(index);
  uchar col = getCol(index);

  uchar shift = a8h1Shifts[index];
  uchar mask = a8h1Masks[index];
  //uchar shift = a1h8Shifts[index];
  //uchar mask = a1h8Masks[index];

  std::cout << "Pieces\n";
  writeBitBoard(friends, std::cout);

  std::cout << "Rotate 45\n";
  U64 bbRot = rotateL45(friends);
  writeBitBoard(bbRot, std::cout);

  std::cout << "Shifted\n";
  U64 bbShift = bbRot >> shift;
  writeBitBoard(bbShift, std::cout);

  uint occupancy = bbShift & mask;
  writeOccupancy(occupancy); std::cout << "\n";

  std::cout << "Attacks\n";
  U64 attackBB = mDiagAttacksNorthWest[index][occupancy] & ~friends;
  writeBitBoard(attackBB, std::cout);

  //MoveList m;
  //generateSliderAttacks(5, friends, false, true, m);
  //m.print(std::cout);
  exit(1);

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

}

void BitBoard::initStraightAttacks()
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

  // Calculate required bitboards
  U64 bbFrom = (ONE << fromSquare);
  U64 bbTo = (ONE << toSquare);
  U64 bbFromTo = bbFrom ^ bbTo;

  // Update bit boards associated with the move
  U64 & pawns = (side == Color::White) ? mWhitePawns : mBlackPawns;
  U64 & rooks = (side == Color::White) ? mWhiteRooks : mBlackRooks;
  U64 & bishops = (side == Color::White) ? mWhiteBishops : mBlackBishops;
  U64 & knights = (side == Color::White) ? mWhiteKnights : mBlackKnights;
  U64 & queens = (side == Color::White) ? mWhiteQueens : mBlackQueens;
  U64 & kings = (side == Color::White) ? mWhiteKings : mBlackKings;
  switch (move.piece()) {
  case Piece::Pawn:
    pawns ^= bbFromTo;
    break;
  case Piece::Rook:
    rooks ^= bbFromTo;
    break;
  case Piece::Knight:
    knights ^= bbFromTo;
    break;
  case Piece::Bishop:
    bishops ^= bbFromTo;
    break;
  case Piece::Queen:
    queens ^= bbFromTo;
    break;
  case Piece::King:
    kings ^= bbFromTo;
    break;
  }

  // Update the color and piece arrays
  mColors[fromSquare] = Color::None;
  mPieces[fromSquare] = Piece::None;
  mColors[toSquare] = side;
  mPieces[toSquare] = move.piece();

  // Handle enPassant captures
  // May need to update bitboards here
  if (move.isEpCapture()) {
    char dir = (side == Color::White) ? SOUTH : NORTH;
    uchar sq = toSquare + dir;
    mPieces[sq] = Piece::None;
    mColors[sq] = Color::None;
  }

  // Handle promotions
  // May need to update bitboards here
  if (move.isPromotion() || move.isPromotionCapture()) {
    mPieces[toSquare] = move.promotedPiece();
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

  // May need to update bitboards here
  if (move.isCastle()) {
    switch (toSquare) {
    case C1:
      mPieces[D1] = Piece::Rook;
      mColors[D1] = Color::White;
      mPieces[A1] = Piece::None;
      mColors[A1] = Color::None;
      break;
    case G1:
      mPieces[F1] = Piece::Rook;
      mColors[F1] = Color::White;
      mPieces[H1] = Piece::None;
      mColors[H1] = Color::None;
      break;
    case C8:
      mPieces[D8] = Piece::Rook;
      mColors[D8] = Color::Black;
      mPieces[A8] = Piece::None;
      mColors[A8] = Color::None;
      break;
    case G8:
      mPieces[F8] = Piece::Rook;
      mColors[F8] = Color::Black;
      mPieces[H8] = Piece::None;
      mColors[H8] = Color::None;
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

  //writeBitBoard(mWhitePieces, std::cout);
  //writeBitBoard(mBlackPieces, std::cout);

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
    Piece capturePiece = mPieces[toSq];
    Move::Type type = (capturePiece == Piece::None) ? Move::Type::Quiet : Move::Type::Capture;
    pushMove(fromSq, toSq, mPieces[fromSq], capturePiece, Piece::None, type, moveList);
    bb &= bb - 1;
  }
}

void BitBoard::pushMove(uchar from, uchar to, Piece piece, Piece capture, Piece promote, Move::Type type, MoveList & moveList) const
{
  Move move(getRow(from), getCol(from), getRow(to), getCol(to), piece);
  move.setCastlingRights(castlingRights());
  move.setHalfMoveClock(halfMoveClock());
  move.setFullMoveCounter(fullMoveCounter());
  move.setType(type);
  move.setCapturedPiece(capture);
  move.setPromotedPiece(promote);
  move.setEnpassantColumn(epColumn());

  moveList.addMove(move);
}

PieceType BitBoard::pieceType(uchar row, uchar col) const
{
  uchar index = getIndex(row, col);
  Piece piece = mPieces[index];
  Color color = mColors[index];

  PieceType type = PieceType::None;
  switch (piece) {
  case Piece::Pawn:
    type = (color == Color::White) ? PieceType::WhitePawn : PieceType::BlackPawn;
    break;
  case Piece::Rook:
    type = (color == Color::White) ? PieceType::WhiteRook : PieceType::BlackRook;
    break;
  case Piece::Knight:
    type = (color == Color::White) ? PieceType::WhiteKnight : PieceType::BlackKnight;
    break;
  case Piece::Bishop:
    type = (color == Color::White) ? PieceType::WhiteBishop : PieceType::BlackBishop;
    break;
  case Piece::Queen:
    type = (color == Color::White) ? PieceType::WhiteQueen : PieceType::BlackQueen;
    break;
  case Piece::King:
    type = (color == Color::White) ? PieceType::WhiteKing : PieceType::BlackKing;
    break;
  default:
    break;
  }

  return type;
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

//  U64 symmBB = 0;
//  for (uint index = 0; index < 64; index++)
//    if (bb & (ONE << index))
//      symmBB |= (ONE << rotR90Matrix[index]);

//  return symmBB;
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
//  U64 symmBB = 0;
//  for (uint index = 0; index < 64; index++)
//    if (bb & (ONE << index))
//      symmBB += (ONE << rotateL45Matrix[index]);

//  return symmBB;

  U64 symmBB = 0;
  for (uint index = 0; index < 64; index++)
    if (bb & (ONE << rotateL45Matrix[index]))
      symmBB += (ONE << index);

  return symmBB;

//  U64 symmBB = 0;
//  for (uint index = 0; index < 64; index++)
//    if (bb & (ONE << index))
//      symmBB |= (ONE << rotMatrix[index]);

//  return symmBB;
}

BitBoard::U64 BitBoard::rotateR45(U64 bb) const
{
//  U64 symmBB = 0;
//  for (uint index = 0; index < 64; index++)
//    if (bb & (ONE << index))
//      symmBB += (ONE << rotR45Matrix[index]);

//  return symmBB;

  U64 symmBB = 0;
  for (uint index = 0; index < 64; index++)
    if (bb & (ONE << rotateR45Matrix[index]))
      symmBB += (ONE << index);

  return symmBB;

//  U64 symmBB = 0;
//  for (uint index = 0; index < 64; index++)
//    if (bb & (ONE << rotMatrix[index]))
//      symmBB |= (ONE << index);

//  return symmBB;
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
      ulonglong bb = ONE << sq;
      switch (type) {
      case PieceType::WhitePawn:
        mPieces[sq] = Piece::Pawn;
        mColors[sq] = Color::White;
        mWhitePawns |= bb;
        break;
      case PieceType::WhiteRook:
        mPieces[sq] = Piece::Rook;
        mColors[sq] = Color::White;
        mWhiteRooks |= bb;
        break;
      case PieceType::WhiteKnight:
        mPieces[sq] = Piece::Knight;
        mColors[sq] = Color::White;
        mWhiteKnights |= bb;
        break;
      case PieceType::WhiteBishop:
        mPieces[sq] = Piece::Bishop;
        mColors[sq] = Color::White;
        mWhiteBishops |= bb;
        break;
      case PieceType::WhiteQueen:
        mPieces[sq] = Piece::Queen;
        mColors[sq] = Color::White;
        mWhiteQueens |= bb;
        break;
      case PieceType::WhiteKing:
        mPieces[sq] = Piece::King;
        mColors[sq] = Color::White;
        mWhiteKings |= bb;
        setKingRow(Color::White, row);
        setKingColumn(Color::White, col);
        break;
      case PieceType::BlackPawn:
        mPieces[sq] = Piece::Pawn;
        mColors[sq] = Color::Black;
        mBlackPawns |= bb;
        break;
      case PieceType::BlackRook:
        mPieces[sq] = Piece::Rook;
        mColors[sq] = Color::Black;
        mBlackRooks |= bb;
        break;
      case PieceType::BlackKnight:
        mPieces[sq] = Piece::Knight;
        mColors[sq] = Color::Black;
        mBlackKnights |= bb;
        break;
      case PieceType::BlackBishop:
        mPieces[sq] = Piece::Bishop;
        mColors[sq] = Color::Black;
        mBlackBishops |= bb;
        break;
      case PieceType::BlackQueen:
        mPieces[sq] = Piece::Queen;
        mColors[sq] = Color::Black;
        mBlackQueens |= bb;
        break;
      case PieceType::BlackKing:
        mPieces[sq] = Piece::King;
        mColors[sq] = Color::Black;
        setKingRow(Color::Black, row);
        setKingColumn(Color::Black, col);
        mBlackKings |= bb;
        break;
      default:
        mPieces[sq] = Piece::None;
        mColors[sq] = Color::None;
        break;
      }
    }
  }

  // Initialize aggregate bitboards
  updateAggregateBitBoards();
}

void BitBoard::unmakeMove(const Move & move)
{
  // Get move information
  Color side = sideToMove();
  Piece piece = move.piece();
  uchar fromSquare = getIndex(move.sourceRow(), move.sourceCol());
  uchar toSquare = getIndex(move.destRow(), move.destCol());

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
  mPieces[fromSquare] = piece;
  mColors[fromSquare] = !side;
  mPieces[toSquare] = Piece::None;
  mColors[toSquare] = Color::None;

  // Update bit boards associated with the move
  U64 & pawns = (side == Color::Black) ? mWhitePawns : mBlackPawns;
  U64 & rooks = (side == Color::Black) ? mWhiteRooks : mBlackRooks;
  U64 & bishops = (side == Color::Black) ? mWhiteBishops : mBlackBishops;
  U64 & knights = (side == Color::Black) ? mWhiteKnights : mBlackKnights;
  U64 & queens = (side == Color::Black) ? mWhiteQueens : mBlackQueens;
  U64 & kings = (side == Color::Black) ? mWhiteKings : mBlackKings;
  switch (move.piece()) {
  case Piece::Pawn:
    pawns ^= bbFromTo;
    break;
  case Piece::Rook:
    rooks ^= bbFromTo;
    break;
  case Piece::Knight:
    knights ^= bbFromTo;
    break;
  case Piece::Bishop:
    bishops ^= bbFromTo;
    break;
  case Piece::Queen:
    queens ^= bbFromTo;
    break;
  case Piece::King:
    kings ^= bbFromTo;
    break;
  }

  // Handle captures
  // May need to update bitboards here
  if (move.isCapture()) {
    uchar sq = toSquare;
    if (move.isEpCapture()) {
      char dir = (side == Color::White) ? NORTH : SOUTH;
      sq = toSquare + dir;
    }

    mPieces[sq] = move.capturePiece();
    mColors[sq] = side;
  }

  // Handle promotions
  // May need to update bitboards here
  if (move.isPromotion() || move.isPromotionCapture()) {
    mPieces[fromSquare] = Piece::Pawn;
  }

  // Handle king moves
  if (piece == Piece::King) {
    setKingRow(!side, move.sourceRow());
    setKingColumn(!side, move.sourceCol());
  }

  // Handle castling move
  // May need to update bitboards here
  if (move.isCastle()) {
    switch (toSquare) {
    case C1:
      mPieces[A1] = Piece::Rook;
      mColors[A1] = Color::White;
      mPieces[D1] = Piece::None;
      mColors[D1] = Color::None;
      break;
    case G1:
      mPieces[H1] = Piece::Rook;
      mColors[H1] = Color::White;
      mPieces[F1] = Piece::None;
      mColors[F1] = Color::None;
      break;
    case C8:
      mPieces[A8] = Piece::Rook;
      mColors[A8] = Color::Black;
      mPieces[D8] = Piece::None;
      mColors[D8] = Color::None;
      break;
    case G8:
      mPieces[H8] = Piece::Rook;
      mColors[H8] = Color::Black;
      mPieces[F8] = Piece::None;
      mColors[F8] = Color::None;
      break;
    }
  }

  // Update the aggregate bitboards
  updateAggregateBitBoards();

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
  //mRotateLeft90Pieces = rotateL90(mAllPieces);
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
