#include "bitboard.h"
#include "fen.h"
#include "move.h"
#include "movelist.h"

#include <cassert>
#include <sstream>

// Macros for mapping (row,col)->index and vice-versa and valid squares
#define C64(constantU64) constantU64##ULL
#define getIndex(row,col) (((row)<<3)+(col))
#define getRow(index) ( (index >> 3) )
#define getCol(index) ( (index % 8) )

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

// Square definitions
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
static const uint rot90Matrix[] =
{
  A1, A2, A3, A4, A5, A6, A7, A8,
  B1, B2, B3, B4, B5, B6, B7, B8,
  C1, C2, C3, C4, C5, C6, C7, C8,
  D1, D2, D3, D4, D5, D6, D7, D8,
  E1, E2, E3, E4, E5, E6, E7, E8,
  F1, F2, F3, F4, F5, F6, F7, F8,
  G1, G2, G3, G4, G5, G6, G7, G8,
  H1, H2, H3, H4, H5, H6, H7, H8
};

static const uint a1h8Matrix[] =
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

static const uint a8h1Matrix[] =
{
  H8,
  G8, H7,
  F8, G7, H6,
  E8, F7, G6, H5,
  D8, E7, F6, G5, H4,
  C8, D7, E6, F5, G4, H3,
  B8, C7, D6, E5, F4, G3, H2,
  A8, B7, C6, D5, E4, F3, G2, H1,
  A7, B6, C5, D4, E3, F2, G1,
  A6, B5, C4, D3, E2, F1,
  A5, B4, C3, D2, E1,
  A4, B3, C2, D1,
  A3, B2, C1,
  A2, B1,
  A1
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
  initAttacks();
  initDiagAttacks();
  initStraightAttacks();
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

void BitBoard::generateAttackMoves(U64 bb, MoveList &moveList) const
{
  U64 bbFriends = bb;

  Color side = sideToMove();
  const U64 * pawnAttacks = (side == Color::White) ? mWhitePawnAttacks : mBlackPawnAttacks;
  U64 oppPieces = (side == Color::White) ? mBlackPieces : mWhitePieces;

  while (bb) {
    uchar fromSq = bitScanForward(bb); // square index from 0..63

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
      generateSliderAttacks(fromSq, bbFriends, false, true, moveList);
      break;
    case Piece::King:
      generateNonSliderAttacks(fromSq, mKingAttacks[fromSq], bbFriends, moveList);
      break;
    }
    bb &= bb-1;
  }
}

void BitBoard::generateMoves(MoveList & moveList) const
{
  generateCastlingMoves(moveList);

  U64 pieces = (sideToMove() == Color::White) ? mWhitePieces : mBlackPieces;
  if (sideToMove() == Color::White) {
    generateWhitePawnMoves(moveList);
  }
  else {
    generateBlackPawnMoves(moveList);
  }

  generateAttackMoves(pieces, moveList);
}

void BitBoard::generateBlackPawnMoves(MoveList &moveList) const
{
  U64 bbMove = 0;
  U64 promoMask = C64(255);

  // Promotion moves
  bbMove = (mBlackPawns << 8) & promoMask & mEmptySquares;
  while (bbMove) {
    int toSq = bitScanForward(bbMove); // square index from 0..63
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
    int toSq = bitScanForward(bbMove); // square index from 0..63
    int fromSq = toSq + NORTH;
    pushMove(fromSq, toSq, mPieces[fromSq], Piece::None, Piece::None, Move::Type::Quiet, moveList);

    bbMove &= bbMove - 1;
  }

  U64 unmovedPawns = (mBlackPawns & (C64(255) << 48));
  U64 pawnDoublePushes = ( (unmovedPawns & (~(mAllPieces << 8)) & (~(mAllPieces << 16)) ) ) >> 16;
  bbMove = pawnDoublePushes;
  while (bbMove) {
    int toSq = bitScanForward(bbMove); // square index from 0..63
    int fromSq = toSq + (NORTH << 1);
    pushMove(fromSq, toSq, mPieces[fromSq], Piece::None, Piece::None, Move::Type::EpPush, moveList);

    bbMove &= bbMove - 1;
  }
}

