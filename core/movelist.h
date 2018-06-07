#ifndef MOVELIST_H
#define MOVELIST_H

#include "move.h"

class MoveList
{
public:
  MoveList();

public:
  uchar size() const;

public:
  void addMove(const Move & newMove);
  void clear();
  Move moveAt(uchar index) const;
  void removeLast();

public:
  const Move & operator[](uchar index) const;

private:
  Move mMoves[256];
  uchar mTotalMoves;
};

inline MoveList::MoveList()
  : mTotalMoves(0)
{
}

inline void MoveList::addMove(const Move & newMove)
{
  mMoves[mTotalMoves++] = newMove;
}

inline void MoveList::clear()
{
  mTotalMoves = 0;
}

inline const Move & MoveList::operator[](uchar index) const
{
  return mMoves[index];
}

inline Move MoveList::moveAt(uchar index) const
{
  return mMoves[index];
}

// Yes, there's no error checking so this could end badly
inline void MoveList::removeLast()
{
  --mTotalMoves;
}

inline uchar MoveList::size() const
{
  return mTotalMoves;
}

#endif // MOVELIST_H
