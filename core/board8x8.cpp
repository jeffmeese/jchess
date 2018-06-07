#include "board8x8.h"
#include "fen.h"
#include "movelist.h"

#include <iostream>

// Macros for mapping (row,col)->index and vice-versa
#define getIndex(row,col) (((row)<<3)+(col))
#define getRow(index) ( (index >> 3) )
#define getCol(index) ( (index % 8) )

// Constant for invalid square position
static const uchar INVALID_SQUARE = 64;

// Attack Offsets
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

// Square index
enum Square
{
  A1=0  , B1, C1, D1, E1, F1, G1, H1,
  A2=8  , B2, C2, D2, E2, F2, G2, H2,
  A3=16 , B3, C3, D3, E3, F3, G3, H3,
  A4=24 , B4, C4, D4, E4, F4, G4, H4,
  A5=32 , B5, C5, D5, E5, F5, G5, H5,
  A6=40 , B6, C6, D6, E6, F6, G6, H6,
  A7=48 , B7, C7, D7, E7, F7, G7, H7,
  A8=56 , B8, C8, D8, E8, F8, G8, H8
};

// Constructor
Board8x8::Board8x8()
{
  init();
  initAttacks();
}

bool Board8x8::checkNonSliderAttack(uchar index, const uchar attackVector[][8], Color attackingColor, Piece piece) const
{
  for (int i = 0; i < 8; i++) {
    uchar destIndex = attackVector[index][i];
    if (destIndex != INVALID_SQUARE && mColors[destIndex] == attackingColor && mPieces[destIndex] == piece) {
      return true;
    }
  }
  return false;
}

bool Board8x8::checkPawnAttack(uchar index, const uchar attackVector[][2], Color attackingColor) const
{
  for (int i = 0; i < 2; i++) {
    uchar destIndex = attackVector[index][i];
    if (destIndex != INVALID_SQUARE && mColors[destIndex] == attackingColor && mPieces[destIndex] == Piece::Pawn) {
      return true;
    }
  }
  return false;
}

bool Board8x8::checkSliderAttack(char row, char col, char rowIncr, char colIncr, Color attackingColor, Piece piece1, Piece piece2) const
{
  for (int i = 0; i < 8; i++) {
    char destRow = row + rowIncr*(i+1);
    char destCol = col + colIncr*(i+1);
    if (destRow < 0 || destRow > 7 || destCol < 0 || destCol > 7) {
      break;
    }

    uchar destIndex = getIndex(destRow, destCol);

    // Piece of the same color
    if (mColors[destIndex] == !attackingColor) {
      break;
    }

    // Found an attacking piece
    if (mColors[destIndex] == attackingColor) {
      if (mPieces[destIndex] == piece1 || mPieces[destIndex] == piece2) {
        return true;
      }
      else {
        // This type of piece cannot attack the square from its current position
        break;
      }
    }
  }
  return false;
}

void Board8x8::generateCastlingMoves(MoveList & moveList) const
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

void Board8x8::generateMoves(MoveList & moveList) const
{
  Color side = sideToMove();

  // Generate castling moves
  generateCastlingMoves(moveList);

  // Generate all other moves
  for (uchar row = 0; row < 8; row++) {
    for (uchar col = 0; col < 8; col++) {
      uchar sq = getIndex(row, col);
      if (mColors[sq] == side) {
        Piece piece = mPieces[sq];
        switch (piece) {
        case Piece::Pawn:
          generatePawnMoves(sq, moveList);
          if (side == Color::White)
            generatePawnCaptures(sq, mWhitePawnAttacks, moveList);
          else
            generatePawnCaptures(sq, mBlackPawnAttacks, moveList);
          break;
        case Piece::Bishop:
          generateSliderMoves(sq, piece, mSliderAttacksNorthEast, moveList);
          generateSliderMoves(sq, piece, mSliderAttacksNorthWest, moveList);
          generateSliderMoves(sq, piece, mSliderAttacksSouthEast, moveList);
          generateSliderMoves(sq, piece, mSliderAttacksSouthWest, moveList);
          break;
        case Piece::Rook:
          generateSliderMoves(sq, piece, mSliderAttacksNorth, moveList);
          generateSliderMoves(sq, piece, mSliderAttacksSouth, moveList);
          generateSliderMoves(sq, piece, mSliderAttacksEast, moveList);
          generateSliderMoves(sq, piece, mSliderAttacksWest, moveList);
          break;
        case Piece::Queen:
          generateSliderMoves(sq, piece, mSliderAttacksNorthEast, moveList);
          generateSliderMoves(sq, piece, mSliderAttacksNorthWest, moveList);
          generateSliderMoves(sq, piece, mSliderAttacksSouthEast, moveList);
          generateSliderMoves(sq, piece, mSliderAttacksSouthWest, moveList);
          generateSliderMoves(sq, piece, mSliderAttacksNorth, moveList);
          generateSliderMoves(sq, piece, mSliderAttacksSouth, moveList);
          generateSliderMoves(sq, piece, mSliderAttacksEast, moveList);
          generateSliderMoves(sq, piece, mSliderAttacksWest, moveList);
          break;
        case Piece::Knight:
          generateNonSliderMoves(sq, piece, mKnightAttacks, moveList);
          break;
        case Piece::King:
          generateNonSliderMoves(sq, piece, mKingAttacks, moveList);
          break;
        }
      }
    }
  }
}

