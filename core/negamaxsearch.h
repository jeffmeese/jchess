#ifndef NEGAMAXSEARCH_H
#define NEGAMAXSEARCH_H

#include "search.h"

#include <fstream>

class NegamaxSearch
    : public Search
{
public:
  NegamaxSearch();

public:
  Move search(Board *board, Evaluation *evaluation, int depth) const;

private:
  double negaMax(int depth, Board * board, Evaluation * evaluation) const;

private:
  static const double INF;
};

#endif // NEGAMAXSEARCH_H
