#include "jcl_bitboard.h"

#include <cassert>
#include <iomanip>
#include <intrin.h>

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

#define ZERO 0LL
#define ONE 1LL

// Macros for mapping (row,col)->index and vice-versa
#define getIndex(row,col) (((row)<<3)+(col))
#define getRow(index) ( (index >> 3) )
#define getCol(index) ( (index & 7) )

BitBoard::BitBoard()
{
  init();
  initBoard();
}

void BitBoard::doReset()
{
  init();
  initBoard();
}

bool BitBoard::generateMoves(MoveList & moveList) const
{
  return true;
}

bool BitBoard::generateMoves(uint8_t row, uint8_t col, MoveList & moveList) const
{
  return true;
}

uint8_t BitBoard::getKingColumn(Color color) const
{
  return mKingColumn.find(color)->second;
}

uint8_t BitBoard::getKingRow(Color color) const
{
  return mKingRow.find(color)->second;
}

PieceType BitBoard::getPieceType(uint8_t row, uint8_t col) const
{
  uint8_t index = getIndex(row, col);
  BitBoardPiece type = mPieces[index];
  switch (type)
  {
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
  default:
    break;
  }
  return PieceType::None;
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
}

void BitBoard::initBoard()
{
  for (int i = 0; i < 12; i++)
  {
    mBitboards[i] = ZERO;
  }

  mBitboards[WhiteRook] = (mMask[A1] | mMask[H1]);
  mBitboards[WhiteKnight] = (mMask[B1] | mMask[G1]);
  mBitboards[WhiteBishop] = (mMask[C1] | mMask[F1]);
  mBitboards[WhiteQueen] = mMask[D1];
  mBitboards[WhiteKing] = mMask[E1];
  for (int8_t i = A2; i <= H2; i++)
  {
    mBitboards[WhitePawn] |= mMask[i];
  }

  mBitboards[BlackRook] = (mMask[A8] | mMask[H8]);
  mBitboards[BlackKnight] = (mMask[B8] | mMask[G8]);
  mBitboards[BlackBishop] = (mMask[C8] | mMask[F8]);
  mBitboards[BlackQueen] = mMask[D8];
  mBitboards[BlackKing] = mMask[E8];
  for (int8_t i = A7; i <= H7; i++)
  {
    mBitboards[BlackPawn] |= mMask[i];
  }

  for (uint64_t i = 0; i < 64; i++)
  {
    mPieces[i] = None;
  }

  mPieces[A1] = mPieces[H1] = WhiteRook;
  mPieces[B1] = mPieces[G1] = WhiteKnight;
  mPieces[C1] = mPieces[F1] = WhiteBishop;
  mPieces[D1] = WhiteQueen;
  mPieces[E1] = WhiteKing;
  for (uint8_t i = A2; i <= H2; i++)
  {
    mPieces[i] = WhitePawn;
  }

  mPieces[A8] = mPieces[H8] = BlackRook;
  mPieces[B8] = mPieces[G8] = BlackKnight;
  mPieces[C8] = mPieces[F8] = BlackBishop;
  mPieces[D8] = BlackQueen;
  mPieces[E8] = BlackKing;
  for (uint8_t i = A7; i <= H7; i++)
  {
    mPieces[i] = BlackPawn;
  }

  mWhitePieceBitboard = 0;
  for (uint8_t i = WhitePawn; i <= WhiteKing; i++)
  {
    mWhitePieceBitboard |= mBitboards[i];
  }

  mBlackPieceBitboard = 0;
  for (int i = BlackPawn; i <= BlackKing; i++)
  {
    mBlackPieceBitboard |= mBitboards[i];
  }

  mKingColumn[Color::White] = 4;
  mKingRow[Color::White] = 0;
  mKingColumn[Color::Black] = 4;
  mKingRow[Color::Black] = 7;

  updateAggregateBitBoards();

  //std::cout << mBitboards[WhitePawn] << " " << mBitboards[BlackPawn] << std::endl;
  // std::cout << std::hex << std::setw(16) << std::setfill('0') << mBitboards[WhitePawn] << std::endl;
  // std::cout << std::hex << std::setw(16) << std::setfill('0') << mBitboards[BlackPawn] << std::endl;
  // std::cout << std::hex << std::setw(16) << std::setfill('0') << mBitboards[WhiteQueen] << std::endl;
  // std::cout << std::hex << std::setw(16) << std::setfill('0') << mBitboards[BlackQueen] << std::endl;
  // std::cout << std::hex << std::setw(16) << std::setfill('0') << mBitboards[BlackKing] << std::endl;

  // writeBitBoard(mBitboards[WhiteRook], std::cout);
  // writeBitBoard(mBitboards[WhiteKnight], std::cout);
  // writeBitBoard(mBitboards[WhiteBishop], std::cout);
  // writeBitBoard(mBitboards[WhiteQueen], std::cout);
  // writeBitBoard(mBitboards[WhiteKing], std::cout);
  // writeBitBoard(mBitboards[WhitePawn], std::cout);
  // writeBitBoard(mWhitePieceBitboard, std::cout);

  // writeBitBoard(mBitboards[BlackRook], std::cout);
  // writeBitBoard(mBitboards[BlackKnight], std::cout);
  // writeBitBoard(mBitboards[BlackBishop], std::cout);
  // writeBitBoard(mBitboards[BlackQueen], std::cout);
  // writeBitBoard(mBitboards[BlackKing], std::cout);
  // writeBitBoard(mBitboards[BlackPawn], std::cout);
  // writeBitBoard(mBlackPieceBitboard, std::cout);
}

