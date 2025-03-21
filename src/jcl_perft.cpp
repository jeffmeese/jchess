#include "jcl_perft.h"

#include "jcl_board.h"
#include "jcl_move.h"
#include "jcl_movelist.h"
//#include "plf_nanotimer.h"

#include <iostream>
#include <iomanip>
#include <map>

namespace jcl
{

Perft::Perft(Board * board)
  : mBoard(board)
  , mSaveTableResults(false)
{
  resetCounters();
}

void Perft::divide(int32_t perftDepth)
{
  mGenMoveTime = mCellAttackTime = mMakeMoveTime = mUnmakeMoveTime = mKingTime = 0;

  // plf::nanotimer timer;
  // timer.start();

  // MoveList moveList;
  // mBoard->generateMoves(moveList);
  // uint validMoves = 0;
  // uint64_t totalNodes = 0;
  // for (uint i = 0; i < moveList.size(); i++) {
  //   mBoard->makeMove(moveList[i]);
  //   uint8_t kingRow = mBoard->kingRow(!mBoard->sideToMove());
  //   uint8_t kingCol = mBoard->kingColumn(!mBoard->sideToMove());
  //   bool attacked = mBoard->isCellAttacked(kingRow, kingCol, mBoard->sideToMove());
  //   if (!attacked) {
  //     uint64_t nodes = executePerft(perftDepth - 1);
  //     std::string moveString = moveList[i].toSmithNotation();
  //     std::cout << moveString << ": " << nodes << "\n";
  //     totalNodes += nodes;
  //     validMoves++;
  //   }

  //   mBoard->unmakeMove(moveList[i]);
  // }

  // std::cout << "\n";
  // std::cout << "Moves: " << validMoves << "\n";
  // std::cout << "Nodes: " << totalNodes << "\n";
  // std::cout << std::endl;
  // std::cout << "Total time elapsed  : " << timer.get_elapsed_ms() << " milliseconds" << std::endl;
}

uint64_t Perft::execute(int32_t perftDepth)
{
  resetCounters();
  return executePerft(perftDepth);
}

uint64_t Perft::executePerft(int32_t perftDepth)
{
  //std::cout << perftDepth << "\n";

  if (perftDepth == 0)
  {
    return 1;
  }

  MoveList moveList;
  mBoard->generateMoves(moveList);
  //std::cout << moveList.size() << std::endl;

  uint64_t totalNodes = 0;
  for (int32_t i = 0; i < moveList.size(); i++)
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

void Perft::resetCounters()
{
  mTotalCaptures = 0;
  mTotalCastles = 0;
  mTotalCheckmates = 0;
  mTotalChecks = 0;
  mTotalEnpassants = 0;
  mTotalPromotions = 0;
}

bool Perft::saveTableResults() const
{
  return mSaveTableResults;
}

void Perft::setSaveTableResults(bool saveResults)
{
  mSaveTableResults = saveResults;
}

uint64_t Perft::totalCaptures() const
{
  return mTotalCaptures;
}

uint64_t Perft::totalCastles() const
{
  return mTotalCastles;
}

uint64_t Perft::totalChecks() const
{
  return mTotalChecks;
}

uint64_t Perft::totalCheckmates() const
{
  return mTotalCheckmates;
}

uint64_t Perft::totalEnpassants() const
{
  return mTotalEnpassants;
}

uint64_t Perft::totalPromotions() const
{
  return mTotalPromotions;
}

}
