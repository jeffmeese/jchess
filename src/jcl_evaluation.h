#ifndef JCL_EVALUATION_H
#define JCL_EVALUATION_H

#include "jcl_board.h"

namespace jcl
{

class Evaluation
{
public:
  Evaluation();

  double evaluateBoard(const Board * board);
};

}
#endif // #ifndef JCL_EVALUATION_H
