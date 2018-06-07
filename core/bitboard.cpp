#include "bitboard.h"

BitBoard::BitBoard()
{
  init();
}

void BitBoard::generateMoves(MoveList & moveList)
{

}

bool BitBoard::isCellAttacked(uchar row, uchar col, Color attackingColor) const
{
  return false;
}

void BitBoard::init()
{

}

void BitBoard::makeMove(const Move & move)
{

}

PieceType BitBoard::pieceType(uchar row, uchar col) const
{
  return PieceType::None;
}

void BitBoard::print(std::ostream & ouput) const
{

}

void BitBoard::setPosition(const std::string & fenString)
{

}

void BitBoard::unmakeMove(const Move & move)
{

}
