/*!
 * \file jcl_movelist.cpp
 *
 * This file contains the implementation for the MoveList object
 */

#include "jcl_movelist.h"

#include <iostream>
#include <sstream>
#include <stdexcept>

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

Move * MoveList::moveAt(uint8_t index)
{
  if (index >= mMoves.size())
  {
    std::ostringstream oss;
    oss << "Invalid index in move list";
    throw std::out_of_range(oss.str());
  }

  return &mMoves[index];
}

const Move * MoveList::moveAt(uint8_t index) const
{
  if (index >= mMoves.size())
  {
    std::ostringstream oss;
    oss << "Invalid index in move list";
    throw std::out_of_range(oss.str());
  }

  return &mMoves[index];
}

const Move * MoveList::operator[](uint8_t index) const
{
  if (index >= mMoves.size())
  {
    std::ostringstream oss;
    oss << "Invalid index in move list";
    throw std::out_of_range(oss.str());
  }

  return &mMoves[index];
}

void MoveList::print(std::ostream & output) const
{
  for (uint8_t i = 0; i < size(); i++)
  {
    output << mMoves[i].toSmithNotation() << "\n";
  }
}

uint32_t MoveList::size() const
{
  return static_cast<uint32_t>(mMoves.size());
}

}
