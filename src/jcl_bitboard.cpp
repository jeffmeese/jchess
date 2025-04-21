#include "jcl_bitboard.h"

#include <bitset>
#include <cassert>
#include <iomanip>
#include <sstream>
//#include <intrin.h>

#include "jcl_fen.h"
#include "jcl_move.h"
#include "jcl_movelist.h"

constexpr int8_t  NORTH          =  8;
constexpr int8_t  SOUTH          = -8;
constexpr int8_t  EAST           =  1;
constexpr int8_t  WEST           = -1;

namespace jcl
{

// Square definition
//
// This is the standard square definition
// with the lowest value being assigned to
// the lower left of the board.
enum Square
{
  A1 =  0, B1, C1, D1, E1, F1, G1, H1,
  A2 =  8, B2, C2, D2, E2, F2, G2, H2,
  A3 = 16, B3, C3, D3, E3, F3, G3, H3,
  A4 = 24, B4, C4, D4, E4, F4, G4, H4,
  A5 = 32, B5, C5, D5, E5, F5, G5, H5,
  A6 = 40, B6, C6, D6, E6, F6, G6, H6,
  A7 = 48, B7, C7, D7, E7, F7, G7, H7,
  A8 = 56, B8, C8, D8, E8, F8, G8, H8
};

// Bitboard square definition
//
// This defines the board from least significant
// bit to most significant bit for each row. This means
// the most significant bit in a bitboard value is the
// highest number (BB_A8) in this definition.
// The writeBitboard function writes the
// rows out in reverse so the most significant
// bit is in the first position in the row
// giving a more natural look to the board.
enum BitboardSquare
{
  BB_H1 =  0, BB_G1, BB_F1, BB_E1, BB_D1, BB_C1, BB_B1, BB_A1,
  BB_H2 =  8, BB_G2, BB_F2, BB_E2, BB_D2, BB_C2, BB_B2, BB_A2,
  BB_H3 = 16, BB_G3, BB_F3, BB_E3, BB_D3, BB_C3, BB_B3, BB_A3,
  BB_H4 = 24, BB_G4, BB_F4, BB_E4, BB_D4, BB_C4, BB_B4, BB_A4,
  BB_H5 = 32, BB_G5, BB_F5, BB_E5, BB_D5, BB_C5, BB_B5, BB_A5,
  BB_H6 = 40, BB_G6, BB_F6, BB_E6, BB_D6, BB_C6, BB_B6, BB_A6,
  BB_H7 = 48, BB_G7, BB_F7, BB_E7, BB_D7, BB_C7, BB_B7, BB_A7,
  BB_H8 = 56, BB_G8, BB_F8, BB_E8, BB_D8, BB_C8, BB_B8, BB_A8
};

#define ZERO 0LL
#define ONE 1LL

const uint64_t FILE_A = 0x8080808080808080ULL;
const uint64_t FILE_B = 0x4040404040404040ULL;
const uint64_t FILE_C = 0x2020202020202020ULL;
const uint64_t FILE_D = 0x1010101010101010ULL;
const uint64_t FILE_E = 0x0808080808080808ULL;
const uint64_t FILE_F = 0x0404040404040404ULL;
const uint64_t FILE_G = 0x0202020202020202ULL;
const uint64_t FILE_H = 0x0101010101010101ULL;
const uint64_t RANK_1 = 0x00000000000000ffULL;
const uint64_t RANK_2 = 0x000000000000ff00ULL;
const uint64_t RANK_3 = 0x0000000000ff0000ULL;
const uint64_t RANK_4 = 0x00000000ff000000ULL;
const uint64_t RANK_5 = 0x000000ff00000000ULL;
const uint64_t RANK_6 = 0x0000ff0000000000ULL;
const uint64_t RANK_7 = 0x00ff000000000000ULL;
const uint64_t RANK_8 = 0xff00000000000000ULL;
const uint64_t FILE_AB = FILE_A | FILE_B;
const uint64_t FILE_GH = FILE_G | FILE_H;
const uint64_t FILE_AH = FILE_A | FILE_H;

// Macros for mapping (row,col)->index and vice-versa
#define getIndex(row,col) (((row)<<3)+(col))
#define getRow(index) ( (index >> 3) )
#define getCol(index) ( (index & 7) )
#define getBitboardIndex(row,col) (63 - (((7-row)<<3)+(col)))

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
inline uint8_t bitScanForward(uint64_t bb)
{
  static const uint8_t index64[64] =
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

  static const uint64_t debruijn64 = uint64_t(0x07EDD5E59A4E28C2);
  assert (bb != 0);
  return index64[((bb & -bb) * debruijn64) >> 58];

  //uint32_t index = 0;
  //_BitScanForward64(&index, bb);
  //return static_cast<uint8_t>(index);
}

BitBoard::BitBoard()
{
  init();
  initBoard();
}

bool BitBoard::doGenerateMoves(MoveList & moveList) const
{
  generateCastlingMoves(moveList);

  uint64_t friendly = mWhitePieceBitboard;
  uint64_t enemy = mBlackPieceBitboard;
  uint64_t knights = mBitboards[WhiteKnight];
  uint64_t kings = mBitboards[WhiteKing];
  uint64_t bishops = mBitboards[WhiteBishop];
  uint64_t queens = mBitboards[WhiteQueen];
  uint64_t pawns = mBitboards[WhitePawn];
  uint64_t rooks = mBitboards[WhiteRook];
  if (this->getSideToMove() == Color::Black)
  {
    friendly = mBlackPieceBitboard;
    enemy = mWhitePieceBitboard;
    knights = mBitboards[BlackKnight];
    kings = mBitboards[BlackKing];
    bishops = mBitboards[BlackBishop];
    queens = mBitboards[BlackQueen];
    pawns = mBitboards[BlackPawn];
    rooks = mBitboards[BlackRook];
  }

  if (this->getSideToMove() == Color::White)
  {
    generatePawnPushesWhite(pawns, mAllPieceBitBoard, moveList);
    generatePawnAttacks(pawns, mPawnAttacksWhite, enemy, RANK_8, moveList);
  }
  else
  {
    generatePawnPushesBlack(pawns, mAllPieceBitBoard, moveList);
    generatePawnAttacks(pawns, mPawnAttacksBlack, enemy, RANK_1, moveList);
  }

  generateLeapMoves(knights, Piece::Knight, mKnightMoves, friendly, enemy, moveList);
  generateLeapMoves(kings, Piece::King, mKingMoves, friendly, enemy, moveList);
  generateRookAttacks(rooks, friendly, enemy, Piece::Rook, moveList);
  generateBishopAttacks(bishops, friendly, enemy, Piece::Bishop, moveList);
  generateRookAttacks(queens, friendly, enemy, Piece::Queen, moveList);
  generateBishopAttacks(queens, friendly, enemy, Piece::Queen, moveList);

  return true;
}

bool BitBoard::doGenerateMoves(uint8_t row, uint8_t col, MoveList & moveList) const
{
  //uint8_t index = getIndex(row, col);
  //generateMoves(index, true, moveList);
  return true;
}

PieceType BitBoard::doGetPieceType(uint8_t row, uint8_t col) const
{
  uint8_t index = getIndex(row, col);
  Piece piece = mPieces[index];
  Color color = mColors[index];

  PieceType pieceType = PieceType::None;
  switch (piece)
  {
  case Piece::Pawn:
    pieceType = (color == Color::White) ? PieceType::WhitePawn : PieceType::BlackPawn;
    break;
  case Piece::Bishop:
    pieceType = (color == Color::White) ? PieceType::WhiteBishop : PieceType::BlackBishop;
    break;
  case Piece::Rook:
    pieceType = (color == Color::White) ? PieceType::WhiteRook : PieceType::BlackRook;
    break;
  case Piece::Knight:
    pieceType = (color == Color::White) ? PieceType::WhiteKnight : PieceType::BlackKnight;
    break;
  case Piece::Queen:
    pieceType = (color == Color::White) ? PieceType::WhiteQueen : PieceType::BlackQueen;
    break;
  case Piece::King:
    pieceType = (color == Color::White) ? PieceType::WhiteKing : PieceType::BlackKing;
    break;
  default:
    break;
  }

  return pieceType;
}

bool BitBoard::doIsCellAttacked(uint8_t row, uint8_t col, Color attackingColor) const
{
  return false;
}

bool BitBoard::doMakeMove(const Move * move)
{
  Color sideToMove = this->getSideToMove();
  Color otherSide = (sideToMove == Color::White) ? Color::Black : Color::White;
  uint8_t sourceRow = move->getSourceRow();
  uint8_t sourceCol = move->getSourceColumn();
  uint8_t destRow = move->getDestinationRow();
  uint8_t destCol = move->getDestinationColumn();
  uint8_t sourceSquare = getIndex(sourceRow, sourceCol);
  uint8_t destinationSquare = getIndex(destRow, destCol);
  uint8_t bbSource = getBitboardIndex(sourceRow, sourceCol);
  uint8_t bbDest = getBitboardIndex(destRow, destCol);

  Piece movePiece = mPieces[sourceSquare];
  BitBoardPiece bbMovePiece = translatePiece(movePiece, sideToMove);

  // Handle quiet move, including double pawn pushes
  if (move->isQuiet() || move->isDoublePush())
  {
    mPieces[destinationSquare] = movePiece;
    mColors[destinationSquare] = sideToMove;
    mPieces[sourceSquare] = Piece::None;
    mColors[sourceSquare] = Color::None;

    // 1. Remove the moving piece from the source square
    // 2. Add the moving piece to the destination square
    mBitboards[bbMovePiece] ^= (ONE << bbSource);
    mBitboards[bbMovePiece] |= (ONE << bbDest);
  }

  // Handle promotion move
  if (move->isPromotion())
  {
    Piece promotedPiece = move->getPromotedPiece();
    BitBoardPiece bbPromotedPiece = translatePiece(promotedPiece, sideToMove);

    mPieces[destinationSquare] = move->getPromotedPiece();
    mColors[destinationSquare] = sideToMove;
    mPieces[sourceSquare] = Piece::None;
    mColors[sourceSquare] = Color::None;

    // 1. Remove the moving piece from the source square
    // 2. Add the promoted piece to the destination square
    mBitboards[bbMovePiece] ^= (ONE << bbSource);
    mBitboards[bbPromotedPiece] |= (ONE << bbDest);
  }

  // Handle capture moves
  if (move->isCapture())
  {
    Piece capturePiece = move->getCapturedPiece();
    BitBoardPiece bbCapturePiece = translatePiece(capturePiece, otherSide);

    if (move->isPromotionCapture())
    {
      Piece promotedPiece = move->getPromotedPiece();
      BitBoardPiece bbPromotedPiece = translatePiece(promotedPiece, sideToMove);

      mPieces[destinationSquare] = move->getPromotedPiece();
      mColors[destinationSquare] = sideToMove;
      mPieces[sourceSquare] = Piece::None;
      mColors[sourceSquare] = Color::None;

      // 1. Remove the moving piece from the source square
      // 2. Add the promoted piece to the destination sqaure
      // 3. Remove the capture piece from the destination square
      mBitboards[bbMovePiece] ^= (ONE << bbSource);
      mBitboards[bbPromotedPiece] |= (ONE << bbDest);
      mBitboards[bbCapturePiece] ^= (ONE << bbDest);
    }
    else if (move->isEnPassantCapture())
    {
      uint8_t sourceRow = move->getSourceRow();
      int8_t epColumn = getEnpassantColumn();
      uint8_t captureSquare = getIndex(sourceRow, epColumn);
      uint8_t bbCaptureSquare = getBitboardIndex(sourceRow, epColumn);

      mPieces[destinationSquare] = movePiece;
      mColors[destinationSquare] = sideToMove;
      mPieces[sourceSquare] = Piece::None;
      mColors[sourceSquare] = Color::None;
      mPieces[captureSquare] = Piece::None;
      mColors[captureSquare] = Color::None;

      // 1. Remove the moving piece from the source square
      // 2. Add the moving piece to the destination square
      // 3. Remove the capture piece from the en-passant capture square
      mBitboards[bbMovePiece] ^= (ONE << bbSource);
      mBitboards[bbMovePiece] |= (ONE << bbDest);
      mBitboards[bbCapturePiece] ^= (ONE << bbCaptureSquare);
    }
    else
    {
      mPieces[destinationSquare] = movePiece;
      mColors[destinationSquare] = sideToMove;
      mPieces[sourceSquare] = Piece::None;
      mColors[sourceSquare] = Color::None;

      // 1. Remove the moving piece from the source square
      // 2. Add the moving piece to the destination square
      // 3. Remove the capture piece from the destination square
      mBitboards[bbMovePiece] ^= (ONE << bbSource);
      mBitboards[bbMovePiece] |= (ONE << bbDest);
      mBitboards[bbCapturePiece] ^= (ONE << bbDest);
    }
  }

  // Handle castling moves
  if (move->isCastle())
  {
    if (destinationSquare == G8)
    {
      mPieces[F8] = Piece::Rook;
      mColors[F8] = Color::Black;
      mPieces[H8] = Piece::None;
      mColors[H8] = Color::None;
      mPieces[G8] = Piece::King;
      mColors[G8] = Color::Black;
      mPieces[E8] = Piece::None;
      mColors[E8] = Color::None;
      mBitboards[BlackKing] ^= (ONE << BB_E8);
      mBitboards[BlackKing] |= (ONE << BB_G8);
      mBitboards[BlackRook] ^= (ONE << BB_H8);
      mBitboards[BlackRook] |= (ONE << BB_F8);
    }
    else if (destinationSquare == C8)
    {
      mPieces[D8] = Piece::Rook;
      mColors[D8] = Color::Black;
      mPieces[A8] = Piece::None;
      mColors[A8] = Color::None;
      mPieces[C8] = Piece::King;
      mColors[C8] = Color::Black;
      mPieces[E8] = Piece::None;
      mColors[E8] = Color::None;
      mBitboards[BlackKing] ^= (ONE << BB_E8);
      mBitboards[BlackKing] |= (ONE << BB_C8);
      mBitboards[BlackRook] ^= (ONE << BB_A8);
      mBitboards[BlackRook] |= (ONE << BB_D8);
    }
    else if (destinationSquare == G1)
    {
      mPieces[F1] = Piece::Rook;
      mColors[F1] = Color::White;
      mPieces[H1] = Piece::None;
      mColors[H1] = Color::None;
      mPieces[G1] = Piece::King;
      mColors[G1] = Color::White;
      mPieces[E1] = Piece::None;
      mColors[E1] = Color::None;
      mBitboards[WhiteKing] ^= (ONE << BB_E1);
      mBitboards[WhiteKing] |= (ONE << BB_G1);
      mBitboards[WhiteRook] ^= (ONE << BB_H1);
      mBitboards[WhiteRook] |= (ONE << BB_F1);

    }
    else if (destinationSquare == C1)
    {
      mPieces[D1] = Piece::Rook;
      mColors[D1] = Color::White;
      mPieces[A1] = Piece::None;
      mColors[A1] = Color::None;
      mPieces[C1] = Piece::King;
      mColors[C1] = Color::White;
      mPieces[E1] = Piece::None;
      mColors[E1] = Color::None;
      mBitboards[WhiteKing] ^= (ONE << BB_E1);
      mBitboards[WhiteKing] |= (ONE << BB_C1);
      mBitboards[WhiteRook] ^= (ONE << BB_A1);
      mBitboards[WhiteRook] |= (ONE << BB_D1);
    }
  }

  updateAggregateBitBoards();

  return true;
}

void BitBoard::doReset()
{
  init();
  initBoard();
}

bool BitBoard::doSetPieceType(uint8_t row, uint8_t col, PieceType pieceType)
{
  uint8_t index = getIndex(row, col);
  uint8_t bbIndex = getBitboardIndex(row, col);

  uint8_t bb = 0;
  switch (pieceType)
  {
  case PieceType::WhitePawn:
    mPieces[index] = Piece::Pawn;
    mColors[index] = Color::White;
    bb = WhitePawn;
    break;
  case PieceType::WhiteRook:
    mPieces[index] = Piece::Rook;
    mColors[index] = Color::White;
    bb = WhiteRook;
    break;
  case PieceType::WhiteBishop:
    mPieces[index] = Piece::Bishop;
    mColors[index] = Color::White;
    bb = WhiteBishop;
    break;
  case PieceType::WhiteKnight:
    mPieces[index] = Piece::Knight;
    mColors[index] = Color::White;
    bb = WhiteKnight;
    break;
  case PieceType::WhiteQueen:
    mPieces[index] = Piece::Queen;
    mColors[index] = Color::White;
    bb = WhiteQueen;
    break;
  case PieceType::WhiteKing:
    mPieces[index] = Piece::King;
    mColors[index] = Color::White;
    bb = WhiteKing;
    break;
  case PieceType::BlackPawn:
    mPieces[index] = Piece::Pawn;
    mColors[index] = Color::Black;
    bb = BlackPawn;
    break;
  case PieceType::BlackRook:
    mPieces[index] = Piece::Rook;
    mColors[index] = Color::Black;
    bb = BlackRook;
    break;
  case PieceType::BlackBishop:
    mPieces[index] = Piece::Bishop;
    mColors[index] = Color::Black;
    bb = BlackBishop;
    break;
  case PieceType::BlackKnight:
    mPieces[index] = Piece::Knight;
    mColors[index] = Color::Black;
    bb = BlackKnight;
    break;
  case PieceType::BlackQueen:
    mPieces[index] = Piece::Queen;
    mColors[index] = Color::Black;
    bb = BlackQueen;
    break;
  case PieceType::BlackKing:
    mPieces[index] = Piece::King;
    mColors[index] = Color::Black;
    bb = BlackKing;
    break;
  case PieceType::None:
    mPieces[index] = Piece::None;
    mColors[index] = Color::None;
    break;
  }

  // Clear any existing piece in each bitboard on this square
  for (int i = WhitePawn; i <= BlackKing; i++)
  {
    mBitboards[i] &= ~(ONE << bbIndex);
  }

  // Add the piece to the square
  if (pieceType != PieceType::None)
  {
    mBitboards[bb] |= (ONE << bbIndex);
  }

  updateAggregateBitBoards();

  return true;
}

bool BitBoard::doSetPosition(const Fen & fen)
{
  initBoard();

  for (uint8_t i = 0; i < 12; i++)
  {
    mBitboards[i] = 0;
  }
  mBlackPieceBitboard = mWhitePieceBitboard = mAllPieceBitBoard = mNoPieceBitboard = 0;

  for (uint8_t i = 0; i < 64; i++)
  {
    mPieces[i] = Piece::None;
    mColors[i] = Color::None;
  }

  for (uint8_t i = 0; i < 8; i++)
  {
    for (uint8_t j = 0; j < 8; j++)
    {
      uint8_t index = getIndex(i, j);
      uint8_t bbIndex = getIndex(i, 7-j);
      PieceType pieceType = fen.getPieceType(i, j);

      switch (pieceType)
      {
      case PieceType::WhitePawn:
        mPieces[index] = Piece::Pawn;
        mColors[index] = Color::White;
        mBitboards[WhitePawn] |= (ONE << bbIndex);
        break;
      case PieceType::WhiteRook:
        mPieces[index] = Piece::Rook;
        mColors[index] = Color::White;
        mBitboards[WhiteRook] |= (ONE << bbIndex);
        break;
      case PieceType::WhiteKnight:
        mPieces[index] = Piece::Knight;
        mColors[index] = Color::White;
        mBitboards[WhiteKnight] |= (ONE << bbIndex);
        break;
      case PieceType::WhiteBishop:
        mPieces[index] = Piece::Bishop;
        mColors[index] = Color::White;
        mBitboards[WhiteBishop] |= (ONE << bbIndex);
        break;
      case PieceType::WhiteQueen:
        mPieces[index] = Piece::Queen;
        mColors[index] = Color::White;
        mBitboards[WhiteQueen] |= (ONE << bbIndex);
        break;
      case PieceType::WhiteKing:
        mPieces[index] = Piece::King;
        mColors[index] = Color::White;
        mBitboards[WhiteKing] |= (ONE << bbIndex);
        break;
      case PieceType::BlackPawn:
        mPieces[index] = Piece::Pawn;
        mColors[index] = Color::Black;
        mBitboards[BlackPawn] |= (ONE << bbIndex);
        break;
      case PieceType::BlackRook:
        mPieces[index] = Piece::Rook;
        mColors[index] = Color::Black;
        mBitboards[BlackRook] |= (ONE << bbIndex);
        break;
      case PieceType::BlackKnight:
        mPieces[index] = Piece::Knight;
        mColors[index] = Color::Black;
        mBitboards[BlackKnight] |= (ONE << bbIndex);
        break;
      case PieceType::BlackBishop:
        mPieces[index] = Piece::Bishop;
        mColors[index] = Color::Black;
        mBitboards[BlackBishop] |= (ONE << bbIndex);
        break;
      case PieceType::BlackQueen:
        mPieces[index] = Piece::Queen;
        mColors[index] = Color::Black;
        mBitboards[BlackQueen] |= (ONE << bbIndex);
        break;
      case PieceType::BlackKing:
        mPieces[index] = Piece::King;
        mColors[index] = Color::Black;
        mBitboards[BlackKing] |= (ONE << bbIndex);
        break;
      case PieceType::None:
        mPieces[index] = Piece::None;
        mColors[index] = Color::None;
        break;
      }
    }
  }

  updateAggregateBitBoards();

  return true;
}

bool BitBoard::doUnmakeMove(const Move * move)
{
  Color sideToMove = this->getSideToMove();
  Color otherSide = (sideToMove == Color::White) ? Color::Black : Color::White;

  uint8_t sourceRow = move->getSourceRow();
  uint8_t sourceCol = move->getSourceColumn();
  uint8_t destRow = move->getDestinationRow();
  uint8_t destCol = move->getDestinationColumn();
  uint8_t sourceSquare = getIndex(sourceRow, sourceCol);
  uint8_t destinationSquare = getIndex(destRow, destCol);
  uint8_t bbSource = getBitboardIndex(sourceRow, sourceCol);
  uint8_t bbDest = getBitboardIndex(destRow, destCol);

  Piece movePiece = move->getPiece();
  BitBoardPiece bbMovePiece = translatePiece(movePiece, otherSide);

  // Update for a quiet move, including double pawn pushes
  if (move->isQuiet() || move->isDoublePush())
  {
    mPieces[sourceSquare] = movePiece;
    mColors[sourceSquare] = otherSide;
    mPieces[destinationSquare] = Piece::None;
    mColors[destinationSquare] = Color::None;

    // 1. Remove moving piece from destination square
    // 2. Add moving piece to source square
    mBitboards[bbMovePiece] ^= (ONE << bbDest);
    mBitboards[bbMovePiece] |= (ONE << bbSource);
  }

  // Update for promotion move
  if (move->isPromotion())
  {
    Piece promotionPiece = move->getPromotedPiece();
    BitBoardPiece bbPromotionPiece = translatePiece(promotionPiece, otherSide);

    mPieces[sourceSquare] = movePiece;
    mColors[sourceSquare] = otherSide;
    mPieces[destinationSquare] = Piece::None;
    mColors[destinationSquare] = Color::None;

    // 1. Remove promotion piece from destination square
    // 2. Add moving piece to source square
    mBitboards[bbPromotionPiece] ^= (ONE << bbDest);
    mBitboards[bbMovePiece] |= (ONE << bbSource);
  }

  // Update for capture move
  if (move->isCapture())
  {
    Piece capturePiece = move->getCapturedPiece();
    BitBoardPiece bbCapturePiece = translatePiece(capturePiece, sideToMove);

    if (move->isPromotionCapture())
    {
      Piece promotionPiece = move->getPromotedPiece();
      BitBoardPiece bbPromotionPiece = translatePiece(promotionPiece, otherSide);

      mPieces[sourceSquare] = movePiece;
      mColors[sourceSquare] = otherSide;
      mPieces[destinationSquare] = capturePiece;
      mColors[destinationSquare] = sideToMove;

      // 1. Remove promoted piece from destination square
      // 2. Add moving piece to source square
      // 3. Add captured piece to destination square
      mBitboards[bbPromotionPiece] ^= (ONE << bbDest);
      mBitboards[bbMovePiece] |= (ONE << bbSource);
      mBitboards[bbCapturePiece] |= (ONE << bbDest);
    }
    else if (move->isEnPassantCapture())
    {
      int8_t epColumn = getEnpassantColumn();
      uint8_t captureSquare = getIndex(sourceRow, epColumn);
      uint8_t bbCaptureSquare = getBitboardIndex(sourceRow, epColumn);

      mPieces[destinationSquare] = Piece::None;
      mColors[destinationSquare] = Color::None;
      mPieces[captureSquare] = capturePiece;
      mColors[captureSquare] = sideToMove;
      mPieces[sourceSquare] = movePiece;
      mColors[sourceSquare] = otherSide;

      // 1. Remove moving piece from destination square
      // 2. Add moving piece to source square
      // 3. Add captured piece to en-passant capture square
      mBitboards[bbMovePiece] ^= (ONE << bbDest);
      mBitboards[bbMovePiece] |= (ONE << bbSource);
      mBitboards[bbCapturePiece] |= (ONE << bbCaptureSquare);
    }
    else
    {
      mPieces[sourceSquare] = movePiece;
      mColors[sourceSquare] = otherSide;
      mPieces[destinationSquare] = capturePiece;
      mColors[destinationSquare] = sideToMove;

      // 1. Remove moving piece from destination square
      // 2. Add moving piece to source square
      // 3. Add captured piece to destination square
      mBitboards[bbMovePiece] ^= (ONE << bbDest);
      mBitboards[bbMovePiece] |= (ONE << bbSource);
      mBitboards[bbCapturePiece] |= (ONE << bbDest);
    }
  }

  // Update for castle move
  if (move->isCastle())
  {
    if (destinationSquare == G8)
    {
      mPieces[H8] = Piece::Rook;
      mColors[H8] = Color::Black;
      mPieces[F8] = Piece::None;
      mColors[F8] = Color::None;
      mPieces[E8] = Piece::King;
      mColors[E8] = Color::Black;
      mPieces[G8] = Piece::None;
      mColors[G8] = Color::None;

      // 1. Remove the black rook from F8
      // 2. Add the black rook to H8
      // 3. Remove the black king from G8
      // 4. Add the black king to E8
      mBitboards[BlackRook] ^= (ONE << BB_F8);
      mBitboards[BlackRook] |= (ONE << BB_H8);
      mBitboards[BlackKing] ^= (ONE << BB_G8);
      mBitboards[BlackKing] |= (ONE << BB_E8);
    }
    else if (destinationSquare == C8)
    {
      mPieces[A8] = Piece::Rook;
      mColors[A8] = Color::Black;
      mPieces[D8] = Piece::None;
      mColors[D8] = Color::None;
      mPieces[E8] = Piece::King;
      mColors[E8] = Color::Black;
      mPieces[C8] = Piece::None;
      mColors[C8] = Color::None;

      // 1. Remove the black rook from D8
      // 2. Add the black rook to A8
      // 3. Remove the black king from C8
      // 4. Add the black king to E8
      mBitboards[BlackRook] ^= (ONE << BB_D8);
      mBitboards[BlackRook] |= (ONE << BB_A8);
      mBitboards[BlackKing] ^= (ONE << BB_C8);
      mBitboards[BlackKing] |= (ONE << BB_E8);
    }
    else if (destinationSquare == G1)
    {
      mPieces[H1] = Piece::Rook;
      mColors[H1] = Color::White;
      mPieces[F1] = Piece::None;
      mColors[F1] = Color::None;
      mPieces[E1] = Piece::King;
      mColors[E1] = Color::White;
      mPieces[G1] = Piece::None;
      mColors[G1] = Color::None;

      // 1. Remove the white rook from F1
      // 2. Add the white rook to H1
      // 3. Remove the white king from G1
      // 4. Add the white king to E1
      mBitboards[WhiteRook] ^= (ONE << BB_F1);
      mBitboards[WhiteRook] |= (ONE << BB_H1);
      mBitboards[WhiteKing] ^= (ONE << BB_G1);
      mBitboards[WhiteKing] |= (ONE << BB_E1);
    }
    else if (destinationSquare == C1)
    {
      mPieces[A1] = Piece::Rook;
      mColors[A1] = Color::White;
      mPieces[D1] = Piece::None;
      mColors[D1] = Color::None;
      mPieces[E1] = Piece::King;
      mColors[E1] = Color::White;
      mPieces[C1] = Piece::None;
      mColors[C1] = Color::None;

      // 1. Remove the white rook from D1
      // 2. Add the white rook to A1
      // 3. Remove the white king from C1
      // 4. Add the white king to E1
      mBitboards[WhiteRook] ^= (ONE << BB_D1);
      mBitboards[WhiteRook] |= (ONE << BB_A1);
      mBitboards[WhiteKing] ^= (ONE << BB_C1);
      mBitboards[WhiteKing] |= (ONE << BB_E1);
    }
  }

  updateAggregateBitBoards();

  return true;
}

void BitBoard::extractMoves(uint64_t bitboard, MoveList & moveList) const
{

}

void BitBoard::generateBishopAttacks(uint64_t bishops, uint64_t friendly, uint64_t enemy, Piece piece, MoveList & moveList) const
{
  while (bishops)
  {
    uint64_t moveBitboard = 0;
    uint8_t fromIndex = bitScanForward(bishops);
    uint64_t bbIndex = (ONE << fromIndex);

    while (true)
    {
      bbIndex >>= 9;
      if (!bbIndex || bbIndex & FILE_A || bbIndex & friendly)
      {
        break;
      }
      moveBitboard |= bbIndex;

      if (bbIndex & enemy)
      {
        break;
      }
    }

    bbIndex = (ONE << fromIndex);
    while (true)
    {
      bbIndex >>= 7;
      if (!bbIndex || bbIndex & FILE_H || bbIndex & friendly)
      {
        break;
      }
      moveBitboard |= bbIndex;

      if (bbIndex & enemy)
      {
        break;
      }
    }

    bbIndex = (ONE << fromIndex);
    while (true)
    {
      bbIndex <<= 9;
      if (!bbIndex || bbIndex & FILE_H || bbIndex & friendly)
      {
        break;
      }
      moveBitboard |= bbIndex;

      if (bbIndex & enemy)
      {
        break;
      }
    }

    bbIndex = (ONE << fromIndex);
    while (true)
    {
      bbIndex <<= 7;
      if (!bbIndex || bbIndex & FILE_A || bbIndex & friendly)
      {
        break;
      }
      moveBitboard |= bbIndex;

      if (bbIndex & enemy)
      {
        break;
      }
    }

    uint64_t captureBitboard = moveBitboard & enemy;
    moveBitboard &= ~captureBitboard;

    while (moveBitboard)
    {
      uint64_t toIndex = bitScanForward(moveBitboard);
      pushMove(fromIndex, toIndex, piece, Piece::None, Piece::None, Move::Type::Quiet, moveList);
      moveBitboard &= moveBitboard-1;
    }

    while (captureBitboard)
    {
      uint64_t toIndex = bitScanForward(captureBitboard);
      uint8_t row = getRow(toIndex);
      uint8_t col = getCol(toIndex);
      uint8_t bbIndex = getBitboardIndex(row, col);
      pushMove(fromIndex, toIndex, piece, mPieces[bbIndex], Piece::None, Move::Type::Capture, moveList);
      captureBitboard &= captureBitboard-1;
    }

    bishops &= bishops-1;
  }
}

void BitBoard::generateCastlingMoves(MoveList & moveList) const
{
  Color sideToMove = this->getSideToMove();
  uint8_t castlingRights = this->getCastlingRights();

  if (castlingRights == CASTLE_NONE)
    return;

  if (sideToMove == Color::White)
  {
    bool b1Empty = mPieces[B1] == Piece::None;
    bool c1Empty = mPieces[C1] == Piece::None;
    bool d1Empty = mPieces[D1] == Piece::None;
    bool f1Empty = mPieces[F1] == Piece::None;
    bool g1Empty = mPieces[G1] == Piece::None;

    if (castlingRights & CASTLE_WHITE_KING)
    {
      bool empty = f1Empty && g1Empty;
      if (empty && !isCellAttacked(E1, Color::Black) && !isCellAttacked(F1, Color::Black) && !isCellAttacked(G1, Color::Black))
      {
        pushMove(BB_E1, BB_G1, Piece::King, Piece::None, Piece::None, Move::Type::Castle, moveList);
      }
    }

    if (castlingRights & CASTLE_WHITE_QUEEN)
    {
      bool empty = b1Empty && c1Empty && d1Empty;
      if (empty && !isCellAttacked(E1, Color::Black) && !isCellAttacked(D1, Color::Black) && !isCellAttacked(C1, Color::Black))
      {
        pushMove(BB_E1, BB_C1, Piece::King, Piece::None, Piece::None, Move::Type::Castle, moveList);
      }
    }
  }
  else
  {
    bool b8Empty = mPieces[B8] == Piece::None;
    bool c8Empty = mPieces[C8] == Piece::None;
    bool d8Empty = mPieces[D8] == Piece::None;
    bool f8Empty = mPieces[F8] == Piece::None;
    bool g8Empty = mPieces[G8] == Piece::None;

    if (castlingRights & CASTLE_BLACK_KING)
    {
      bool empty = f8Empty && g8Empty;
      if (empty && !isCellAttacked(E8, Color::White) && !isCellAttacked(F8, Color::White) && !isCellAttacked(G8, Color::White))
      {
        pushMove(BB_E8, BB_G8, Piece::King, Piece::None, Piece::None, Move::Type::Castle, moveList);
      }
    }

    if (castlingRights & CASTLE_BLACK_QUEEN)
    {
      bool empty = b8Empty && c8Empty && d8Empty;
      if (empty && !isCellAttacked(E8, Color::White) && !isCellAttacked(C8, Color::White) && !isCellAttacked(D8, Color::White))
      {
        pushMove(BB_E8, BB_C8, Piece::King, Piece::None, Piece::None, Move::Type::Castle, moveList);
      }
    }
  }
}

void BitBoard::generateEvasiveMoves(uint8_t index, MoveList & moveList) const
{

}

void BitBoard::generateLeapMoves(uint64_t pieceBitBoard, Piece piece, const uint64_t * moves, uint64_t friendly, uint64_t enemy, MoveList & moveList) const
{
  while (pieceBitBoard)
  {
    uint8_t fromIndex = bitScanForward(pieceBitBoard);
    uint64_t attacks = moves[fromIndex];
    uint64_t moveBitboard = attacks & ~friendly;
    uint64_t captureBitboard = moveBitboard & enemy;
    moveBitboard &= ~captureBitboard;

    while (moveBitboard)
    {
      uint8_t toIndex = bitScanForward(moveBitboard);
      uint8_t row = getRow(toIndex);
      uint8_t col = getCol(toIndex);
      uint8_t bbIndex = getBitboardIndex(row, col);
      pushMove(fromIndex, toIndex, piece, mPieces[bbIndex], Piece::None, Move::Type::Quiet, moveList);
      moveBitboard &= moveBitboard-1;
    }

    while (captureBitboard)
    {
      uint8_t toIndex = bitScanForward(captureBitboard);
      uint8_t row = getRow(toIndex);
      uint8_t col = getCol(toIndex);
      uint8_t bbIndex = getBitboardIndex(row, col);
      pushMove(fromIndex, toIndex, piece, mPieces[bbIndex], Piece::None, Move::Type::Capture, moveList);
      captureBitboard &= captureBitboard-1;
    }

    pieceBitBoard &= pieceBitBoard-1;
  }
}

// void BitBoard::generateKnightMoves(uint8_t index, uint64_t friendlyPieces, MoveList & moveList) const
// {
//   uint64_t moveBitboard = mKnightMoves[index];
//   moveBitboard &= ~friendlyPieces;
//   writeBitBoard(moveBitboard, std::cout);

//   //std::bitset<64> bitset{moveBitboard};
//   //uint8_t destinationSquares[64];
//   //uint8_t * ptr = destinationSquares;
//   //std::copy(bitset.)

//   while (moveBitboard)
//   {
//     uint8_t toSq = bitScanForward(moveBitboard);
//     Move::Type type = (mPieces[toSq] == Piece::None) ? Move::Type::Quiet : Move::Type::Capture;
//     pushMove(index, toSq, mPieces[index], mPieces[toSq], Piece::None, type, moveList);
//     moveBitboard &= moveBitboard-1;
//   }
// }

// void BitBoard::generateMoves(uint8_t index, bool includeCastling, MoveList & moveList) const
// {
//   uint8_t row = getRow(index);
//   uint8_t col = getCol(index);
//   uint8_t bbIndex = getBitboardIndex(row, col);
//   uint64_t friendlyPieces = (this->getSideToMove() == Color::White) ? mWhitePieceBitboard : mBlackPieceBitboard;

//   if (includeCastling)
//   {
//     generateCastlingMoves(moveList);
//   }

//   Color color = mColors[index];
//   if (color == this->getSideToMove())
//   {
//     Piece piece = mPieces[index];
//     switch (piece)
//     {
//     case Piece::Pawn:
//       break;
//     case Piece::Rook:
//       break;
//     case Piece::Bishop:
//       break;
//     case Piece::Knight:
//       //generateKnightMoves(index, friendlyPieces, moveList);
//       break;
//     case Piece::Queen:
//       break;
//     case Piece::King:
//       break;
//     default:
//       break;
//     }
//   }
// }

void BitBoard::generatePawnAttacks(uint64_t pawns, const uint64_t * pawnAttacks, uint64_t enemy, uint64_t promoRank, MoveList & moveList) const
{
  while (pawns)
  {
    uint8_t fromIndex = bitScanForward(pawns);
    uint64_t attacks = pawnAttacks[fromIndex];
    uint64_t moveBitboard = attacks & enemy;
    uint64_t promoBitboard = moveBitboard & promoRank;
    moveBitboard &= ~promoBitboard;

    while (moveBitboard)
    {
      uint8_t toIndex = bitScanForward(moveBitboard);
      uint8_t row = getRow(toIndex);
      uint8_t col = getCol(toIndex);
      uint8_t bbIndex = getBitboardIndex(row, col);

      pushMove(fromIndex, toIndex, Piece::Pawn, mPieces[bbIndex], Piece::None, Move::Type::Capture, moveList);
      moveBitboard &= moveBitboard-1;
    }

    while (promoBitboard)
    {
      uint8_t toIndex = bitScanForward(promoBitboard);
      uint8_t row = getRow(toIndex);
      uint8_t col = getCol(toIndex);
      uint8_t bbIndex = getBitboardIndex(row, col);

      pushMove(fromIndex, toIndex, Piece::Pawn, mPieces[bbIndex], Piece::Queen, Move::Type::PromotionCapture, moveList);
      pushMove(fromIndex, toIndex, Piece::Pawn, mPieces[bbIndex], Piece::Rook, Move::Type::PromotionCapture, moveList);
      pushMove(fromIndex, toIndex, Piece::Pawn, mPieces[bbIndex], Piece::Bishop, Move::Type::PromotionCapture, moveList);
      pushMove(fromIndex, toIndex, Piece::Pawn, mPieces[bbIndex], Piece::Knight, Move::Type::PromotionCapture, moveList);
      promoBitboard &= promoBitboard-1;
    }

    pawns &= pawns-1;
  }
}

void BitBoard::generatePawnPushesBlack(uint64_t pawns, uint64_t blockers, MoveList & moveList) const
{
  uint64_t doubleEmpty = ~(blockers << 8) & ~(blockers << 16);
  uint64_t unmovedPawns = (pawns & RANK_7);
  uint64_t pawnPushes = (pawns & (~blockers << 8)) >> 8;
  uint64_t doublePawnPushes = (unmovedPawns & doubleEmpty) >> 16;
  uint64_t pawnPromotions = pawnPushes & RANK_1;
  pawnPushes &= ~pawnPromotions;

  //writeBitBoard((blockers << 8), std::cout);
  //writeBitBoard((blockers << 16), std::cout);
  //writeBitBoard((unmovedPawns >> 16), std::cout);
  //writeBitBoard(unmovedPawns, std::cout);
  //writeBitBoard(pawnPushes, std::cout);
  //writeBitBoard(doublePawnPushes, std::cout);

  while (pawnPushes)
  {
    uint8_t toSq = bitScanForward(pawnPushes);
    uint8_t fromSq = toSq + 8;
    pushMove(fromSq, toSq, Piece::Pawn, Piece::None, Piece::None, Move::Type::Quiet, moveList);
    pawnPushes &= pawnPushes-1;
  }

  while (doublePawnPushes)
  {
    uint8_t toSq = bitScanForward(doublePawnPushes);
    uint8_t fromSq = toSq + 16;
    pushMove(fromSq, toSq, Piece::Pawn, Piece::None, Piece::None, Move::Type::DoublePush, moveList);
    doublePawnPushes &= doublePawnPushes-1;
  }

  while (pawnPromotions)
  {
    uint8_t toSq = bitScanForward(pawnPromotions);
    uint8_t fromSq = toSq + 8;
    pushMove(fromSq, toSq, Piece::Pawn, Piece::None, Piece::Queen, Move::Type::Promotion, moveList);
    pushMove(fromSq, toSq, Piece::Pawn, Piece::None, Piece::Rook, Move::Type::Promotion, moveList);
    pushMove(fromSq, toSq, Piece::Pawn, Piece::None, Piece::Bishop, Move::Type::Promotion, moveList);
    pushMove(fromSq, toSq, Piece::Pawn, Piece::None, Piece::Knight, Move::Type::Promotion, moveList);
    pawnPromotions &= pawnPromotions-1;
  }

  //writeBitBoard(unmovedPawns, std::cout);
  //writeBitBoard(pawnPushes, std::cout);
  //writeBitBoard(doublePawnPushes, std::cout);
  //writeBitBoard(pawnPromotions, std::cout);
}

void BitBoard::generatePawnPushesWhite(uint64_t pawns, uint64_t blockers, MoveList & moveList) const
{
  uint64_t doubleEmpty = ~(blockers >> 8) & ~(blockers >> 16);
  uint64_t unmovedPawns = (pawns & RANK_2);
  uint64_t pawnPushes = (pawns & (~blockers >> 8)) << 8;
  uint64_t doublePawnPushes = (unmovedPawns & doubleEmpty) << 16;
  uint64_t pawnPromotions = pawnPushes & RANK_8;
  pawnPushes &= ~pawnPromotions;

  //writeBitBoard(pawnPushes, std::cout);

  while (pawnPushes)
  {
    uint8_t toSq = bitScanForward(pawnPushes);
    uint8_t fromSq = toSq - 8;
    //std::cout << static_cast<int>(fromSq) << " " << static_cast<int>(toSq) << std::endl;
    pushMove(fromSq, toSq, Piece::Pawn, Piece::None, Piece::None, Move::Type::Quiet, moveList);
    pawnPushes &= pawnPushes-1;
  }

  while (doublePawnPushes)
  {
    uint8_t toSq = bitScanForward(doublePawnPushes);
    uint8_t fromSq = toSq - 16;
    pushMove(fromSq, toSq, Piece::Pawn, Piece::None, Piece::None, Move::Type::DoublePush, moveList);
    doublePawnPushes &= doublePawnPushes-1;
  }

  while (pawnPromotions)
  {
    uint8_t toSq = bitScanForward(pawnPromotions);
    uint8_t fromSq = toSq - 8;
    pushMove(fromSq, toSq, Piece::Pawn, Piece::None, Piece::Queen, Move::Type::Promotion, moveList);
    pushMove(fromSq, toSq, Piece::Pawn, Piece::None, Piece::Rook, Move::Type::Promotion, moveList);
    pushMove(fromSq, toSq, Piece::Pawn, Piece::None, Piece::Bishop, Move::Type::Promotion, moveList);
    pushMove(fromSq, toSq, Piece::Pawn, Piece::None, Piece::Knight, Move::Type::Promotion, moveList);
    pawnPromotions &= pawnPromotions-1;
  }

  //writeBitBoard(unmovedPawns, std::cout);
  //writeBitBoard(pawnPushes, std::cout);
  //writeBitBoard(doublePawnPushes, std::cout);
  //writeBitBoard(pawnPromotions, std::cout);
}

void BitBoard::generateRookAttacks(uint64_t rooks, uint64_t friendly, uint64_t enemy, Piece piece, MoveList & moveList) const
{
  while (rooks)
  {
    uint64_t moveBitboard = 0;
    uint8_t fromIndex = bitScanForward(rooks);
    uint64_t bbIndex = (ONE << fromIndex);

    while (true)
    {
      bbIndex >>= 1;
      if (!bbIndex || bbIndex & FILE_A || bbIndex & friendly)
      {
        break;
      }
      moveBitboard |= bbIndex;

      if (bbIndex & enemy)
      {
        break;
      }
    }

    bbIndex = (ONE << fromIndex);
    while (true)
    {
      bbIndex >>= 8;
      if (!bbIndex || bbIndex & friendly)
      {
        break;
      }
      moveBitboard |= bbIndex;

      if (bbIndex & enemy)
      {
        break;
      }
    }

    bbIndex = (ONE << fromIndex);
    while (true)
    {
      bbIndex <<= 1;
      if (!bbIndex || bbIndex & FILE_H || bbIndex & friendly)
      {
        break;
      }
      moveBitboard |= bbIndex;

      if (bbIndex & enemy)
      {
        break;
      }
    }

    bbIndex = (ONE << fromIndex);
    while (true)
    {
      bbIndex <<= 8;
      if (!bbIndex || bbIndex & friendly)
      {
        break;
      }
      moveBitboard |= bbIndex;

      if (bbIndex & enemy)
      {
        break;
      }
    }

    uint64_t captureBitboard = moveBitboard & enemy;
    moveBitboard &= ~captureBitboard;

    while (moveBitboard)
    {
      uint64_t toIndex = bitScanForward(moveBitboard);
      pushMove(fromIndex, toIndex, piece, Piece::None, Piece::None, Move::Type::Quiet, moveList);
      moveBitboard &= moveBitboard-1;
    }

    while (captureBitboard)
    {
      uint64_t toIndex = bitScanForward(captureBitboard);
      uint8_t row = getRow(toIndex);
      uint8_t col = getCol(toIndex);
      uint8_t bbIndex = getBitboardIndex(row, col);
      pushMove(fromIndex, toIndex, piece, mPieces[bbIndex], Piece::None, Move::Type::Capture, moveList);
      captureBitboard &= captureBitboard-1;
    }

    rooks &= rooks-1;
  }
}

void BitBoard::init()
{
  for (uint8_t i = 0; i < 64; i++)
  {
    mMask[i] = (ONE << i);
  }

  mPieceToType[WhitePawn] = PieceType::WhitePawn;
  mPieceToType[WhiteRook] = PieceType::WhiteRook;
  mPieceToType[WhiteKnight] = PieceType::WhiteKnight;
  mPieceToType[WhiteBishop] = PieceType::WhiteBishop;
  mPieceToType[WhiteQueen] = PieceType::WhiteQueen;
  mPieceToType[WhiteKing] = PieceType::WhiteKing;
  mPieceToType[BlackPawn] = PieceType::BlackPawn;
  mPieceToType[BlackRook] = PieceType::BlackRook;
  mPieceToType[BlackKnight] = PieceType::BlackKnight;
  mPieceToType[BlackBishop] = PieceType::BlackBishop;
  mPieceToType[BlackQueen] = PieceType::BlackQueen;
  mPieceToType[BlackKing] = PieceType::BlackKing;
  mPieceToType[None] = PieceType::None;

  initKnightMoves();
  initKingMoves();
  initPawnAttacks();
  initRookAttacks();
  initBishopAttacks();
}

void BitBoard::initBoard()
{
  for (uint64_t i = 0; i < 64; i++)
  {
    mPieces[i] = Piece::None;
    mColors[i] = Color::None;
  }

  mPieces[A1] = mPieces[H1] = mPieces[A8] = mPieces[H8] = Piece::Rook;
  mPieces[B1] = mPieces[G1] = mPieces[B8] = mPieces[G8] = Piece::Knight;
  mPieces[C1] = mPieces[F1] = mPieces[C8] = mPieces[F8] = Piece::Bishop;
  mPieces[D1] = mPieces[D8] = Piece::Queen;
  mPieces[E1] = mPieces[E8] = Piece::King;

  for (uint8_t i = A1; i <= H2; i++)
  {
    mColors[i] = Color::White;
  }

  for (uint8_t i = A2; i <= H2; i++)
  {
    mPieces[i] = Piece::Pawn;
  }

  for (uint8_t i = A7; i <= H8; i++)
  {
    mColors[i] = Color::Black;
  }

  for (uint8_t i = A7; i <= H7; i++)
  {
    mPieces[i] = Piece::Pawn;
  }

  for (int i = 0; i < 12; i++)
  {
    mBitboards[i] = ZERO;
  }

  mBitboards[WhiteRook] = ( (ONE << BB_H1) | (ONE << BB_A1));
  mBitboards[WhiteKnight] = ( (ONE << BB_G1) | (ONE << BB_B1));
  mBitboards[WhiteBishop] = ( (ONE << BB_F1) | (ONE << BB_C1));
  mBitboards[WhiteQueen] = (ONE << BB_D1);
  mBitboards[WhiteKing] = (ONE << BB_E1);
  for (int8_t i = BB_H2; i <= BB_A2; i++)
  {
    mBitboards[WhitePawn] |= (ONE << i);
  }

  mBitboards[BlackRook] = ( (ONE << BB_H8) | (ONE << BB_A8));
  mBitboards[BlackKnight] = ( (ONE << BB_G8) | (ONE << BB_B8));
  mBitboards[BlackBishop] = ( (ONE << BB_F8) | (ONE << BB_C8));
  mBitboards[BlackQueen] = (ONE << BB_D8);
  mBitboards[BlackKing] = (ONE << BB_E8);
  for (int8_t i = BB_H7; i <= BB_A7; i++)
  {
    mBitboards[BlackPawn] |= (ONE << i);
  }

  updateAggregateBitBoards();
}

void BitBoard::initBishopAttacks()
{
  for (int8_t i = 0; i < 8; i++)
  {
    for (int8_t j = 0; j < 8; j++)
    {
      uint64_t moveBitboard = 0;
      uint8_t index = getIndex(i, j);
      uint64_t bbIndex = (ONE << index);

      while (true)
      {
        bbIndex >>= 9;
        if (!bbIndex || bbIndex & FILE_A)
        {
          break;
        }
        moveBitboard |= bbIndex;
      }

      bbIndex = (ONE << index);
      while (true)
      {
        bbIndex >>= 7;
        if (!bbIndex || bbIndex & FILE_H)
        {
          break;
        }
        moveBitboard |= bbIndex;
      }

      bbIndex = (ONE << index);
      while (true)
      {
        bbIndex <<= 9;
        if (!bbIndex || bbIndex & FILE_H)
        {
          break;
        }
        moveBitboard |= bbIndex;
      }

      bbIndex = (ONE << index);
      while (true)
      {
        bbIndex <<= 7;
        if (!bbIndex || bbIndex & FILE_A)
        {
          break;
        }
        moveBitboard |= bbIndex;
      }

      mBishopAttacks[index] = moveBitboard;

      //writeBitBoard(mBishopAttacks[index], std::cout);
    }
  }
}

void BitBoard::initKingMoves()
{
  // Precompute attacks for each square
  for (uint8_t row = 0; row < 8; row++)
  {
    for (uint8_t col = 0; col < 8; col++)
    {
      uint64_t moveBitboard = 0;
      uint8_t index = getBitboardIndex(row, col);
      uint64_t bbIndex = (ONE << index);

      moveBitboard |= (bbIndex << 1) & ~FILE_H;
      moveBitboard |= (bbIndex << 7) & ~FILE_A;
      moveBitboard |= (bbIndex << 8);
      moveBitboard |= (bbIndex << 9) & ~FILE_H;
      moveBitboard |= (bbIndex >> 1) & ~FILE_A;
      moveBitboard |= (bbIndex >> 7) & ~FILE_H;
      moveBitboard |= (bbIndex >> 8);
      moveBitboard |= (bbIndex >> 9) & ~FILE_A;

      mKingMoves[index] = moveBitboard;
    }
  }
}

void BitBoard::initKnightMoves()
{
  for (uint8_t row = 0; row < 8; row++)
  {
    for (uint8_t col = 0; col < 8; col++)
    {
      uint64_t moveBitboard = 0;
      uint8_t index = getBitboardIndex(row, col);
      uint64_t bbIndex = (ONE << index);

      moveBitboard |= (bbIndex << 6)  & ~FILE_AB;
      moveBitboard |= (bbIndex >> 10) & ~FILE_AB;
      moveBitboard |= (bbIndex << 15) & ~FILE_AB;
      moveBitboard |= (bbIndex >> 17) & ~FILE_AB;
      moveBitboard |= (bbIndex << 10) & ~FILE_GH;
      moveBitboard |= (bbIndex << 17) & ~FILE_GH;
      moveBitboard |= (bbIndex >> 6)  & ~FILE_GH;
      moveBitboard |= (bbIndex >> 15) & ~FILE_GH;

      mKnightMoves[index] = moveBitboard;
    }
  }

  //writeBitBoard(mKnightMoves[0], std::cout);
}

void BitBoard::initPawnAttacks()
{
  for (uint8_t row = 0; row < 8; row++)
  {
    for (uint8_t col = 0; col < 8; col++)
    {
      uint64_t whiteBitBoard = 0;
      uint64_t blackBitBoard = 0;
      uint8_t index = getBitboardIndex(row, col);
      uint64_t bbIndex = (ONE << index);

      whiteBitBoard |= (bbIndex << 7) & ~FILE_A;
      whiteBitBoard |= (bbIndex << 9) & ~FILE_H;

      blackBitBoard |= (bbIndex >> 7) & ~FILE_H;
      blackBitBoard |= (bbIndex >> 9) & ~FILE_A;

      mPawnAttacksWhite[index] = whiteBitBoard;
      mPawnAttacksBlack[index] = blackBitBoard;
    }
  }
}

void BitBoard::initRookAttacks()
{
  for (int8_t i = 0; i < 8; i++)
  {
    for (int8_t j = 0; j < 8; j++)
    {
      uint64_t moveBitboard = 0;
      uint8_t index = getIndex(i, j);
      uint64_t bbIndex = (ONE << index);

      while (true)
      {
        bbIndex >>= 1;
        if (!bbIndex || bbIndex & FILE_A)
        {
          break;
        }
        moveBitboard |= bbIndex;
      }

      bbIndex = (ONE << index);
      while (true)
      {
        bbIndex >>= 8;
        if (!bbIndex)
        {
          break;
        }
        moveBitboard |= bbIndex;
      }

      bbIndex = (ONE << index);
      while (true)
      {
        bbIndex <<= 1;
        if (!bbIndex || bbIndex & FILE_H)
        {
          break;
        }
        moveBitboard |= bbIndex;
      }

      bbIndex = (ONE << index);
      while (true)
      {
        bbIndex <<= 8;
        if (!bbIndex)
        {
          break;
        }
        moveBitboard |= bbIndex;
      }

      mRookAttacks[index] = moveBitboard;

      //writeBitBoard(mRookAttacks[index], std::cout);
    }
  }
}

bool BitBoard::isCellAttacked(uint8_t index, Color attackColor) const
{
  return false;
}

void BitBoard::pushMove(uint8_t from, uint8_t to, Piece piece, Piece capture, Piece promote, Move::Type type, MoveList & moveList) const
{
  uint8_t sourceRow = getRow(from);
  uint8_t sourceCol = 7-getCol(from);
  uint8_t destRow = getRow(to);
  uint8_t destCol = 7-getCol(to);

  Board::pushMove(sourceRow, sourceCol, destRow, destCol, piece, capture, promote, type, moveList);
}

BitBoard::BitBoardPiece BitBoard::translatePiece(Piece piece, Color color) const
{
  BitBoardPiece pieceType = None;

  if (color == Color::White)
  {
    switch (piece)
    {
    case Piece::Pawn:
      pieceType = WhitePawn;
      break;
    case Piece::Rook:
      pieceType = WhiteRook;
      break;
    case Piece::Knight:
      pieceType = WhiteKnight;
      break;
    case Piece::Bishop:
      pieceType = WhiteBishop;
      break;
    case Piece::Queen:
      pieceType = WhiteQueen;
      break;
    case Piece::King:
      pieceType = WhiteKing;
      break;
    default:
      break;
    }
  }
  else
  {
    switch (piece)
    {
    case Piece::Pawn:
      pieceType = BlackPawn;
      break;
    case Piece::Rook:
      pieceType = BlackRook;
      break;
    case Piece::Knight:
      pieceType = BlackKnight;
      break;
    case Piece::Bishop:
      pieceType = BlackBishop;
      break;
    case Piece::Queen:
      pieceType = BlackQueen;
      break;
    case Piece::King:
      pieceType = BlackKing;
      break;
    default:
      break;
    }
  }

  return pieceType;
}

void BitBoard::updateAggregateBitBoards()
{
  mWhitePieceBitboard = 0;
  mWhitePieceBitboard |= mBitboards[WhitePawn];
  mWhitePieceBitboard |= mBitboards[WhiteRook];
  mWhitePieceBitboard |= mBitboards[WhiteKnight];
  mWhitePieceBitboard |= mBitboards[WhiteBishop];
  mWhitePieceBitboard |= mBitboards[WhiteQueen];
  mWhitePieceBitboard |= mBitboards[WhiteKing];

  mBlackPieceBitboard = 0;
  mBlackPieceBitboard |= mBitboards[BlackPawn];
  mBlackPieceBitboard |= mBitboards[BlackRook];
  mBlackPieceBitboard |= mBitboards[BlackKnight];
  mBlackPieceBitboard |= mBitboards[BlackBishop];
  mBlackPieceBitboard |= mBitboards[BlackQueen];
  mBlackPieceBitboard |= mBitboards[BlackKing];

  mAllPieceBitBoard = mWhitePieceBitboard | mBlackPieceBitboard;
  mNoPieceBitboard = ~mAllPieceBitBoard;
}

void BitBoard::writeBitBoard(uint64_t bb, std::ostream & output) const
{
  static const char files[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};

  for (char row = 7; row >= 0; row--)
  {
    output << (row+1) << " ";
    for (char col = 7; col >= 0; col--)
    {
      int shift = getIndex(row, col);
      if ((bb >> shift) & 0x1)
        output << "X ";
      else
        output << ". ";
    }
    output << "\n";
  }

  output << "  ";
  for (uint8_t col = 0; col < 8; col++)
  {
    output << files[col] << " ";
  }
  output << "\n\n";
}

}

