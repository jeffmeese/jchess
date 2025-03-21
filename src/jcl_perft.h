#ifndef JCL_PERFT_H
#define JCL_PERFT_H

#include "jcl_board.h"

namespace jcl
{

class Perft
{
public:
  Perft(Board * board);

public:
  bool saveTableResults() const;
  void setSaveTableResults(bool saveResults);
  uint64_t totalCaptures() const;
  uint64_t totalCastles() const;
  uint64_t totalChecks() const;
  uint64_t totalCheckmates() const;
  uint64_t totalEnpassants() const;
  uint64_t totalPromotions() const;

public:
  void divide(int32_t perftDepth);
  uint64_t execute(int32_t perftDepth);

private:
  uint64_t executePerft(int32_t perftDepth);
  void resetCounters();

private:
  Board * mBoard;
  bool mSaveTableResults;
  uint64_t mTotalCaptures;
  uint64_t mTotalCastles;
  uint64_t mTotalChecks;
  uint64_t mTotalCheckmates;
  uint64_t mTotalEnpassants;
  uint64_t mTotalPromotions;
  double mGenMoveTime;
  double mCellAttackTime;
  double mMakeMoveTime;
  double mUnmakeMoveTime;
  double mKingTime;
};

}

#endif // #ifndef JCL_PERFT_H
