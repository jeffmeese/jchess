#include "gtest/gtest.h"

#include "jcl_bitboard.h"

TEST(BitboardTest, TestConstuction)
{
  jcl::BitBoard bitBoard;
  EXPECT_EQ(bitBoard.getPieceType(0, 0), jcl::PieceType::WhiteRook);
  EXPECT_EQ(bitBoard.getPieceType(0, 1), jcl::PieceType::WhiteKnight);
  EXPECT_EQ(bitBoard.getPieceType(0, 2), jcl::PieceType::WhiteBishop);
  EXPECT_EQ(bitBoard.getPieceType(0, 3), jcl::PieceType::WhiteQueen);
  EXPECT_EQ(bitBoard.getPieceType(0, 4), jcl::PieceType::WhiteKing);
  EXPECT_EQ(bitBoard.getPieceType(0, 5), jcl::PieceType::WhiteBishop);
  EXPECT_EQ(bitBoard.getPieceType(0, 6), jcl::PieceType::WhiteKnight);
  EXPECT_EQ(bitBoard.getPieceType(0, 7), jcl::PieceType::WhiteRook);

  for (uint8_t i = 0; i < 8; i++)
  {
    EXPECT_EQ(bitBoard.getPieceType(1, i), jcl::PieceType::WhitePawn);
  }

  EXPECT_EQ(bitBoard.getPieceType(7, 0), jcl::PieceType::BlackRook);
  EXPECT_EQ(bitBoard.getPieceType(7, 1), jcl::PieceType::BlackKnight);
  EXPECT_EQ(bitBoard.getPieceType(7, 2), jcl::PieceType::BlackBishop);
  EXPECT_EQ(bitBoard.getPieceType(7, 3), jcl::PieceType::BlackQueen);
  EXPECT_EQ(bitBoard.getPieceType(7, 4), jcl::PieceType::BlackKing);
  EXPECT_EQ(bitBoard.getPieceType(7, 5), jcl::PieceType::BlackBishop);
  EXPECT_EQ(bitBoard.getPieceType(7, 6), jcl::PieceType::BlackKnight);
  EXPECT_EQ(bitBoard.getPieceType(7, 7), jcl::PieceType::BlackRook);

  for (uint8_t i = 0; i < 8; i++)
  {
    EXPECT_EQ(bitBoard.getPieceType(6, i), jcl::PieceType::BlackPawn);
  }

  EXPECT_EQ(bitBoard.getEnpassantColumn(), jcl::Board::INVALID_ENPASSANT_COLUMN);
  EXPECT_EQ(bitBoard.getHalfMoveClock(), 0);
  EXPECT_EQ(bitBoard.getFullMoveNumber(), 1);
  EXPECT_EQ(bitBoard.getCastlingRights(), jcl::Board::CASTLE_WHITE_KING | jcl::Board::CASTLE_WHITE_QUEEN | jcl::Board::CASTLE_BLACK_KING | jcl::Board::CASTLE_BLACK_QUEEN);

  EXPECT_EQ(bitBoard.getPawns(jcl::Color::White), 0x000000000000ff00);
  EXPECT_EQ(bitBoard.getRooks(jcl::Color::White), 0x0000000000000081);
  EXPECT_EQ(bitBoard.getPawns(jcl::Color::Black), 0x00ff000000000000);
}

TEST(BitboardTest, TestSetboard)
{
  jcl::BitBoard bitBoard;

  EXPECT_EQ(bitBoard.setPosition("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"), true);

  EXPECT_EQ(bitBoard.getPieceType(0, 0), jcl::PieceType::WhiteRook);
  EXPECT_EQ(bitBoard.getPieceType(0, 1), jcl::PieceType::WhiteKnight);
  EXPECT_EQ(bitBoard.getPieceType(0, 2), jcl::PieceType::WhiteBishop);
  EXPECT_EQ(bitBoard.getPieceType(0, 3), jcl::PieceType::WhiteQueen);
  EXPECT_EQ(bitBoard.getPieceType(0, 4), jcl::PieceType::WhiteKing);
  EXPECT_EQ(bitBoard.getPieceType(0, 5), jcl::PieceType::WhiteBishop);
  EXPECT_EQ(bitBoard.getPieceType(0, 6), jcl::PieceType::WhiteKnight);
  EXPECT_EQ(bitBoard.getPieceType(0, 7), jcl::PieceType::WhiteRook);

  for (uint8_t i = 0; i < 8; i++)
  {
    EXPECT_EQ(bitBoard.getPieceType(1, i), jcl::PieceType::WhitePawn);
  }

  EXPECT_EQ(bitBoard.getPieceType(7, 0), jcl::PieceType::BlackRook);
  EXPECT_EQ(bitBoard.getPieceType(7, 1), jcl::PieceType::BlackKnight);
  EXPECT_EQ(bitBoard.getPieceType(7, 2), jcl::PieceType::BlackBishop);
  EXPECT_EQ(bitBoard.getPieceType(7, 3), jcl::PieceType::BlackQueen);
  EXPECT_EQ(bitBoard.getPieceType(7, 4), jcl::PieceType::BlackKing);
  EXPECT_EQ(bitBoard.getPieceType(7, 5), jcl::PieceType::BlackBishop);
  EXPECT_EQ(bitBoard.getPieceType(7, 6), jcl::PieceType::BlackKnight);
  EXPECT_EQ(bitBoard.getPieceType(7, 7), jcl::PieceType::BlackRook);

  for (uint8_t i = 0; i < 8; i++)
  {
    EXPECT_EQ(bitBoard.getPieceType(6, i), jcl::PieceType::BlackPawn);
  }

  EXPECT_EQ(bitBoard.getEnpassantColumn(), jcl::Board::INVALID_ENPASSANT_COLUMN);
  EXPECT_EQ(bitBoard.getHalfMoveClock(), 0);
  EXPECT_EQ(bitBoard.getFullMoveNumber(), 1);
  EXPECT_EQ(bitBoard.getCastlingRights(), jcl::Board::CASTLE_WHITE_KING | jcl::Board::CASTLE_WHITE_QUEEN | jcl::Board::CASTLE_BLACK_KING | jcl::Board::CASTLE_BLACK_QUEEN);

  EXPECT_EQ(bitBoard.setPosition("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0"), false);
}