// #pragma intrinsic(_BitScanForward64)

// // Helper macros for bitboards
// #define C64(constantU64) constantU64##ULL
// #define ZERO C64(0)
// #define ONE C64(1)

// // Macros for mapping (row,col)->index and vice-versa
// #define getIndex(row,col) (((row)<<3)+(col))
// #define getRow(index) ( (index >> 3) )
// #define getCol(index) ( (index & 7) )

// // Mask for full rank
// static const int FullRank = 255;

// // Promotion masks
// static const uint64_t WHITE_PROMOTION_MASK = (C64(255) << 56);
// static const uint64_t BLACK_PROMOTION_MASK = (C64(255));

// // Pawn offsets
// static const char NORTH            = +8;
// static const char SOUTH            = -8;
// static const char WHITE_ATTACK_ROW = +1;
// static const char BLACK_ATTACK_ROW = -1;

// // Square definition
// enum Square
// {
//   A1, B1, C1, D1, E1, F1, G1, H1,
//   A2, B2, C2, D2, E2, F2, G2, H2,
//   A3, B3, C3, D3, E3, F3, G3, H3,
//   A4, B4, C4, D4, E4, F4, G4, H4,
//   A5, B5, C5, D5, E5, F5, G5, H5,
//   A6, B6, C6, D6, E6, F6, G6, H6,
//   A7, B7, C7, D7, E7, F7, G7, H7,
//   A8, B8, C8, D8, E8, F8, G8, H8
// };

