#include "board0x88.h"

#include "fen.h"
#include "move.h"
#include "movelist.h"

#include <cassert>
#include <iostream>
#include <map>

// Values for invalid entries
static const uchar NO_PIECE_INDEX = 17;
static const uchar INVALID_SQ     = 0x88;

// Macros for mapping (row,col)->index and vice-versa and valid squares
#define getIndex(row,col) (((row)<<4)+(col))
#define getRow(index) ( (index >> 4) )
#define getCol(index) ( (index & 7) )
#define validSquare(x)  ( !((x) & INVALID_SQ ))

// Index values for piece entries
static const uchar FIRST_ROOK_INDEX    = 0;
static const uchar FIRST_KNIGHT_INDEX  = 1;
static const uchar FIRST_BISHOP_INDEX  = 2;
static const uchar QUEEN_INDEX         = 3;
static const uchar KING_INDEX          = 4;
static const uchar SECOND_BISHOP_INDEX = 5;
static const uchar SECOND_KNIGHT_INDEX = 6;
static const uchar SECOND_ROOK_INDEX   = 7;
static const uchar FIRST_PAWN_INDEX    = 8;

// Attack Offsets
static const char NORTH =  16;
static const char SOUTH = -16;
static const char EAST  =   1;
static const char WEST  =  -1;
static const char NW    =  15;
static const char NE    =  17;
static const char SW    = -17;
static const char SE    = -15;
static const char DDL   = -33;
static const char DDR   = -31;
static const char LLD   = -18;
static const char RRD   = -14;
static const char RRU   =  18;
static const char LLU   =  14;
static const char UUR   =  33;
static const char UUL   =  31;

// Attack vectors for pieces
// The king and queen attacks are identical but are both included for clarity
static const char whitePawnAttacks[] = {NE, NW};
static const char blackPawnAttacks[] = {SE, SW};
static const char rookAttacks[]      = {NORTH, SOUTH, EAST, WEST};
static const char bishopAttacks[]    = {NW, NE, SE, SW};
static const char knightAttacks[]    = {DDL, DDR, RRD, LLD, RRU, LLU, UUR, UUL};
static const char queenAttacks[]     = {NORTH, SOUTH, EAST, WEST, NW, NE, SW, SE};
static const char kingAttacks[]      = {NORTH, SOUTH, EAST, WEST, NW, NE, SW, SE};

enum Square
{
  A1=0  , B1, C1, D1, E1, F1, G1, H1,
  A2=16 , B2, C2, D2, E2, F2, G2, H2,
  A3=32 , B3, C3, D3, E3, F3, G3, H3,
  A4=48 , B4, C4, D4, E4, F4, G4, H4,
  A5=64 , B5, C5, D5, E5, F5, G5, H5,
  A6=80 , B6, C6, D6, E6, F6, G6, H6,
  A7=96 , B7, C7, D7, E7, F7, G7, H7,
  A8=112, B8, C8, D8, E8, F8, G8, H8
};

Board0x88::Board0x88()
{
  init();
}

void Board0x88::generateCastlingMoves(MoveList & moveList) const
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

void Board0x88::generateMoves(MoveList & moveList) const
{
//  generateCastlingMoves(moveList);

//  PieceEntry * pieceEntries = (sideToMove() == Color::White) ? mWhitePieces : mBlackPieces;
//  const char * pawnAttacks = (sideToMove() == Color::White) ? whitePawnAttacks : blackPawnAttacks;
//  for (uchar i = 0; i < 16; i++) {
//    Piece piece = pieceEntries[i].piece;
//    uchar sq = pieceEntries[i].sq;
//    if (validSquare(sq)) {
//      if (piece == Piece::Pawn) {
//        generatePawnMoves(sq, moveList);
//        generatePawnCaptures(sq, pawnAttacks, 2, moveList);
//      }
//      else if (piece == Piece::Bishop) {
//        generateNonPawnMoves(piece, sq, bishopAttacks, 4, true, moveList);
//      }
//      else if (piece == Piece::Rook) {
//        generateNonPawnMoves(piece, sq, rookAttacks, 4, true, moveList);
//      }
//      else if (piece == Piece::Queen) {
//        generateNonPawnMoves(piece, sq, queenAttacks, 8, true, moveList);
//      }
//      else if (piece == Piece::Knight) {
//        generateNonPawnMoves(piece, sq, knightAttacks, 8, false, moveList);
//      }
//      else if (piece == Piece::King) {
//        generateNonPawnMoves(piece, sq, kingAttacks, 8, false, moveList);
//      }
//    }
//  }

  generateCastlingMoves(moveList);

  const char * pawnAttacks = (sideToMove() == Color::White) ? whitePawnAttacks : blackPawnAttacks;
  for (uchar row = 0; row < 8; row++) {
    for (uchar col = 0; col < 8; col++) {
      uchar index = getIndex(row, col);
      if (mColors[index] == sideToMove()) {
        Piece piece = mPieces[index];
        switch (piece) {
        case Piece::Pawn:
          generatePawnMoves(index, moveList);
          generatePawnCaptures(index, pawnAttacks, 2, moveList);
          break;
        case Piece::Bishop:
          generateNonPawnMoves(piece, index, bishopAttacks, 4, true, moveList);
          break;
        case Piece::Rook:
          generateNonPawnMoves(piece, index, rookAttacks, 4, true, moveList);
          break;
        case Piece::Queen:
          generateNonPawnMoves(piece, index, queenAttacks, 8, true, moveList);
          break;
        case Piece::Knight:
          generateNonPawnMoves(piece, index, knightAttacks, 8, false, moveList);
          break;
        case Piece::King:
          generateNonPawnMoves(piece, index, kingAttacks, 8, false, moveList);
          break;
        }
      }
    }
  }
}

