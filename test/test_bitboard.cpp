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
  uint64_t pawnPosition = 0x000000001000ef00;
  jcl::Move move(1, 4, 3, 4, jcl::Piece::Pawn);
  move.setType(jcl::Move::Type::EpPush);

  EXPECT_EQ(mBitBoard.makeMove(&move), true);
  EXPECT_EQ(mBitBoard.getPieceType(3, 4), jcl::PieceType::WhitePawn);
  EXPECT_EQ(mBitBoard.getPieceType(1, 4), jcl::PieceType::None);
  EXPECT_EQ(mBitBoard.getPawns(jcl::Color::White), pawnPosition);
  EXPECT_EQ(mBitBoard.getEnpassantColumn(), 4);

}

TEST_F(BitboardTest, TestCapture)
{
   uint64_t pawnPosition = 0x000000080000ef00;
  mBitBoard.setPosition("rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 2");

  jcl::Move move(3, 4, 4, 3, jcl::Piece::Pawn);
  move.setType(jcl::Move::Type::Capture);
  EXPECT_EQ(mBitBoard.makeMove(&move), true);
  EXPECT_EQ(mBitBoard.getPieceType(4, 3), jcl::PieceType::WhitePawn);
  EXPECT_EQ(mBitBoard.getPieceType(1, 4), jcl::PieceType::None);
  EXPECT_EQ(mBitBoard.getPawns(jcl::Color::White), pawnPosition);
  EXPECT_EQ(mBitBoard.getEnpassantColumn(), jcl::Board::INVALID_ENPASSANT_COLUMN);
  EXPECT_EQ(mBitBoard.getHalfMoveClock(), 0);
}

TEST_F(BitboardTest, TestWhitePromotion)
{
  uint64_t whitePieces = 0x100000000000000;
  mBitBoard.setPosition("4k3/P7/8/8/8/8/8/4K3 w - - 0 2");

  jcl::Move queenMove(6, 0, 7, 0, jcl::Piece::Pawn);
  queenMove.setType(jcl::Move::Type::Promotion);
  queenMove.setPromotedPiece(jcl::Piece::Queen);
  EXPECT_EQ(mBitBoard.makeMove(&queenMove), true);
  EXPECT_EQ(mBitBoard.getPieceType(7, 0), jcl::PieceType::WhiteQueen);
  EXPECT_EQ(mBitBoard.getPieceType(6, 0), jcl::PieceType::None);
  EXPECT_EQ(mBitBoard.getQueens(jcl::Color::White), whitePieces);

  mBitBoard.setPosition("4k3/P7/8/8/8/8/8/4K3 w - - 0 2");

  jcl::Move rookMove(6, 0, 7, 0, jcl::Piece::Pawn);
  rookMove.setType(jcl::Move::Type::Promotion);
  rookMove.setPromotedPiece(jcl::Piece::Rook);
  EXPECT_EQ(mBitBoard.makeMove(&rookMove), true);
  EXPECT_EQ(mBitBoard.getPieceType(7, 0), jcl::PieceType::WhiteRook);
  EXPECT_EQ(mBitBoard.getPieceType(6, 0), jcl::PieceType::None);
  EXPECT_EQ(mBitBoard.getRooks(jcl::Color::White), whitePieces);

  mBitBoard.setPosition("4k3/P7/8/8/8/8/8/4K3 w - - 0 2");

  jcl::Move bishopMove(6, 0, 7, 0, jcl::Piece::Pawn);
  bishopMove.setType(jcl::Move::Type::Promotion);
  bishopMove.setPromotedPiece(jcl::Piece::Bishop);
  EXPECT_EQ(mBitBoard.makeMove(&bishopMove), true);
  EXPECT_EQ(mBitBoard.getPieceType(7, 0), jcl::PieceType::WhiteBishop);
  EXPECT_EQ(mBitBoard.getPieceType(6, 0), jcl::PieceType::None);
  EXPECT_EQ(mBitBoard.getBishops(jcl::Color::White), whitePieces);

  mBitBoard.setPosition("4k3/P7/8/8/8/8/8/4K3 w - - 0 2");

  jcl::Move knightMove(6, 0, 7, 0, jcl::Piece::Pawn);
  knightMove.setType(jcl::Move::Type::Promotion);
  knightMove.setPromotedPiece(jcl::Piece::Knight);
  EXPECT_EQ(mBitBoard.makeMove(&knightMove), true);
  EXPECT_EQ(mBitBoard.getPieceType(7, 0), jcl::PieceType::WhiteKnight);
  EXPECT_EQ(mBitBoard.getPieceType(6, 0), jcl::PieceType::None);
  EXPECT_EQ(mBitBoard.getKnights(jcl::Color::White), whitePieces);
}

