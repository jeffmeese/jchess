#include "bitboard.h"
#include "fen.h"

#include <cassert>
#include <sstream>

// Helper macros for bitboards
#define c64(constantU64) constantU64##ULL
#define ZERO c64(0)
#define ONE c64(1)
#define RANK8 c64(56)

// Macros for mapping (row,col)->index and vice-versa
#define getIndex(row,col) (((row)<<3)+(col))
#define getRow(index) ( (index >> 3) )
#define getCol(index) ( (index & 7) )

// Mask for full rank
static const int FullRank = 255;

// Promotion masks
static const unsigned long long WHITE_PROMOTION_MASK = (c64(255));
static const unsigned long long BLACK_PROMOTION_MASK = (c64(255) << 56);

// Pawn offsets
static const char NORTH          = -8;
static const char SOUTH          = +8;
static const char WHITE_ROW_INCR = -1;
static const char BLACK_ROW_INCR = +1;

enum Square
{
  A8, B8, C8, D8, E8, F8, G8, H8,
  A7, B7, C7, D7, E7, F7, G7, H7,
  A6, B6, C6, D6, E6, F6, G6, H6,
  A5, B5, C5, D5, E5, F5, G5, H5,
  A4, B4, C4, D4, E4, F4, G4, H4,
  A3, B3, C3, D3, E3, F3, G3, H3,
  A2, B2, C2, D2, E2, F2, G2, H2,
  A1, B1, C1, D1, E1, F1, G1, H1
};

// Shifts for ranks
static const uint rankShifts[64] =
{
  0, 0, 0, 0, 0, 0, 0, 0,
  8, 8, 8, 8, 8, 8, 8, 8,
  16, 16, 16, 16, 16, 16, 16, 16,
  24, 24, 24, 24, 24, 24, 24, 24,
  32, 32, 32, 32, 32, 32, 32, 32,
  40, 40, 40, 40, 40, 40, 40, 40,
  48, 48, 48, 48, 48, 48, 48, 48,
  56, 56, 56, 56, 56, 56, 56, 56
};

// Masks for ranks
static const uint rankMasks[64] =
{
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255
};

static const uint rotateL90Matrix[64] = {
  H8,H7,H6,H5,H4,H3,H2,H1,
  G8,G7,G6,G5,G4,G3,G2,G1,
  F8,F7,F6,F5,F4,F3,F2,F1,
  E8,E7,E6,E5,E4,E3,E2,E1,
  D8,D7,D6,D5,D4,D3,D2,D1,
  C8,C7,C6,C5,C4,C3,C2,C1,
  B8,B7,B6,B5,B4,B3,B2,B1,
  A8,A7,A6,A5,A4,A3,A2,A1,
};

static const uint rotateR90Matrix[64] = {
  A1,A2,A3,A4,A5,A6,A7,A8,
  B1,B2,B3,B4,B5,B6,B7,B8,
  C1,C2,C3,C4,C5,C6,C7,C8,
  D1,D2,D3,D4,D5,D6,D7,D8,
  E1,E2,E3,E4,E5,E6,E7,E8,
  F1,F2,F3,F4,F5,F6,F7,F8,
  G1,G2,G3,G4,G5,G6,G7,G8,
  H1,H2,H3,H4,H5,H6,H7,H8
};

// Shifts for files
static const uint fileShifts[64] =
{
  56, 48, 40, 32, 24, 16, 8, 0,
  56, 48, 40, 32, 24, 16, 8, 0,
  56, 48, 40, 32, 24, 16, 8, 0,
  56, 48, 40, 32, 24, 16, 8, 0,
  56, 48, 40, 32, 24, 16, 8, 0,
  56, 48, 40, 32, 24, 16, 8, 0,
  56, 48, 40, 32, 24, 16, 8, 0,
  56, 48, 40, 32, 24, 16, 8, 0,
};

// Masks for files
static const uint fileMasks[64] =
{
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255
};

static const uint rotateR45Matrix[64] = {
A8,
A7,B8,
A6,B7,C8,
A5,B6,C7,D8,
A4,B5,C6,D7,E8,
A3,B4,C5,D6,E7,F8,
A2,B3,C4,D5,E6,F7,G8,
A1,B2,C3,D4,E5,F6,G7,H8,
B1,C2,D3,E4,F5,G6,H7,
C1,D2,E3,F4,G5,H6,
D1,E2,F3,G4,H5,
E1,F2,G3,H4,
F1,G2,H3,
G1,H2,
H1
};

static const uint rotateL45Matrix[64] = {
H8,
G8,H7,
F8,G7,H6,
E8,F7,G6,H5,
D8,E7,F6,G5,H4,
C8,D7,E6,F5,G4,H3,
B8,C7,D6,E5,F4,G3,H2,
A8,B7,C6,D5,E4,F3,G2,H1,
A7,B6,C5,D4,E3,F2,G1,
A6,B5,C4,D3,E2,F1,
A5,B4,C3,D2,E1,
A4,B3,C2,D1,
A3,B2,C1,
A2,B1,
A1
};