// // Rotation matrices

// // Rotation matrix for 90 degree CW rotation
// static const uint32_t rotateR90Matrix[] =
// {
//   A8, A7, A6, A5, A4, A3, A2, A1,
//   B8, B7, B6, B5, B4, B3, B2, B1,
//   C8, C7, C6, C5, C4, C3, C2, C1,
//   D8, D7, D6, D5, D4, D3, D2, D1,
//   E8, E7, E6, E5, E4, E3, E2, E1,
//   F8, F7, F6, F5, F4, F3, F2, F1,
//   G8, G7, G6, G5, G4, G3, G2, G1,
//   H8, H7, H6, H5, H4, H3, H2, H1
// };

// // Rotation matrix for 90 degree CCW rotation
// static const uint32_t rotateL90Matrix[] =
// {
//   H1, H2, H3, H4, H5, H6, H7, H8,
//   G1, G2, G3, G4, G5, G6, G7, G8,
//   F1, F2, F3, F4, F5, F6, F7, F8,
//   E1, E2, E3, E4, E5, E6, E7, E8,
//   D1, D2, D3, D4, D5, D6, D7, D8,
//   C1, C2, C3, C4, C5, C6, C7, C8,
//   B1, B2, B3, B4, B5, B6, B7, B8,
//   A1, A2, A3, A4, A5, A6, A7, A8
// };