void BitBoard::generateNonSliderAttacks(uchar fromSq, U64 bbAttack, U64 bbFriendly, MoveList & moveList) const
{
  U64 bbMove = bbAttack & ~(bbFriendly);
  while (bbMove) {
    uchar toSq = bitScanForward(bbMove); // square index from 0..63
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
  U64 bbMove = 0;

  if (straight) {
    uchar row = getRow(fromSq);
    uchar col = getCol(fromSq);

    // Generate rank attacks
    occupancy = (mAllPieces >> (row*8)) & 255;
    bbMove = mRankAttacks[fromSq][occupancy] & ~(bbFriends);
    pushBitBoardMoves(bbMove, fromSq, moveList);

    // Generate file attacks
    occupancy = (mRotate90AllPieces >> (col*8)) & 255;
    bbMove = mFileAttacks[fromSq][occupancy] & ~(rotate90(bbFriends));
    pushBitBoardMoves(bbMove, fromSq, moveList);
  }

  if (diag) {
    // Generate north east occupancy
    occupancy = (mRotate45RightPieces >> mA1H8Shifts[fromSq]) & mA1H8Mask[fromSq];
    bbMove = mDiagAttacksNorthEast[fromSq][occupancy] & ~(rotate45(bbFriends, a1h8Matrix));
    pushBitBoardMoves(bbMove, fromSq, moveList);

    // Generate north west attacks
    occupancy = (mRotate45LeftPieces >> mA8H1Shifts[fromSq]) & mA8H1Mask[fromSq];
    bbMove = mDiagAttacksNorthWest[fromSq][occupancy] & ~(rotateNeg45(bbFriends, a8h1Matrix));
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
    int toSq = bitScanForward(bbMove); // square index from 0..63
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

void BitBoard::initAttacks()
{
  static const int knightRowIncr[]    = {1, 2, 2, 1, -1, -2, -2, -1};
  static const int knightColIncr[]    = {-2, -1, 1, 2, 2, 1, -1, -2};
  static const int kingRowIncr[]      = {1, 1, 1, 0, -1, -1, -1, 0};
  static const int kingColIncr[]      = {-1, 0, 1, 1, 1, 0, -1, -1};
  static const int whitePawnRowIncr[] = {1, 1};
  static const int whitePawnColIncr[] = {-1, 1};
  static const int blackPawnRowIncr[] = {-1, -1};
  static const int blackPawnColIncr[] = {-1, 1};
  //static const int straightRowIncr[]  = {0, 0, 1, -1};
  //static const int straightColIncr[]  = {1, -1, 0, 0};
  //static const int diagRowIncr[]      = {1, 1, -1, -1};
  //static const int diagColIncr[]      = {1, -1, -1, 1};

  for (int i = 0; i < 64; i++) {
    //mBishopAttacks[i] = 0ULL;
    //mRookAttacks[i] = 0ULL;
    mKingAttacks[i] = 0ULL;
    mKnightAttacks[i] = 0ULL;
    mWhitePawnAttacks[i] = 0ULL;
    mBlackPawnAttacks[i] = 0ULL;
    //mNorthAttacks[i] = 0ULL;
    //mSouthAttacks[i] = 0ULL;
    //mEastAttacks[i] = 0ULL;
    //mWestAttacks[i] = 0ULL;
    //mNorthEastAttacks[i] = 0ULL;
    //mNorthWestAttacks[i] = 0ULL;
    //mSouthEastAttacks[i] = 0ULL;
    //mSouthWestAttacks[i] = 0ULL;
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
          mWhitePawnAttacks[index] |= (C64(1) << destIndex);
        }
      }

      // Black pawn attacks
      for (int k = 0; k < 2; k++) {
        char destRow = sourceRow + blackPawnRowIncr[k];
        char destCol = sourceCol + blackPawnColIncr[k];
        if (destRow >= 0 && destRow < 8 && destCol >= 0 && destCol < 8) {
          uchar destIndex = getIndex(destRow, destCol);
          mBlackPawnAttacks[index] |= (C64(1) << destIndex);
        }
      }

      // Knight attacks
      for (char k = 0; k < 8; k++) {
        char destRow = sourceRow + knightRowIncr[k];
        char destCol = sourceCol + knightColIncr[k];
        if (destRow >= 0 && destRow < 8 && destCol >= 0 && destCol < 8) {
          uchar destIndex = getIndex(destRow, destCol);
          mKnightAttacks[index] |= (C64(1) << destIndex);
        }
      }

      // King attacks
      for (char k = 0; k < 8; k++) {
        char destRow = sourceRow + kingRowIncr[k];
        char destCol = sourceCol + kingColIncr[k];
        if (destRow >= 0 && destRow < 8 && destCol >= 0 && destCol < 8) {
          uchar destIndex = getIndex(destRow, destCol);
          mKingAttacks[index] |= (C64(1) << destIndex);
        }
      }
    }
  }
  // Slider attacks
//      for (int k = 0; k < 4; k++) {
//        for (char i = 0; i < 8; i++) {
//          char destRow = sourceRow + straightRowIncr[k]*(i+1);
//          char destCol = sourceCol + straightColIncr[k]*(i+1);
//          if (destRow >= 0 && destRow < 8 && destCol >= 0 && destCol < 8) {
//            uchar destIndex = getIndex(destRow, destCol);
//            mRookAttacks[index] |= (C64(1) << destIndex);
//            if (k == 0) {
//              mEastAttacks[index] |= (C64(1) << destIndex);
//            }
//            else if (k == 1) {
//              mWestAttacks[index] |= (C64(1) << destIndex);
//            }
//            else if (k == 2) {
//              mNorthAttacks[index] |= (C64(1) << destIndex);
//            }
//            else if (k == 3) {
//              mSouthAttacks[index] |= (C64(1) << destIndex);
//            }
//          }
//        }

//        for (char i = 0; i < 8; i++) {
//          char destRow = sourceRow + diagRowIncr[k]*(i+1);
//          char destCol = sourceCol + diagColIncr[k]*(i+1);
//          if (destRow >= 0 && destRow < 8 && destCol >= 0 && destCol < 8) {
//            uchar destIndex = getIndex(destRow, destCol);
//            mBishopAttacks[index] |= (C64(1) << destIndex);
//            if (k == 0) {
//              mNorthEastAttacks[index] |= (C64(1) << destIndex);
//            }
//            else if (k == 1) {
//              mNorthWestAttacks[index] |= (C64(1) << destIndex);
//            }
//            else if (k == 2) {
//              mSouthWestAttacks[index] |= (C64(1) << destIndex);
//            }
//            else if (k == 3) {
//              mSouthEastAttacks[index] |= (C64(1) << destIndex);
//            }
//          }
        //}
    //}

  //writeBitBoard(mNorthAttacks[15], std::cout);
  //U64 bb = (C64(255) << 48);
  //U64 unmovedPawns = (mBlackPawns & (C64(255) << 48));
  //writeBitBoard(unmovedPawns, std::cout);
  //writeBitBoard(C64(255), std::cout);

  //MoveList m;
  //generateBlackPawnMoves(m);
  //generateWhitePawnMoves(m);
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

  // Initialize aggregate bitboards
  mWhitePieces = mWhiteBishops | mWhiteKings | mWhiteKnights | mWhitePawns | mWhiteQueens | mWhiteRooks;
  mBlackPieces = mBlackBishops | mBlackKings | mBlackKnights | mBlackPawns | mBlackQueens | mBlackRooks;
  mAllPieces = mWhitePieces | mBlackPieces;
  mEmptySquares = ~mAllPieces;
  mRotate90AllPieces = rotate90(mAllPieces);
  mRotate45RightPieces = rotate45(mAllPieces, a1h8Matrix);
  mRotate45LeftPieces = rotate45(mAllPieces, a8h1Matrix);
  for (uchar index = 0; index < 64; index++) {
    //mRotate45Right[index] = rotate45((C64(1)<<index), a1h8Matrix);
    //mRotate45Left[index] = rotate45((C64(1)<<index), a8h1Matrix);
    //mRotate90[index] = rotate90((C64(1)<<index));
  }
}

