#ifndef ALPHABETASEARCH_H
#define ALPHABETASEARCH_H

#include "search.h"

class AlphaBetaSearch
    : public Search
{
public:
  AlphaBetaSearch();

public:
  Move search(Board * board, Evaluation * evaluation, int depth) const;

private:
  double alphaBeta(int depth, double alpha, double beta, Board * board, Evaluation * evaluation);
  double quiescence(double alpha, double beta, Board * board, Evaluation * evaluation);

private:
};

#endif // ALPHABETASEARCH_H