// // Shifts for ranks
// static const uint32_t rankShifts[64] =
// {
//   0, 0, 0, 0, 0, 0, 0, 0,
//   8, 8, 8, 8, 8, 8, 8, 8,
//   16, 16, 16, 16, 16, 16, 16, 16,
//   24, 24, 24, 24, 24, 24, 24, 24,
//   32, 32, 32, 32, 32, 32, 32, 32,
//   40, 40, 40, 40, 40, 40, 40, 40,
//   48, 48, 48, 48, 48, 48, 48, 48,
//   56, 56, 56, 56, 56, 56, 56, 56
// };

// // Shifts for files
// static const uint32_t fileShifts[64] =
// {
//   0, 8, 16, 24, 32, 40, 48, 56,
//   0, 8, 16, 24, 32, 40, 48, 56,
//   0, 8, 16, 24, 32, 40, 48, 56,
//   0, 8, 16, 24, 32, 40, 48, 56,
//   0, 8, 16, 24, 32, 40, 48, 56,
//   0, 8, 16, 24, 32, 40, 48, 56,
//   0, 8, 16, 24, 32, 40, 48, 56,
//   0, 8, 16, 24, 32, 40, 48, 56
// };

// // Rotation matrix for 45 degree CCW rotation
// // Formatted for ease of visualization
// static const uint32_t rotateL45Matrix[] =
// {
//   H8,                                   // 0
//   G8, H7,                               // 1
//   F8, G7, H6,                           // 3
//   E8, F7, G6, H5,                       // 6
//   D8, E7, F6, G5, H4,                   // 10
//   C8, D7, E6, F5, G4, H3,               // 15
//   B8, C7, D6, E5, F4, G3, H2,           // 21
//   A8, B7, C6, D5, E4, F3, G2, H1,       // 28
//   A7, B6, C5, D4, E3, F2, G1,           // 36
//   A6, B5, C4, D3, E2, F1,               // 43
//   A5, B4, C3, D2, E1,                   // 49
//   A4, B3, C2, D1,                       // 54
//   A3, B2, C1,                           // 58
//   A2, B1,                               // 61
//   A1                                    // 63
// };

