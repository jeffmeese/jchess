#ifndef TYPES_H
#define TYPES_H

typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned long long ulonglong;

enum class Color
{
  White = 0,
  Black = 1,
  None = 2
};

inline Color operator!(Color color)
{
  return (color == Color::White) ? Color::Black : Color::White;
}

enum class Piece
{
  None = 0,
  King,
  Queen,
  Rook,
  Bishop,
  Knight,
  Pawn
};

enum class PieceType
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

#endif // TYPES_H
