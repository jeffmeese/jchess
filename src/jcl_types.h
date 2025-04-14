/*!
 * \file jcl_types.h
 *
 * This file contains the definitions of various types used throughout the jcl library
 */

#ifndef JCL_TYPES_H
#define JCL_TYPES_H

#include <cstdint>

namespace jcl
{

/*!
 * \brief Defines a color
 *
 * The Color enumeration defines a color associated
 * with a piece or square on the chess board.
 */
enum class Color : int32_t
{
  White = 0,   /*!< A white piece or square */
  Black = 1,   /*!< A black piece or square */
  None = 2     /*!< A square with no piece */
};

inline Color operator!(Color color)
{
  return (color == Color::White) ? Color::Black : Color::White;
}

/*!
 * \brief Defines a piece
 *
 * The Piece enumeration defines a piece without
 * regard to its color. Sometimes only the piece
 * itself is important and the color is unimportant.
 */
enum class Piece : int32_t
{
  None = 0,   /*!< Defines no piece */
  King,       /*!< Defines a King */
  Queen,      /*!< Defines a Queen */
  Rook,       /*!< Defines a Rook */
  Bishop,     /*!< Defines a Bishop */
  Knight,     /*!< Defines a Knight */
  Pawn        /*!< Defines a Pawn */
};

/*!
 * \brief Defines a piece
 *
 * The PieceType enumeration defines a piece
 * based on both its type and its color. This can
 * be important in distinguishing the exact piece
 * at a particular square on the board.
 */
enum class PieceType : int32_t
{
  None = 0,          /*!< Defines no piece */
  WhitePawn = 1,     /*!< Defines a white pawn */
  WhiteRook = 2,     /*!< Defines a white rook */
  WhiteKnight = 3,   /*!< Defines a white knight */
  WhiteBishop = 4,   /*!< Defines a white bishop */
  WhiteQueen = 5,    /*!< Defines a white queen */
  WhiteKing = 6,     /*!< Defines a white king */
  BlackPawn = 7,     /*!< Defines a black pawn */
  BlackRook = 8,     /*!< Defines a black rook */
  BlackKnight = 9,   /*!< Defines a black knight */
  BlackBishop = 10,  /*!< Defines a black bishop */
  BlackQueen = 11,   /*!< Defines a black queen */
  BlackKing = 12     /*!< Defines a black king */
};

}

#endif // #ifndef JCL_TYPES_H
