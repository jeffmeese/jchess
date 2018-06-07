#include "evaluation.h"

#include "board.h"
#include "types.h"

#include <iostream>

const int Evaluation::PAWN_WEIGHT = 100;
const int Evaluation::KNIGHT_WEIGHT = 200;
const int Evaluation::BISHOP_WEIGHT = 400;
const int Evaluation::ROOK_WEIGHT = 550;
const int Evaluation::QUEEN_WEIGHT = 950;
const int Evaluation::KING_WEIGHT = 1950;

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

double Evaluation::calculateMaterialWeight(Board * board) const
{
  int whitePawns = 0, whiteBishops = 0, whiteKnights = 0, whiteRooks = 0, whiteQueens = 0, whiteKings = 0;
  int blackPawns = 0, blackBishops = 0, blackKnights = 0, blackRooks = 0, blackQueens = 0, blackKings = 0;

  for (uchar row = 0; row < 8; row++) {
    for (uchar col = 0; col < 8; col++) {
      PieceType type = board->pieceType(row, col);
      switch (type) {
      case PieceType::WhitePawn:
        whitePawns++;
        break;
      case PieceType::WhiteKnight:
        whiteKnights++;
        break;
      case PieceType::WhiteBishop:
        whiteBishops++;
        break;
      case PieceType::WhiteRook:
        whiteRooks++;
        break;
      case PieceType::WhiteQueen:
        whiteQueens++;
        break;
      case PieceType::WhiteKing:
        whiteKings++;
        break;
      case PieceType::BlackPawn:
        blackPawns++;
        break;
      case PieceType::BlackKnight:
        blackKnights++;
        break;
      case PieceType::BlackBishop:
        blackBishops++;
        break;
      case PieceType::BlackRook:
        blackRooks++;
        break;
      case PieceType::BlackQueen:
        blackQueens++;
        break;
      case PieceType::BlackKing:
        blackKings++;
        break;
      }
    }
  }

  double materialScore = 0;
  materialScore += (whiteKings-blackKings) * KING_WEIGHT;
  materialScore += (whiteQueens-blackQueens) * QUEEN_WEIGHT;
  materialScore += (whiteRooks-blackRooks) * ROOK_WEIGHT;
  materialScore += (whiteBishops-blackBishops) * BISHOP_WEIGHT;
  materialScore += (whiteKnights-blackKnights) * KNIGHT_WEIGHT;
  materialScore += (whitePawns-blackPawns) * PAWN_WEIGHT;

  return materialScore;
}

double Evaluation::calculatePositionScore(Board *board) const
{
  double positionScore = 0.0;
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      PieceType type = board->pieceType(row, col);
      uchar index = (row * 8) + col;
      switch (type) {
        case PieceType::WhitePawn:
          positionScore += PawnSquareValue[63-index];
          break;
        case PieceType::WhiteKnight:
          positionScore += KnightSquareValue[63-index];
          break;
        case PieceType::WhiteBishop:
          positionScore += BishopSquareValue[63-index];
          break;
        case PieceType::WhiteRook:
          positionScore += RookSquareValue[63-index];
          break;
        case PieceType::WhiteQueen:
          positionScore += QueenSquareValue[63-index];
          break;
        case PieceType::WhiteKing:
          positionScore += KingSquareValue[63-index];
          break;
        case PieceType::BlackPawn:
          positionScore -= PawnSquareValue[index];
          break;
        case PieceType::BlackKnight:
          positionScore -= KnightSquareValue[index];
          break;
        case PieceType::BlackBishop:
          positionScore -= BishopSquareValue[index];
          break;
        case PieceType::BlackRook:
          positionScore -= RookSquareValue[index];
          break;
        case PieceType::BlackQueen:
          positionScore -= QueenSquareValue[index];
          break;
        case PieceType::BlackKing:
          positionScore -= KingSquareValue[index];
          break;
      }
    }
  }
  return positionScore;
}

double Evaluation::evaluate(Board *board) const
{
  double totalScore = 0.0;
  totalScore += calculateMaterialWeight(board);
  totalScore += calculatePositionScore(board);

  if (board->sideToMove() == Color::Black) {
    totalScore = -totalScore;
  }
  return totalScore;
}
