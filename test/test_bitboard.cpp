#include "gtest/gtest.h"

#include "jcl_bitboard.h"

class BitboardTest : public testing::Test
{
protected:
  BitboardTest()
  {

  }

  void verifyStandardBoard()
  {
    EXPECT_EQ(mBitBoard.getPieceType(0, 0), jcl::PieceType::WhiteRook);
    EXPECT_EQ(mBitBoard.getPieceType(0, 1), jcl::PieceType::WhiteKnight);
    EXPECT_EQ(mBitBoard.getPieceType(0, 2), jcl::PieceType::WhiteBishop);
    EXPECT_EQ(mBitBoard.getPieceType(0, 3), jcl::PieceType::WhiteQueen);
    EXPECT_EQ(mBitBoard.getPieceType(0, 4), jcl::PieceType::WhiteKing);
    EXPECT_EQ(mBitBoard.getPieceType(0, 5), jcl::PieceType::WhiteBishop);
    EXPECT_EQ(mBitBoard.getPieceType(0, 6), jcl::PieceType::WhiteKnight);
    EXPECT_EQ(mBitBoard.getPieceType(0, 7), jcl::PieceType::WhiteRook);

    for (uint8_t i = 0; i < 8; i++)
    {
      EXPECT_EQ(mBitBoard.getPieceType(1, i), jcl::PieceType::WhitePawn);
    }

    EXPECT_EQ(mBitBoard.getPieceType(7, 0), jcl::PieceType::BlackRook);
    EXPECT_EQ(mBitBoard.getPieceType(7, 1), jcl::PieceType::BlackKnight);
    EXPECT_EQ(mBitBoard.getPieceType(7, 2), jcl::PieceType::BlackBishop);
    EXPECT_EQ(mBitBoard.getPieceType(7, 3), jcl::PieceType::BlackQueen);
    EXPECT_EQ(mBitBoard.getPieceType(7, 4), jcl::PieceType::BlackKing);
    EXPECT_EQ(mBitBoard.getPieceType(7, 5), jcl::PieceType::BlackBishop);
    EXPECT_EQ(mBitBoard.getPieceType(7, 6), jcl::PieceType::BlackKnight);
    EXPECT_EQ(mBitBoard.getPieceType(7, 7), jcl::PieceType::BlackRook);

    for (uint8_t i = 0; i < 8; i++)
    {
      EXPECT_EQ(mBitBoard.getPieceType(6, i), jcl::PieceType::BlackPawn);
    }

    EXPECT_EQ(mBitBoard.getEnpassantColumn(), jcl::Board::INVALID_ENPASSANT_COLUMN);
    EXPECT_EQ(mBitBoard.getHalfMoveClock(), 0);
    EXPECT_EQ(mBitBoard.getFullMoveNumber(), 1);
    EXPECT_EQ(mBitBoard.getCastlingRights(), jcl::Board::CASTLE_WHITE_KING | jcl::Board::CASTLE_WHITE_QUEEN | jcl::Board::CASTLE_BLACK_KING | jcl::Board::CASTLE_BLACK_QUEEN);

    EXPECT_EQ(mBitBoard.getPawns(jcl::Color::White),   0x000000000000ff00);
    EXPECT_EQ(mBitBoard.getRooks(jcl::Color::White),   0x0000000000000081);
    EXPECT_EQ(mBitBoard.getKnights(jcl::Color::White), 0x0000000000000042);
    EXPECT_EQ(mBitBoard.getBishops(jcl::Color::White), 0x0000000000000024);
    EXPECT_EQ(mBitBoard.getQueens(jcl::Color::White),  0x0000000000000008);
    EXPECT_EQ(mBitBoard.getKings(jcl::Color::White),   0x0000000000000010);
    EXPECT_EQ(mBitBoard.getPieces(jcl::Color::White),  0x000000000000ffff);
    EXPECT_EQ(mBitBoard.getPawns(jcl::Color::Black),   0x00ff000000000000);
    EXPECT_EQ(mBitBoard.getRooks(jcl::Color::Black),   0x8100000000000000);
    EXPECT_EQ(mBitBoard.getKnights(jcl::Color::Black), 0x4200000000000000);
    EXPECT_EQ(mBitBoard.getBishops(jcl::Color::Black), 0x2400000000000000);
    EXPECT_EQ(mBitBoard.getQueens(jcl::Color::Black),  0x0800000000000000);
    EXPECT_EQ(mBitBoard.getKings(jcl::Color::Black),   0x1000000000000000);
    EXPECT_EQ(mBitBoard.getPieces(jcl::Color::Black),  0xffff000000000000);
  }

protected:
  jcl::BitBoard mBitBoard;
};