void Board8x8::generateNonSliderMoves(uchar index, Piece piece, const uchar attackVector[][8], MoveList &moveList) const
{
  Color side = sideToMove();

  for (uchar i = 0; i < 8; i++) {
    uchar destIndex = attackVector[index][i];

    // Check for valid destination
    if (destIndex != INVALID_SQUARE) {
      if (mPieces[destIndex] == Piece::None) { // Standard move
        pushMove(index, destIndex, piece, Piece::None, Piece::None, Move::Type::Quiet, moveList);
      }
      else if (mColors[destIndex] == !side) { // Capture move
        pushMove(index, destIndex, piece, mPieces[destIndex], Piece::None, Move::Type::Capture, moveList);
      }
    }
  }
}

void Board8x8::generatePawnCaptures(uchar index, const uchar attackVector[][2], MoveList & moveList) const
{
  Color side = sideToMove();

  for (uchar i = 0; i < 2; i++) {
    uchar sq = attackVector[index][i];
    uchar row = getRow(sq);

    // Check for valid destination
    if (sq != INVALID_SQUARE) {

      // Check for enemy piece
      if (mColors[sq] == !side) {
        if (row == BLACK_PROMOTION_ROW || row == WHITE_PROMOTION_ROW) { // Promotion captures
          pushMove(index, sq, Piece::Pawn, mPieces[sq], Piece::Queen, Move::Type::PromotionCapture, moveList);
          pushMove(index, sq, Piece::Pawn, mPieces[sq], Piece::Rook, Move::Type::PromotionCapture, moveList);
          pushMove(index, sq, Piece::Pawn, mPieces[sq], Piece::Bishop, Move::Type::PromotionCapture, moveList);
          pushMove(index, sq, Piece::Pawn, mPieces[sq], Piece::Knight, Move::Type::PromotionCapture, moveList);
        }
        else { // Standard capture
          pushMove(index, sq, Piece::Pawn, mPieces[sq], Piece::None, Move::Type::Capture, moveList);
        }
      }

      if (epColumn() != BoardBase::INVALID_EP) { // En-passant capture
        uchar epIndex = getIndex(epRow(), epColumn());
        char epDir = (side == Color::White) ? SOUTH : NORTH;
        if (epIndex == sq) {
          pushMove(index, sq, Piece::Pawn, mPieces[sq+epDir], Piece::None, Move::Type::EpCapture, moveList);
        }
      }
    }
  }
}

void Board8x8::generatePawnMoves(uchar index, MoveList & moveList) const
{
  Color side = sideToMove();
  char dir = (side == Color::White) ? NORTH : SOUTH;
  char startRow = (side == Color::White) ? 1 : 6;

  uchar sq = index+dir;
  uchar sq2 = index+dir+dir;

  // Check to see if we're blocked
  if (mPieces[sq] == Piece::None) {

    // Single pawn push
    uchar destRow = getRow(sq);
    if (destRow == BLACK_PROMOTION_ROW || destRow == WHITE_PROMOTION_ROW) { // Promotion moves
      pushMove(index, sq, Piece::Pawn, Piece::None, Piece::Queen, Move::Type::Promotion, moveList);
      pushMove(index, sq, Piece::Pawn, Piece::None, Piece::Rook, Move::Type::Promotion, moveList);
      pushMove(index, sq, Piece::Pawn, Piece::None, Piece::Bishop, Move::Type::Promotion, moveList);
      pushMove(index, sq, Piece::Pawn, Piece::None, Piece::Knight, Move::Type::Promotion, moveList);
    }
    else { // Standard move
      pushMove(index, sq, Piece::Pawn, Piece::None, Piece::None, Move::Type::Quiet, moveList);
    }

    if (getRow(index) == startRow && mPieces[sq2] == Piece::None) { // Double pawn push
      pushMove(index, sq2, Piece::Pawn, Piece::None, Piece::None, Move::Type::EpPush, moveList);
    }
  }
}