void Board0x88::generateNonPawnMoves(Piece piece, uchar index, const char * attackVector, uchar vectorSize, bool slider, MoveList & moveList) const
{
  Color side = sideToMove();

  for (uchar num = 0; num < vectorSize; num++) {
    for (uchar sq = index;;) {
      sq = sq + attackVector[num];

      if (!validSquare(sq))
        break;

      if (mColors[sq] == Color::None) {  // Standard move
        pushMove(index, sq, piece, Piece::None, Piece::None, Move::Type::Quiet, moveList);
      }
      else if (mColors[sq] == !side) { // Capture move
        pushMove(index, sq, piece, mPieces[sq], Piece::None, Move::Type::Capture, moveList);
        break;
      }
      else {
        break;
      }

      // Break on non-sliding pieces (King, Knight)
      if (!slider) {
        break;
      }
    }
  }
}

void Board0x88::generatePawnCaptures(uchar index, const char * attackVector, uchar vectorSize, MoveList & moveList) const
{
  Color side = sideToMove();
  char epDir = (side == Color::White) ? SOUTH : NORTH;
  uchar epIndex = getIndex(epRow(), epColumn());

  for (uchar i = 0; i < vectorSize; i++) {
    uchar sq = index + attackVector[i];
    uchar row = getRow(sq);

    // Check for enemy piece
    if (validSquare(sq) && mColors[sq] == !side) {
      if (row == BLACK_PROMOTION_ROW || row == WHITE_PROMOTION_ROW) {
        // Promotion captures
        pushMove(index, sq, Piece::Pawn, mPieces[sq], Piece::Queen, Move::Type::PromotionCapture, moveList);
        pushMove(index, sq, Piece::Pawn, mPieces[sq], Piece::Rook, Move::Type::PromotionCapture, moveList);
        pushMove(index, sq, Piece::Pawn, mPieces[sq], Piece::Bishop, Move::Type::PromotionCapture, moveList);
        pushMove(index, sq, Piece::Pawn, mPieces[sq], Piece::Knight, Move::Type::PromotionCapture, moveList);
      }
      else {
        // Standard capture
        pushMove(index, sq, Piece::Pawn, mPieces[sq], Piece::None, Move::Type::Capture, moveList);
      }
    }

    // En-passant capture
    if (validSquare(sq) && epIndex == sq) {
      pushMove(index, sq, Piece::Pawn, mPieces[sq+epDir], Piece::None, Move::Type::EpCapture, moveList);
    }
  }
}