TEST_F(BitboardTest, TestConstruction)
{
  verifyStandardBoard();
}

TEST_F(BitboardTest, TestSetPosition)
{
  EXPECT_EQ(mBitBoard.setPosition("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"), true);
  verifyStandardBoard();

  //EXPECT_EQ(mBitBoard.setPosition("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"), true);
}

TEST_F(BitboardTest, TestDoublePushMove)
{
  uint64_t x = 0x000000001000ef00;
  jcl::Move move(1, 4, 3, 4, jcl::Piece::Pawn);
  move.setType(jcl::Move::Type::EpPush);
  mBitBoard.makeMove(&move);

  EXPECT_EQ(mBitBoard.getPieceType(3, 4), jcl::PieceType::WhitePawn);
  EXPECT_EQ(mBitBoard.getPieceType(1, 4), jcl::PieceType::None);
  EXPECT_EQ(mBitBoard.getPawns(jcl::Color::White), x);
  EXPECT_EQ(mBitBoard.getEnpassantColumn(), 4);

}

TEST_F(BitboardTest, TestCapture)
{
  mBitBoard.setPosition("rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 2");
}
// TEST(BitboardTest, TestConstuction)
// {
//   jcl::BitBoard bitBoard;
//   EXPECT_EQ(bitBoard.getPieceType(0, 0), jcl::PieceType::WhiteRook);
//   EXPECT_EQ(bitBoard.getPieceType(0, 1), jcl::PieceType::WhiteKnight);
//   EXPECT_EQ(bitBoard.getPieceType(0, 2), jcl::PieceType::WhiteBishop);
//   EXPECT_EQ(bitBoard.getPieceType(0, 3), jcl::PieceType::WhiteQueen);
//   EXPECT_EQ(bitBoard.getPieceType(0, 4), jcl::PieceType::WhiteKing);
//   EXPECT_EQ(bitBoard.getPieceType(0, 5), jcl::PieceType::WhiteBishop);
//   EXPECT_EQ(bitBoard.getPieceType(0, 6), jcl::PieceType::WhiteKnight);
//   EXPECT_EQ(bitBoard.getPieceType(0, 7), jcl::PieceType::WhiteRook);

//   for (uint8_t i = 0; i < 8; i++)
//   {
//     EXPECT_EQ(bitBoard.getPieceType(1, i), jcl::PieceType::WhitePawn);
//   }

//   EXPECT_EQ(bitBoard.getPieceType(7, 0), jcl::PieceType::BlackRook);
//   EXPECT_EQ(bitBoard.getPieceType(7, 1), jcl::PieceType::BlackKnight);
//   EXPECT_EQ(bitBoard.getPieceType(7, 2), jcl::PieceType::BlackBishop);
//   EXPECT_EQ(bitBoard.getPieceType(7, 3), jcl::PieceType::BlackQueen);
//   EXPECT_EQ(bitBoard.getPieceType(7, 4), jcl::PieceType::BlackKing);
//   EXPECT_EQ(bitBoard.getPieceType(7, 5), jcl::PieceType::BlackBishop);
//   EXPECT_EQ(bitBoard.getPieceType(7, 6), jcl::PieceType::BlackKnight);
//   EXPECT_EQ(bitBoard.getPieceType(7, 7), jcl::PieceType::BlackRook);

//   for (uint8_t i = 0; i < 8; i++)
//   {
//     EXPECT_EQ(bitBoard.getPieceType(6, i), jcl::PieceType::BlackPawn);
//   }

