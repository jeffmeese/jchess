/*!
 * \file jcl_evaluation.cpp
 *
 * This file contains the implementation for the Evaluation object
 */

#include "jcl_evaluation.h"

#define getIndex(row,col) (((row)<<3)+(col))
#define getRow(index) ( (index >> 3) )
#define getCol(index) ( (index % 8) )

namespace jcl
{

constexpr int PAWN_WEIGHT = 100;
constexpr int KNIGHT_WEIGHT = 300;
constexpr int BISHOP_WEIGHT = 350;
constexpr int ROOK_WEIGHT = 500;
constexpr int QUEEN_WEIGHT = 950;
constexpr int KING_WEIGHT = 60000;

static const double PawnSquareValue[64] =
{
  90, 92, 94, 96, 96, 94, 92, 90,
  80, 82, 84, 86, 86, 84, 82, 80,
  60, 62, 64, 66, 66, 64, 62, 60,
  60, 62, 64, 66, 66, 64, 62, 60,
  30, 32, 34, 36, 36, 34, 32, 30,
  10, 12, 14, 16, 16, 14, 12, 10,
  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0
};

static const double BishopSquareValue[64] =
{
  0,  0,  0,  0,  0,  0,  0,  0,
  0, 30, 30, 40, 40, 30, 30,  0,
  0, 30, 30, 40, 40, 30, 30,  0,
  0, 30, 30, 50, 50, 30, 30,  0,
  0, 30, 30, 50, 50, 30, 30,  0,
  0, 30, 30, 40, 40, 30, 30,  0,
  0, 30, 30, 40, 40, 30, 30,  0,
  0,  0,  0,  0,  0,  0,  0,  0
};

static const double RookSquareValue[64] =
{
  0,  0,  0,  0,  0,  0,  0,  0,
  0, 30, 30, 40, 40, 30, 30,  0,
  0, 30, 30, 40, 40, 30, 30,  0,
  0, 30, 30, 50, 50, 30, 30,  0,
  0, 30, 30, 50, 50, 30, 30,  0,
  0, 30, 30, 40, 40, 30, 30,  0,
  0, 30, 30, 40, 40, 30, 30,  0,
  0,  0,  0,  0,  0,  0,  0,  0
};

static const double KnightSquareValue[64] =
{
  0,  0,  0,  0,  0,  0,  0,  0,
  0,  5, 10, 20, 20, 10,  5,  0,
  0, 10, 20, 30, 30, 20, 10,  0,
  0, 10, 20, 50, 50, 20, 10,  0,
  0, 10, 20, 50, 50, 20, 10,  0,
  0, 10, 20, 30, 30, 20, 10,  0,
  0,  5, 10, 20, 20, 10,  5,  0,
  0,  0,  0,  0,  0,  0,  0,  0
};

static const double QueenSquareValue[64] =
{
  0,  0,  0,  0,  0,  0,  0,  0,
  0, 30, 30, 40, 40, 30, 30,  0,
  0, 30, 30, 40, 40, 30, 30,  0,
  0, 30, 30, 50, 50, 30, 30,  0,
  0, 30, 30, 50, 50, 30, 30,  0,
  0, 30, 30, 40, 40, 30, 30,  0,
  0, 30, 30, 40, 40, 30, 30,  0,
  0,  0,  0,  0,  0,  0,  0,  0
};

static const double KingSquareValue[64] =
{
  0,  0,  0,  0,  0,  0,  0,  0,
  0,  5, 10, 20, 20, 10,  5,  0,
  0, 10, 20, 30, 30, 20, 10,  0,
  0, 10, 20, 50, 50, 20, 10,  0,
  0, 10, 20, 50, 50, 20, 10,  0,
  0, 10, 20, 30, 30, 20, 10,  0,
  0,  5, 10, 20, 20, 10,  5,  0,
  0,  0,  0,  0,  0,  0,  0,  0
};

Evaluation::Evaluation()
{

}

double Evaluation::evaluateBoard(const Board * board)
{
  double score = 0.0;
  for (uint8_t i = 0; i < 8; i++)
  {
    for (uint8_t j = 0; j < 8; j++)
    {
      uint8_t index = getIndex(i, j);

      PieceType pieceType = board->getPieceType(i, j);
      switch (pieceType)
      {
      case PieceType::WhitePawn:
        score += PAWN_WEIGHT;
        score += PawnSquareValue[index];
        break;
      case PieceType::WhiteRook:
        score += ROOK_WEIGHT;
        score += RookSquareValue[index];
        break;
      case PieceType::WhiteKnight:
        score += KNIGHT_WEIGHT;
        score += KnightSquareValue[index];
        break;
      case PieceType::WhiteBishop:
        score += BISHOP_WEIGHT;
        score += BishopSquareValue[index];
        break;
      case PieceType::WhiteQueen:
        score += QUEEN_WEIGHT;
        score += QueenSquareValue[index];
        break;
      case PieceType::WhiteKing:
        score += KING_WEIGHT;
        score += KingSquareValue[index];
        break;
      case PieceType::BlackPawn:
        score -= PAWN_WEIGHT;
        score -= PawnSquareValue[index];
        break;
      case PieceType::BlackRook:
        score -= ROOK_WEIGHT;
        score -= RookSquareValue[index];
        break;
      case PieceType::BlackKnight:
        score -= KNIGHT_WEIGHT;
        score -= KnightSquareValue[index];
        break;
      case PieceType::BlackBishop:
        score -= BISHOP_WEIGHT;
        score -= BishopSquareValue[index];
        break;
      case PieceType::BlackQueen:
        score -= QUEEN_WEIGHT;
        score -= QueenSquareValue[index];
        break;
      case PieceType::BlackKing:
        score -= KING_WEIGHT;
        score -= KingSquareValue[index];
        break;
      default:
        break;
      }
    }
  }

  return score;
}

}