/* These simply store the length of the diagonal in the required sense */
static const uint a1h8Lengths[64] = {
  1,2,3,4,5,6,7,8,
  2,3,4,5,6,7,8,7,
  3,4,5,6,7,8,7,6,
  4,5,6,7,8,7,6,5,
  5,6,7,8,7,6,5,4,
  6,7,8,7,6,5,4,3,
  7,8,7,6,5,4,3,2,
  8,7,6,5,4,3,2,1
};

static const uint a8h1Lengths[64] = {
  8,7,6,5,4,3,2,1,
  7,8,7,6,5,4,3,2,
  6,7,8,7,6,5,4,3,
  5,6,7,8,7,6,5,4,
  4,5,6,7,8,7,6,5,
  3,4,5,6,7,8,7,6,
  2,3,4,5,6,7,8,7,
  1,2,3,4,5,6,7,8
};

static const uint a1h8Shifts[64] = {
   0, 1, 3, 6,10,15,21,28,
   1, 3, 6,10,15,21,28,36,
   3, 6,10,15,21,28,36,43,
   6,10,15,21,28,36,43,49,
  10,15,21,28,36,43,49,54,
  15,21,28,36,43,49,54,58,
  21,28,36,43,49,54,58,61,
  28,36,43,49,54,58,61,63
};

static const uint a8h1Shifts[64] = {
  28,21,15,10, 6, 3, 1, 0,
  36,28,21,15,10, 6, 3, 1,
  43,36,28,21,15,10, 6, 3,
  49,43,36,28,21,15,10, 6,
  54,49,43,36,28,21,15,10,
  58,54,49,43,36,28,21,15,
  61,58,54,49,43,36,28,21,
  63,61,58,54,49,43,36,28
};

