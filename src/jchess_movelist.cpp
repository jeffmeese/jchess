#include "jchess_movelist.h"

#include <iostream>

#include "jchess_move.h"

namespace jchess
{

MoveList::MoveList()
{
}

void MoveList::addMove(std::unique_ptr<Move> newMove)
{
  mMoves.push_back(std::move(newMove));
}

void MoveList::clear()
{
  mMoves.clear();
}

// inline const Move & MoveList::operator[](uint8_t index) const
// {
//   return mMoves[index];
// }

Move * MoveList::moveAt(uint8_t index)
{
  return mMoves.at(index).get();
}

const Move * MoveList::moveAt(uint8_t index) const
{
  return mMoves.at(index).get();
}

// // Yes, there's no error checking so this could end badly
// inline void MoveList::removeLast()
// {
//   --mTotalMoves;
// }

uint8_t MoveList::size() const
{
  return mMoves.size();
}

void MoveList::print(std::ostream & output) const
{
  for (uint8_t i = 0; i < size(); i++)
  {
    output << mMoves[i]->toSmithNotation() << "\n";
  }
}

}
