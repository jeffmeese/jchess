/*!
 * \file jcl_movelist.h
 *
 * This file contains the interface for the MoveList object
 */

#ifndef JCL_MOVELIST_H
#define JCL_MOVELIST_H

#include <vector>

#include "jcl_move.h"

namespace jcl
{

/*!
 * \brief Defines a list of moves
 *
 * The MoveList object holds a list of chess moves. It is a
 * small wrapper around a std::vector that introduces some
 * useful functions.
 */
class MoveList
{
public:

  /*!
   * \brief Constructor
   *
   * This function creates a default MoveList
   */
  MoveList();

  /*!
   * \brief Adds a move
   *
   * This function adds a move to the move list.
   *
   * \param move The move to add
   */
  void addMove(const Move & move);

  /*!
   * \brief Clears the move list
   *
   * This function removes all moves from the list.
   */
  void clear();

  /*!
   * \brief Gets a move
   *
   * This function gets a pointer to the move
   * at the specified index. If the index
   * is not valid for the list a std::out_of_range
   * exception will be thrown.
   *
   * \param index The index of the move to retrieve
   *
   * \return A pointer to the move
   */
  Move * moveAt(uint8_t index);

  /*!
   * \brief Gets a move
   *
   * This function gets a pointer to the move
   * at the specified index. If the index
   * is not valid for the list a std::out_of_range
   * exception will be thrown.
   *
   * \param index The index of the move to retrieve
   *
   * \return A pointer to the move
   */
  const Move * moveAt(uint8_t index) const;

  /*!
   * \brief Prints the move list
   *
   * This function will print all moves in the move
   * list to the supplied output stream. Each
   * move will be written in standard smith notation.
   *
   * \param output The output stream to which to print the moves
   */
  void print(std::ostream & output) const;

  /*!
   * \brief Returns the size of the move list
   *
   * This function will return the amount of moves current held
   * in the move list.
   *
   * \return
   */
  uint32_t size() const;

  /*!
   * \brief Gets a move
   *
   * This function gets a pointer to the move
   * at the specified index. If the index
   * is not valid for the list a std::out_of_range
   * exception will be thrown.
   *
   * \param index The index of the move to retrieve
   *
   * \return A pointer to the move
   */
  const Move * operator[](uint8_t index) const;

private:
  std::vector<Move> mMoves;
};

}

#endif // #ifndef JCL_MOVELIST_H
