#include "negamaxsearch.h"

#include "board.h"
#include "evaluation.h"
#include "movelist.h"
#include "types.h"

#include <iostream>
#include <iomanip>

const double NegamaxSearch::INF = 1e10;

NegamaxSearch::NegamaxSearch()
{
}

double NegamaxSearch::negaMax(int depth, Board * board, Evaluation * evaluation) const
{
  if (depth == 0) {
    double score = evaluation->evaluate(board);
    return score;
  }

  MoveList moveList;
  board->generateMoves(moveList);

  double maxScore = -INF;
  for (uchar i = 0; i < moveList.size(); i++) {
    Move currentMove = moveList[i];

    board->makeMove(currentMove);
    uchar kingRow = board->kingRow(board->sideToMove());
    uchar kingCol = board->kingColumn(board->sideToMove());
    if (!board->isCellAttacked(kingRow, kingCol, !board->sideToMove())) {
      double score = -negaMax(depth-1, board, evaluation);
      if (score > maxScore) {
        maxScore = score;
      }
    }
    board->unmakeMove(currentMove);
  }

  return maxScore;
}

Move NegamaxSearch::search(Board *board, Evaluation *evaluation, int depth) const
{
  std::cout << "Performing negaMax search\n";
  MoveList moveList;
  board->generateMoves(moveList);

  int bestMoveIndex = -1;
  double maxScore = -INF;
  for (uchar i = 0; i < moveList.size(); i++) {
    Move currentMove = moveList[i];
    board->makeMove(currentMove);
    uchar kingRow = board->kingRow(board->sideToMove());
    uchar kingCol = board->kingColumn(board->sideToMove());
    if (!board->isCellAttacked(kingRow, kingCol, !board->sideToMove())) {
      std::cout << currentMove.toSmithNotation() << " ";
      double score = -negaMax(depth, board, evaluation);
      std::cout << score << " ";
      if (score > maxScore) {
        maxScore = score;
        bestMoveIndex = i;
      }
    }
    std::cout << maxScore << "\n";
    board->unmakeMove(currentMove);
  }

  return moveList[bestMoveIndex];
}