//! \class BitBoard
BitBoard::BitBoard()
{
  init();
  initBoard();
  initAttacks();
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
inline uchar BitBoard::bitScanForward(u64 bb) const
{
  static const uchar index64[64] =
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

  //ulong index = 0;
  //_BitScanForward64(&index, bb);
  //return static_cast<uchar>(index);
   static const ulonglong debruijn64 = ulonglong(0x07EDD5E59A4E28C2);
   assert (bb != 0);
   return index64[((bb & -bb) * debruijn64) >> 58];
}

void BitBoard::generateBlackPawnMoves(MoveList &moveList) const
{
  u64 bbMove = 0;

  // Promotion moves
  bbMove = (mPiece[BlackPawn] << 8) & BLACK_PROMOTION_MASK & mPiece[None];
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
  u64 pawnPushes = (mPiece[BlackPawn] << 8) & mPiece[None];
  bbMove = pawnPushes & ~BLACK_PROMOTION_MASK;
  while (bbMove) {
    int toSq = bitScanForward(bbMove);
    int fromSq = toSq + NORTH;
    pushMove(fromSq, toSq, Type::BlackPawn, Type::None, Type::None, Move::Type::Quiet, moveList);
    bbMove &= bbMove - 1;
  }

  // Double pawn pushes
  u64 unmovedPawns = (mPiece[BlackPawn] & (c64(255) << 8));
  u64 pawnDoublePushes = ( (unmovedPawns & (~(mPiece[All] >> 8)) & (~(mPiece[All] >> 16)) ) ) << 16;
  bbMove = pawnDoublePushes;
  //writeBitBoard(unmovedPawns);
  //writeBitBoard(mPiece[All]);
  //writeBitBoard(~mPiece[All]);
  //writeBitBoard(~(mPiece[All] << 8));
  //writeBitBoard(~(mPiece[All] << 16));
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

void BitBoard::generateJumpAttacks(u64 piece, const u64 * attacks, u64 friends, MoveList & moveList) const
{
  while (piece) {
    uchar fromSq = bitScanForward(piece);
    u64 attack = attacks[fromSq];
    u64 move = attack & ~friends;
    while (move) {
      uchar toSq = bitScanForward(move);
      Move::Type type = (mType[toSq] == Type::None) ? Move::Type::Quiet : Move::Type::Capture;
      pushMove(fromSq, toSq, mType[fromSq], mType[toSq], Type::None, type, moveList);
      move &= move-1;
    }
    piece &= piece-1;
  }
}

void BitBoard::generateMoves(MoveList & moveList) const
{
  //writeBitBoard(mPiece[WhitePawn], std::cout);

  // Get the bitboards for the pieces on the side to move
  u64 pawns = mPiece[WhitePawn + mSide];
  u64 knights = mPiece[WhiteKnight + mSide];
  u64 kings = mPiece[WhiteKing + mSide];
  u64 rooks = mPiece[WhiteRook + mSide];
  u64 bishops = mPiece[WhiteBishop + mSide];
  u64 queens = mPiece[WhiteQueen + mSide];
  u64 friends = mPiece[mSide];
  u64 enemies = ~friends & mPiece[All];

  // Generate castling moves
  generateCastlingMoves(moveList);

  // Generate pawn moves
  if (mSide == White) {
    generatePawnAttacks(pawns, mPawnAttacks[White], enemies, moveList);
    generateWhitePawnMoves(moveList);
  }
  else {
    generatePawnAttacks(pawns, mPawnAttacks[Black], enemies, moveList);
    generateBlackPawnMoves(moveList);
  }

  generateJumpAttacks(knights, mKnightAttacks, friends, moveList);
  generateJumpAttacks(kings, mKingAttacks, friends, moveList);
  generateSlideAttacks(rooks, mRankAttacks, friends, mPiece[All], rankShifts, rankMasks, moveList);
  generateSlideAttacks(rooks, mFileAttacks, friends, mRotateR90, fileShifts, fileMasks, moveList);
  //generateSlideAttacks(bishops, mNorthEastAttacks, friends, mRotateRight45Pieces, a1h8Shifts, a1h8Masks, moveList);
  //generateSlideAttacks(bishops, mNorthWestAttacks, friends, mRotateLeft45Pieces, a8h1Shifts, a8h1Masks, moveList);
  generateSlideAttacks(queens, mRankAttacks, friends, mPiece[All], rankShifts, rankMasks, moveList);
  generateSlideAttacks(queens, mFileAttacks, friends, mRotateR90, fileShifts, fileMasks, moveList);
  //generateSlideAttacks(queens, mNorthEastAttacks, friends, mRotateRight45Pieces, a1h8Shifts, a1h8Masks, moveList);
  //generateSlideAttacks(queens, mNorthWestAttacks, friends, mRotateLeft45Pieces, a8h1Shifts, a8h1Masks, moveList);
}

void BitBoard::generatePawnAttacks(u64 piece, const u64 *attacks, u64 enemies, MoveList &moveList) const
{
  while (piece) {
    uchar fromSq = bitScanForward(piece);
    u64 attack = attacks[fromSq];
    u64 move = attack & enemies;
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
      u64 bbMove = attack;
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

void BitBoard::generateSlideAttacks(u64 piece, const u64 attacks[][256], u64 friends, u64 pieces, const uint * shifts, const uint * masks, MoveList & moveList) const
{
  while (piece) {
    uchar fromSq = bitScanForward(piece);
    uint shift = shifts[fromSq];
    uint occupancy = (pieces >> shift) & masks[fromSq];
    u64 move = attacks[fromSq][occupancy] & ~friends;
    pushBitBoardMoves(move, fromSq, moveList);
    piece &= piece-1;
  }
}

void BitBoard::generateWhitePawnMoves(MoveList &moveList) const
{
  u64 bbMove = 0;

  // Promotion moves
  bbMove = (mPiece[WhitePawn] >> 8) & WHITE_PROMOTION_MASK & mPiece[None];
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
  u64 pawnPushes = (mPiece[WhitePawn] >> 8) & mPiece[None];
  bbMove = pawnPushes & ~WHITE_PROMOTION_MASK;
  while (bbMove) {
    int toSq = bitScanForward(bbMove);
    int fromSq = toSq + SOUTH;
    pushMove(fromSq, toSq, Type::WhitePawn, Type::None, Type::None, Move::Type::Quiet, moveList);
    bbMove &= bbMove - 1;
  }

  // Pawn double pushes
  u64 unmovedPawns = (mPiece[WhitePawn] & (c64(255) << 48));
  u64 pawnDoublePushes = ( (unmovedPawns & (~(mPiece[All] << 8)) & (~(mPiece[All] << 16)) ) ) >> 16;
  bbMove = pawnDoublePushes;
  while (bbMove) {
    int toSq = bitScanForward(bbMove);
    int fromSq = toSq + (SOUTH << 1);
    pushMove(fromSq, toSq, Type::WhitePawn, Type::None, Type::None, Move::Type::EpPush, moveList);
    bbMove &= bbMove - 1;
  }
}

void BitBoard::init()
{
  // Initialize the type to piece array
  // This allows quick translation of our internal piece types in pushMove
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

  // Masks used to shift a zero or one bit into a bitboard
  // Useful during make/unmake move
  for (uchar i = 0; i < 64; i++) {
    mMask[i] = (ONE << i);
    mInvMask[i] = ~mMask[i];
  }
}

void BitBoard::initAttacks()
{
  initPawnAttacks(White, WHITE_ROW_INCR);
  initPawnAttacks(Black, BLACK_ROW_INCR);
  initKingAttacks();
  initKnightAttacks();
  initBishopAttacks();
  initRookAttacks();
}

void BitBoard::initBishopAttacks()
{

}

void BitBoard::initBoard()
{
  // Initialize members
  mSide = White;
  mUndoPos = 0;

  // Initialize type array
  mType[A7] = mType[B7] = mType[C7] = mType[D7] = mType[E7] = mType[F7] = mType[G7] = mType[H7] = BlackPawn;
  mType[A8] = mType[H8] = BlackRook;
  mType[B8] = mType[G8] = BlackKnight;
  mType[C8] = mType[F8] = BlackBishop;
  mType[D8] = BlackQueen;
  mType[E8] = BlackKing;

  mType[A2] = mType[B2] = mType[C2] = mType[D2] = mType[E2] = mType[F2] = mType[G2] = mType[H2] = WhitePawn;
  mType[A1] = mType[H1] = WhiteRook;
  mType[B1] = mType[G1] = WhiteKnight;
  mType[C1] = mType[F1] = WhiteBishop;
  mType[D1] = WhiteQueen;
  mType[E1] = WhiteKing;

  // Intialize all bitboards
  for (uchar i = 0; i < All; i++) {
    mPiece[0] = ZERO;
  }

  // Initialize white pieces
  for (uchar i = A2; i <= H2; i++) {
    mPiece[WhitePawn] |= (ONE << i);
  }
  mPiece[WhiteRook] |= (ONE << A1);
  mPiece[WhiteRook] |= (ONE << H1);
  mPiece[WhiteKnight] |= (ONE << B1);
  mPiece[WhiteKnight] |= (ONE << G1);
  mPiece[WhiteBishop] |= (ONE << C1);
  mPiece[WhiteBishop] |= (ONE << F1);
  mPiece[WhiteQueen] |= (ONE << D1);
  mPiece[WhiteKing] |= (ONE << E1);

  // Intialize black pieces
  for (uchar i = A7; i <= H7; i++) {
    mPiece[BlackPawn] |= (ONE << i);
  }
  mPiece[BlackRook] |= (ONE << A8);
  mPiece[BlackRook] |= (ONE << H8);
  mPiece[BlackKnight] |= (ONE << B8);
  mPiece[BlackKnight] |= (ONE << G8);
  mPiece[BlackBishop] |= (ONE << C8);
  mPiece[BlackBishop] |= (ONE << F8);
  mPiece[BlackQueen] |= (ONE << D8);
  mPiece[BlackKing] |= (ONE << E8);

  // Update aggregate bitboards
  updateAggregateBitBoards();

  // Update rotated bitboards
  updateRotatedBitBoards();
}

void BitBoard::initKingAttacks()
{
  static const int kingRowIncr[] = {1, 1, 1, 0, -1, -1, -1, 0};
  static const int kingColIncr[] = {-1, 0, 1, 1, 1, 0, -1, -1};

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
  static const int knightRowIncr[] = {1, 2, 2, 1, -1, -2, -2, -1};
  static const int knightColIncr[] = {-2, -1, 1, 2, 2, 1, -1, -2};

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

void BitBoard::initPawnAttacks(Side side, char rowIncr)
{
  // Precompute attacks for each square
  for (char sourceRow = 0; sourceRow < 8; sourceRow++) {
    for (char sourceCol = 0; sourceCol < 8; sourceCol++) {
      uchar index = getIndex(sourceRow, sourceCol);
      mPawnAttacks[side][index] = 0ULL;

      for (int colIncr = -1; colIncr <= 1; colIncr += 2) {
        char destRow = sourceRow + rowIncr;
        char destCol = sourceCol + colIncr;
        if (destRow >= 0 && destRow < 8 && destCol >= 0 && destCol < 8) {
          uchar destIndex = getIndex(destRow, destCol);
          mPawnAttacks[side][index] |= (ONE << destIndex);
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
      u64 bb = 0;

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
      u64 bb = 0;

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

//BitBoard::u64 BitBoard::invRotateL45(u64 bb) const
//{
//  u64 symmBB = 0;
//  for (uint index = 0; index < 64; index++)
//    if (bb & (ONE << invRotateL45Matrix[index]))
//      symmBB |= (ONE << index);

//  return symmBB;
//}

//BitBoard::u64 BitBoard::invRotateR45(u64 bb) const
//{
//  u64 symmBB = 0;
//  for (uint index = 0; index < 64; index++)
//    if (bb & (ONE << invRotateR45Matrix[index]))
//      symmBB |= (ONE << index);

//  return symmBB;
//}

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
  uchar fromSquare = getIndex(7-move.sourceRow(), move.sourceCol());
  uchar toSquare = getIndex(7-move.destRow(), move.destCol());
  Type fromPiece = mType[fromSquare];
  Type toPiece = mType[toSquare];

  Undo undo;
  undo.rotate90 = mRotateR90;
  undo.rotateL45 = mRotateL45;
  undo.rotateR45 = mRotateR45;
  mUndoStack[mUndoPos++] = undo;

  // Calculate required bitboards
  u64 bbFrom = (ONE << fromSquare);
  u64 bbTo = (ONE << toSquare);
  u64 bbFromTo = bbFrom ^ bbTo;

  // Update the type array
  mType[fromSquare] = Type::None;
  mType[toSquare] = fromPiece;

  // Handle non promotion moves
  if (!move.isPromotion() && !move.isPromotionCapture()) {
    mPiece[fromPiece] ^= bbFromTo;
  }

  // Handle capture
  if (move.isCapture() && !move.isEpCapture()) {
    mPiece[toPiece] ^= (ONE << toSquare);
  }

  // Handle enPassant captures
  if (move.isEpCapture()) {
    char dir = (side == Color::White) ? SOUTH : NORTH;
    uchar sq = toSquare + dir;
    mType[sq] = Type::None;
    mPiece[WhitePawn + !mSide] &= ~(ONE << sq);
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

    mPiece[WhitePawn + mSide] ^= (ONE << fromSquare);
    switch (promotedPiece) {
    case Piece::Queen:
      mPiece[WhiteQueen + mSide] ^= (ONE << toSquare);
      mType[toSquare] = static_cast<Type>(WhiteQueen + mSide);
      break;
    case Piece::Rook:
      mPiece[WhiteRook + mSide] ^= (ONE << toSquare);
      mType[toSquare] = static_cast<Type>(WhiteRook + mSide);
      break;
    case Piece::Bishop:
      mPiece[WhiteBishop + mSide] ^= (ONE << toSquare);
      mType[toSquare] = static_cast<Type>(WhiteBishop + mSide);
      break;
    case Piece::Knight:
      mPiece[WhiteKnight + mSide] ^= (ONE << toSquare);
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
      mPiece[WhiteRook] ^= (ONE << D1);
      mPiece[WhiteRook] ^= (ONE << A1);
      break;
    case G1:
      mType[F1] = Type::WhiteRook;
      mType[H1] = Type::None;
      mPiece[WhiteRook] ^= (ONE << F1);
      mPiece[WhiteRook] ^= (ONE << H1);
      break;
    case C8:
      mType[D8] = Type::BlackRook;
      mType[A8] = Type::None;
      mPiece[BlackRook] ^= (ONE << D8);
      mPiece[BlackRook] ^= (ONE << A8);
      break;
    case G8:
      mType[F8] = Type::BlackRook;
      mType[H8] = Type::None;
      mPiece[BlackRook] ^= (ONE << F8);
      mPiece[BlackRook] ^= (ONE << H8);
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

  //writeBitBoard(mPiece[WhitePawn]);
  //writeBitBoard(mPiece[BlackPawn]);

  // Update the rotated bit boards
  // TODO: Update these incrementally above
  mRotateR90 = rotateR90(mPiece[All]);
  mRotateR45 = rotateR45(mPiece[All]);
  mRotateL45 = rotateL45(mPiece[All]);

  // Switch sides
  mSide = static_cast<Side>(!mSide);
  toggleSideToMove();
}

PieceType BitBoard::pieceType(uchar row, uchar col) const
{
  uchar index = getIndex(7-row, col);
  switch (mType[index]) {
  case WhitePawn:
    return PieceType::WhitePawn;
  case WhiteRook:
    return PieceType::WhiteRook;
  case WhiteKnight:
    return PieceType::WhiteKnight;
  case WhiteBishop:
    return PieceType::WhiteBishop;
  case WhiteQueen:
    return PieceType::WhiteQueen;
  case WhiteKing:
    return PieceType::WhiteKing;
  case BlackPawn:
    return PieceType::BlackPawn;
  case BlackRook:
    return PieceType::BlackRook;
  case BlackKnight:
    return PieceType::BlackKnight;
  case BlackBishop:
    return PieceType::BlackBishop;
  case BlackQueen:
    return PieceType::BlackQueen;
  case BlackKing:
    return PieceType::BlackKing;
  }
  return PieceType::None;
}

void BitBoard::pushBitBoardMoves(u64 bb, uchar fromSq, MoveList & moveList) const
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
  Move move(7-getRow(from), getCol(from), 7-getRow(to), getCol(to), mTypeToPiece[piece]);
  move.setCastlingRights(castlingRights());
  move.setHalfMoveClock(halfMoveClock());
  move.setFullMoveCounter(fullMoveCounter());
  move.setType(type);
  move.setCapturedPiece(mTypeToPiece[capture]);
  move.setPromotedPiece(mTypeToPiece[promote]);
  move.setEnpassantColumn(epColumn());

  moveList.addMove(move);
}

BitBoard::u64 BitBoard::rotateL45(u64 bb) const
{
  u64 symmBB = 0;
  for (uint index = 0; index < 64; index++)
    if (bb & (ONE << rotateL45Matrix[index]))
      symmBB += (ONE << index);

  return symmBB;

//  u64 symmBB = 0;
//  for (uint index = 0; index < 64; index++)
//    if (bb & (ONE << index))
//      symmBB += (ONE << rotateL45Matrix[index]);

//  return symmBB;
}

BitBoard::u64 BitBoard::rotateL90(u64 bb) const
{
  u64 symmBB = 0;
  for (uint index = 0; index < 64; index++)
    if (bb & (ONE << rotateL90Matrix[index]))
      symmBB |= (ONE << index);

  return symmBB;
//  u64 symmBB = 0;
//  for (uint index = 0; index < 64; index++)
//    if (bb & (ONE << index))
//      symmBB |= (ONE << rotateL90Matrix[index]);

//  return symmBB;
}

BitBoard::u64 BitBoard::rotateR45(u64 bb) const
{
  u64 symmBB = 0;
  for (uint index = 0; index < 64; index++)
    if (bb & (ONE << rotateR45Matrix[index]))
      symmBB += (ONE << index);

  return symmBB;
//  u64 symmBB = 0;
//  for (uint index = 0; index < 64; index++)
//    if (bb & (ONE << index))
//      symmBB += (ONE << rotateR45Matrix[index]);

//  return symmBB;
}

BitBoard::u64 BitBoard::rotateR90(u64 bb) const
{
  u64 symmBB = 0;
  for (uint index = 0; index < 64; index++)
    if (bb & (ONE << rotateR90Matrix[index]))
      symmBB |= (ONE << index);

  return symmBB;

//  u64 symmBB = 0;
//  for (uint index = 0; index < 64; index++)
//    if (bb & (ONE << index))
//      symmBB |= (ONE << rotateR90Matrix[index]);

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
  mSide = (color == Color::White) ? White : Black;
  setSideToMove(color);

  clearEpCol();
  if (fenData.epColumn() >= 0) {
    setEpColumn(fenData.epColumn());
  }

  for (int i = 0; i < All; i++) {
    mPiece[i] = 0;
  }

  for (char row = 0; row < 8; row++) {
    for (char col = 0; col < 8; col++) {
      PieceType type = fenData.pieceType(row, col);
      uchar sq = getIndex(7-row, col);
      u64 bb = (ONE << sq);

      mType[sq] = None;
      switch (type) {
      case PieceType::WhitePawn:
        mPiece[WhitePawn] |= bb;
        mType[sq] = WhitePawn;
        break;
      case PieceType::WhiteRook:
        mPiece[WhiteRook] |= bb;
        mType[sq] = WhiteRook;
        break;
      case PieceType::WhiteKnight:
        mPiece[WhiteKnight] |= bb;
        mType[sq] = WhiteKnight;
        break;
      case PieceType::WhiteBishop:
        mPiece[WhiteBishop] |= bb;
        mType[sq] = WhiteBishop;
        break;
      case PieceType::WhiteQueen:
        mPiece[WhiteQueen] |= bb;
        mType[sq] = WhiteQueen;
        break;
      case PieceType::WhiteKing:
        mPiece[WhiteKing] |= bb;
        mType[sq] = WhiteKing;
        setKingRow(Color::White, row);
        setKingColumn(Color::White, col);
        break;
      case PieceType::BlackPawn:
        mPiece[BlackPawn] |= bb;
        mType[sq] = BlackPawn;
        break;
      case PieceType::BlackRook:
        mPiece[BlackRook] |= bb;
        mType[sq] = BlackRook;
        break;
      case PieceType::BlackKnight:
        mPiece[BlackKnight] |= bb;
        mType[sq] = BlackKnight;
        break;
      case PieceType::BlackBishop:
        mPiece[BlackBishop] |= bb;
        mType[sq] = BlackBishop;
        break;
      case PieceType::BlackQueen:
        mPiece[BlackQueen] |= bb;
        mType[sq] = BlackQueen;
        break;
      case PieceType::BlackKing:
        mPiece[BlackKing] |= bb;
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

  // Update rotated bitboards
  updateRotatedBitBoards();

  //writeBitBoard(mPiece[WhitePawn]);
  //writeBitBoard(mPiece[BlackPawn]);
  //writeBitBoard(rotateR45(mPiece[All]));
  //writeOccupancy(rotateR90(mPiece[All]) >> a1h8Shifts[A8] & 255);
  //writeBitBoard(rotateL45(mPiece[All]) );
  //writeBitBoard(mPiece[All]);
  //writeBitBoard((rotateL45(mPiece[All]) >> a8h1Shifts[B1]) & 255);
  //exit(1);
  //writeBitBoard(rotateR45(rotateL45(mPiece[WhitePieces])));
  //writeBitBoard(mPiece[WhitePawn]);
}

void BitBoard::unmakeMove(const Move & move)
{
  // Get move information
  Color side = sideToMove();
  Piece piece = move.piece();
  uchar fromSquare = getIndex(7-move.sourceRow(), move.sourceCol());
  uchar toSquare = getIndex(7-move.destRow(), move.destCol());
  Type toPiece = mType[toSquare];

  Undo undo = mUndoStack[--mUndoPos];
  mRotateR90 = undo.rotate90;
  mRotateL45 = undo.rotateL45;
  mRotateR45 = undo.rotateR45;

  // Calculate required bitboards
  u64 bbFrom = (ONE << fromSquare);
  u64 bbTo = (ONE << toSquare);
  u64 bbFromTo = bbFrom ^ bbTo;

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
    mPiece[toPiece] ^= bbFromTo;
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
        mType[sq] = static_cast<Type>(i);
        mPiece[i] |= (ONE << sq);
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

    mPiece[WhitePawn + !mSide] ^= (ONE << fromSquare);
    mType[fromSquare] = static_cast<Type>(WhitePawn + !mSide);
    switch (promotedPiece) {
    case Piece::Queen:
      mPiece[WhiteQueen + !mSide] ^= (ONE << toSquare);
      break;
    case Piece::Rook:
      mPiece[WhiteRook + !mSide] ^= (ONE << toSquare);
      break;
    case Piece::Bishop:
      mPiece[WhiteBishop + !mSide] ^= (ONE << toSquare);
      break;
    case Piece::Knight:
      mPiece[WhiteKnight + !mSide] ^= (ONE << toSquare);
      break;
    }
  }

  // Handle castling move
  if (move.isCastle()) {
    switch (toSquare) {
    case C1:
      mType[A1] = Type::WhiteRook;
      mType[D1] = Type::None;
      mPiece[WhiteRook] ^= (ONE << A1);
      mPiece[WhiteRook] ^= (ONE << D1);
      break;
    case G1:
      mType[H1] = Type::WhiteRook;
      mType[F1] = Type::None;
      mPiece[WhiteRook] ^= (ONE << H1);
      mPiece[WhiteRook] ^= (ONE << F1);
      break;
    case C8:
      mType[A8] = Type::BlackRook;
      mType[D8] = Type::None;
      mPiece[BlackRook] ^= (ONE << A8);
      mPiece[BlackRook] ^= (ONE << D8);
      break;
    case G8:
      mType[H8] = Type::BlackRook;
      mType[F8] = Type::None;
      mPiece[BlackRook] ^= (ONE << H8);
      mPiece[BlackRook] ^= (ONE << F8);
      break;
    }
  }

  // Update the aggregate bitboards
  updateAggregateBitBoards();

  //writeBitBoard(mPiece[WhitePawn]);
  //writeBitBoard(mPiece[BlackPawn]);

  // Switch sides
  mSide = static_cast<Side>(!mSide);
  toggleSideToMove();
}

void BitBoard::updateAggregateBitBoards()
{
  mPiece[WhitePieces] = mPiece[WhiteRook] | mPiece[WhiteKnight] | mPiece[WhiteBishop] | mPiece[WhiteQueen] | mPiece[WhiteKing] | mPiece[WhitePawn];
  mPiece[BlackPieces] = mPiece[BlackRook] | mPiece[BlackKnight] | mPiece[BlackBishop] | mPiece[BlackQueen] | mPiece[BlackKing] | mPiece[BlackPawn];
  mPiece[All] = mPiece[WhitePieces] | mPiece[BlackPieces];
  mPiece[None] = ~mPiece[All];
}

void BitBoard::updateRotatedBitBoards()
{
  mRotateR90 = rotateR90(mPiece[All]);
  mRotateR45 = rotateR45(mPiece[All]);
  mRotateL45 = rotateL45(mPiece[All]);
}

void BitBoard::writeBitBoard(u64 bb, std::ostream &output) const
{
  std::ostringstream oss;

  for (uchar sq = 0; sq < 64; sq++) {
    if (getCol(sq) == 0)
      oss << 8-getRow(sq) << " ";

    if (bb & (ONE << sq)) {
      oss << "X ";
    }
    else {
      oss << "- ";
    }
    if (sq%8 == 7)
      oss << "\n";
  }
//  for (int i = 7; i >= 0; i--) {
//    oss << i+1 << " ";
//    for (int j = 0; j < 8; j++) {
//      int shift = 8*i + j ;
//      if ( (bb >> shift) & 0x1)
//        oss << "X ";
//      else
//        oss << "- ";
//    }
//    oss << "\n";
//  }
  oss << "  ";
  for (int j = 0; j < 8; j++) {
    switch (j) {
      case 0:
        oss << "A ";
        break;
      case 1:
        oss << "B ";
        break;
      case 2:
        oss << "C ";
        break;
      case 3:
        oss << "D ";
        break;
      case 4:
        oss << "E ";
        break;
      case 5:
        oss << "F ";
        break;
      case 6:
        oss << "G ";
        break;
      case 7:
        oss << "H";
        break;
    }
  }
  oss << "\n\n";
  output << oss.str();
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

// Square mapping
// We use little endian rank-file (LERF) mapping
//enum Square
//{
//  A1=0 , B1, C1, D1, E1, F1, G1, H1,
//  A2=8 , B2, C2, D2, E2, F2, G2, H2,
//  A3=16, B3, C3, D3, E3, F3, G3, H3,
//  A4=24, B4, C4, D4, E4, F4, G4, H4,
//  A5=32, B5, C5, D5, E5, F5, G5, H5,
//  A6=40, B6, C6, D6, E6, F6, G6, H6,
//  A7=48, B7, C7, D7, E7, F7, G7, H7,
//  A8=56, B8, C8, D8, E8, F8, G8, H8
//};


// Shifts for ranks
//static const uint rankShifts[64] =
//{
//  0, 0, 0, 0, 0, 0, 0, 0,
//  8, 8, 8, 8, 8, 8, 8, 8,
//  16, 16, 16, 16, 16, 16, 16, 16,
//  24, 24, 24, 24, 24, 24, 24, 24,
//  32, 32, 32, 32, 32, 32, 32, 32,
//  40, 40, 40, 40, 40, 40, 40, 40,
//  48, 48, 48, 48, 48, 48, 48, 48,
//  56, 56, 56, 56, 56, 56, 56, 56
//};

//// Masks for ranks
//static const uint rankMasks[64] =
//{
//  255, 255, 255, 255, 255, 255, 255, 255,
//  255, 255, 255, 255, 255, 255, 255, 255,
//  255, 255, 255, 255, 255, 255, 255, 255,
//  255, 255, 255, 255, 255, 255, 255, 255,
//  255, 255, 255, 255, 255, 255, 255, 255,
//  255, 255, 255, 255, 255, 255, 255, 255,
//  255, 255, 255, 255, 255, 255, 255, 255,
//  255, 255, 255, 255, 255, 255, 255, 255
//};

//// Square mapping for a 90 degree rotation in the counter-clockwise direction
//static const int rotateL90Matrix[64] =
//{
//  H1, H2, H3, H4, H5, H6, H7, H8,
//  G1, G2, G3, G4, G5, G6, G7, G8,
//  F1, F2, F3, F4, F5, F6, F7, F8,
//  E1, E2, E3, E4, E5, E6, E7, E8,
//  D1, D2, D3, D4, D5, D6, D7, D8,
//  C1, C2, C3, C4, C5, C6, C7, C8,
//  B1, B2, B3, B4, B5, B6, B7, B8,
//  A1, A2, A3, A4, A5, A6, A7, A8,
//};

//// Square mapping for a 90 degree rotation in the clockwise direction
//static const int rotateR90Matrix[64] =
//{
//  A8, A7, A6, A5, A4, A3, A2, A1,
//  B8, B7, B6, B5, B4, B3, B2, B1,
//  C8, C7, C6, C5, C4, C3, C2, C1,
//  D8, D7, D6, D5, D4, D3, D2, D1,
//  E8, E7, E6, E5, E4, E3, E2, E1,
//  F8, F7, F6, F5, F4, F3, F2, F1,
//  G8, G7, G6, G5, G4, G3, G2, G1,
//  H8, H7, H6, H5, H4, H3, H2, H1,
//};

//// Shifts for files
//static const uint fileShifts[64] =
//{
//  56, 48, 40, 32, 24, 16, 8, 0,
//  56, 48, 40, 32, 24, 16, 8, 0,
//  56, 48, 40, 32, 24, 16, 8, 0,
//  56, 48, 40, 32, 24, 16, 8, 0,
//  56, 48, 40, 32, 24, 16, 8, 0,
//  56, 48, 40, 32, 24, 16, 8, 0,
//  56, 48, 40, 32, 24, 16, 8, 0,
//  56, 48, 40, 32, 24, 16, 8, 0,
//};

//// Masks for files
//static const uint fileMasks[64] =
//{
//  255, 255, 255, 255, 255, 255, 255, 255,
//  255, 255, 255, 255, 255, 255, 255, 255,
//  255, 255, 255, 255, 255, 255, 255, 255,
//  255, 255, 255, 255, 255, 255, 255, 255,
//  255, 255, 255, 255, 255, 255, 255, 255,
//  255, 255, 255, 255, 255, 255, 255, 255,
//  255, 255, 255, 255, 255, 255, 255, 255,
//  255, 255, 255, 255, 255, 255, 255, 255
//};

//// Square mapping for a 45 degree rotation in the counter-clockwise (a1h8) direction
//// Represents the trailing (NW) diagonals
//// Formatted for ease of visualization
////static const int rotateL45Matrix[64] =
////{
////  H1,
////  G1, H2,
////  F1, G2, F1,
////  E1, G3, F2, E1,
////  D1, G4, F3, E2, D1,
////  C1, G5, F4, E3, D2, C1,
////  H7, G6, F5, E4, D3, C2, B1,
////  H8, G7, F6, E5, D4, C3, B2, A1,
////  G8, F7, E6, D5, C4, B3, A2,
////  F8, E7, D6, C5, B4, A3,
////  E8, D7, C6, B5, A4,
////  D8, C7, B6, A5,
////  C8, B7, A6,
////  B8, A7,
////  A8
////};

//// Square mapping for a 45 degree rotation in the counter-clockwise (a1h8) direction
//// Represents the trailing (NW) diagonals
//// Formatted for ease of visualization
//static const uint rotateL45Matrix[64] =
//{
//  H8,
//  G8,	H7,
//  F8,	G7,	H6,
//  E8,	F7,	G6,	H5,
//  D8,	E7,	F6,	G5,	H6,
//  C8,	D7,	E6,	F5,	G4,	H3,
//  B8,	C7,	D6,	E5,	F4,	G3,	H2,
//  A8,	B7,	C6,	D5,	E4,	F3,	G2,	H1,
//  A7,	B6,	C5,	D4,	E3,	F2,	G1,
//  A6,	B5,	C4,	D3,	E2,	F1,
//  A5,	B4,	C3,	D2,	E1,
//  A4,	B3,	C2,	D1,
//  A3,	B2,	C1,
//  A2,	B1,
//  A1
//};

//static const uint a1h8Shifts[64] =
//{
//  56,
//};

//// Square mapping for a 45 degree rotation in the clockwise (a8h1) direction
//// Represents the leading (NE) diagonals
//// Formatted for ease of visualization
//static const int rotateR45Matrix[64] =
//{
//  A1,
//  A2,	B1,
//  A3,	B2,	C1,
//  A4,	B3,	C2,	D1,
//  A5,	B4,	C3,	D2,	E1,
//  A6,	B5,	C4,	D3,	E2,	F1,
//  A7,	B6,	C5,	D4,	E3,	F2,	G1,
//  A8,	B7,	C6,	D5,	E4,	F3,	G1,	H1,
//  B8,	C7,	D6,	E5,	F4,	G3,	H2,
//  C8,	D7,	E6,	F5,	G4,	H3,
//  D8,	E7,	F6,	G5,	H4,
//  E8,	F7,	G6,	H5,
//  F8,	G7,	H6,
//  G8,	H7,
//  H8,
//};

//static const uint a8h1Shifts[64] =
//{

//};

//static const uint a1h8Masks[64] =
//{

//};

//static const uint a8h1Masks[64] =
//{

//};

//static const uint a1h8Lengths[64] =
//{

//};

//static const uint a8h1Lengths[64] =
//{

//};
