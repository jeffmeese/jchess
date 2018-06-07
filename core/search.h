#ifndef SEARCH_H
#define SEARCH_H

#include "board.h"
#include "move.h"

class Evaluation;
class Move;

class Search
{
public:
  int maxDepth() const;
  void setMaxDepth(int maxDepth);

public:
  Move search(Board * board, Evaluation * evaluation) const;
  virtual Move search(Board * board, Evaluation * evaluation, int depth) const = 0;

protected:
  Search();

private:
  bool mUseIterativeDeepening;
  int mMaxDepth;
};

#endif // #ifndef SEARCH_H
