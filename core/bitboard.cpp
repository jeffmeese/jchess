#include "bitboard.h"
#include "fen.h"

#include <cassert>
#include <sstream>

// Macros for mapping (row,col)->index and vice-versa and valid squares
#define C64(constantU64) constantU64##ULL
#define getIndex(row,col) (((row)<<3)+(col))
#define getRow(index) ( (index >> 3) )
#define getCol(index) ( (index % 8) )

// Attack offsets
static const char NORTH =  16;
static const char SOUTH = -16;
static const char EAST  =   1;
static const char WEST  =  -1;
static const char NW    =  15;
static const char NE    =  17;
static const char SW    = -17;
static const char SE    = -15;
static const char SSW   = -17;
static const char SSE   = -15;
static const char WWS   = -10;
static const char EES   =  -6;
static const char EEN   =  10;
static const char WWN   =   6;
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
const int index64[64] =
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
int BitBoard::bitScanForward(U64 bb) const
{
  //unsigned long index = 0;
  //_BitScanForward64(&index, bb);
  //return index;
   static const ulonglong debruijn64 = ulonglong(0x07EDD5E59A4E28C2);
   assert (bb != 0);
   return index64[((bb & -bb) * debruijn64) >> 58];
}

int BitBoard::bitScanForwardPopCount(U64 bb) const
{
  assert (bb != 0);
  return popCount( (bb & -bb) - 1 );
}

void BitBoard::generateMoves(MoveList & moveList) const
{

}

void BitBoard::initAttacks()
{
  for (int i = 0; i < 64; i++) {
    mBishopAttacks[i] = 0ULL;
    mRookAttacks[i] = 0ULL;
    mKingAttacks[i] = 0ULL;
    mKnightAttacks[i] = 0ULL;
    mWhitePawnAttacks[i] = 0ULL;
    mBlackPawnAttacks[i] = 0ULL;
  }

  // The loops below could probably be functionalized
  for (char sourceRow = 0; sourceRow < 8; sourceRow++) {
    for (char sourceCol = 0; sourceCol < 8; sourceCol++) {
      uchar index = getIndex(sourceRow, sourceCol);

      // Knight attacks
      for (char i = 0; i < 8; i++) {
        uchar destIndex = index + knightAttacks[i];
        uchar destRow = getRow(destIndex);
        uchar destCol = getCol(destIndex);
        if (abs(destRow-sourceRow) <= 2 && abs(destCol-sourceCol) <= 2) {
          if (destRow >= 0 && destRow < 8 && destCol >= 0 && destCol < 8) {
            mKnightAttacks[index] |= (C64(1) << destIndex);
          }
        }
      }

      // King attacks
      for (char i = 0; i < 8; i++) {
        uchar destIndex = index + kingAttacks[i];
        uchar destRow = getRow(destIndex);
        uchar destCol = getCol(destIndex);

        if (abs(destRow-sourceRow) <= 1 && abs(destCol-sourceCol) <= 1) {
          if (destRow >= 0 && destRow < 8 && destCol >= 0 && destCol < 8) {
            mKingAttacks[index] |= (C64(1) << destIndex);
          }
        }
      }

      // White pawn attacks
      for (char i = 0; i < 2; i++) {
        uchar destIndex = index + whitePawnAttacks[i];
        uchar destRow = getRow(destIndex);
        uchar destCol = getCol(destIndex);
        if (abs(destRow-sourceRow) <= 1 && abs(destCol-sourceCol) <= 1) {
          if (destRow >= 0 && destRow < 8 && destCol >= 0 && destCol < 8) {
            mWhitePawnAttacks[index] |= (C64(1) << destIndex);
          }
        }
      }

      // Black pawn attacks
      for (char i = 0; i < 2; i++) {
        uchar destIndex = index + blackPawnAttacks[i];
        uchar destRow = getRow(destIndex);
        uchar destCol = getCol(destIndex);
        std::cout << static_cast<int>(destRow) << " " << static_cast<int>(destCol) << "\n";
        if (abs(destRow-sourceRow) <= 1 && abs(destCol-sourceCol) <= 1) {
          if (destRow >= 0 && destRow < 8 && destCol >= 0 && destCol < 8) {
            mBlackPawnAttacks[index] |= (C64(1) << destIndex);
          }
        }
      }

      // East attacks
      for (char i = 0; i < 8; i++) {
        char destCol = sourceCol + (i+1);
        if (destCol > 7)
          break;

        uchar destIndex = getIndex(sourceRow, destCol);
        mRookAttacks[index] |= (C64(1) << destIndex);
      }

      // West attacks
      for (char i = 0; i < 8; i++) {
        char destCol = sourceCol - (i+1);
        if (destCol < 0)
          break;

        uchar destIndex = getIndex(sourceRow, destCol);
        mRookAttacks[index] |= (C64(1) << destIndex);
      }

      // North attacks
      for (char i = 0; i < 8; i++) {
        char destRow = sourceRow + (i+1);
        if (destRow > 7)
          break;

        uchar destIndex = getIndex(destRow, sourceCol);
        mRookAttacks[index] |= (C64(1) << destIndex);
      }

      // South attacks
      for (char i = 0; i < 8; i++) {
        char destRow = sourceRow - (i+1);
        if (destRow < 0)
          break;

        uchar destIndex = getIndex(destRow, sourceCol);
        mRookAttacks[index] |= (C64(1) << destIndex);
      }

      // North east attacks
      for (char i = 0; i < 8; i++) {
        char destRow = sourceRow + (i+1);
        char destCol = sourceCol + (i+1);
        if (destRow > 7 || destCol > 7) {
          break;
        }
        uchar destIndex = getIndex(destRow, destCol);
        mBishopAttacks[index] |= (C64(1) << destIndex);
      }

      // North west attacks
      for (char i = 0; i < 8; i++) {
        char destRow = sourceRow + (i+1);
        char destCol = sourceCol - (i+1);
        if (destRow > 7 || destCol < 0) {
          break;
        }
        uchar destIndex = getIndex(destRow, destCol);
        mBishopAttacks[index] |= (C64(1) << destIndex);
      }

      // South west attacks
      for (char i = 0; i < 8; i++) {
        char destRow = sourceRow - (i+1);
        char destCol = sourceCol - (i+1);
        if (destRow < 0 || destCol < 0) {
          break;
        }
        uchar destIndex = getIndex(destRow, destCol);
        mBishopAttacks[index] |= (C64(1) << destIndex);
      }

      // South east attacks
      for (char i = 0; i < 8; i++) {
        char destRow = sourceRow - (i+1);
        char destCol = sourceCol + (i+1);
        if (destRow < 0 || destCol > 7) {
          break;
        }
        uchar destIndex = getIndex(destRow, destCol);
        mBishopAttacks[index] |= (C64(1) << destIndex);
      }
    }
  }
  writeBitBoard(mBishopAttacks[18], std::cout);
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

  // Initialize the piece bit boards
  mWhiteBishops = 0x24ULL;
  mWhiteKings = 0x10ULL;
  mWhiteKnights = 0x42ULL;
  mWhitePawns = (0xffULL) << 8ULL;
  mWhiteQueens = 0x08ULL;
  mWhiteRooks = 0x81ULL;
  mWhitePieces = mWhiteBishops | mWhiteKings | mWhiteKnights | mWhitePawns | mWhiteQueens | mWhiteRooks;

  mBlackBishops = (0x24ULL << C64(56));
  mBlackKings = (0x10ULL << C64(56));
  mBlackKnights = (0x42ULL << C64(56));
  mBlackPawns = (0xffULL << C64(48));
  mBlackQueens = (0x08ULL << C64(56));
  mBlackRooks = (0x81ULL << C64(56));
  mWhitePieces = mBlackBishops | mBlackKings | mBlackKnights | mBlackPawns | mBlackQueens | mBlackRooks;

  // Initialize other bitboards
  mAllPieces = mWhitePieces | mBlackPieces;
  mEmptySquares = ~mAllPieces;
  mRotate90AllPieces = rotate90(mAllPieces);
  mRotate45RightPieces = rotate45(mAllPieces, a1h8Matrix);
  mRotate45LeftPieces = rotate45(mAllPieces, a8h1Matrix);
  for (uchar index = 0; index < 64; index++) {
    mRotate45Right[index] = rotate45((C64(1)<<index), a1h8Matrix);
    mRotate45Left[index] = rotate45((C64(1)<<index), a8h1Matrix);
    mRotate90[index] = rotate90((C64(1)<<index));
  }
}

