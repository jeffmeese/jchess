#ifndef JCL_MOVELIST_H
#define JCL_MOVELIST_H

#include <memory>
#include <vector>

#include "jcl_move.h"

namespace jcl
{

class MoveList
{
public:
  MoveList();

public:
  uint32_t size() const;

public:
  void addMove(std::unique_ptr<Move> newMove);
  void clear();
  Move * moveAt(uint8_t index);
  const Move * moveAt(uint8_t index) const;
  void print(std::ostream & output) const;
  void removeLast();

//public:
//  const Move * operator[](uint8_t index) const;

private:
  //std::unique_ptr<Move> mMoves[256];
  std::vector<std::unique_ptr<Move>> mMoves;
  //uint8_t mTotalMoves;
};

}

#endif // #ifndef JCL_MOVELIST_H