void Board8x8::generateSliderMoves(uchar index, Piece piece, const uchar attackVector[][8], MoveList & moveList) const
{
  Color color = sideToMove();

  for (uchar i = 0; i < 8; i++) {
    uchar destIndex = attackVector[index][i];

    // Valid slider attacks are always at the beginning of the array
    if (destIndex == INVALID_SQUARE)
      break;

    // We hit our own piece
    if (mPieces[destIndex] != Piece::None && mColors[destIndex] == color)
      break;

    if (mPieces[destIndex] == Piece::None) { // Standard move
      pushMove(index, destIndex, piece, Piece::None, Piece::None, Move::Type::Quiet, moveList);
    }
    else if (mColors[destIndex] == !color) { // Capture move, stop moving
      pushMove(index, destIndex, piece, mPieces[destIndex], Piece::None, Move::Type::Capture, moveList);
      break;
    }
  }
}

void Board8x8::init()
{
  static const Piece pieces[] = { Piece::Rook, Piece::Knight, Piece::Bishop, Piece::Queen,
                                  Piece::King, Piece::Bishop, Piece::Knight, Piece::Rook };

  for (uchar sq = 0; sq < 64; sq++) {
    mPieces[sq] = Piece::None;
    mColors[sq] = Color::None;
  }

  for (uchar col = 0; col < 8; col++) {
    mPieces[getIndex(1, col)] = Piece::Pawn;
    mColors[getIndex(1, col)] = Color::White;

    mPieces[getIndex(0, col)] = pieces[col];
    mColors[getIndex(0, col)] = Color::White;

    mPieces[getIndex(6, col)] = Piece::Pawn;
    mColors[getIndex(6, col)] = Color::Black;

    mPieces[getIndex(7, col)] = pieces[col];
    mColors[getIndex(7, col)] = Color::Black;
  }
}