bool BitBoard::isCellAttacked(uint8_t row, uint8_t col, Color attackingColor) const
{
  return true;
}

void BitBoard::makeCastleMove(uint8_t destinationSquare)
{
  // The King position has already been updated above
  // so we simply need to update the rook position
  if (destinationSquare == G8)
  {
    mPieces[F8] = BitBoardPiece::BlackRook;
    mPieces[H8] = BitBoardPiece::None;
    mBitboards[BlackRook] ^= (ONE << H8);
    mBitboards[BlackRook] |= (ONE << F8);
  }
  else if (destinationSquare == C8)
  {
    mPieces[D8] = BitBoardPiece::BlackRook;
    mPieces[H8] = BitBoardPiece::None;
    mBitboards[BlackRook] ^= (ONE << A8);
    mBitboards[BlackRook] |= (ONE << D8);
  }
  else if (destinationSquare == G1)
  {
    mPieces[F1] = BitBoardPiece::WhiteRook;
    mPieces[H1] = BitBoardPiece::None;
    mBitboards[WhiteRook] ^= (ONE << H1);
    mBitboards[WhiteRook] |= (ONE << F1);

  }
  else if (destinationSquare == C1)
  {
    mPieces[D1] = BitBoardPiece::WhiteRook;
    mPieces[H1] = BitBoardPiece::None;
    mBitboards[WhiteRook] ^= (ONE << A1);
    mBitboards[WhiteRook] |= (ONE << D1);
  }
}

bool BitBoard::makeMove(const Move * move)
{
  Color sideToMove = this->getSideToMove();
  Color otherSide = (sideToMove == Color::White) ? Color::Black : Color::White;
  uint8_t sourceSquare = getIndex(move->getSourceRow(), move->getSourceColumn());
  uint8_t destinationSquare = getIndex(move->getDestinationRow(), move->getDestinationColumn());

  BitBoardPiece fromPiece = mPieces[sourceSquare];
  uint64_t bbFrom = ONE << sourceSquare;
  uint64_t bbTo = ONE << destinationSquare;
  uint64_t bbFromTo = bbFrom ^ bbTo;

  // Update for a quiet move
  mBitboards[fromPiece] ^= bbFromTo;
  mPieces[destinationSquare] = fromPiece;
  mPieces[sourceSquare] = BitBoardPiece::None;

  // Update for capture moves, except for en-passant captures
  // Those are handled below
  if (move->isCapture() || move->isPromotionCapture())
  {
    jcl::Piece capturePiece = move->getCapturedPiece();
    BitBoardPiece bbPiece = translatePiece(capturePiece, otherSide);
    mBitboards[bbPiece] ^= bbFromTo;
  }

  // Update for a promotion moves
  if (move->isPromotion())
  {
    BitBoardPiece bbPiece = translatePiece(move->getPromotedPiece(), sideToMove);
    mPieces[destinationSquare] = bbPiece;
    mBitboards[bbPiece] = (ONE << destinationSquare);
  }

  // Update for an enpassant capture
  if (move->isEnPassantCapture())
  {
    int8_t dir = NORTH;
    BitBoardPiece piece = WhitePawn;
    BitBoardPiece enemyPiece = BlackPawn;
    if (sideToMove == Color::Black)
    {
      dir = SOUTH;
      piece = BlackPawn;
      enemyPiece = WhitePawn;
    }

    int8_t epColumn = getEnpassantColumn();
    uint8_t epIndex = getIndex(move->getSourceRow(), epColumn) + dir;
    uint8_t captureIndex = getIndex(move->getSourceRow(), epColumn);

    mBitboards[piece] |= (ONE << epIndex);
    mBitboards[enemyPiece] ^= (ONE << captureIndex);
    mBitboards[enemyPiece] ^= (ONE << epIndex);
    mBitboards[enemyPiece] ^= (ONE << sourceSquare);
  }

  // Update for a double pawn push
  setEnPassantColumn(INVALID_ENPASSANT_COLUMN);
  if (move->isDoublePush())
  {
    setEnPassantColumn(move->getSourceColumn());
  }

  // Update for king moves
  if (move->getPiece() == Piece::King)
  {
    mKingRow[sideToMove] = move->getDestinationRow();
    mKingColumn[sideToMove] = move->getDestinationColumn();
  }

  // Update for castling moves
  if (move->isCastle())
  {
    makeCastleMove(destinationSquare);
  }

  // Update other board state
  updateCastlingRights(sourceSquare, destinationSquare);
  updateMoveClocks(move);
  updateAggregateBitBoards();
  setSideToMove(otherSide);

  return true;
}

void BitBoard::printPerformanceMetrics() const
{

}

