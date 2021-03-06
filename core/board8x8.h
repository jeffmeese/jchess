#ifndef BOARD8X8_H
#define BOARD8X8_H

#include "boardbase.h"
#include "move.h"

class MoveList;

//! \class Board8x8
//! Represents a standard 8x8 board representation
//!
//! This is the easiest type of board representation to visualize
//! as it matches reality, however, it suffers from a perfomance
//! penalty due to the many tests required to determine when piece
//! moves or attacks off the board
class Board8x8
    : public BoardBase
{
  // Construction
public:
  Board8x8();

  // Methods
public:
  void generateMoves(MoveList & moveList) const;
  bool isCellAttacked(uchar row, uchar col, Color attackingColor) const;
  void makeMove(const Move & move);
  PieceType pieceType(uchar row, uchar col) const;
  void setPosition(const std::string & fenString);
  void unmakeMove(const Move & move);

  // Implementation
private:
  bool checkNonSliderAttack(uchar index, const uchar attackVector[][8], Color attackingColor, Piece piece) const;
  bool checkPawnAttack(uchar index, const uchar attackVector[][2], Color attackingColor) const;
  bool checkSliderAttack(char row, char col, char rowIncr, char colIncr, Color attackingColor, Piece piece1, Piece piece2) const;
  void generateCastlingMoves(MoveList & moveList) const;
  void generateNonSliderMoves(uchar index, Piece piece, const uchar attackVector[][8], MoveList & moveList) const;
  void generatePawnCaptures(uchar index, const uchar attackVector[][2], MoveList & moveList) const;
  void generatePawnMoves(uchar index, MoveList & moveList) const;
  void generateSliderMoves(uchar index, Piece piece, const uchar attackVector[][8], MoveList & moveList) const;
  void init();
  void initAttacks();
  bool isCellAttacked(uchar index, Color attackingColor) const;
  void pushMove(uchar from, uchar to, Piece piece, Piece capture, Piece promote, Move::Type type, MoveList & moveList) const;

  // Members
  // The attack vectors below are precomputed in initAttacks
  // and contain the possible destination squares for each type
  // of attack for each square on the board
  // Each sliding piece can attack a maximum of 8 squares in each possible direction (N, S, ...)
  // Kings and knights also have eight possible attacks
  // Pawns can only attack two squares (enPassant captures are handled differently)
  // Attacks that would occur off the board will contain the INVALID_SQUARE constant
private:
  Piece mPieces[64];
  Color mColors[64];
  uchar mKnightAttacks[64][8];
  uchar mKingAttacks[64][8];
  uchar mSliderAttacksNorthEast[64][8];
  uchar mSliderAttacksNorthWest[64][8];
  uchar mSliderAttacksSouthEast[64][8];
  uchar mSliderAttacksSouthWest[64][8];
  uchar mSliderAttacksNorth[64][8];
  uchar mSliderAttacksSouth[64][8];
  uchar mSliderAttacksEast[64][8];
  uchar mSliderAttacksWest[64][8];
  uchar mWhitePawnAttacks[64][2];
  uchar mBlackPawnAttacks[64][2];
};

#endif // #ifndef BOARD8X8_H