void Board8x8::initAttacks()
{
  for (uchar row = 0; row < 8; row++) {
    for (uchar col = 0; col < 8; col++) {
      uchar index = getIndex(row, col);

      for (uchar i = 0; i < 2; i++) {
        mWhitePawnAttacks[index][i] = INVALID_SQUARE;
        mBlackPawnAttacks[index][i] = INVALID_SQUARE;
      }

      for (uchar i = 0; i < 8; i++) {
        mSliderAttacksEast[index][i] = INVALID_SQUARE;
        mSliderAttacksNorth[index][i] = INVALID_SQUARE;
        mSliderAttacksSouth[index][i] = INVALID_SQUARE;
        mSliderAttacksWest[index][i] = INVALID_SQUARE;
        mSliderAttacksNorthEast[index][i] = INVALID_SQUARE;
        mSliderAttacksNorthWest[index][i] = INVALID_SQUARE;
        mSliderAttacksSouthEast[index][i] = INVALID_SQUARE;
        mSliderAttacksSouthWest[index][i] = INVALID_SQUARE;
        mKingAttacks[index][i] = INVALID_SQUARE;
        mKnightAttacks[index][i] = INVALID_SQUARE;
      }
    }
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
            mKnightAttacks[index][i] = destIndex;
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
            mKingAttacks[index][i] = destIndex;
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
            mWhitePawnAttacks[index][i] = destIndex;
          }
        }
      }

      // Black pawn attacks
      for (char i = 0; i < 2; i++) {
        uchar destIndex = index + blackPawnAttacks[i];
        uchar destRow = getRow(destIndex);
        uchar destCol = getCol(destIndex);
        if (abs(destRow-sourceRow) <= 1 && abs(destCol-sourceCol) <= 1) {
          if (destRow >= 0 && destRow < 8 && destCol >= 0 && destCol < 8) {
            mBlackPawnAttacks[index][i] = destIndex;
          }
        }
      }

      // East attacks
      for (char i = 0; i < 8; i++) {
        char destCol = sourceCol + (i+1);
        if (destCol > 7)
          break;

        uchar destIndex = getIndex(sourceRow, destCol);
        mSliderAttacksEast[index][i] = destIndex;
      }

      // West attacks
      for (char i = 0; i < 8; i++) {
        char destCol = sourceCol - (i+1);
        if (destCol < 0)
          break;

        uchar destIndex = getIndex(sourceRow, destCol);
        mSliderAttacksWest[index][i] = destIndex;
      }

      // North attacks
      for (char i = 0; i < 8; i++) {
        char destRow = sourceRow + (i+1);
        if (destRow > 7)
          break;

        uchar destIndex = getIndex(destRow, sourceCol);
        mSliderAttacksNorth[index][i] = destIndex;
      }

      // South attacks
      for (char i = 0; i < 8; i++) {
        char destRow = sourceRow - (i+1);
        if (destRow < 0)
          break;

        uchar destIndex = getIndex(destRow, sourceCol);
        mSliderAttacksSouth[index][i] = destIndex;
      }

      // North east attacks
      for (char i = 0; i < 8; i++) {
        char destRow = sourceRow + (i+1);
        char destCol = sourceCol + (i+1);
        if (destRow > 7 || destCol > 7) {
          break;
        }
        uchar destIndex = getIndex(destRow, destCol);
        mSliderAttacksNorthEast[index][i] = destIndex;
      }

      // North west attacks
      for (char i = 0; i < 8; i++) {
        char destRow = sourceRow + (i+1);
        char destCol = sourceCol - (i+1);
        if (destRow > 7 || destCol < 0) {
          break;
        }
        uchar destIndex = getIndex(destRow, destCol);
        mSliderAttacksNorthWest[index][i] = destIndex;
      }

      // South west attacks
      for (char i = 0; i < 8; i++) {
        char destRow = sourceRow - (i+1);
        char destCol = sourceCol - (i+1);
        if (destRow < 0 || destCol < 0) {
          break;
        }
        uchar destIndex = getIndex(destRow, destCol);
        mSliderAttacksSouthWest[index][i] = destIndex;
      }

      // South east attacks
      for (char i = 0; i < 8; i++) {
        char destRow = sourceRow - (i+1);
        char destCol = sourceCol + (i+1);
        if (destRow < 0 || destCol > 7) {
          break;
        }
        uchar destIndex = getIndex(destRow, destCol);
        mSliderAttacksSouthEast[index][i] = destIndex;
      }
    }
  }
}

bool Board8x8::isCellAttacked(uchar row, uchar col, Color attackingColor) const
{
  return isCellAttacked(getIndex(row, col), attackingColor);
}

bool Board8x8::isCellAttacked(uchar index, Color attackingColor) const
{
  char row = getRow(index);
  char col = getCol(index);

  // Attacks from the north
  if (checkSliderAttack(row, col, 1, 0, attackingColor, Piece::Rook, Piece::Queen))
    return true;

  // Attacks from the south
  if (checkSliderAttack(row, col, -1, 0, attackingColor, Piece::Rook, Piece::Queen))
    return true;

  // Attacks from the east
  if (checkSliderAttack(row, col, 0, 1, attackingColor, Piece::Rook, Piece::Queen))
    return true;

  // Attacks from the west
  if (checkSliderAttack(row, col, 0, -1, attackingColor, Piece::Rook, Piece::Queen))
    return true;

  // Attacks from the north east
  if (checkSliderAttack(row, col, 1, 1, attackingColor, Piece::Bishop, Piece::Queen))
    return true;

  // Attacks from the north west
  if (checkSliderAttack(row, col, 1, -1, attackingColor, Piece::Bishop, Piece::Queen))
    return true;

  // Attacks from the south east
  if (checkSliderAttack(row, col, -1, 1, attackingColor, Piece::Bishop, Piece::Queen))
    return true;

  // Attacks from the south west
  if (checkSliderAttack(row, col, -1, -1, attackingColor, Piece::Bishop, Piece::Queen))
    return true;

  // Knight attacks
  if (checkNonSliderAttack(index, mKnightAttacks, attackingColor, Piece::Knight))
    return true;

  // King attacks
  if (checkNonSliderAttack(index, mKingAttacks, attackingColor, Piece::King))
    return true;

  // Pawn attacks
  // The attack vectors are flipped since we're looking from
  // the point of view of the attacked square
  // For example, a black pawn attacking this square is the same
  // as a white pawn of this square attacking the black pawns' square
  if (attackingColor == Color::Black) {
    if (checkPawnAttack(index, mWhitePawnAttacks, attackingColor))
      return true;
  }
  else {
    if (checkPawnAttack(index, mBlackPawnAttacks, attackingColor))
      return true;
  }

  return false;
}

