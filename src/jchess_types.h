#ifndef JCHESS_TYPES_H
#define JCHESS_TYPES_H

#include <cstdint>

namespace jchess
{

enum class Color : int32_t
{
  White = 0,
  Black = 1,
  None = 2
};

inline Color operator!(Color color)
{
  return (color == Color::White) ? Color::Black : Color::White;
}

enum class Piece : int32_t
{
  None = 0,
  King,
  Queen,
  Rook,
  Bishop,
  Knight,
  Pawn
};

enum class PieceType : int32_t
{
  None = 0,
  WhitePawn = 1,
  WhiteRook = 2,
  WhiteKnight = 3,
  WhiteBishop = 4,
  WhiteQueen = 5,
  WhiteKing = 6,
  BlackPawn = -1,
  BlackRook = -2,
  BlackKnight = -3,
  BlackBishop = -4,
  BlackQueen = -5,
  BlackKing = -6
};

}

#endif // #ifndef JCHESS_TYPES_H
