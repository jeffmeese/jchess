#ifndef PERFT_H
#define PERFT_H

#include "board.h"
#include "types.h"

#include <iostream>
#include <iomanip>
#include <map>
#include <memory>

//class Board;

class Perft
{
public:
  Perft(Board * board);

public:
  bool saveTableResults() const;
  void setSaveTableResults(bool saveResults);
  ulonglong totalCaptures() const;
  ulonglong totalCastles() const;
  ulonglong totalChecks() const;
  ulonglong totalCheckmates() const;
  ulonglong totalEnpassants() const;
  ulonglong totalPromotions() const;

public:
  void divide(uint perftDepth);
  ulonglong execute(uint perftDepth);

private:
  ulonglong executePerft(uint perftDepth);
  void resetCounters();

private:
  Board * mBoard;
  bool mSaveTableResults;
  ulonglong mTotalCaptures;
  ulonglong mTotalCastles;
  ulonglong mTotalChecks;
  ulonglong mTotalCheckmates;
  ulonglong mTotalEnpassants;
  ulonglong mTotalPromotions;
  double mGenMoveTime;
  double mCellAttackTime;
  double mMakeMoveTime;
  double mUnmakeMoveTime;
  double mKingTime;
};

#endif // PERFT_H
