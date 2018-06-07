#include "engine.h"
#include "move.h"
#include "search.h"

Engine::Engine(Board *board, Search *search, Evaluation *evaluation)
{
  setBoard(board);
  setSearch(search);
  setEvaluation(evaluation);
}

Move Engine::generateMove()
{
  return mSearch->search(mBoard, mEvaluation);
}

Board * Engine::board() const
{
  return mBoard;
}

Evaluation * Engine::evaluation() const
{
  return mEvaluation;
}

Search * Engine::search() const
{
  return mSearch;
}

void Engine::setBoard(Board *board)
{
  mBoard = board;
}

void Engine::setEvaluation(Evaluation *evaluation)
{
  mEvaluation = evaluation;
}

void Engine::setSearch(Search *search)
{
  mSearch = search;
}
