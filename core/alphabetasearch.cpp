#include "alphabetasearch.h"
#include "evaluation.h"
#include "movelist.h"

AlphaBetaSearch::AlphaBetaSearch()
{
}

double AlphaBetaSearch::alphaBeta(int depth, double alpha, double beta, Board * board, Evaluation * evaluation)
{
  if (depth == 0) {
    return quiescence(alpha, beta, board, evaluation);
  }

  MoveList moveList;
  board->generateMoves(moveList);

  return 0.0;
}

double AlphaBetaSearch::quiescence(double alpha, double beta, Board * board, Evaluation * evaluation)
{
  return 0.0;
}

Move AlphaBetaSearch::search(Board *board, Evaluation *evaluation, int depth) const
{
  Move m(0,0,1,1,Piece::Pawn);
  return m;
}