//   EXPECT_EQ(bitBoard.getEnpassantColumn(), jcl::Board::INVALID_ENPASSANT_COLUMN);
//   EXPECT_EQ(bitBoard.getHalfMoveClock(), 0);
//   EXPECT_EQ(bitBoard.getFullMoveNumber(), 1);
//   EXPECT_EQ(bitBoard.getCastlingRights(), jcl::Board::CASTLE_WHITE_KING | jcl::Board::CASTLE_WHITE_QUEEN | jcl::Board::CASTLE_BLACK_KING | jcl::Board::CASTLE_BLACK_QUEEN);

//   EXPECT_EQ(bitBoard.getPawns(jcl::Color::White), 0x000000000000ff00);
//   EXPECT_EQ(bitBoard.getRooks(jcl::Color::White), 0x0000000000000081);
//   EXPECT_EQ(bitBoard.getPawns(jcl::Color::Black), 0x00ff000000000000);
// }

// TEST(BitboardTest, TestSetboard)
// {
//   jcl::BitBoard bitBoard;

//   EXPECT_EQ(bitBoard.setPosition("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"), true);

//   EXPECT_EQ(bitBoard.getPieceType(0, 0), jcl::PieceType::WhiteRook);
//   EXPECT_EQ(bitBoard.getPieceType(0, 1), jcl::PieceType::WhiteKnight);
//   EXPECT_EQ(bitBoard.getPieceType(0, 2), jcl::PieceType::WhiteBishop);
//   EXPECT_EQ(bitBoard.getPieceType(0, 3), jcl::PieceType::WhiteQueen);
//   EXPECT_EQ(bitBoard.getPieceType(0, 4), jcl::PieceType::WhiteKing);
//   EXPECT_EQ(bitBoard.getPieceType(0, 5), jcl::PieceType::WhiteBishop);
//   EXPECT_EQ(bitBoard.getPieceType(0, 6), jcl::PieceType::WhiteKnight);
//   EXPECT_EQ(bitBoard.getPieceType(0, 7), jcl::PieceType::WhiteRook);

//   for (uint8_t i = 0; i < 8; i++)
//   {
//     EXPECT_EQ(bitBoard.getPieceType(1, i), jcl::PieceType::WhitePawn);
//   }

//   EXPECT_EQ(bitBoard.getPieceType(7, 0), jcl::PieceType::BlackRook);
//   EXPECT_EQ(bitBoard.getPieceType(7, 1), jcl::PieceType::BlackKnight);
//   EXPECT_EQ(bitBoard.getPieceType(7, 2), jcl::PieceType::BlackBishop);
//   EXPECT_EQ(bitBoard.getPieceType(7, 3), jcl::PieceType::BlackQueen);
//   EXPECT_EQ(bitBoard.getPieceType(7, 4), jcl::PieceType::BlackKing);
//   EXPECT_EQ(bitBoard.getPieceType(7, 5), jcl::PieceType::BlackBishop);
//   EXPECT_EQ(bitBoard.getPieceType(7, 6), jcl::PieceType::BlackKnight);
//   EXPECT_EQ(bitBoard.getPieceType(7, 7), jcl::PieceType::BlackRook);

//   for (uint8_t i = 0; i < 8; i++)
//   {
//     EXPECT_EQ(bitBoard.getPieceType(6, i), jcl::PieceType::BlackPawn);
//   }

//   EXPECT_EQ(bitBoard.getEnpassantColumn(), jcl::Board::INVALID_ENPASSANT_COLUMN);
//   EXPECT_EQ(bitBoard.getHalfMoveClock(), 0);
//   EXPECT_EQ(bitBoard.getFullMoveNumber(), 1);
//   EXPECT_EQ(bitBoard.getCastlingRights(), jcl::Board::CASTLE_WHITE_KING | jcl::Board::CASTLE_WHITE_QUEEN | jcl::Board::CASTLE_BLACK_KING | jcl::Board::CASTLE_BLACK_QUEEN);

//   EXPECT_EQ(bitBoard.setPosition("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0"), false);
// }