void BitBoard::initDiagAttacks()
{
  // Initialize the diagonal shifts
  static const uint numElements[] = {1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 5, 4, 3, 2, 1};
  static const uint shift[] = {0, 1, 3, 6, 10, 15, 21, 28, 36, 43, 49, 54, 58, 61, 63};

  uint index = 0;
  for (uint i = 0; i < 15; i++) {
    for (uint j = 0; j < numElements[i]; j++) {
      mA1H8Shifts[a1h8Matrix[index]] = shift[i];
      mA8H1Shifts[a8h1Matrix[index]] = shift[i];

      uint mask = 0, k = 0;
      while (k < numElements[i]) {
        mask |= (C64(1) << k);
        k++;
      }

      mA1H8Mask[a1h8Matrix[index]] = mask;
      mA8H1Mask[a8h1Matrix[index]] = mask;
      index++;
    }
  }

  for (uint index = 0; index < 64; index++) {
    for (uint occ = 0; occ < 256; occ++) {
      mDiagAttacksNorthEast[index][occ] = 0;
      mDiagAttacksNorthWest[index][occ] = 0;
    }
  }

  for (int index = 0; index < 64; index++) {
    int row = index >> 3;
    int col = index & 7;

    int rankCol = (col > row) ? row : col;
    for (uint occupancy = 0; occupancy <= mA1H8Mask[index]; occupancy++) {
      uint maskedOccupancy = mRankAttacks[rankCol][occupancy] & mA1H8Mask[index];

      ulonglong bb = C64(0);
      for (int i = rankCol+1; i < 8; i++) {
        if (maskedOccupancy & (C64(1) << i))
          bb |= (C64(1) << i);
      }

      for (int i = rankCol-1; i >= 0; i--) {
        if (maskedOccupancy & (C64(1) << i))
          bb |= (C64(1) << i);
      }

      mDiagAttacksNorthEast[index][occupancy] = (rotateNeg45(bb << mA1H8Shifts[index], a1h8Matrix));
    }

    rankCol = (col > (7-row)) ? (7-row) : col;
    for (uint occupancy = 0; occupancy <= mA8H1Mask[index]; occupancy++) {
      uint maskedOccupancy = mRankAttacks[rankCol][occupancy] & mA8H1Mask[index];

      ulonglong bb = C64(0);
      for (int i = rankCol+1; i < 8; i++) {
        if (maskedOccupancy & (C64(1) << i))
          bb |= (C64(1) << i);
      }

      for (int i = rankCol-1; i >= 0; i--) {
        if (maskedOccupancy & (C64(1) << i))
          bb |= (C64(1) << i);
      }

      mDiagAttacksNorthWest[index][occupancy] = (rotateNeg45(bb << mA8H1Shifts[index], a8h1Matrix));
    }
  }
}

