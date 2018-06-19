#include "perft.h"

#include "board.h"
#include "move.h"
#include "movelist.h"
#include "plf_nanotimer.h"

#include <iostream>
#include <iomanip>
#include <map>

Perft::Perft(Board * board)
  : mBoard(board)
  , mSaveTableResults(false)
{
  resetCounters();
}

void Perft::divide(uint perftDepth)
{
  mGenMoveTime = mCellAttackTime = mMakeMoveTime = mUnmakeMoveTime = mKingTime = 0;

  plf::nanotimer timer;
  timer.start();

  MoveList moveList;
  mBoard->generateMoves(moveList);
  uint validMoves = 0;
  ulonglong totalNodes = 0;
  for (uint i = 0; i < moveList.size(); i++) {
    mBoard->makeMove(moveList[i]);
    uchar kingRow = mBoard->kingRow(!mBoard->sideToMove());
    uchar kingCol = mBoard->kingColumn(!mBoard->sideToMove());
    bool attacked = mBoard->isCellAttacked(kingRow, kingCol, mBoard->sideToMove());
    if (!attacked) {
      ulonglong nodes = executePerft(perftDepth - 1);
      std::string moveString = moveList[i].toSmithNotation();
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
  std::cout << "Total time elapsed  : " << timer.get_elapsed_ms() << " milliseconds" << std::endl;
}

ulonglong Perft::execute(uint perftDepth)
{
  resetCounters();
  return executePerft(perftDepth);
}

ulonglong Perft::executePerft(uint perftDepth)
{
  if (perftDepth == 0)
    return 1;

  MoveList moveList;
  mBoard->generateMoves(moveList);

  ulonglong totalNodes = 0;
  for (uchar i = 0; i < moveList.size(); i++) {
    mBoard->makeMove(moveList[i]);
    uchar kingRow = mBoard->kingRow(!mBoard->sideToMove());
    uchar kingCol = mBoard->kingColumn(!mBoard->sideToMove());
    bool attacked = mBoard->isCellAttacked(kingRow, kingCol, mBoard->sideToMove());
    if (!attacked) {
      totalNodes += executePerft(perftDepth-1);
    }
    mBoard->unmakeMove(moveList[i]);
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

ulonglong Perft::totalCaptures() const
{
  return mTotalCaptures;
}

ulonglong Perft::totalCastles() const
{
  return mTotalCastles;
}

ulonglong Perft::totalChecks() const
{
  return mTotalChecks;
}

ulonglong Perft::totalCheckmates() const
{
  return mTotalCheckmates;
}

ulonglong Perft::totalEnpassants() const
{
  return mTotalEnpassants;
}

ulonglong Perft::totalPromotions() const
{
  return mTotalPromotions;
}