void Board0x88::generatePawnMoves(uchar index, MoveList & moveList) const
{
  Color side = sideToMove();
  char dir = (side == Color::White) ? NORTH : SOUTH;
  char startRow = (side == Color::White) ? 1 : 6;

  uchar sq = index+dir;
  uchar sq2 = index+dir+dir;
  if (mPieces[sq] == Piece::None) {

    // Single pawn push
    uchar destRow = getRow(sq);
    if (destRow == BLACK_PROMOTION_ROW || destRow == WHITE_PROMOTION_ROW) {
      pushMove(index, sq, Piece::Pawn, Piece::None, Piece::Queen, Move::Type::Promotion, moveList);
      pushMove(index, sq, Piece::Pawn, Piece::None, Piece::Rook, Move::Type::Promotion, moveList);
      pushMove(index, sq, Piece::Pawn, Piece::None, Piece::Bishop, Move::Type::Promotion, moveList);
      pushMove(index, sq, Piece::Pawn, Piece::None, Piece::Knight, Move::Type::Promotion, moveList);
    }
    else {
      pushMove(index, sq, Piece::Pawn, Piece::None, Piece::None, Move::Type::Quiet, moveList);
    }

    // Double pawn push
    if (getRow(index) == startRow && mPieces[sq2] == Piece::None) {
      pushMove(index, sq2, Piece::Pawn, Piece::None, Piece::None, Move::Type::EpPush, moveList);
    }
  }
}

void Board0x88::init()
{
  static const Piece pieces[] = { Piece::Rook, Piece::Knight, Piece::Bishop, Piece::Queen,
                                  Piece::King, Piece::Bishop, Piece::Knight, Piece::Rook };

  static const uchar idx[] = {FIRST_ROOK_INDEX, FIRST_KNIGHT_INDEX,  FIRST_BISHOP_INDEX,  QUEEN_INDEX,
                              KING_INDEX,       SECOND_BISHOP_INDEX, SECOND_KNIGHT_INDEX, SECOND_ROOK_INDEX };

  for (uint index = 0; index < 128; index++) {
    mPieces[index] = Piece::None;
    mColors[index] = Color::None;
    mPieceIndex[index] = NO_PIECE_INDEX;
  }

  for (uint col = 0; col < 8; col++) {
    mWhitePieces[FIRST_PAWN_INDEX + col].sq = getIndex(1, col);
    mWhitePieces[FIRST_PAWN_INDEX + col].piece = Piece::Pawn;

    mWhitePieces[idx[col]].sq = getIndex(0, col);
    mWhitePieces[idx[col]].piece = pieces[col];

    mBlackPieces[FIRST_PAWN_INDEX + col].sq = getIndex(6, col);
    mBlackPieces[FIRST_PAWN_INDEX + col].piece = Piece::Pawn;

    mBlackPieces[idx[col]].sq = getIndex(7, col);
    mBlackPieces[idx[col]].piece = pieces[col];

    mPieceIndex[getIndex(0, col)] = idx[col];
    mPieceIndex[getIndex(1, col)] = FIRST_PAWN_INDEX + col;
    mPieceIndex[getIndex(6, col)] = FIRST_PAWN_INDEX + col;

    mPieceIndex[getIndex(7, col)] = idx[col];

    mColors[getIndex(0, col)] = Color::White;
    mColors[getIndex(1, col)] = Color::White;
    mColors[getIndex(6, col)] = Color::Black;
    mColors[getIndex(7, col)] = Color::Black;

    mPieces[getIndex(0, col)] = pieces[col];
    mPieces[getIndex(1, col)] = Piece::Pawn;
    mPieces[getIndex(6, col)] = Piece::Pawn;
    mPieces[getIndex(7, col)] = pieces[col];
  }
}

bool Board0x88::isCellAttacked(uchar row, uchar col, Color attackingColor) const
{
  return isCellAttacked(getIndex(row, col), attackingColor);
}

bool Board0x88::isCellAttacked(uchar index, Color attackingColor) const
{
  // Rook, Queen, and King straight attacks
  for (uchar i = 0; i < 4; i++) {
    char dir = rookAttacks[i];
    for (uchar pos = index + dir;; pos += dir) {

      if (!validSquare(pos))
        break;

      if (mPieces[pos] != Piece::None) {

        if ( (pos == index + dir) && (mColors[pos] == attackingColor) && (mPieces[pos] == Piece::King) )
          return true;

        if ( (mColors[pos] == attackingColor) && (mPieces[pos] == Piece::Queen || mPieces[pos] == Piece::Rook) )
          return true;

        break;
      }
    }
  }

  // Bishop, Queen, and King diagonal attacks
  for (uchar i = 0; i < 4; i++) {

    char dir = bishopAttacks[i];
    for (uchar pos = index + dir;; pos += dir) {

      if (!validSquare(pos))
        break;

      if (mPieces[pos] != Piece::None) {

        if ( (pos == index + dir) && (mColors[pos] == attackingColor) && (mPieces[pos] == Piece::King) )
          return true;

        if ( (mColors[pos] == attackingColor) && (mPieces[pos] == Piece::Queen || mPieces[pos] == Piece::Bishop) )
          return true;

        break;
      }
    }
  }

  // Knight Attacks
  for (uchar i = 0; i < 8; i++) {
    uchar pos = index + knightAttacks[i];
    if (validSquare(pos) && mPieces[pos] == Piece::Knight && mColors[pos] == attackingColor)
      return true;
  }

  // Pawn attacks
  const char * pawnAttacks = (attackingColor == Color::White) ? blackPawnAttacks : whitePawnAttacks;
  for (uchar i = 0; i < 2; i++) {
    uchar sq = index + pawnAttacks[i];
    if (mPieces[sq] == Piece::Pawn && mColors[sq] == attackingColor) {
      return true;
    }
  }

  return false;
}