void BitBoard::initStraightAttacks()
{
  for (int col = 0; col < 8; col++) {
    for (int occupancy = 0; occupancy < 256; occupancy++) {
      U64 bb = 0;

      for (int file = col-1; file >= 0; file--) {
        bb |= (C64(1) << file);
        if (occupancy & (C64(1) << file))
          break;
      }

      for (int file = col + 1; file < 8; file++) {
        bb |= (C64(1) << file);
        if (occupancy & (C64(1) << file))
          break;
      }

      mRankAttacks[col][occupancy] = bb;
      mFileAttacks[col][occupancy] = rotate90(bb);
    }
  }
  //MoveList m;
  //writeBitBoard(mRankAttacks[0][0], std::cout);
  //generateSliderAttacks(0, mWhitePieces, true, false, m);
  //for (uchar i = 0; i < m.size(); i++) {
  //  std::cout << m[i].toSmithNotation() << "\n";
 // }
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
  Color side = sideToMove();
  uchar fromSquare = getIndex(move.sourceRow(), move.sourceCol());
  uchar toSquare = getIndex(move.destRow(), move.destCol());
  U64 bbFrom = (C64(1) << fromSquare);
  U64 bbTo = (C64(1) << toSquare);
  U64 bbFromTo = bbFrom ^ bbTo;

  // Update bit boards associated with the move
  switch (move.piece()) {
  case Piece::Pawn:
    if (side == Color::White)
      mWhitePawns ^= bbFromTo;
    else
      mBlackPawns ^= bbFromTo;
    break;
  case Piece::Rook:
    if (side == Color::White)
      mWhiteRooks ^= bbFromTo;
    else
      mBlackRooks ^= bbFromTo;
    break;
  case Piece::Knight:
    if (side == Color::White)
      mWhiteKnights ^= bbFromTo;
    else
      mBlackKnights ^= bbFromTo;
    break;
  case Piece::Bishop:
    if (side == Color::White)
      mWhiteBishops ^= bbFromTo;
    else
      mBlackBishops ^= bbFromTo;
    break;
  case Piece::Queen:
    if (side == Color::White)
      mWhiteQueens ^= bbFromTo;
    else
      mBlackKings ^= bbFromTo;
    break;
  case Piece::King:
    if (side == Color::White)
      mWhiteKings ^= bbFromTo;
    else
      mBlackKings ^= bbFromTo;
    break;
  }

  // Update aggregate bitboards
  mWhitePieces = mWhiteBishops | mWhiteKings | mWhiteKnights | mWhitePawns | mWhiteQueens | mWhiteRooks;
  mBlackPieces = mBlackBishops | mBlackKings | mBlackKnights | mBlackPawns | mBlackQueens | mBlackRooks;
  mAllPieces = mWhitePieces | mBlackPieces;
  mEmptySquares = ~mAllPieces;
  mRotate90AllPieces = rotate90(mAllPieces);
  mRotate45RightPieces = rotate45(mAllPieces, a1h8Matrix);
  mRotate45LeftPieces = rotate45(mAllPieces, a8h1Matrix);

  // Update the color and piece arrays
  mColors[fromSquare] = Color::None;
  mPieces[fromSquare] = Piece::None;
  mColors[toSquare] = side;
  mPieces[toSquare] = move.piece();

  // Handle enPassant captures
  if (move.isEpCapture()) {
    char dir = (side == Color::White) ? SOUTH : NORTH;
    uchar sq = toSquare + dir;
    mPieces[sq] = Piece::None;
    mColors[sq] = Color::None;
  }

  // Handle promotions
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

  setCastlingRights(castling);

  // Update board state
  incrementHalfMoveClock();
  if (move.piece() == Piece::Pawn || move.isCapture())
    setHalfMoveClock(0);

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
    if (capturePiece != Piece::None)
      break;

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

BitBoard::U64 BitBoard::rotate90(U64 bb) const
{
  U64 symmBB = 0;
  for (uint index = 0; index < 64; index++)
    if (bb & (C64(1) << index))
      symmBB |= (C64(1) << rot90Matrix[index]);

  return symmBB;
}

BitBoard::U64 BitBoard::rotateNeg45(U64 bb, const uint * rotMatrix) const
{
  U64 symmBB = 0;
  for (uint index = 0; index < 64; index++)
    if (bb & (C64(1) << index))
      symmBB |= (C64(1) << rotMatrix[index]);

  return symmBB;
}

BitBoard::U64 BitBoard::rotate45(U64 bb, const uint * rotMatrix) const
{
  U64 symmBB = 0;
  for (uint index = 0; index < 64; index++)
    if (bb & (C64(1) << rotMatrix[index]))
      symmBB |= (C64(1) << index);

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

  for (uchar row = 0; row < 8; row++) {
    for (uchar col = 0; col < 8; col++) {
      PieceType type = fenData.pieceType(row, col);
      uchar sq = getIndex(row, col);
      ulonglong bb = C64(1) << sq;
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
  mWhitePieces = mWhiteBishops | mWhiteKings | mWhiteKnights | mWhitePawns | mWhiteQueens | mWhiteRooks;
  mBlackPieces = mBlackBishops | mBlackKings | mBlackKnights | mBlackPawns | mBlackQueens | mBlackRooks;
  mAllPieces = mWhitePieces | mBlackPieces;
  mEmptySquares = ~mAllPieces;
  mRotate90AllPieces = rotate90(mAllPieces);
  mRotate45RightPieces = rotate45(mAllPieces, a1h8Matrix);
  mRotate45LeftPieces = rotate45(mAllPieces, a8h1Matrix);
  for (uchar index = 0; index < 64; index++) {
    //mRotate45Right[index] = rotate45((C64(1)<<index), a1h8Matrix);
    //mRotate45Left[index] = rotate45((C64(1)<<index), a8h1Matrix);
    //mRotate90[index] = rotate90((C64(1)<<index));
  }
}

void BitBoard::unmakeMove(const Move & move)
{
  Color side = sideToMove();
  Piece piece = move.piece();
  uchar fromSquare = getIndex(move.sourceRow(), move.sourceCol());
  uchar toSquare = getIndex(move.destRow(), move.destCol());
  U64 bbFrom = (C64(1) << fromSquare);
  U64 bbTo = (C64(1) << toSquare);
  U64 bbFromTo = bbFrom ^ bbTo;

  // Reset the board state from the move
  setFullMoveCounter(move.fullMoveCounter());
  setHalfMoveClock(move.halfMoveClock());
  setCastlingRights(move.castlingRights());
  setEpColumn(move.enPassantCol());

  mPieces[fromSquare] = piece;
  mColors[fromSquare] = !side;
  mPieces[toSquare] = Piece::None;
  mColors[toSquare] = Color::None;

  // Update bit boards associated with the move
  switch (move.piece()) {
  case Piece::Pawn:
    if (side == Color::Black)
      mWhitePawns ^= bbFromTo;
    else
      mBlackPawns ^= bbFromTo;
    break;
  case Piece::Rook:
    if (side == Color::Black)
      mWhiteRooks ^= bbFromTo;
    else
      mBlackRooks ^= bbFromTo;
    break;
  case Piece::Knight:
    if (side == Color::Black)
      mWhiteKnights ^= bbFromTo;
    else
      mBlackKnights ^= bbFromTo;
    break;
  case Piece::Bishop:
    if (side == Color::Black)
      mWhiteBishops ^= bbFromTo;
    else
      mBlackBishops ^= bbFromTo;
    break;
  case Piece::Queen:
    if (side == Color::Black)
      mWhiteQueens ^= bbFromTo;
    else
      mBlackKings ^= bbFromTo;
    break;
  case Piece::King:
    if (side == Color::Black)
      mWhiteKings ^= bbFromTo;
    else
      mBlackKings ^= bbFromTo;
    break;
  }

  // Update aggregate bitboards
  mWhitePieces = mWhiteBishops | mWhiteKings | mWhiteKnights | mWhitePawns | mWhiteQueens | mWhiteRooks;
  mBlackPieces = mBlackBishops | mBlackKings | mBlackKnights | mBlackPawns | mBlackQueens | mBlackRooks;
  mAllPieces = mWhitePieces | mBlackPieces;
  mEmptySquares = ~mAllPieces;
  mRotate90AllPieces = rotate90(mAllPieces);
  mRotate45RightPieces = rotate45(mAllPieces, a1h8Matrix);
  mRotate45LeftPieces = rotate45(mAllPieces, a8h1Matrix);

  // Handle captures
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
  if (move.isPromotion() || move.isPromotionCapture()) {
    mPieces[fromSquare] = Piece::Pawn;
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

  //writeBitBoard(mWhitePieces, std::cout);
  //writeBitBoard(mBlackPieces, std::cout);

  toggleSideToMove();
}

void BitBoard::writeBitBoard(U64 bitboard, std::ostream & output) const
{
  for (int i = 7; i >= 0; i--) {
    for (int j = 0; j < 8; j++) {
      int shift = 8*i + j ;
      if ( (bitboard >> shift) & 0x1)
        std::cout << 'X';
      else
        std::cout << '-';
    }
    std::cout << "\n";
  }
  std::cout << "\n\n";
}