void Board8x8::makeMove(const Move & move)
{
  Color side = sideToMove();
  uchar fromSquare = getIndex(move.sourceRow(), move.sourceCol());
  uchar toSquare = getIndex(move.destRow(), move.destCol());

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

  // Handle castle moves
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

  // Update board state
  incrementHalfMoveClock();
  if (move.piece() == Piece::Pawn || move.isCapture())
    setHalfMoveClock(0);

  toggleSideToMove();
}

PieceType Board8x8::pieceType(uchar row, uchar col) const
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

void Board8x8::pushMove(uchar from, uchar to, Piece piece, Piece capture, Piece promote, Move::Type type, MoveList & moveList) const
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

void Board8x8::setPosition(const std::string & fenString)
{
  Fen fen;
  fen.readString(fenString);

  Color sideMoving = (fen.whiteToMove()) ? Color::White : Color::Black;

  // Set the castling rights
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

  // Set board state
  setHalfMoveClock(fen.halfMoveClock());
  setFullMoveCounter(fen.fullMoveCounter());
  setSideToMove(sideMoving);

  // Set the enPassant data
  clearEpCol();
  if (fen.epColumn() >= 0) {
    setEpColumn(fen.epColumn());
  }

  // Set the pieces
  for (uchar row = 0; row < 8; row++) {
    for (uchar col = 0; col < 8; col++) {
      PieceType type = fen.pieceType(row, col);
      uchar sq = getIndex(row, col);
      switch (type) {
      case PieceType::WhitePawn:
        mPieces[sq] = Piece::Pawn;
        mColors[sq] = Color::White;
        break;
      case PieceType::WhiteRook:
        mPieces[sq] = Piece::Rook;
        mColors[sq] = Color::White;
        break;
      case PieceType::WhiteKnight:
        mPieces[sq] = Piece::Knight;
        mColors[sq] = Color::White;
        break;
      case PieceType::WhiteBishop:
        mPieces[sq] = Piece::Bishop;
        mColors[sq] = Color::White;
        break;
      case PieceType::WhiteQueen:
        mPieces[sq] = Piece::Queen;
        mColors[sq] = Color::White;
        break;
      case PieceType::WhiteKing:
        mPieces[sq] = Piece::King;
        mColors[sq] = Color::White;
        setKingRow(Color::White, row);
        setKingColumn(Color::White, col);
        break;
      case PieceType::BlackPawn:
        mPieces[sq] = Piece::Pawn;
        mColors[sq] = Color::Black;
        break;
      case PieceType::BlackRook:
        mPieces[sq] = Piece::Rook;
        mColors[sq] = Color::Black;
        break;
      case PieceType::BlackKnight:
        mPieces[sq] = Piece::Knight;
        mColors[sq] = Color::Black;
        break;
      case PieceType::BlackBishop:
        mPieces[sq] = Piece::Bishop;
        mColors[sq] = Color::Black;
        break;
      case PieceType::BlackQueen:
        mPieces[sq] = Piece::Queen;
        mColors[sq] = Color::Black;
        break;
      case PieceType::BlackKing:
        mPieces[sq] = Piece::King;
        mColors[sq] = Color::Black;
        setKingRow(Color::Black, row);
        setKingColumn(Color::Black, col);
        break;
      default:
        mPieces[sq] = Piece::None;
        mColors[sq] = Color::None;
        break;
      }
    }
  }
}

void Board8x8::unmakeMove(const Move & move)
{
  Color side = sideToMove();
  uchar fromSquare = getIndex(move.sourceRow(), move.sourceCol());
  uchar toSquare = getIndex(move.destRow(), move.destCol());
  Piece piece = move.piece();

  // Reset the board state from the move
  setFullMoveCounter(move.fullMoveCounter());
  setHalfMoveClock(move.halfMoveClock());
  setCastlingRights(move.castlingRights());
  setEpColumn(move.enPassantCol());

  // Update the piece and color arrays
  mPieces[fromSquare] = piece;
  mColors[fromSquare] = !side;
  mPieces[toSquare] = Piece::None;
  mColors[toSquare] = Color::None;

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

  toggleSideToMove();
}
