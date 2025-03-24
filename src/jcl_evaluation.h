/*!
 * \file jcl_evaluation.h
 *
 * This file contains the interface for the Evaluation object
 */

#ifndef JCL_EVALUATION_H
#define JCL_EVALUATION_H

#include "jcl_board.h"

namespace jcl
{

/*!
 * \brief Defines the Evaluation class
 *
 * The Evaluation class defines the evaluation of the chess
 * board at any given time in the game.
 *
 * The evaulateBoard function will return a score for the
 * board position from the standpoint of the white player.
 * Positive scores mean that white has a better position
 * while negative scores mean the black has a better position.
 */
class Evaluation
{
public:

  /*!
   * \brief Constructor
   *
   * Defines a default Evaluation object
   */
  Evaluation();

  /*!
   * \brief Evaluates the board
   *
   * This function provides a numerical score for the specified
   * board, with positive scores meaning white has a better
   * position, while negative scores meaning black has a better
   * position.
   *
   * \param board The board to evaluate
   *
   * \return The score for the board position
   */
  double evaluateBoard(const Board * board);
};

}
#endif // #ifndef JCL_EVALUATION_H
