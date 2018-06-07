#ifndef ENGINE_H
#define ENGINE_H

#include "board.h"
#include "move.h"

//class Board;
class Evaluation;
class Search;

class Engine
{
public:
  Engine(Board * board, Search * search, Evaluation * evaluation);

public:
  Board * board() const;
  Evaluation * evaluation() const;
  Search * search() const;
  void setBoard(Board * board);
  void setEvaluation(Evaluation * evaluation);
  void setSearch(Search * search);

public:
  Move generateMove();

private:
  Board * mBoard;
  Search * mSearch;
  Evaluation * mEvaluation;
};

#endif // #ifndef ENGINE_H
