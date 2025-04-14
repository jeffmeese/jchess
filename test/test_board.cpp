#include "gtest/gtest.h"

#include "jcl_board.h"
#include "jcl_fen.h"
#include "jcl_move.h"
#include "jcl_movelist.h"
#include "jcl_types.h"

class MockBoard : public jcl::Board
{
protected:

  // Override
  bool doGenerateMoves(jcl::MoveList & moveList) const override
  {
    return true;
  }

  bool doGenerateMoves(uint8_t row, uint8_t col, jcl::MoveList & moveList) const override
  {
    return true;
  }

  jcl::PieceType doGetPieceType(uint8_t row, uint8_t col) const override
  {
    return jcl::PieceType::None;
  }

  bool doIsCellAttacked(uint8_t row, uint8_t col, jcl::Color attackingColor) const override
  {
    return true;
  }

  bool doMakeMove(const jcl::Move * move) override
  {
    return true;
  }

  void doReset() override
  {
  }

  bool doSetPieceType(uint8_t row, uint8_t col, jcl::PieceType pieceType) override
  {
    return true;
  }

  bool doSetPosition(const jcl::Fen & fen) override
  {
    return true;
  }

  bool doUnmakeMove(const jcl::Move * move) override
  {
    return true;
  }
};

class BoardTest : public testing::Test
{
protected:
  BoardTest()
  {
  }

protected:
  MockBoard mBoard;
};

TEST_F(BoardTest, TestConstruction)
{
  uint8_t castlingRights = 0;
  castlingRights |= jcl::Board::CASTLE_WHITE_KING|jcl::Board::CASTLE_WHITE_QUEEN;
  castlingRights |= jcl::Board::CASTLE_BLACK_KING|jcl::Board::CASTLE_BLACK_QUEEN;

  EXPECT_EQ(mBoard.getSideToMove(), jcl::Color::White);
  EXPECT_EQ(mBoard.getCastlingRights(), castlingRights);
  EXPECT_EQ(mBoard.getEnpassantColumn(), jcl::Board::INVALID_ENPASSANT_COLUMN);
  EXPECT_EQ(mBoard.getHalfMoveClock(), 0);
  EXPECT_EQ(mBoard.getFullMoveNumber(), 1);
  EXPECT_EQ(mBoard.getKingRow(jcl::Color::White), 0);
  EXPECT_EQ(mBoard.getKingColumn(jcl::Color::White), 4);
  EXPECT_EQ(mBoard.getKingRow(jcl::Color::Black), 7);
  EXPECT_EQ(mBoard.getKingColumn(jcl::Color::Black), 4);
}