bool BitBoard::setPosition(const std::string & fenString)
{
  Fen fen;
  if (!fen.setFromString(fenString))
  {
    std::cout << "Invalid FEN string" << std::endl;
    return false;
  }

  initBoard();

  for (uint8_t i = 0; i < 12; i++)
  {
    mBitboards[i] = 0;
  }

  for (uint8_t i = 0; i < 64; i++)
  {
    mPieces[i] = BitBoardPiece::None;
  }

  for (uint8_t i = 0; i < 8; i++)
  {
    for (uint8_t j = 0; j < 8; j++)
    {
      uint8_t index = getIndex(i, j);
      PieceType pieceType = fen.getPieceType(i, j);

      switch (pieceType)
      {
      case PieceType::WhitePawn:
        mPieces[index] = BitBoardPiece::WhitePawn;
        mBitboards[WhitePawn] |= mMask[index];
        break;
      case PieceType::WhiteRook:
        mPieces[index] = BitBoardPiece::WhiteRook;
        mBitboards[WhiteRook] |= mMask[index];
        break;
      case PieceType::WhiteKnight:
        mPieces[index] = BitBoardPiece::WhiteKnight;
        mBitboards[WhiteKnight] |= mMask[index];
        break;
      case PieceType::WhiteBishop:
        mPieces[index] = BitBoardPiece::WhiteBishop;
        mBitboards[WhiteBishop] |= mMask[index];
        break;
      case PieceType::WhiteQueen:
        mPieces[index] = BitBoardPiece::WhiteQueen;
        mBitboards[WhiteQueen] |= mMask[index];
        break;
      case PieceType::WhiteKing:
        mPieces[index] = BitBoardPiece::WhiteKing;
        mBitboards[WhiteKing] |= mMask[index];
        mKingRow[Color::White] = i;
        mKingColumn[Color::White] = j;
        break;
      case PieceType::BlackPawn:
        mPieces[index] = BitBoardPiece::BlackPawn;
        mBitboards[BlackPawn] |= mMask[index];
        break;
      case PieceType::BlackRook:
        mPieces[index] = BitBoardPiece::BlackRook;
        mBitboards[BlackRook] |= mMask[index];
        break;
      case PieceType::BlackKnight:
        mPieces[index] = BitBoardPiece::BlackKnight;
        mBitboards[BlackKnight] |= mMask[index];
        break;
      case PieceType::BlackBishop:
        mPieces[index] = BitBoardPiece::BlackBishop;
        mBitboards[BlackBishop] |= mMask[index];
        break;
      case PieceType::BlackQueen:
        mPieces[index] = BitBoardPiece::BlackQueen;
        mBitboards[BlackQueen] |= mMask[index];
        break;
      case PieceType::BlackKing:
        mPieces[index] = BitBoardPiece::BlackKing;
        mBitboards[BlackKing] |= mMask[index];
        mKingRow[Color::Black] = i;
        mKingColumn[Color::Black] = j;
        break;
      case PieceType::None:
        mPieces[index] = BitBoardPiece::None;
        break;
      }
    }
  }

  setCastlingRights(fen.getCastlingRights());
  setEnPassantColumn(fen.getEnPassantColumn());
  setFullMoveCounter(fen.getFullMoveCounter());
  setHalfMoveClock(fen.getHalfMoveClock());
  setSideToMove(fen.getSideToMove());

  updateAggregateBitBoards();

  return true;
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


bool BitBoard::unmakeMove(const Move * move)
{
  return true;
}

void BitBoard::updateAggregateBitBoards()
{
  mWhitePieceBitboard = mBlackPieceBitboard = mAllPieceBitBoard = 0;

  for (int8_t i = WhitePawn; i <= WhiteKing; i++)
  {
    mWhitePieceBitboard |= mBitboards[i];
    mAllPieceBitBoard |= mBitboards[i];
  }

  for (int8_t i = BlackPawn; i <= BlackKing; i++)
  {
    mBlackPieceBitboard |= mBitboards[i];
    mAllPieceBitBoard |= mBitboards[i];
  }
}

void BitBoard::updateCastlingRights(uint8_t fromSquare, uint8_t toSquare)
{
  uint8_t castling = this->getCastlingRights();

  switch (fromSquare)
  {
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
  default:
    break;
  }

  switch (toSquare)
  {
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
  default:
    break;
  }

  setCastlingRights(castling);
}

void BitBoard::updateMoveClocks(const Move * move)
{
  setHalfMoveClock(getHalfMoveClock() + 1);
  if (move->getPiece() == Piece::Pawn || move->isCapture())
  {
    setHalfMoveClock(0);
  }

  if (getSideToMove() == Color::Black)
  {
    setFullMoveCounter(getFullMoveNumber() + 1);
  }
}

void BitBoard::writeBitBoard(uint64_t bb, std::ostream & output) const
{
  static const char files[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};

  for (char row = 7; row >= 0; row--)
  {
    output << (row+1) << " ";
    for (char col = 0; col < 8; col++)
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
//     U64 pawnDoublePushes = ( (unmovedPawns & (~(mPiece[All] << 8)) & (~(mPiece[All] << 16)) ) ) >> 16;
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
