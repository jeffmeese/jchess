#include "search.h"

Search::Search()
  : mMaxDepth(3)
{
}

int Search::maxDepth() const
{
  return mMaxDepth;
}

Move Search::search(Board *board, Evaluation *evaluation) const
{
  return search(board, evaluation, mMaxDepth);
}

void Search::setMaxDepth(int maxDepth)
{
  mMaxDepth = maxDepth;
}
