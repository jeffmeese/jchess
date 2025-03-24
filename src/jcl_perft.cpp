/*!
 * \file jcl_perft.cpp
 *
 * This file contains the implementation for the Perft object
 */

#include "jcl_perft.h"

#include <iostream>

#include "jcl_board.h"
#include "jcl_move.h"
#include "jcl_movelist.h"

namespace jcl
{

Perft::Perft(Board * board)
  : mBoard(board)
{
}

void Perft::divide(int32_t perftDepth)
{
  MoveList moveList;
  mBoard->generateMoves(moveList);
  uint64_t validMoves = 0;
  uint64_t totalNodes = 0;
  for (uint8_t i = 0; i < moveList.size(); i++)
  {
    mBoard->makeMove(moveList[i]);
    uint8_t kingRow = mBoard->getKingRow(!mBoard->getSideToMove());
    uint8_t kingCol = mBoard->getKingColumn(!mBoard->getSideToMove());
    bool attacked = mBoard->isCellAttacked(kingRow, kingCol, mBoard->getSideToMove());
    if (!attacked)
    {
      uint64_t nodes = executePerft(perftDepth - 1);
      std::string moveString = moveList[i]->toSmithNotation();
      std::cout << moveString << ": " << nodes << "\n";
      totalNodes += nodes;
      validMoves++;
    }

    mBoard->unmakeMove(moveList[i]);
  }

  std::cout << "\n";
  std::cout << "Moves: " << validMoves << "\n";
  std::cout << "Nodes: " << totalNodes << "\n";
  std::cout << std::endl;
  //std::cout << "Total time elapsed  : " << timer.get_elapsed_ms() << " milliseconds" << std::endl;
}

uint64_t Perft::execute(int32_t perftDepth)
{
  return executePerft(perftDepth);
}

uint64_t Perft::executePerft(int32_t perftDepth)
{
  if (perftDepth == 0)
  {
    return 1;
  }

  MoveList moveList;
  mBoard->generateMoves(moveList);

  uint64_t totalNodes = 0;
  for (uint32_t i = 0; i < moveList.size(); i++)
  {
    mBoard->makeMove(moveList.moveAt(i));
    uint8_t kingRow = mBoard->getKingRow(!mBoard->getSideToMove());
    uint8_t kingCol = mBoard->getKingColumn(!mBoard->getSideToMove());
    bool attacked = mBoard->isCellAttacked(kingRow, kingCol, mBoard->getSideToMove());
    if (!attacked)
    {
      totalNodes += executePerft(perftDepth-1);
    }
    mBoard->unmakeMove(moveList.moveAt(i));
  }

  return totalNodes;
}

}