void Board0x88::makeMove(const Move & move)
{
  Color side = sideToMove();
  uchar fromSquare = getIndex(move.sourceRow(), move.sourceCol());
  uchar toSquare = getIndex(move.destRow(), move.destCol());
  //PieceEntry * pieceEntries = (side == Color::White) ? mWhitePieces : mBlackPieces;
  //PieceEntry * oppPieces = (side == Color::White) ? mBlackPieces : mWhitePieces;

  // Get the piece index for both the to and from square
  //uchar fromIndex = mPieceIndex[fromSquare];
  //uchar toIndex = mPieceIndex[toSquare];

  // Update the color and piece arrays
  mColors[fromSquare] = Color::None;
  mPieces[fromSquare] = Piece::None;
  mColors[toSquare] = side;
  mPieces[toSquare] = move.piece();

  // Update the piece entry square and the piece index array
  //pieceEntries[fromIndex].sq = toSquare;
  //mPieceIndex[fromSquare] = NO_PIECE_INDEX;
  //mPieceIndex[toSquare] = fromIndex;

  // Handle captures
  if (move.isCapture()) {

    if (move.isEpCapture()) {
      char dir = (side == Color::White) ? SOUTH : NORTH;
      uchar sq = toSquare + dir;
      mPieces[sq] = Piece::None;
      mColors[sq] = Color::None;

      //uchar epIndex = mPieceIndex[sq];
      //mIndexList.push(epIndex);
      //oppPieces[epIndex].sq = INVALID_SQ;
    }
    else {
      //mIndexList.push(toIndex);
      //oppPieces[toIndex].sq = INVALID_SQ;
    }
  }

  // Handle promotions
  if (move.isPromotion() || move.isPromotionCapture()) {
    mPieces[toSquare] = move.promotedPiece();
    //pieceEntries[fromIndex].piece = move.promotedPiece();
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

  if (move.isCastle()) {
    switch (toSquare) {
    case C1:
      mPieces[D1] = Piece::Rook;
      mColors[D1] = Color::White;
      mPieces[A1] = Piece::None;
      mColors[A1] = Color::None;
      //pieceEntries[mPieceIndex[A1]].sq = D1;
      //pieceEntries[mPieceIndex[E1]].sq = C1;
      break;
    case G1:
      mPieces[F1] = Piece::Rook;
      mColors[F1] = Color::White;
      mPieces[H1] = Piece::None;
      mColors[H1] = Color::None;
      //pieceEntries[mPieceIndex[H1]].sq = F1;
      //pieceEntries[mPieceIndex[E1]].sq = G1;
      break;
    case C8:
      mPieces[D8] = Piece::Rook;
      mColors[D8] = Color::Black;
      mPieces[A8] = Piece::None;
      mColors[A8] = Color::None;
      //pieceEntries[mPieceIndex[A8]].sq = D8;
      //pieceEntries[mPieceIndex[E8]].sq = C8;
      break;
    case G8:
      mPieces[F8] = Piece::Rook;
      mColors[F8] = Color::Black;
      mPieces[H8] = Piece::None;
      mColors[H8] = Color::None;
      //pieceEntries[mPieceIndex[H8]].sq = F8;
      //pieceEntries[mPieceIndex[E8]].sq = G8;
      break;
    }
  }

  incrementHalfMoveClock();
  if (move.piece() == Piece::Pawn || move.isCapture())
    setHalfMoveClock(0);

  toggleSideToMove();
}

PieceType Board0x88::pieceType(uchar row, uchar col) const
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

void Board0x88::pushMove(uchar from, uchar to, Piece piece, Piece capture, Piece promote, Move::Type type, MoveList & moveList) const
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

void Board0x88::setPosition(const std::string &fenString)
{
  Fen fen;
  fen.readString(fenString);

  uchar castling = CASTLE_NONE;
  if (fen.canBlackCastleKingSide())
    castling |= CASTLE_BLACK_KING;
  if (fen.canBlackCastleQueenSide())
    castling |= CASTLE_BLACK_QUEEN;
  if (fen.canWhiteCastleKingSide())
    castling |= CASTLE_WHITE_KING;
  if (fen.canWhiteCastleQueenSide())
    castling |= CASTLE_WHITE_QUEEN;

  setCastlingRights(castling);
  setHalfMoveClock(fen.halfMoveClock());
  setFullMoveCounter(fen.fullMoveCounter());

  Color color = (fen.whiteToMove()) ? Color::White : Color::Black;
  setSideToMove(color);

  clearEpCol();
  if (fen.epColumn() >= 0) {
    setEpColumn(fen.epColumn());
  }

  uchar whiteIndex = 0, blackIndex = 0;
  for (uchar row = 0; row < 8; row++) {
    for (uchar col = 0; col < 8; col++) {
      PieceType type = fen.pieceType(row, col);
      uchar sq = getIndex(row, col);
      switch (type) {
      case PieceType::WhitePawn:
        mPieces[sq] = Piece::Pawn;
        mColors[sq] = Color::White;
        //mWhitePieces[whiteIndex].sq = sq;
        //mWhitePieces[whiteIndex].piece = Piece::Pawn;
        //mPieceIndex[sq] = whiteIndex;
        //whiteIndex++;
        break;
      case PieceType::WhiteRook:
        mPieces[sq] = Piece::Rook;
        mColors[sq] = Color::White;
        //mWhitePieces[whiteIndex].sq = sq;
        //mWhitePieces[whiteIndex].piece = Piece::Rook;
        //mPieceIndex[sq] = whiteIndex;
        //whiteIndex++;
        break;
      case PieceType::WhiteKnight:
        mPieces[sq] = Piece::Knight;
        mColors[sq] = Color::White;
        //mWhitePieces[whiteIndex].sq = sq;
        //mWhitePieces[whiteIndex].piece = Piece::Knight;
        //mPieceIndex[sq] = whiteIndex;
        //whiteIndex++;
        break;
      case PieceType::WhiteBishop:
        mPieces[sq] = Piece::Bishop;
        mColors[sq] = Color::White;
        //mWhitePieces[whiteIndex].sq = sq;
        //mWhitePieces[whiteIndex].piece = Piece::Bishop;
        //mPieceIndex[sq] = whiteIndex;
        //whiteIndex++;
        break;
      case PieceType::WhiteQueen:
        mPieces[sq] = Piece::Queen;
        mColors[sq] = Color::White;
        //mWhitePieces[whiteIndex].sq = sq;
        //mWhitePieces[whiteIndex].piece = Piece::Queen;
        //mPieceIndex[sq] = whiteIndex;
        //whiteIndex++;
        break;
      case PieceType::WhiteKing:
        mPieces[sq] = Piece::King;
        mColors[sq] = Color::White;
        setKingRow(Color::White, row);
        setKingColumn(Color::White, col);
        //mWhitePieces[whiteIndex].sq = sq;
        //mWhitePieces[whiteIndex].piece = Piece::King;
        //mPieceIndex[sq] = whiteIndex;
        //whiteIndex++;
        break;
      case PieceType::BlackPawn:
        mPieces[sq] = Piece::Pawn;
        mColors[sq] = Color::Black;
        //mBlackPieces[blackIndex].sq = sq;
        //mBlackPieces[blackIndex].piece = Piece::Pawn;
        //mPieceIndex[sq] = blackIndex;
        //blackIndex++;
        break;
      case PieceType::BlackRook:
        mPieces[sq] = Piece::Rook;
        mColors[sq] = Color::Black;
        //mBlackPieces[blackIndex].sq = sq;
        //mBlackPieces[blackIndex].piece = Piece::Rook;
        //mPieceIndex[sq] = blackIndex;
        //blackIndex++;
        break;
      case PieceType::BlackKnight:
        mPieces[sq] = Piece::Knight;
        mColors[sq] = Color::Black;
        //mBlackPieces[blackIndex].sq = sq;
        //mBlackPieces[blackIndex].piece = Piece::Knight;
        //mPieceIndex[sq] = blackIndex;
        //blackIndex++;
        break;
      case PieceType::BlackBishop:
        mPieces[sq] = Piece::Bishop;
        mColors[sq] = Color::Black;
        //mBlackPieces[blackIndex].sq = sq;
        //mBlackPieces[blackIndex].piece = Piece::Bishop;
        //mPieceIndex[sq] = blackIndex;
        //blackIndex++;
        break;
      case PieceType::BlackQueen:
        mPieces[sq] = Piece::Queen;
        mColors[sq] = Color::Black;
        //mBlackPieces[blackIndex].sq = sq;
        //mBlackPieces[blackIndex].piece = Piece::Queen;
        //mPieceIndex[sq] = blackIndex;
        //blackIndex++;
        break;
      case PieceType::BlackKing:
        mPieces[sq] = Piece::King;
        mColors[sq] = Color::Black;
        setKingRow(Color::Black, row);
        setKingColumn(Color::Black, col);
        //mBlackPieces[blackIndex].sq = sq;
        //mBlackPieces[blackIndex].piece = Piece::King;
        //mPieceIndex[sq] = blackIndex;
        //blackIndex++;
        break;
      default:
        mPieces[sq] = Piece::None;
        mColors[sq] = Color::None;
        break;
      }
    }
  }

}

void Board0x88::unmakeMove(const Move & move)
{
  // Update the board state from the move
  setFullMoveCounter(move.fullMoveCounter());
  setHalfMoveClock(move.halfMoveClock());
  setCastlingRights(move.castlingRights());
  setEpColumn(move.enPassantCol());

  Color side = sideToMove();
  uchar fromSquare = getIndex(move.sourceRow(), move.sourceCol());
  uchar toSquare = getIndex(move.destRow(), move.destCol());
  Piece piece = move.piece();
  //PieceEntry * pieceEntries = (side == Color::White) ? mBlackPieces : mWhitePieces;
  //PieceEntry * oppPieces = (side == Color::White) ? mWhitePieces : mBlackPieces;

  // Update the piece and color arrays
  mPieces[fromSquare] = piece;
  mColors[fromSquare] = !side;
  mPieces[toSquare] = Piece::None;
  mColors[toSquare] = Color::None;

  // Update the piece index array and the piece entry
  //uchar toIndex = mPieceIndex[toSquare];
  //mPieceIndex[fromSquare] = toIndex;
  //mPieceIndex[toSquare] = NO_PIECE_INDEX;
  //pieceEntries[toIndex].sq = fromSquare;

  // Handle captures
  if (move.isCapture()) {
    uchar sq = toSquare;
    if (move.isEpCapture()) {
      char dir = (side == Color::White) ? NORTH : SOUTH;
      sq = toSquare + dir;
    }

    mPieces[sq] = move.capturePiece();
    mColors[sq] = side;

    //uchar captureIndex = mIndexList.top();
    //mIndexList.pop();
    //assert(captureIndex != NO_PIECE_INDEX);

    //mPieceIndex[sq] = captureIndex;
    //oppPieces[captureIndex].sq = sq;
  }

  // Handle promotions
  if (move.isPromotion() || move.isPromotionCapture()) {
    mPieces[fromSquare] = Piece::Pawn;
    //pieceEntries[toIndex].piece = Piece::Pawn;
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
      //pieceEntries[mPieceIndex[C1]].sq = E1;
      //pieceEntries[mPieceIndex[D1]].sq = A1;
      break;
    case G1:
      mPieces[H1] = Piece::Rook;
      mColors[H1] = Color::White;
      mPieces[F1] = Piece::None;
      mColors[F1] = Color::None;
      //pieceEntries[mPieceIndex[G1]].sq = E1;
      //pieceEntries[mPieceIndex[F1]].sq = H1;
      break;
    case C8:
      mPieces[A8] = Piece::Rook;
      mColors[A8] = Color::Black;
      mPieces[D8] = Piece::None;
      mColors[D8] = Color::None;
      //pieceEntries[mPieceIndex[C8]].sq = E8;
      //pieceEntries[mPieceIndex[D8]].sq = A8;
      break;
    case G8:
      mPieces[H8] = Piece::Rook;
      mColors[H8] = Color::Black;
      mPieces[F8] = Piece::None;
      mColors[F8] = Color::None;
      //pieceEntries[mPieceIndex[G8]].sq = E8;
      //pieceEntries[mPieceIndex[F8]].sq = H8;
      break;
    }
  }

  toggleSideToMove();
}