// // TODO: Find a way to make this unnecessary
// // Rotation matrix for inverse 45 degree CCW rotation
// // Formatted for ease of visualization
// static const uint32_t invRotateL45Matrix[] =
// {
//   H8,                                   // 0
//   G8, E8,                               // 1
//   B8, F7, A7,                           // 3
//   C6, D5, F8, D8,                       // 6
//   A8, E7, H6, B6, C5,                   // 10
//   D4, C8, H7, D7, G6, A6,               // 15
//   B5, C4, E3, G7, C7, F6, H5,           // 21
//   A5, B4, D3, G2, B7, E6, G5, H4,       // 28
//   A4, C3, F2, B2, D6, F5, G4,           // 36
//   H3, B3, E2, A2, F1, E5,               // 43
//   F4, G3, A3, D2, H1,                   // 49
//   E1, C1, E4, F3,                       // 54
//   H2, C2, G1,                           // 58
//   D1, B1,                               // 61
//   A1                                    // 63
// };

// // Rotation matrix for 45 degree CW rotation
// // Formatted for ease of visualization
// static const uint32_t rotateR45Matrix[] =
// {
//   A8,                                   // 0
//   A7, B8,                               // 1
//   A6, B7, C8,                           // 3
//   A5, B6, C7, D8,                       // 6
//   A4, B5, C6, D7, E8,                   // 10
//   A3, B4, C5, D6, E7, F8,               // 15
//   A2, B3, C4, D5, E6, F7, G8,           // 21
//   A1, B2, C3, D4, E5, F6, G7, H8,       // 28
//   B1, C2, D3, E4, F5, G6, H7,           // 36
//   C1, D2, E3, F4, G5, H6,               // 43
//   D1, E2, F3, G4, H5,                   // 49
//   E1, F2, G3, H4,                       // 54
//   F1, G2, H3,                           // 58
//   G1, H2,                               // 61
//   H1                                    // 63
// };

// // TODO: Find a way to make this unnecessary
// // Rotation matrix for inverse 45 degree CW rotation
// // Formatted for ease of visualization
// static const uint32_t invRotateR45Matrix[] =
// {
//   E4,                                   // 0
//   E5, D6,                               // 1
//   B7, G7, C8,                           // 3
//   F8, H8, F3, F4,                       // 6
//   F5, E6, C7, H7, D8,                   // 10
//   G8, H2, G3, G4, G5, F6,               // 15
//   D7, A8, E8, C2, A3, H3, H4,           // 21
//   H5, G6, E7, B8, G1, D2, B3, A4,       // 28
//   A5, A6, H6, F7, D1, H1, E2,           // 36
//   C3, B4, B5, B6, A7, B1,               // 43
//   E1, A2, F2, D3, C4,                   // 49
//   C5, C6, A1, C1,                       // 54
//   F1, B2, G2,                           // 58
//   E3, D4,                               // 61
//   D5                                    // 63
// };

// // Lengths for north east diagonals
// static const uint32_t a1h8Lengths[64] =
// {
//   8, 7, 6, 5, 4, 3, 2, 1,
//   7, 8, 7, 6, 5, 4, 3, 2,
//   6, 7, 8, 7, 6, 5, 4, 3,
//   5, 6, 7, 8, 7, 6, 5, 4,
//   4, 5, 6, 7, 8, 7, 6, 5,
//   3, 4, 5, 6, 7, 8, 7, 6,
//   2, 3, 4, 5, 6, 7, 8, 7,
//   1, 2, 3, 4, 5, 6, 7, 8
// };

// // Masks for north east diagonals
// static const uint32_t a1h8Masks[64] =
// {
//   255, 127,  63,  31,  15,   7,   3,   1,
//   127, 255, 127,  63,  31,  15,   7,   3,
//    63, 127, 255, 127,  63,  31,  15,   7,
//    31,  63, 127, 255, 127,  63,  31,  15,
//    15,  31,  63, 127, 255, 127,  63,  31,
//     7,  15,  31,  63, 127, 255, 127,  63,
//     3,   7,  15,  31,  63, 127, 255, 127,
//     1,   3,   7,  15,  31,  63, 127, 255
// };

// // Shifts for north east diagonals
// static const uint32_t a1h8Shifts[64] =
// {
//   28, 36, 43, 49, 54, 58, 61, 63,
//   21, 28, 36, 43, 49, 54, 58, 61,
//   15, 21, 28, 36, 43, 49, 54, 58,
//   10, 15, 21, 28, 36, 43, 49, 54,
//    6, 10, 15, 21, 28, 36, 43, 49,
//    3,  6, 10, 15, 21, 28, 36, 43,
//    1,  3,  6, 10, 15, 21, 28, 36,
//    0,  1,  3,  6, 10, 15, 21, 28
// };

// // Lengths for north west diagonals
// static const uint32_t a8h1Lengths[64] =
// {
//   1, 2, 3, 4, 5, 6, 7, 8,
//   2, 3, 4, 5, 6, 7, 8, 7,
//   3, 4, 5, 6, 7, 8, 7, 6,
//   4, 5, 6, 7, 8, 7, 6, 5,
//   5, 6, 7, 8, 7, 6, 5, 4,
//   6, 7, 8, 7, 6, 5, 4, 3,
//   7, 8, 7, 6, 5, 4, 3, 2,
//   8, 7, 6, 5, 4, 3, 2, 1
// };

// // Masks for north west diagonals
// static const uint32_t a8h1Masks[64] =
// {
//     1,   3,   7,  15,  31,  63, 127, 255,
//     3,   7,  15,  31,  63, 127, 255, 127,
//     7,  15,  31,  63, 127, 255, 127,  63,
//    15,  31,  63, 127, 255, 127,  63,  31,
//    31,  63, 127, 255, 127,  63,  31,  15,
//    63, 127, 255, 127,  63,  31,  15,   7,
//   127, 255, 127,  63,  31,  15,   7,   3,
//   255, 127,  63,  31,  15,   7,   3,   1,
// };

// // Shifts for north west diagonals
// static const uint32_t a8h1Shifts[64] =
// {
//   63, 61, 58, 54, 49, 43, 36, 28,
//   61, 58, 54, 49, 43, 36, 28, 21,
//   58, 54, 49, 43, 36, 28, 21, 15,
//   54, 49, 43, 36, 28, 21, 15, 10,
//   49, 43, 36, 28, 21, 15, 10,  6,
//   43, 36, 28, 21, 15, 10,  6,  3,
//   36, 28, 21, 15, 10,  6,  3,  1,
//   28, 21, 15, 10,  6,  3,  1,  0
// };

// namespace jcl
// {

// // Constructor
// BitBoard::BitBoard()
// {
//   init();
//   initAttacks();
//   reset();
//   //testRotations();
// }

// BitBoard::~BitBoard()
// {

// }

// /**
//  * bitScanForward
//  * @author Martin Läuter (1997)
//  *         Charles E. Leiserson
//  *         Harald Prokop
//  *         Keith H. Randall
//  * "Using de Bruijn Sequences to Index a 1 in a Computer Word"
//  * @param bb bitboard to scan
//  * @precondition bb != 0
//  * @return index (0..63) of least significant one bit
//  */
// inline uint8_t BitBoard::bitScanForward(U64 bb) const
// {
//  static const uint8_t index64[64] =
//  {
//     63,  0, 58,  1, 59, 47, 53,  2,
//     60, 39, 48, 27, 54, 33, 42,  3,
//     61, 51, 37, 40, 49, 18, 28, 20,
//     55, 30, 34, 11, 43, 14, 22,  4,
//     62, 57, 46, 52, 38, 26, 32, 41,
//     50, 36, 17, 19, 29, 10, 13, 21,
//     56, 45, 25, 31, 35, 16,  9, 12,
//     44, 24, 15,  8, 23,  7,  6,  5
//  };

//   static const uint64_t debruijn64 = uint64_t(0x07EDD5E59A4E28C2);
//   assert (bb != 0);
//   return index64[((bb & -bb) * debruijn64) >> 58];

//   //uint32_t index = 0;
//   //_BitScanForward64(&index, bb);
//   //return static_cast<uint8_t>(index);
// }

// void BitBoard::generateCastlingMoves(MoveList & moveList) const
// {
//   if (this->getSideToMove() == Color::White) {
//     if (getCastlingRights() & CASTLE_WHITE_KING) {
//       if (mType[Square::F1] == Type::None &&
//           mType[Square::G1] == Type::None &&
//           !isCellAttacked(Square::E1, Color::Black) &&
//           !isCellAttacked(Square::F1, Color::Black) &&
//           !isCellAttacked(Square::G1, Color::Black))
//       {
//         pushMove(E1, G1, Type::WhiteKing, Type::None, Type::None, Move::Type::Castle, moveList);
//       }
//     }
//     if (getCastlingRights() & CASTLE_WHITE_QUEEN) {
//       if (mType[Square::D1] == Type::None &&
//           mType[Square::C1] == Type::None &&
//           mType[Square::B1] == Type::None &&
//           !isCellAttacked(Square::E1, Color::Black) &&
//           !isCellAttacked(Square::D1, Color::Black) &&
//           !isCellAttacked(Square::C1, Color::Black))
//       {
//         pushMove(E1, C1, Type::WhiteKing, Type::None, Type::None, Move::Type::Castle, moveList);
//       }
//     }
//   }
//   else {
//     if (getCastlingRights() & CASTLE_BLACK_KING) {
//       if (mType[Square::F8] == Type::None &&
//           mType[Square::G8] == Type::None &&
//           !isCellAttacked(Square::E8, Color::White) &&
//           !isCellAttacked(Square::F8, Color::White) &&
//           !isCellAttacked(Square::G8, Color::White))
//       {
//         pushMove(E8, G8, Type::BlackKing, Type::None, Type::None, Move::Type::Castle, moveList);
//       }
//     }
//     if (getCastlingRights() & CASTLE_BLACK_QUEEN) {
//       if (mType[Square::D8] == Type::None &&
//           mType[Square::C8] == Type::None &&
//           mType[Square::B8] == Type::None &&
//           !isCellAttacked(Square::E8, Color::White) &&
//           !isCellAttacked(Square::D8, Color::White) &&
//           !isCellAttacked(Square::C8, Color::White))
//       {
//         pushMove(E8, C8, Type::BlackKing, Type::None, Type::None, Move::Type::Castle, moveList);
//       }
//     }
//   }
// }

// void BitBoard::generateJumpAttacks(U64 piece, const U64 * attacks, U64 friends, MoveList & moveList) const
// {
//   while (piece) {
//     uint8_t fromSq = bitScanForward(piece);
//     U64 attack = attacks[fromSq];
//     U64 move = attack & ~friends;
//     while (move) {
//       uint8_t toSq = bitScanForward(move);
//       Move::Type type = (mType[toSq] == Type::None) ? Move::Type::Quiet : Move::Type::Capture;
//       pushMove(fromSq, toSq, mType[fromSq], mType[toSq], Type::None, type, moveList);
//       move &= move-1;
//     }
//     piece &= piece-1;
//   }
// }

// bool BitBoard::generateMoves(MoveList & moveList) const
// {
//   // Get the bitboards for the pieces on the side to move
//   U64 pawns = mPiece[WhitePawn + mSide];
//   U64 knights = mPiece[WhiteKnight + mSide];
//   U64 kings = mPiece[WhiteKing + mSide];
//   U64 rooks = mPiece[WhiteRook + mSide];
//   U64 bishops = mPiece[WhiteBishop + mSide];
//   U64 queens = mPiece[WhiteQueen + mSide];
//   U64 friends = mPiece[mSide];
//   U64 enemies = ~friends & mPiece[All];

//   // Generate castling moves
//   generateCastlingMoves(moveList);

