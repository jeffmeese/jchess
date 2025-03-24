/*!
 * \file jcl_perft.h
 *
 * This file contains the interface for the Perft object
 */

#ifndef JCL_PERFT_H
#define JCL_PERFT_H

#include "jcl_board.h"

namespace jcl
{

/*!
 * \brief Defines an object for Perft execution
 *
 * The Perft object encapsulates the perft algorithm that can
 * be used to determine the number of moves that be generated
 * from a chess position.
 *
 * The level of the perft defines the depth
 * at which the perft will be performed. For example, a perft level
 * of 1 will generate all moves for the player whose turn it is
 * to move. A perft level of 2 will generate all moves for the current
 * player as well as all response moves from the other player. Levels
 * beyond that work similarly.
 *
 * The perft algorithm can be used to both validate move generators
 * as well as quantify their performance.
 */
class Perft
{
public:

  /*!
   * \brief Constructor
   *
   * This function constructs a default Perft object
   *
   * \param board The board associated with the perft
   */
  Perft(Board * board);

  /*!
   * \brief Generates the number of moves for all current moves
   *
   * This function generates a list of all moves associated with
   * each available move for the board. That is all moves available
   * for the current board position will be determined and each move
   * will be executed followed by a perft for that move to the
   * specified perft level. This can be helpful during move generation
   * testing.
   *
   * \param perftDepth The perft level to use
   */
  void divide(int32_t perftDepth);

  /*!
   * \brief Executes the perft
   *
   * This function will execute the perft algorithm at the
   * specified depth level.
   *
   * It should be noted that perft levels beyond 6 or 7 can be
   * slow depending on how many moves are available.
   *
   * \param perftDepth The level of perft to execute
   *
   * \return The number of moves generated
   */
  uint64_t execute(int32_t perftDepth);

private:

  /*!
   * \brief Executes the perft algorithm
   *
   * This function executes the perft algorithm to the supplied depth
   *
   * \param perftDepth The level of the perft
   *
   * \return The number of moves found
   */
  uint64_t executePerft(int32_t perftDepth);

  Board * mBoard;
};

}

#endif // #ifndef JCL_PERFT_H