TEST_F(BitboardTest, TestBlackPromotion)
{
  uint64_t postMovePieces = 0x00000000000080;
  mBitBoard.setPosition("4k3/8/8/8/8/8/7p/4K3 b - - 0 2");

  jcl::Move queenMove(1, 7, 0, 7, jcl::Piece::Pawn);
  queenMove.setType(jcl::Move::Type::Promotion);
  queenMove.setPromotedPiece(jcl::Piece::Queen);
  EXPECT_EQ(mBitBoard.makeMove(&queenMove), true);
  EXPECT_EQ(mBitBoard.getPieceType(0, 7), jcl::PieceType::BlackQueen);
  EXPECT_EQ(mBitBoard.getPieceType(1, 7), jcl::PieceType::None);
  EXPECT_EQ(mBitBoard.getQueens(jcl::Color::Black), postMovePieces);

  mBitBoard.setPosition("4k3/8/8/8/8/8/7p/4K3 b - - 0 2");

  jcl::Move rookMove(1, 7, 0, 7, jcl::Piece::Pawn);
  rookMove.setType(jcl::Move::Type::Promotion);
  rookMove.setPromotedPiece(jcl::Piece::Rook);
  EXPECT_EQ(mBitBoard.makeMove(&rookMove), true);
  EXPECT_EQ(mBitBoard.getPieceType(0, 7), jcl::PieceType::BlackRook);
  EXPECT_EQ(mBitBoard.getPieceType(1, 7), jcl::PieceType::None);
  EXPECT_EQ(mBitBoard.getRooks(jcl::Color::Black), postMovePieces);

  mBitBoard.setPosition("4k3/8/8/8/8/8/7p/4K3 b - - 0 2");

  jcl::Move bishopMove(1, 7, 0, 7, jcl::Piece::Pawn);
  bishopMove.setType(jcl::Move::Type::Promotion);
  bishopMove.setPromotedPiece(jcl::Piece::Bishop);
  EXPECT_EQ(mBitBoard.makeMove(&bishopMove), true);
  EXPECT_EQ(mBitBoard.getPieceType(0, 7), jcl::PieceType::BlackBishop);
  EXPECT_EQ(mBitBoard.getPieceType(1, 7), jcl::PieceType::None);
  EXPECT_EQ(mBitBoard.getBishops(jcl::Color::Black), postMovePieces);

  mBitBoard.setPosition("4k3/8/8/8/8/8/7p/4K3 b - - 0 2");

  jcl::Move knightMove(1, 7, 0, 7, jcl::Piece::Pawn);
  knightMove.setType(jcl::Move::Type::Promotion);
  knightMove.setPromotedPiece(jcl::Piece::Knight);
  EXPECT_EQ(mBitBoard.makeMove(&knightMove), true);
  EXPECT_EQ(mBitBoard.getPieceType(0, 7), jcl::PieceType::BlackKnight);
  EXPECT_EQ(mBitBoard.getPieceType(1, 7), jcl::PieceType::None);
  EXPECT_EQ(mBitBoard.getKnights(jcl::Color::Black), postMovePieces);
}

TEST_F(BitboardTest, TestWhiteEnPassantCapture)
{
  mBitBoard.setPosition("rnbqkbnr/pp1ppppp/8/2pP4/8/8/PPP1PPPP/RNBQKBNR w KQkq c6 0 1");

  uint64_t blackPawns = 0x00fb000000000000;
  uint64_t whitePawns = 0x000004000000f700;
  jcl::Move move(4, 3, 5, 2, jcl::Piece::Pawn);
  move.setType(jcl::Move::Type::EpCapture);
  move.setCapturedPiece(jcl::Piece::Pawn);
  EXPECT_EQ(mBitBoard.makeMove(&move), true);
  EXPECT_EQ(mBitBoard.getPawns(jcl::Color::White), whitePawns);
  EXPECT_EQ(mBitBoard.getPawns(jcl::Color::Black), blackPawns);
  EXPECT_EQ(mBitBoard.getSideToMove(), jcl::Color::Black);
  EXPECT_EQ(mBitBoard.getEnpassantColumn(), jcl::Board::INVALID_ENPASSANT_COLUMN);
}

TEST_F(BitboardTest, TestBlackEnPassantCapture)
{
  mBitBoard.setPosition("rnbqkbnr/ppp1pppp/8/8/2Pp4/8/PP1PPPPP/RNBQKBNR b KQkq c3 0 1");

  uint64_t blackPawns = 0x00f7000000040000;
  uint64_t whitePawns = 0x000000000000fb00;
  jcl::Move move(3, 3, 2, 2, jcl::Piece::Pawn);
  move.setType(jcl::Move::Type::EpCapture);
  move.setCapturedPiece(jcl::Piece::Pawn);
  EXPECT_EQ(mBitBoard.makeMove(&move), true);
  EXPECT_EQ(mBitBoard.getPawns(jcl::Color::White), whitePawns);
  EXPECT_EQ(mBitBoard.getPawns(jcl::Color::Black), blackPawns);
  EXPECT_EQ(mBitBoard.getEnpassantColumn(), jcl::Board::INVALID_ENPASSANT_COLUMN);
}