//   // Generate pawn moves
//   if (mSide == White) {
//     U64 pawnPushes = (mPiece[WhitePawn] << 8) & mPiece[None];
//     U64 unmovedPawns = (mPiece[WhitePawn] & (C64(255) << 8));
//     U64 pawnDoublePushes = ( (unmovedPawns & (~(mPiece[All] >> 8)) & (~(mPiece[All] >> 16)) ) ) << 16;
//     generatePawnMoves(SOUTH, pawnPushes, pawnDoublePushes, WHITE_PROMOTION_MASK, moveList);
//     generatePawnAttacks(pawns, mPawnAttacks[White], enemies, SOUTH, WHITE_PROMOTION_ROW, moveList);
//   }
//   else {
//     U64 pawnPushes = (mPiece[BlackPawn] >> 8) & mPiece[None];
//     U64 unmovedPawns = (mPiece[BlackPawn] & (C64(255) << 48));
//     U64 pawnDoublePushes = ( (unmovedPawns & ( ~(mPiece[All] << 8) ) & ( ~(mPiece[All] << 16) ) ) ) >> 16;
//     generatePawnMoves(NORTH, pawnPushes, pawnDoublePushes, BLACK_PROMOTION_MASK, moveList);
//     generatePawnAttacks(pawns, mPawnAttacks[Black], enemies, NORTH, BLACK_PROMOTION_ROW, moveList);
//   }

//   // Generate all other moves
//   generateJumpAttacks(knights, mKnightAttacks, friends, moveList);
//   generateJumpAttacks(kings, mKingAttacks, friends, moveList);
//   generateSlideAttacks(rooks, mRankAttacks, friends, mPiece[All], rankShifts, mRankMasks, moveList);
//   generateSlideAttacks(rooks, mFileAttacks, friends, mRotateR90, fileShifts, mFileMasks, moveList);
//   generateSlideAttacks(bishops, mNorthEastAttacks, friends, mRotateR45, a1h8Shifts, mNorthEastMasks, moveList);
//   generateSlideAttacks(bishops, mNorthWestAttacks, friends, mRotateL45, a8h1Shifts, mNorthWestMasks, moveList);
//   generateSlideAttacks(queens, mRankAttacks, friends, mPiece[All], rankShifts, mRankMasks, moveList);
//   generateSlideAttacks(queens, mFileAttacks, friends, mRotateR90, fileShifts, mFileMasks, moveList);
//   generateSlideAttacks(queens, mNorthEastAttacks, friends, mRotateR45, a1h8Shifts, mNorthEastMasks, moveList);
//   generateSlideAttacks(queens, mNorthWestAttacks, friends, mRotateL45, a8h1Shifts, mNorthWestMasks, moveList);
// }

// void BitBoard::generatePawnAttacks(U64 piece, const U64 *attacks, U64 enemies, char dir, uint8_t promotionRow, MoveList &moveList) const
// {
//   Type pawnType   = static_cast<Type>(WhitePawn   + mSide);
//   Type queenType  = static_cast<Type>(WhiteQueen  + mSide);
//   Type rookType   = static_cast<Type>(WhiteRook   + mSide);
//   Type bishopType = static_cast<Type>(WhiteBishop + mSide);
//   Type knightType = static_cast<Type>(WhiteKnight + mSide);

//   while (piece) {
//     uint8_t fromSq = bitScanForward(piece);
//     U64 attack = attacks[fromSq];
//     U64 move = attack & enemies;

//     while (move) {
//       uint8_t toSq = bitScanForward(move);
//       uint8_t destRow = getRow(toSq);
//       Type capturePiece = mType[toSq];

//       if (destRow == promotionRow) { // Promotion capture
//         pushMove(fromSq, toSq, pawnType, capturePiece, queenType, Move::Type::PromotionCapture, moveList);
//         pushMove(fromSq, toSq, pawnType, capturePiece, rookType, Move::Type::PromotionCapture, moveList);
//         pushMove(fromSq, toSq, pawnType, capturePiece, bishopType, Move::Type::PromotionCapture, moveList);
//         pushMove(fromSq, toSq, pawnType, capturePiece, knightType, Move::Type::PromotionCapture, moveList);
//       }
//       else { // Standard capture
//         pushMove(fromSq, toSq, pawnType, capturePiece, Type::None, Move::Type::Capture, moveList);
//       }
//       move &= move-1;
//     }

//     if (getEnpassantColumn() != Board::INVALID_ENPASSANT_COLUMN) { // En-passant capture
//       while (attack) {
//         uint8_t toSq = bitScanForward(attack);
//         uint8_t epIndex = getIndex(epRow(), epColumn());
//         if (epIndex == toSq) {
//           pushMove(fromSq, toSq, pawnType, mType[toSq+dir], Type::None, Move::Type::EpCapture, moveList);
//         }
//         attack &= attack-1;
//       }
//     }

//     piece &= piece-1;
//   }
// }

// void BitBoard::generatePawnMoves(char dir, U64 pawnPushes, U64 pawnDoublePushes, U64 promoMask, MoveList & moveList) const
// {
//   Type pawnType   = static_cast<Type>(WhitePawn   + mSide);
//   Type queenType  = static_cast<Type>(WhiteQueen  + mSide);
//   Type rookType   = static_cast<Type>(WhiteRook   + mSide);
//   Type bishopType = static_cast<Type>(WhiteBishop + mSide);
//   Type knightType = static_cast<Type>(WhiteKnight + mSide);

//   // Promotion moves
//   U64 promos = pawnPushes & promoMask;
//   while (promos) {
//     int toSq = bitScanForward(promos);
//     int fromSq = toSq + dir;
//     pushMove(fromSq, toSq, pawnType, Type::None, queenType, Move::Type::Promotion, moveList);
//     pushMove(fromSq, toSq, pawnType, Type::None, rookType, Move::Type::Promotion, moveList);
//     pushMove(fromSq, toSq, pawnType, Type::None, bishopType, Move::Type::Promotion, moveList);
//     pushMove(fromSq, toSq, pawnType, Type::None, knightType, Move::Type::Promotion, moveList);
//     promos &= promos - 1;
//   }

//   // Pawn pushes
//   U64 pushes = pawnPushes & ~promoMask;
//   while (pushes) {
//     int toSq = bitScanForward(pushes);
//     int fromSq = toSq + dir;
//     pushMove(fromSq, toSq, pawnType, Type::None, Type::None, Move::Type::Quiet, moveList);
//     pushes &= pushes - 1;
//   }

//   // Double pawn pushes
//   while (pawnDoublePushes) {
//     int toSq = bitScanForward(pawnDoublePushes);
//     int fromSq = toSq + (dir << 1);
//     pushMove(fromSq, toSq, pawnType, Type::None, Type::None, Move::Type::EpPush, moveList);
//     pawnDoublePushes &= pawnDoublePushes - 1;
//   }
// }

// void BitBoard::generateSlideAttacks(U64 piece, const U64 attacks[][256], U64 friends, U64 pieces, const uint32_t * shifts, const uint32_t * masks, MoveList & moveList) const
// {
//   while (piece) {
//     uint8_t fromSq = bitScanForward(piece);
//     uint32_t shift = shifts[fromSq];
//     uint32_t occupancy = (pieces >> shift) & masks[fromSq];
//     U64 move = attacks[fromSq][occupancy] & ~friends;
//     pushBitBoardMoves(move, fromSq, moveList);
//     piece &= piece-1;
//   }
// }

// void BitBoard::init()
// {
//   // Initialize the type to piece array
//   mTypeToPiece[None] = Piece::None;
//   mTypeToPiece[WhitePawn] = Piece::Pawn;
//   mTypeToPiece[BlackPawn] = Piece::Pawn;
//   mTypeToPiece[WhiteRook] = Piece::Rook;
//   mTypeToPiece[BlackRook] = Piece::Rook;
//   mTypeToPiece[WhiteBishop] = Piece::Bishop;
//   mTypeToPiece[BlackBishop] = Piece::Bishop;
//   mTypeToPiece[WhiteKnight] = Piece::Knight;
//   mTypeToPiece[BlackKnight] = Piece::Knight;
//   mTypeToPiece[WhiteQueen] = Piece::Queen;
//   mTypeToPiece[BlackQueen] = Piece::Queen;
//   mTypeToPiece[WhiteKing] = Piece::King;
//   mTypeToPiece[BlackKing] = Piece::King;

//   // Do some precalculations that will be helpful later on
//   for (uint8_t sq = 0; sq < 64; sq++) {
//     mMask[sq] = (ONE << sq);
//     mInvMask[sq] = ~mMask[sq];
//     mInvRotR45[rotateR45Matrix[sq]] = sq;
//     mInvRotL45[rotateL45Matrix[sq]] = sq;
//     mInvRotR90[rotateR90Matrix[sq]] = sq;
//     mNorthEastMasks[sq] = (1 << a1h8Lengths[sq]) - 1;
//     mNorthWestMasks[sq] = (1 << a8h1Lengths[sq]) - 1;
//     mFileMasks[sq] = 255;
//     mRankMasks[sq] = 255;
//   }
// }

// void BitBoard::initAttacks()
// {
//   initPawnAttacks(White, WHITE_ATTACK_ROW);
//   initPawnAttacks(Black, BLACK_ATTACK_ROW);
//   initKingAttacks();
//   initKnightAttacks();
//   initBishopAttacks();
//   initRookAttacks();
// }

// void BitBoard::initBishopAttacks()
// {
//   // North east attacks
//   for (uint8_t index = 0; index < 64; index++) {
//     uint8_t col = getCol(index);
//     uint8_t row = getRow(index);
//     uint8_t shift = a1h8Shifts[index];
//     uint8_t length = a1h8Lengths[index];
//     uint8_t mask = mNorthEastMasks[index];

//     for (uint8_t occupancy = 0; occupancy <= mask; occupancy++) {
//       U64 bb = 0;

//       int start = (col > row) ? row : col;
//       for (int i = (start)+1; i < length; i++) {
//         bb += (ONE << i);
//         if (occupancy & (1 << i)) break;
//       }

//       for (int i = (start)-1; i >= 0; i--) {
//         bb += (ONE << i);
//         if (occupancy & (1 << i)) break;
//       }

//       U64 bbRot = invRotateR45(bb << shift);
//       mNorthEastAttacks[index][occupancy] = bbRot;
//     }
//   }

//   // North west attacks
//   for (uint8_t index = 0; index < 64; index++) {
//     uint8_t col = getCol(index);
//     uint8_t row = getRow(index);
//     uint8_t shift = a8h1Shifts[index];
//     uint8_t length = a8h1Lengths[index];
//     uint8_t mask = mNorthWestMasks[index];

//     for (uint32_t occupancy = 0; occupancy <= mask; occupancy++) {
//       U64 bb = 0;

//       int start = (row > (7-col)) ? (7-row) : col;
//       for (int i = start+1; i < length; i++) {
//         bb += (ONE << i);
//         if (occupancy & (1 << i)) break;
//       }

//       for (int i = start-1; i >= 0; i--) {
//         bb += (ONE << i);
//         if (occupancy & (1 << i)) break;
//       }

//       U64 bbRot = invRotateL45(bb << shift);
//       mNorthWestAttacks[index][occupancy] = bbRot;
//     }
//   }
// }

// void BitBoard::initKingAttacks()
// {
//   static const int kingRowIncr[] = {1, 1, 1, 0, -1, -1, -1, 0};
//   static const int kingColIncr[] = {-1, 0, 1, 1, 1, 0, -1, -1};

//   // Precompute attacks for each square
//   for (char sourceRow = 0; sourceRow < 8; sourceRow++) {
//     for (char sourceCol = 0; sourceCol < 8; sourceCol++) {
//       uint8_t index = getIndex(sourceRow, sourceCol);
//       mKingAttacks[index] = 0ULL;

//       for (char k = 0; k < 8; k++) {
//         char destRow = sourceRow + kingRowIncr[k];
//         char destCol = sourceCol + kingColIncr[k];
//         if (destRow >= 0 && destRow < 8 && destCol >= 0 && destCol < 8) {
//           uint8_t destIndex = getIndex(destRow, destCol);
//           mKingAttacks[index] |= (ONE << destIndex);
//         }
//       }
//     }
//   }
// }

// void BitBoard::initKnightAttacks()
// {
//   static const int knightRowIncr[] = {1, 2, 2, 1, -1, -2, -2, -1};
//   static const int knightColIncr[] = {-2, -1, 1, 2, 2, 1, -1, -2};

//   // Precompute attacks for each square
//   for (char sourceRow = 0; sourceRow < 8; sourceRow++) {
//     for (char sourceCol = 0; sourceCol < 8; sourceCol++) {
//       uint8_t index = getIndex(sourceRow, sourceCol);
//       mKnightAttacks[index] = 0ULL;

//       for (char k = 0; k < 8; k++) {
//         char destRow = sourceRow + knightRowIncr[k];
//         char destCol = sourceCol + knightColIncr[k];
//         if (destRow >= 0 && destRow < 8 && destCol >= 0 && destCol < 8) {
//           uint8_t destIndex = getIndex(destRow, destCol);
//           mKnightAttacks[index] |= (ONE << destIndex);
//         }
//       }
//     }
//   }
// }

// void BitBoard::initPawnAttacks(Side side, int rowIncr)
// {
//   // Precompute attacks for each square
//   for (char sourceRow = 0; sourceRow < 8; sourceRow++) {
//     for (char sourceCol = 0; sourceCol < 8; sourceCol++) {
//       uint8_t index = getIndex(sourceRow, sourceCol);
//       mPawnAttacks[side][index] = 0ULL;

//       for (int colIncr = -1; colIncr <= 1; colIncr += 2) {
//         char destRow = sourceRow + rowIncr;
//         char destCol = sourceCol + colIncr;
//         if (destRow >= 0 && destRow < 8 && destCol >= 0 && destCol < 8) {
//           uint8_t destIndex = getIndex(destRow, destCol);
//           mPawnAttacks[side][index] |= (ONE << destIndex);
//         }
//       }
//     }
//   }
// }

// void BitBoard::initRookAttacks()
// {
//   // Rank attacks
//   for (int col = 0; col < 8; col++) {
//     for (int occupancy = 0; occupancy < 256; occupancy++) {
//       U64 bb = 0;

//       for (int i = col-1; i >=0; i--) {
//         bb += (ONE << i);
//         if (occupancy & (1<<i)) {
//           break;
//         }
//       }

//       for (int i = col+1; i < 8; i++) {
//         bb += (ONE << i);
//         if (occupancy & (1<<i)) {
//           break;
//         }
//       }

//       for (int row = 0; row < 8; row++) {
//         uint32_t shift = row*8;
//         mRankAttacks[(row*8)+col][occupancy] = (bb << shift);
//       }
//     }
//   }

//   // File attacks
//   for (int row = 0; row < 8; row++) {
//     for (uint32_t occupancy = 0; occupancy < 256; occupancy++) {
//       U64 bb = 0;

//       for (int i = (7-row)+1; i < 8; i++) {
//         bb += (ONE << (8*(7-i)));
//         if (occupancy & (1 << i)) break;
//       }

//       for (int i = (7-row)-1; i >= 0; i--) {
//         bb += (ONE << (8*(7-i)));
//         if (occupancy & (1 << i)) break;
//       }

//       for (int col = 0; col < 8; col++) {
//         mFileAttacks[(row*8)+col][occupancy] = bb << col;
//       }
//     }
//   }
// }

// BitBoard::U64 BitBoard::invRotateL45(U64 bb) const
// {
//   U64 symmBB = 0;
//   for (uint32_t index = 0; index < 64; index++)
//     if (bb & (ONE << invRotateL45Matrix[index]))
//       symmBB |= (ONE << index);

//   return symmBB;
// }

// BitBoard::U64 BitBoard::invRotateR45(U64 bb) const
// {
//   U64 symmBB = 0;
//   for (uint32_t index = 0; index < 64; index++)
//     if (bb & (ONE << invRotateR45Matrix[index]))
//       symmBB |= (ONE << index);

