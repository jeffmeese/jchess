#include "jcl_movelist.h"

#include <iostream>

#include "jcl_move.h"

namespace jcl
{

MoveList::MoveList()
{
  mMoves.reserve(256);
}

void MoveList::addMove(const Move & move)
{
  mMoves.push_back(move);
}

void MoveList::clear()
{
  mMoves.clear();
}

const Move * MoveList::operator[](uint8_t index) const
{
  return &mMoves[index];
}

Move * MoveList::moveAt(uint8_t index)
{
  return &mMoves[index];
}

const Move * MoveList::moveAt(uint8_t index) const
{
  return &mMoves[index];
}

uint32_t MoveList::size() const
{
  return mMoves.size();
}

void MoveList::print(std::ostream & output) const
{
  for (uint8_t i = 0; i < size(); i++)
  {
    output << mMoves[i].toSmithNotation() << "\n";
  }
}

}
