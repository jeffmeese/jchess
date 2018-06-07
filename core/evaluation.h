#ifndef EVALUATION_H
#define EVALUATION_H

#include "board.h"
//class Board;

class Evaluation
{
public:
  Evaluation();

public:
  double evaluate(Board * board) const;

private:
  double calculateMaterialWeight(Board * board) const;
  double calculatePositionScore(Board * board) const;

private:
  static const int PAWN_WEIGHT;
  static const int KNIGHT_WEIGHT;
  static const int BISHOP_WEIGHT;
  static const int ROOK_WEIGHT;
  static const int QUEEN_WEIGHT;
  static const int KING_WEIGHT;
};

#endif // #ifndef EVALUATION_H