//   return symmBB;
// }

// bool BitBoard::isCellAttacked(uint8_t index, Color attackingColor) const
// {
//   Side bySide = (attackingColor == Color::White) ? White : Black;

//   if (mPawnAttacks[bySide^1][index] & mPiece[WhitePawn + bySide])
//     return true;

//   if (mKnightAttacks[index] & mPiece[WhiteKnight + bySide])
//     return true;

//   if (mKingAttacks[index] & mPiece[WhiteKing + bySide])
//     return true;

//   uint32_t occupancy = 0;

//   U64 rooksQueens = mPiece[WhiteRook + bySide] | mPiece[WhiteQueen + bySide];
//   occupancy = (mPiece[All] >> rankShifts[index]) & mRankMasks[index];
//   if (mRankAttacks[index][occupancy] & rooksQueens)
//     return true;

//   occupancy = (mRotateR90>> fileShifts[index]) & mFileMasks[index];
//   if (mFileAttacks[index][occupancy] & rooksQueens)
//     return true;

//   U64 bishopsQueens = mPiece[WhiteBishop + bySide] | mPiece[WhiteQueen + bySide];
//   occupancy = (mRotateR45 >> a1h8Shifts[index]) & mNorthEastMasks[index];
//   if (mNorthEastAttacks[index][occupancy] & bishopsQueens)
//     return true;

//   occupancy = (mRotateL45 >> a8h1Shifts[index]) & mNorthWestMasks[index];
//   if (mNorthWestAttacks[index][occupancy] & bishopsQueens)
//     return true;

//   return false;
// }

// bool BitBoard::isCellAttacked(uint8_t row, uint8_t col, Color attackingColor) const
// {
//   return isCellAttacked(getIndex(row, col), attackingColor);
// }

// bool BitBoard::makeMove(const Move * move)
// {
//   // Add to the undo stack
//   Undo undo;
//   undo.rotate90 = mRotateR90;
//   undo.rotateL45 = mRotateL45;
//   undo.rotateR45 = mRotateR45;
//   mUndoStack[mUndoPos++] = undo;

//   // Get move information
//   Color side = this->getSideToMove();
//   Color otherSide = (side == Color::White) ? Color::Black : Color::White;
//   uint8_t fromSquare = getIndex(move->getSourceRow(), move->getSourceColumn());
//   uint8_t toSquare = getIndex(move->getDestinationRow(), move->getDestinationColumn());
//   Type fromPiece = mType[fromSquare];
//   Type toPiece = mType[toSquare];

//   // Calculate required bitboards
//   U64 bbFrom = (ONE << fromSquare);
//   U64 bbTo = (ONE << toSquare);
//   U64 bbFromTo = bbFrom ^ bbTo;

//   // Update the type array
//   mType[fromSquare] = Type::None;
//   mType[toSquare] = fromPiece;

//   // Handle standard moves
//   if (!move->isPromotion() && !move->isPromotionCapture()) {
//     mPiece[fromPiece] ^= bbFromTo;
//   }

//   // Handle capture
//   if (move->isCapture() && !move->isEnPassantCapture()) {
//     mPiece[toPiece] ^= (ONE << toSquare);
//   }

//   // Handle enPassant captures
//   if (move->isEnPassantCapture()) {
//     char dir = (mSide == White) ? SOUTH : NORTH;
//     uint8_t sq = toSquare + dir;
//     mType[sq] = Type::None;
//     mPiece[WhitePawn + !mSide] &= ~(ONE << sq);

//     mRotateR90 &= mInvMask[rotateL90Matrix[sq]];
//     mRotateR45 &= mInvMask[mInvRotR45[sq]];
//     mRotateL45 &= mInvMask[mInvRotL45[sq]];
//   }

//   // Handle king move
//   if (move->getPiece() == Piece::King) {
//     setKingColumn(side, move.destCol());
//     setKingRow(side, move.destRow());
//   }

//   // Update the en-passant index
//   setEnPassantColumn(Board::INVALID_ENPASSANT_COLUMN);
//   if (move->isDoublePush()) {
//     setEnPassantColumn(move->getDestinationColumn());
//   }

//   // Handle promotions
//   if (move->isPromotion() || move->isPromotionCapture()) {
//     Piece promotedPiece = move->getPromotedPiece();

//     mPiece[WhitePawn + mSide] ^= (ONE << fromSquare);
//     switch (promotedPiece) {
//     case Piece::Queen:
//       mPiece[WhiteQueen + mSide] ^= (ONE << toSquare);
//       mType[toSquare] = static_cast<Type>(WhiteQueen + mSide);
//       break;
//     case Piece::Rook:
//       mPiece[WhiteRook + mSide] ^= (ONE << toSquare);
//       mType[toSquare] = static_cast<Type>(WhiteRook + mSide);
//       break;
//     case Piece::Bishop:
//       mPiece[WhiteBishop + mSide] ^= (ONE << toSquare);
//       mType[toSquare] = static_cast<Type>(WhiteBishop + mSide);
//       break;
//     case Piece::Knight:
//       mPiece[WhiteKnight + mSide] ^= (ONE << toSquare);
//       mType[toSquare] = static_cast<Type>(WhiteKnight + mSide);
//       break;
//     }
//   }

//   // Update castling rights
//   // If the square associated with a king or rook is
//   // modified in any way remove the castling ability
//   // for that piece
//   uint8_t castling = this->getCastlingRights();
//   switch (fromSquare) {
//   case H1:
//     castling &= ~CASTLE_WHITE_KING;
//     break;
//   case E1:
//     castling &= ~(CASTLE_WHITE_KING|CASTLE_WHITE_QUEEN);
//     break;
//   case A1:
//     castling &= ~CASTLE_WHITE_QUEEN;
//     break;
//   case H8:
//     castling &= ~CASTLE_BLACK_KING;
//     break;
//   case E8:
//     castling &= ~(CASTLE_BLACK_KING|CASTLE_BLACK_QUEEN);
//     break;
//   case A8:
//     castling &= ~CASTLE_BLACK_QUEEN;
//     break;
//   }

//   switch (toSquare) {
//   case H1:
//     castling &= ~CASTLE_WHITE_KING;
//     break;
//   case E1:
//     castling &= ~(CASTLE_WHITE_KING|CASTLE_WHITE_QUEEN);
//     break;
//   case A1:
//     castling &= ~CASTLE_WHITE_QUEEN;
//     break;
//   case H8:
//     castling &= ~CASTLE_BLACK_KING;
//     break;
//   case E8:
//     castling &= ~(CASTLE_BLACK_KING|CASTLE_BLACK_QUEEN);
//     break;
//   case A8:
//     castling &= ~CASTLE_BLACK_QUEEN;
//     break;
//   }

//   // Handle castle move
//   if (move->isCastle()) {
//     switch (toSquare) {
//     case C1:
//       mType[D1] = Type::WhiteRook;
//       mType[A1] = Type::None;
//       mPiece[WhiteRook] ^= (ONE << D1);
//       mPiece[WhiteRook] ^= (ONE << A1);
//       mRotateR90 ^= mMask[rotateL90Matrix[A1]] | mMask[rotateL90Matrix[C1]] | mMask[rotateL90Matrix[D1]] | mMask[rotateL90Matrix[E1]];
//       mRotateR45 ^= mMask[mInvRotR45[A1]] | mMask[mInvRotR45[C1]] | mMask[mInvRotR45[D1]] | mMask[mInvRotR45[E1]];
//       mRotateL45 ^= mMask[mInvRotL45[A1]] | mMask[mInvRotL45[C1]] | mMask[mInvRotL45[D1]] | mMask[mInvRotL45[E1]];
//       break;
//     case G1:
//       mType[F1] = Type::WhiteRook;
//       mType[H1] = Type::None;
//       mPiece[WhiteRook] ^= (ONE << F1);
//       mPiece[WhiteRook] ^= (ONE << H1);
//       mRotateR90 ^= mMask[rotateL90Matrix[E1]] | mMask[rotateL90Matrix[F1]] | mMask[rotateL90Matrix[G1]] | mMask[rotateL90Matrix[H1]];
//       mRotateR45 ^= mMask[mInvRotR45[E1]] | mMask[mInvRotR45[F1]] | mMask[mInvRotR45[G1]] | mMask[mInvRotR45[H1]];
//       mRotateL45 ^= mMask[mInvRotL45[E1]] | mMask[mInvRotL45[F1]] | mMask[mInvRotL45[G1]] | mMask[mInvRotL45[H1]];
//       break;
//     case C8:
//       mType[D8] = Type::BlackRook;
//       mType[A8] = Type::None;
//       mPiece[BlackRook] ^= (ONE << D8);
//       mPiece[BlackRook] ^= (ONE << A8);
//       mRotateR90 ^= mMask[rotateL90Matrix[A8]] | mMask[rotateL90Matrix[C8]] | mMask[rotateL90Matrix[D8]] | mMask[rotateL90Matrix[E8]];
//       mRotateR45 ^= mMask[mInvRotR45[A8]] | mMask[mInvRotR45[C8]] | mMask[mInvRotR45[D8]] | mMask[mInvRotR45[E8]];
//       mRotateL45 ^= mMask[mInvRotL45[A8]] | mMask[mInvRotL45[C8]] | mMask[mInvRotL45[D8]] | mMask[mInvRotL45[E8]];
//       break;
//     case G8:
//       mType[F8] = Type::BlackRook;
//       mType[H8] = Type::None;
//       mPiece[BlackRook] ^= (ONE << F8);
//       mPiece[BlackRook] ^= (ONE << H8);
//       mRotateR90 ^= mMask[rotateL90Matrix[E8]] | mMask[rotateL90Matrix[F8]] | mMask[rotateL90Matrix[G8]] | mMask[rotateL90Matrix[H8]];
//       mRotateR45 ^= mMask[mInvRotR45[E8]] | mMask[mInvRotR45[F8]] | mMask[mInvRotR45[G8]] | mMask[mInvRotR45[H8]];
//       mRotateL45 ^= mMask[mInvRotL45[E8]] | mMask[mInvRotL45[F8]] | mMask[mInvRotL45[G8]] | mMask[mInvRotL45[H8]];
//       break;
//     }
//   }

//   // Update rotated bitboards
//   if (!move->isCastle()) {
//     mRotateR90 &= mInvMask[rotateL90Matrix[fromSquare]];
//     mRotateR90 |= mMask[rotateL90Matrix[toSquare]];
//     mRotateR45 &= mInvMask[mInvRotR45[fromSquare]];
//     mRotateR45 |= mMask[mInvRotR45[toSquare]];
//     mRotateL45 &= mInvMask[mInvRotL45[fromSquare]];
//     mRotateL45 |= mMask[mInvRotL45[toSquare]];
//   }

//   // Set the castling rights
//   setCastlingRights(castling);

//   // Update board state
//   //incrementHalfMoveClock();
//   setHalfMoveClock(getHalfMoveClock() + 1);
//   if (move->getPiece() == Piece::Pawn || move->isCapture())
//     setHalfMoveClock(0);

//   // Update aggregate bitboards
//   updateAggregateBitBoards();

//   // Switch side to move
//   //mSide = static_cast<Side>(!mSide);
//   setSideToMove(otherSide);
//   return true;
//   //toggleSideToMove();
// }

// PieceType BitBoard::pieceType(uint8_t row, uint8_t col) const
// {
//   uint8_t index = getIndex(row, col);
//   Type type = mType[index];
//   switch (type) {
//   case WhitePawn:
//     return PieceType::WhitePawn;
//   case WhiteRook:
//     return PieceType::WhiteRook;
//   case WhiteKnight:
//     return PieceType::WhiteKnight;
//   case WhiteBishop:
//     return PieceType::WhiteBishop;
//   case WhiteQueen:
//     return PieceType::WhiteQueen;
//   case WhiteKing:
//     return PieceType::WhiteKing;
//   case BlackPawn:
//     return PieceType::BlackPawn;
//   case BlackRook:
//     return PieceType::BlackRook;
//   case BlackKnight:
//     return PieceType::BlackKnight;
//   case BlackBishop:
//     return PieceType::BlackBishop;
//   case BlackQueen:
//     return PieceType::BlackQueen;
//   case BlackKing:
//     return PieceType::BlackKing;
//   }
//   return PieceType::None;
// }

// inline void BitBoard::pushBitBoardMoves(U64 bb, uint8_t fromSq, MoveList & moveList) const
// {
//   while (bb) {
//     int toSq = bitScanForward(bb);
//     Type capturePiece = mType[toSq];
//     Move::Type type = (capturePiece == Type::None) ? Move::Type::Quiet : Move::Type::Capture;
//     pushMove(fromSq, toSq, mType[fromSq], capturePiece, Type::None, type, moveList);
//     bb &= bb-1;
//   }
// }

// inline void BitBoard::pushMove(uint8_t from, uint8_t to, Type piece, Type capture, Type promote, Move::Type type, MoveList & moveList) const
// {
//   Move move(getRow(from), getCol(from), getRow(to), getCol(to), mTypeToPiece[piece]);
//   move.setCastlingRights(this->getCastlingRights());
//   move.setHalfMoveClock(this->getHalfMoveClock());
//   move.setFullMoveCounter(this->getFullMoveNumber());
//   move.setType(type);
//   move.setCapturedPiece(mTypeToPiece[capture]);
//   move.setPromotedPiece(mTypeToPiece[promote]);
//   move.setEnpassantColumn(this->getEnpassantColumn());

//   moveList.addMove(move);
// }

// void BitBoard::reset()
// {
//   // Reset variables
//   mUndoPos = 0;
//   //mSide = White;
//   for (uint8_t sq = 0; sq < 64; sq++) {
//     mType[sq] = None;
//   }

//   // Intialize white piece types
//   mType[A1] = mType[H1] = WhiteRook;
//   mType[B1] = mType[G1] = WhiteKnight;
//   mType[C1] = mType[F1] = WhiteBishop;
//   mType[D1] = WhiteQueen;
//   mType[E1] = WhiteKing;
//   for (uint8_t sq = A3; sq <= H3; sq++)
//     mType[sq] = WhitePawn;

//   // Initialize black piece types
//   mType[A8] = mType[H8] = BlackRook;
//   mType[B8] = mType[G8] = BlackKnight;
//   mType[C8] = mType[F8] = BlackBishop;
//   mType[D8] = BlackQueen;
//   mType[E8] = BlackKing;
//   for (uint8_t sq = A7; sq <= H7; sq++)
//     mType[sq] = BlackPawn;

//   // Initialize white piece bit boards
//   mPiece[WhitePawn] = 0;
//   mPiece[WhiteRook]   = mMask[A1] | mMask[H1];
//   mPiece[WhiteKnight] = mMask[B1] | mMask[G1];
//   mPiece[WhiteBishop] = mMask[C1] | mMask[F1];
//   mPiece[WhiteQueen]  = mMask[D1];
//   mPiece[WhiteKing]   = mMask[E1];
//   for (uint8_t sq = A2; sq <= H2; sq++) {
//     mPiece[WhitePawn] |= mMask[sq];
//   }

