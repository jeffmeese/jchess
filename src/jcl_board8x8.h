#ifndef JCL_BOARD8X8_H
#define JCL_BOARD8X8_H

#include <map>

#include "jcl_board.h"
#include "jcl_move.h"

namespace jcl
{

class MoveList;

//! \class Board8x8
//! Represents a standard 8x8 board representation
//!
//! This is the easiest type of board representation to visualize
//! as it matches reality, however, it suffers from a perfomance
//! penalty due to the many tests required to determine when piece
//! moves or attacks off the board
class Board8x8
    : public Board
{
public:

  // Construction
  Board8x8();

  // Methods
  bool generateMoves(MoveList & moveList) const override;
  bool generateMoves(uint8_t row, uint8_t col, MoveList & moveList) const override;
  uint8_t getKingColumn(Color color) const override;
  uint8_t getKingRow(Color color) const override;
  PieceType getPieceType(uint8_t row, uint8_t col) const override;
  bool isCellAttacked(uint8_t row, uint8_t col, Color attackColor) const override;
  bool makeMove(const Move * move) override;
  bool setPosition(const std::string & fenString) override;
  bool unmakeMove(const Move * move) override;

protected:
  void doReset() override;

private:

  // Implementation
  bool checkAttack(uint8_t index,
                   int8_t rowIncrement,
                   int8_t colIncrement,
                   Color attackColor) const;

  void generateCastlingMoves(MoveList & moveList) const;

  bool generateMoves(uint8_t row,
                     uint8_t col,
                     MoveList & moveList,
                     bool generateCastling) const;

  bool generateMoves(uint8_t index,
                     MoveList & moveList) const;

  void generatePawnMoves(uint8_t index,
                         Color sideToMove,
                         MoveList & moveList) const;

  void generateSliderMoves(uint8_t index,
                           int8_t rowIncrement,
                           int8_t colIncrement,
                           bool slider,
                           bool king,
                           MoveList & moveList) const;
  void initBoard();
  void initMoves();
  bool isCellAttacked(uint8_t index, Color attackColor) const;
  void pushMove(uint8_t from,
                uint8_t to,
                Piece piece,
                Piece capturedPiece,
                Piece promotedPiece,
                Move::Type type,
                MoveList & moveList) const;

  // Members
  Piece mPieces[64];
  Color mColors[64];
  std::map<Color, uint8_t> mKingColumn;
  std::map<Color, uint8_t> mKingRow;
  uint8_t mWhitePawnMoves[64][4];
  uint8_t mBlackPawnMoves[64][4];
  uint8_t mBishopMoves[64][4];
  uint8_t mQueenMoves[64][8];
  uint8_t mRookMoves[64][4];

  // Methods
//public:
  // void generateMoves(MoveList & moveList) const;
  // bool isCellAttacked(uchar row, uchar col, Color attackingColor) const;
  // void makeMove(const Move & move);
  // PieceType pieceType(uchar row, uchar col) const;
  // void setPosition(const std::string & fenString);
  // void unmakeMove(const Move & move);

  // Implementation
//private:
  // bool checkNonSliderAttack(uchar index, const uchar attackVector[][8], Color attackingColor, Piece piece) const;
  // bool checkPawnAttack(uchar index, const uchar attackVector[][2], Color attackingColor) const;
  // bool checkSliderAttack(char row, char col, char rowIncr, char colIncr, Color attackingColor, Piece piece1, Piece piece2) const;
  // void generateCastlingMoves(MoveList & moveList) const;
  // void generateNonSliderMoves(uchar index, Piece piece, const uchar attackVector[][8], MoveList & moveList) const;
  // void generatePawnCaptures(uchar index, const uchar attackVector[][2], MoveList & moveList) const;
  // void generatePawnMoves(uchar index, MoveList & moveList) const;
  // void generateSliderMoves(uchar index, Piece piece, const uchar attackVector[][8], MoveList & moveList) const;
  // void init();
  // void initAttacks();
  // bool isCellAttacked(uchar index, Color attackingColor) const;
  // void pushMove(uchar from, uchar to, Piece piece, Piece capture, Piece promote, Move::Type type, MoveList & moveList) const;

  // Members
  // The attack vectors below are precomputed in initAttacks
  // and contain the possible destination squares for each type
  // of attack for each square on the board
  // Each sliding piece can attack a maximum of 8 squares in each possible direction (N, S, ...)
  // Kings and knights also have eight possible attacks
  // Pawns can only attack two squares (enPassant captures are handled differently)
  // Attacks that would occur off the board will contain the INVALID_SQUARE constant
//private:
  // Piece mPieces[64];
  // Color mColors[64];
  // uchar mKnightAttacks[64][8];
  // uchar mKingAttacks[64][8];
  // uchar mSliderAttacksNorthEast[64][8];
  // uchar mSliderAttacksNorthWest[64][8];
  // uchar mSliderAttacksSouthEast[64][8];
  // uchar mSliderAttacksSouthWest[64][8];
  // uchar mSliderAttacksNorth[64][8];
  // uchar mSliderAttacksSouth[64][8];
  // uchar mSliderAttacksEast[64][8];
  // uchar mSliderAttacksWest[64][8];
  // uchar mWhitePawnAttacks[64][2];
  // uchar mBlackPawnAttacks[64][2];
};

}

#endif // #ifndef JCL_BOARD8X8_H
