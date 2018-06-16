#include "movelist.h"
#include "move.h"

void MoveList::print(std::ostream & output) const
{
  for (uchar i = 0; i < size(); i++) {
    output << mMoves[i].toSmithNotation() << "\n";
  }
}