TEST_F(BitboardTest, TestKingMoves)
{
  mBitBoard.setPosition("rnbqkbnr/ppp1pppp/8/8/2Pp4/8/PP1PPPPP/RNBQKBNR b KQkq c3 0 1");

  jcl::Move blackMove(7, 4, 6, 4, jcl::Piece::King);
  EXPECT_EQ(mBitBoard.makeMove(&blackMove), true);

  uint8_t castlingRights = mBitBoard.getCastlingRights();
  EXPECT_EQ(castlingRights & jcl::Board::CASTLE_BLACK_KING, 0);
  EXPECT_EQ(castlingRights & jcl::Board::CASTLE_BLACK_QUEEN, 0);
  EXPECT_EQ(castlingRights & jcl::Board::CASTLE_WHITE_KING, jcl::Board::CASTLE_WHITE_KING);
  EXPECT_EQ(castlingRights & jcl::Board::CASTLE_WHITE_QUEEN, jcl::Board::CASTLE_WHITE_QUEEN);
  EXPECT_EQ(mBitBoard.getKingRow(jcl::Color::Black), 6);
  EXPECT_EQ(mBitBoard.getKingColumn(jcl::Color::Black), 4);

  mBitBoard.setPosition("rnbqkbnr/pp1ppppp/8/2pP4/8/8/PPP1PPPP/RNBQKBNR w KQkq c6 0 1");

  jcl::Move whiteMove(0, 4, 1, 4, jcl::Piece::King);
  EXPECT_EQ(mBitBoard.makeMove(&whiteMove), true);

  castlingRights = mBitBoard.getCastlingRights();
  EXPECT_EQ(castlingRights & jcl::Board::CASTLE_WHITE_KING, 0);
  EXPECT_EQ(castlingRights & jcl::Board::CASTLE_WHITE_QUEEN, 0);
  EXPECT_EQ(castlingRights & jcl::Board::CASTLE_BLACK_KING, jcl::Board::CASTLE_BLACK_KING);
  EXPECT_EQ(castlingRights & jcl::Board::CASTLE_BLACK_QUEEN, jcl::Board::CASTLE_BLACK_QUEEN);
  EXPECT_EQ(mBitBoard.getKingRow(jcl::Color::White), 1);
  EXPECT_EQ(mBitBoard.getKingColumn(jcl::Color::White), 4);
}