//   // Initialize black piece bit boards
//   mPiece[BlackPawn] = 0;
//   mPiece[BlackRook]   = mMask[A8] | mMask[H8];
//   mPiece[BlackKnight] = mMask[B8] | mMask[G8];
//   mPiece[BlackBishop] = mMask[C8] | mMask[F8];
//   mPiece[BlackQueen]  = mMask[D8];
//   mPiece[BlackKing]   = mMask[E8];
//   for (uint8_t sq = A7; sq <= H7; sq++) {
//     mPiece[BlackPawn] |= mMask[sq];
//   }

//   // Update aggregate bit boards
//   updateAggregateBitBoards();

//   // Update rotated bitboards
//   updateRotatedBitBoards();
// }

// BitBoard::U64 BitBoard::rotateL45(U64 bb) const
// {
//   U64 symmBB = 0;
//   for (uint32_t index = 0; index < 64; index++)
//     if (bb & (ONE << rotateL45Matrix[index]))
//       symmBB |= (ONE << index);

//   return symmBB;
// }

// BitBoard::U64 BitBoard::rotateL90(U64 bb) const
// {
//   U64 symmBB = 0;
//   for (uint32_t index = 0; index < 64; index++)
//     if (bb & (ONE << rotateL90Matrix[index]))
//       symmBB |= (ONE << index);

//   return symmBB;
// }

// BitBoard::U64 BitBoard::rotateR45(U64 bb) const
// {
//   U64 symmBB = 0;
//   for (uint32_t index = 0; index < 64; index++)
//     if (bb & (ONE << rotateR45Matrix[index]))
//       symmBB |= (ONE << index);

//   return symmBB;
// }

// BitBoard::U64 BitBoard::rotateR90(U64 bb) const
// {
//   U64 symmBB = 0;
//   for (uint32_t index = 0; index < 64; index++)
//     if (bb & (ONE << rotateR90Matrix[index]))
//       symmBB |= (ONE << index);

//   return symmBB;
// }

// void BitBoard::setPosition(const std::string & fenString)
// {
//   Fen fenData;
//   fenData.readString(fenString);

//   mUndoPos = 0;

//   uint8_t castling = CASTLE_NONE;
//   if (fenData.canBlackCastleKingSide())
//     castling |= CASTLE_BLACK_KING;
//   if (fenData.canBlackCastleQueenSide())
//     castling |= CASTLE_BLACK_QUEEN;
//   if (fenData.canWhiteCastleKingSide())
//     castling |= CASTLE_WHITE_KING;
//   if (fenData.canWhiteCastleQueenSide())
//     castling |= CASTLE_WHITE_QUEEN;

//   setCastlingRights(castling);
//   setHalfMoveClock(fenData.halfMoveClock());
//   setFullMoveCounter(fenData.fullMoveCounter());

//   Color color = (fenData.whiteToMove()) ? Color::White : Color::Black;
//   mSide = (color == Color::White) ? White : Black;
//   setSideToMove(color);

//   clearEpCol();
//   if (fenData.epColumn() >= 0) {
//     setEpColumn(fenData.epColumn());
//   }

//   for (int i = 0; i < 64; i++) {
//     mType[i] = None;
//   }

//   for (int i = 0; i < All; i++) {
//     mPiece[i] = 0;
//   }

//   for (uint8_t row = 0; row < 8; row++) {
//     for (uint8_t col = 0; col < 8; col++) {
//       PieceType type = fenData.pieceType(row, col);
//       uint8_t sq = getIndex(row, col);
//       U64 bb = (ONE << sq);

//       switch (type) {
//       case PieceType::WhitePawn:
//         mPiece[WhitePawn] |= bb;
//         mType[sq] = WhitePawn;
//         break;
//       case PieceType::WhiteRook:
//         mPiece[WhiteRook] |= bb;
//         mType[sq] = WhiteRook;
//         break;
//       case PieceType::WhiteKnight:
//         mPiece[WhiteKnight] |= bb;
//         mType[sq] = WhiteKnight;
//         break;
//       case PieceType::WhiteBishop:
//         mPiece[WhiteBishop] |= bb;
//         mType[sq] = WhiteBishop;
//         break;
//       case PieceType::WhiteQueen:
//         mPiece[WhiteQueen] |= bb;
//         mType[sq] = WhiteQueen;
//         break;
//       case PieceType::WhiteKing:
//         mPiece[WhiteKing] |= bb;
//         mType[sq] = WhiteKing;
//         setKingRow(Color::White, row);
//         setKingColumn(Color::White, col);
//         break;
//       case PieceType::BlackPawn:
//         mPiece[BlackPawn] |= bb;
//         mType[sq] = BlackPawn;
//         break;
//       case PieceType::BlackRook:
//         mPiece[BlackRook] |= bb;
//         mType[sq] = BlackRook;
//         break;
//       case PieceType::BlackKnight:
//         mPiece[BlackKnight] |= bb;
//         mType[sq] = BlackKnight;
//         break;
//       case PieceType::BlackBishop:
//         mPiece[BlackBishop] |= bb;
//         mType[sq] = BlackBishop;
//         break;
//       case PieceType::BlackQueen:
//         mPiece[BlackQueen] |= bb;
//         mType[sq] = BlackQueen;
//         break;
//       case PieceType::BlackKing:
//         mPiece[BlackKing] |= bb;
//         mType[sq] = BlackKing;
//         setKingRow(Color::Black, row);
//         setKingColumn(Color::Black, col);
//         break;
//       default:
//         break;
//       }
//     }
//   }

//   // Update aggregate bitboards
//   updateAggregateBitBoards();

//   // Update rotated bitboards
//   updateRotatedBitBoards();
// }

// void BitBoard::testRotations()
// {
//   // Make the R
// //  mPiece[All] = 0;
// //  mPiece[All] |= (ONE << B8) | (ONE << C8) | (ONE << D8) | (ONE << E8);
// //  mPiece[All] |= (ONE << B7) | (ONE << F7);
// //  mPiece[All] |= (ONE << B6) | (ONE << F6);
// //  mPiece[All] |= (ONE << B5) | (ONE << E5);
// //  mPiece[All] |= (ONE << B4) | (ONE << C4) | (ONE << D4);
// //  mPiece[All] |= (ONE << B3) | (ONE << C3);
// //  mPiece[All] |= (ONE << B2) | (ONE << D2);
// //  mPiece[All] |= (ONE << B1) | (ONE << E1);

//   //mPiece[WhitePawn] ^= (ONE << A2);
//   //mPiece[WhitePawn] ^= (ONE << A3);
//   //mPiece[BlackPawn] ^= (ONE << A7);
//   //mPiece[BlackPawn] ^= (ONE << A6);
//   //updateAggregateBitBoards();
//   //updateRotatedBitBoards();
//   mSide = White;

// //  MoveList m;
// //  generateMoves(m);
// //  for (int i = 0; i < m.size(); i++) {
// //    std::cout << m[i].toSmithNotation() << "\n";
// //  }
//   writeBitBoard(mPiece[All]);
//   writeBitBoard(mRotateL45);

//   U64 bb = mRotateL45;
//   const uint32_t * shifts = a8h1Shifts;
//   const uint32_t * lengths = a8h1Lengths;
//   const uint32_t * masks = mNorthWestMasks;
//   const char sq[] = {H1, G1, F1, E1, D1, C1, B1, A1};

//   for (int i = 0; i < 8; i++) {
//     char s = sq[i];
//     std::cout << lengths[s] << " " << shifts[s] << " ";
//     writeOccupancy((bb >> shifts[s] & masks[s]), lengths[s]);
//   }

//   //std::cout << a8h1Shifts[D1] << "\n";
//   exit(1);

//   // Prepare move
//   uint8_t fromSq = B1;
//   uint8_t toSq = C3;
//   U64 from = (ONE << fromSq);
//   U64 to = (ONE << toSq);
//   U64 fromTo = from ^ to;
//   uint32_t shift = a1h8Shifts[fromSq];
//   uint32_t length = a1h8Lengths[fromSq];
//   uint32_t mask = (1 << length) - 1;

//   // Do occupancy
//   U64 rotated = rotateR90(mPiece[All]);
//   U64 shifted = rotated >> shift;
//   uint32_t occupancy = (shifted & mask);
//   std::cout << "\nPieces\n";              writeBitBoard(mPiece[All]);
//   std::cout << "\nRotated\n";             writeBitBoard(rotated);
//   std::cout << "\nRotated and Shifted\n"; writeBitBoard(shifted);
//   std::cout << "\nOccupancy\n";           writeOccupancy(occupancy);

//   // Do move
//   mPiece[All] ^= fromTo;
//   rotated &= mInvMask[mInvRotR90[fromSq]];
//   rotated |= mMask[mInvRotR90[toSq]];
//   bool success = (rotateR90(mPiece[All]) == rotated);
//   std::string result = (success) ? "success" : "failure";

//   std::cout << "\nPieces after move\n";             writeBitBoard(mPiece[All]);
//   std::cout << "\nRotated after move\n";            writeBitBoard(rotateR90(mPiece[All]));
//   std::cout << "\nIncremental update after move\n"; writeBitBoard(rotated);
//   std::cout << "\nResult: ";                        std::cout << result << "\n";

//   exit(1);
// }

// bool BitBoard::unmakeMove(const Move * move)
// {
//   Color otherSide = (this->getSideToMove() == Color::White) ? Color::Black : Color::White;
//   Undo undo = mUndoStack[--mUndoPos];
//   mRotateR90 = undo.rotate90;
//   mRotateL45 = undo.rotateL45;
//   mRotateR45 = undo.rotateR45;

//   // Get move information
//   uint8_t fromSquare = getIndex(move->getSourceRow(), move->getSourceColumn());
//   uint8_t toSquare = getIndex(move->getDestinationRow(), move->getDestinationColumn());
//   Type toPiece = mType[toSquare];

//   // Calculate required bitboards
//   U64 bbFrom = (ONE << fromSquare);
//   U64 bbTo = (ONE << toSquare);
//   U64 bbFromTo = bbFrom ^ bbTo;

//   // Reset the board state
//   setFullMoveCounter(move->getFullMoveCounter());
//   setHalfMoveClock(move->getHalfMoveClock());
//   setCastlingRights(move->getCastlingRights());
//   setEnPassantColumn(move->getEnPassantColumn());

//   // Update the piece and color tables
//   mType[fromSquare] = toPiece;
//   mType[toSquare] = Type::None;

//   // Handle non promotion moves
//   if (!move->isPromotion() && !move->isPromotionCapture())
//   {
//     mPiece[toPiece] ^= bbFromTo;
//   }

//   // Handle captures
//   if (move->isCapture()) {
//     uint8_t sq = toSquare;
//     if (move->isEnPassantCapture()) {
//       char dir = (mSide == White) ? NORTH : SOUTH;
//       sq = toSquare + dir;
//     }

//     // Update the relevant bit board
//     Piece capturePiece = move->getCapturedPiece();
//     int index = (mSide == Black) ? BlackPawn : WhitePawn;

//     for (int i = index; i < None; i += 2) {
//       if (mTypeToPiece[i] == capturePiece) {
//         mType[sq] = static_cast<Type>(i);
//         mPiece[i] |= (ONE << sq);
//         break;
//       }
//     }
//   }

//   // Handle king moves
//   if (move->getPiece() == Piece::King) {
//     //this-setKingRow(otherSide, move->getSourceRow());
//     //setKingColumn(otherSide, move->getSourceColumn());
//   }

//   // Handle promotions
//   if (move->isPromotion() || move->isPromotionCapture()) {
//     Piece promotedPiece = move->getPromotedPiece();

//     mPiece[WhitePawn + !mSide] ^= (ONE << fromSquare);
//     mType[fromSquare] = static_cast<Type>(WhitePawn + !mSide);
//     switch (promotedPiece) {
//     case Piece::Queen:
//       mPiece[WhiteQueen + !mSide] ^= (ONE << toSquare);
//       break;
//     case Piece::Rook:
//       mPiece[WhiteRook + !mSide] ^= (ONE << toSquare);
//       break;
//     case Piece::Bishop:
//       mPiece[WhiteBishop + !mSide] ^= (ONE << toSquare);
//       break;
//     case Piece::Knight:
//       mPiece[WhiteKnight + !mSide] ^= (ONE << toSquare);
//       break;
//     }
//   }

//   // Handle castling move
//   if (move->isCastle()) {
//     switch (toSquare) {
//     case C1:
//       mType[A1] = Type::WhiteRook;
//       mType[D1] = Type::None;
//       mPiece[WhiteRook] ^= (ONE << A1);
//       mPiece[WhiteRook] ^= (ONE << D1);
//       break;
//     case G1:
//       mType[H1] = Type::WhiteRook;
//       mType[F1] = Type::None;
//       mPiece[WhiteRook] ^= (ONE << H1);
//       mPiece[WhiteRook] ^= (ONE << F1);
//       break;
//     case C8:
//       mType[A8] = Type::BlackRook;
//       mType[D8] = Type::None;
//       mPiece[BlackRook] ^= (ONE << A8);
//       mPiece[BlackRook] ^= (ONE << D8);
//       break;
//     case G8:
//       mType[H8] = Type::BlackRook;
//       mType[F8] = Type::None;
//       mPiece[BlackRook] ^= (ONE << H8);
//       mPiece[BlackRook] ^= (ONE << F8);
//       break;
//     }
//   }

//   // Update aggregate bitboards
//   updateAggregateBitBoards();

//   // Switch side to move
//   mSide = static_cast<Side>(!mSide);
//   setSideToMove(otherSide);
//   return true;
//   //toggleSideToMove();
// }

// inline void BitBoard::updateAggregateBitBoards()
// {
//   mPiece[WhitePieces] = mPiece[WhiteRook] | mPiece[WhiteKnight] | mPiece[WhiteBishop] | mPiece[WhiteQueen] | mPiece[WhiteKing] | mPiece[WhitePawn];
//   mPiece[BlackPieces] = mPiece[BlackRook] | mPiece[BlackKnight] | mPiece[BlackBishop] | mPiece[BlackQueen] | mPiece[BlackKing] | mPiece[BlackPawn];
//   mPiece[All] = mPiece[WhitePieces] | mPiece[BlackPieces];
//   mPiece[None] = ~mPiece[All];
// }

// void BitBoard::updateRotatedBitBoards()
// {
//   mRotateR90 = rotateR90(mPiece[All]);
//   mRotateR45 = rotateR45(mPiece[All]);
//   mRotateL45 = rotateL45(mPiece[All]);
// }

// void BitBoard::writeBitBoard(U64 bb, std::ostream & output) const
// {
//   static const char files[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};

//   for (char row = 7; row >= 0; row--) {
//     output << (row+1) << " ";
//     for (char col = 0; col < 8; col++) {
//       int shift = getIndex(row, col);
//       if ((bb >> shift) & 0x1)
//         output << "X ";
//       else
//         output << ". ";
//     }
//     output << "\n";
//   }

//   output << "  ";
//   for (char col = 0; col < 8; col++) {
//     output << files[col] << " ";
//   }
//   output << "\n\n";
// }

// void BitBoard::writeOccupancy(uint32_t occupancy, int length, std::ostream & output) const
// {
//   for (int i = length-1; i >=0; i--) {
//     if (occupancy & (1 << i))
//       output << 'X';
//     else
//       output << '-';
//   }
//   output << "\n";
// }

// }