bool BitBoard::isCellAttacked(uchar row, uchar col, Color attackingColor) const
{
  return false;
}

int BitBoard::popCount (U64 x) const
{
  int count = 0;
  while (x) {
    count++;
    x &= x - 1; // reset LS1B
  }
  return count;
}

void BitBoard::makeMove(const Move & move)
{

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

  // TODO: Still need to fill bitboards
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

  mWhitePieces = mWhiteBishops | mWhiteKings | mWhiteKnights | mWhitePawns | mWhiteQueens | mWhiteRooks;
  mBlackPieces = mBlackBishops | mBlackKings | mBlackKnights | mBlackPawns | mBlackQueens | mBlackRooks;

  // Initialize other bitboards
  mAllPieces = mWhitePieces | mBlackPieces;
  mEmptySquares = ~mAllPieces;
  mRotate90AllPieces = rotate90(mAllPieces);
  mRotate45RightPieces = rotate45(mAllPieces, a1h8Matrix);
  mRotate45LeftPieces = rotate45(mAllPieces, a8h1Matrix);
  for (uchar index = 0; index < 64; index++) {
    mRotate45Right[index] = rotate45((C64(1)<<index), a1h8Matrix);
    mRotate45Left[index] = rotate45((C64(1)<<index), a8h1Matrix);
    mRotate90[index] = rotate90((C64(1)<<index));
  }
}

void BitBoard::unmakeMove(const Move & move)
{

}

void BitBoard::writeBitBoard(U64 bitboard, std::ostream & output) const
{
  std::ostringstream oss;

  for (int i = 7; i >= 0; i--) {
    for (int j = 0; j < 8; j++) {
      int shift = 8*i + j ;
      if ( (bitboard >> shift) & 0x1)
        oss << 'X';
      else
        oss << '-';
    }
    oss << "\n";
  }
  output << oss.str() << "\n\n";
}