TEST_F(BitboardTest, TestCastleMoves)
{
  mBitBoard.setPosition("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1");

  jcl::Move whiteKingCastle(0, 4, 0, 6, jcl::Piece::King);
  whiteKingCastle.setType(jcl::Move::Type::Castle);
  EXPECT_EQ(mBitBoard.makeMove(&whiteKingCastle), true);

  uint8_t castlingRights = mBitBoard.getCastlingRights();
  EXPECT_EQ(castlingRights & jcl::Board::CASTLE_WHITE_KING, 0);
  EXPECT_EQ(castlingRights & jcl::Board::CASTLE_WHITE_QUEEN, 0);
  EXPECT_EQ(castlingRights & jcl::Board::CASTLE_BLACK_KING, jcl::Board::CASTLE_BLACK_KING);
  EXPECT_EQ(castlingRights & jcl::Board::CASTLE_BLACK_QUEEN, jcl::Board::CASTLE_BLACK_QUEEN);
  EXPECT_EQ(mBitBoard.getKingRow(jcl::Color::White), 0);
  EXPECT_EQ(mBitBoard.getKingColumn(jcl::Color::White), 6);
  EXPECT_EQ(mBitBoard.getRooks(jcl::Color::White), 0x0000000000000021);

  mBitBoard.setPosition("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1");

  jcl::Move whiteQueenCastle(0, 4, 0, 2, jcl::Piece::King);
  whiteQueenCastle.setType(jcl::Move::Type::Castle);
  EXPECT_EQ(mBitBoard.makeMove(&whiteQueenCastle), true);

  castlingRights = mBitBoard.getCastlingRights();
  EXPECT_EQ(castlingRights & jcl::Board::CASTLE_WHITE_KING, 0);
  EXPECT_EQ(castlingRights & jcl::Board::CASTLE_WHITE_QUEEN, 0);
  EXPECT_EQ(castlingRights & jcl::Board::CASTLE_BLACK_KING, jcl::Board::CASTLE_BLACK_KING);
  EXPECT_EQ(castlingRights & jcl::Board::CASTLE_BLACK_QUEEN, jcl::Board::CASTLE_BLACK_QUEEN);
  EXPECT_EQ(mBitBoard.getKingRow(jcl::Color::White), 0);
  EXPECT_EQ(mBitBoard.getKingColumn(jcl::Color::White), 2);
  EXPECT_EQ(mBitBoard.getRooks(jcl::Color::White), 0x0000000000000088);

  mBitBoard.setPosition("r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1");

  jcl::Move blackQueenCastle(7, 4, 7, 2, jcl::Piece::King);
  blackQueenCastle.setType(jcl::Move::Type::Castle);
  EXPECT_EQ(mBitBoard.makeMove(&blackQueenCastle), true);

  castlingRights = mBitBoard.getCastlingRights();
  EXPECT_EQ(castlingRights & jcl::Board::CASTLE_WHITE_KING, jcl::Board::CASTLE_WHITE_KING);
  EXPECT_EQ(castlingRights & jcl::Board::CASTLE_WHITE_QUEEN, jcl::Board::CASTLE_WHITE_QUEEN);
  EXPECT_EQ(castlingRights & jcl::Board::CASTLE_BLACK_KING, 0);
  EXPECT_EQ(castlingRights & jcl::Board::CASTLE_BLACK_QUEEN, 0);
  EXPECT_EQ(mBitBoard.getKingRow(jcl::Color::Black), 7);
  EXPECT_EQ(mBitBoard.getKingColumn(jcl::Color::Black), 2);
  EXPECT_EQ(mBitBoard.getRooks(jcl::Color::Black), 0x8800000000000000);

  mBitBoard.setPosition("r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1");

  jcl::Move blackKingCastle(7, 4, 7, 6, jcl::Piece::King);
  blackKingCastle.setType(jcl::Move::Type::Castle);
  EXPECT_EQ(mBitBoard.makeMove(&blackKingCastle), true);

  castlingRights = mBitBoard.getCastlingRights();
  EXPECT_EQ(castlingRights & jcl::Board::CASTLE_WHITE_KING, jcl::Board::CASTLE_WHITE_KING);
  EXPECT_EQ(castlingRights & jcl::Board::CASTLE_WHITE_QUEEN, jcl::Board::CASTLE_WHITE_QUEEN);
  EXPECT_EQ(castlingRights & jcl::Board::CASTLE_BLACK_KING, 0);
  EXPECT_EQ(castlingRights & jcl::Board::CASTLE_BLACK_QUEEN, 0);
  EXPECT_EQ(mBitBoard.getKingRow(jcl::Color::Black), 7);
  EXPECT_EQ(mBitBoard.getKingColumn(jcl::Color::Black), 6);
  EXPECT_EQ(mBitBoard.getRooks(jcl::Color::Black), 0x2100000000000000);

  // jcl::Move blackMove(7, 4, 6, 4, jcl::Piece::King);
  // EXPECT_EQ(mBitBoard.makeMove(&blackMove), true);

  // uint8_t castlingRights = mBitBoard.getCastlingRights();
  // EXPECT_EQ(castlingRights & jcl::Board::CASTLE_BLACK_KING, 0);
  // EXPECT_EQ(castlingRights & jcl::Board::CASTLE_BLACK_QUEEN, 0);
  // EXPECT_EQ(castlingRights & jcl::Board::CASTLE_WHITE_KING, jcl::Board::CASTLE_WHITE_KING);
  // EXPECT_EQ(castlingRights & jcl::Board::CASTLE_WHITE_QUEEN, jcl::Board::CASTLE_WHITE_QUEEN);
  // EXPECT_EQ(mBitBoard.getKingRow(jcl::Color::Black), 6);
  // EXPECT_EQ(mBitBoard.getKingColumn(jcl::Color::Black), 4);

  // mBitBoard.setPosition("rnbqkbnr/pp1ppppp/8/2pP4/8/8/PPP1PPPP/RNBQKBNR w KQkq c6 0 1");

  // jcl::Move whiteMove(0, 4, 1, 4, jcl::Piece::King);
  // EXPECT_EQ(mBitBoard.makeMove(&whiteMove), true);

  // castlingRights = mBitBoard.getCastlingRights();
  // EXPECT_EQ(castlingRights & jcl::Board::CASTLE_WHITE_KING, 0);
  // EXPECT_EQ(castlingRights & jcl::Board::CASTLE_WHITE_QUEEN, 0);
  // EXPECT_EQ(castlingRights & jcl::Board::CASTLE_BLACK_KING, jcl::Board::CASTLE_BLACK_KING);
  // EXPECT_EQ(castlingRights & jcl::Board::CASTLE_BLACK_QUEEN, jcl::Board::CASTLE_BLACK_QUEEN);
  // EXPECT_EQ(mBitBoard.getKingRow(jcl::Color::White), 1);
  // EXPECT_EQ(mBitBoard.getKingColumn(jcl::Color::White), 4);
}
