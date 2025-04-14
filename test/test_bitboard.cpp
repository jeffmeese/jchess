#include "gtest/gtest.h"

#include "jcl_bitboard.h"

#define ONE 1LL

class BitboardTest : public testing::Test
{
protected:
  BitboardTest()
  {
    mCharToPiece['P'] = jcl::PieceType::WhitePawn;
    mCharToPiece['R'] = jcl::PieceType::WhiteRook;
    mCharToPiece['N'] = jcl::PieceType::WhiteKnight;
    mCharToPiece['B'] = jcl::PieceType::WhiteBishop;
    mCharToPiece['Q'] = jcl::PieceType::WhiteQueen;
    mCharToPiece['K'] = jcl::PieceType::WhiteKing;
    mCharToPiece['p'] = jcl::PieceType::BlackPawn;
    mCharToPiece['r'] = jcl::PieceType::BlackRook;
    mCharToPiece['n'] = jcl::PieceType::BlackKnight;
    mCharToPiece['b'] = jcl::PieceType::BlackBishop;
    mCharToPiece['q'] = jcl::PieceType::BlackQueen;
    mCharToPiece['k'] = jcl::PieceType::BlackKing;
    mCharToPiece['-'] = jcl::PieceType::None;
  }

  void testBitboards(char * pieces)
  {
    uint64_t blackPawns   = 0;
    uint64_t blackRooks   = 0;
    uint64_t blackKnights = 0;
    uint64_t blackBishops = 0;
    uint64_t blackQueens  = 0;
    uint64_t blackKings   = 0;
    uint64_t blackPieces  = 0;
    uint64_t whitePawns   = 0;
    uint64_t whiteRooks   = 0;
    uint64_t whiteKnights = 0;
    uint64_t whiteBishops = 0;
    uint64_t whiteQueens  = 0;
    uint64_t whiteKings   = 0;
    uint64_t whitePieces  = 0;
    uint64_t noPieces     = 0;
    uint64_t allPieces    = 0;

    for (int i = 0; i < 64; i++)
    {
      int index = 63-i;
      switch (pieces[i])
      {
      case 'P':
        whitePawns |= (ONE << index);
        whitePieces |= (ONE << index);
        break;
      case 'R':
        whiteRooks |= (ONE << index);
        whitePieces |= (ONE << index);
        break;
      case 'N':
        whiteKnights |= (ONE << index);
        whitePieces |= (ONE << index);
        break;
      case 'B':
        whiteBishops |= (ONE << index);
        whitePieces |= (ONE << index);
        break;
      case 'Q':
        whiteQueens |= (ONE << index);
        whitePieces |= (ONE << index);
        break;
      case 'K':
        whiteKings |= (ONE << index);
        whitePieces |= (ONE << index);
        break;
      case 'p':
        blackPawns |= (ONE << index);
        blackPieces |= (ONE << index);
        break;
      case 'r':
        blackRooks |= (ONE << index);
        blackPieces |= (ONE << index);
        break;
      case 'n':
        blackKnights |= (ONE << index);
        blackPieces |= (ONE << index);
        break;
      case 'b':
        blackBishops |= (ONE << index);
        blackPieces |= (ONE << index);
        break;
      case 'q':
        blackQueens |= (ONE << index);
        blackPieces |= (ONE << index);
        break;
      case 'k':
        blackKings |= (ONE << index);
        blackPieces |= (ONE << index);
        break;
      case '-':
        noPieces |= (ONE << index);
        break;
      }
    }

    allPieces = whitePieces | blackPieces;

    EXPECT_EQ(mBitBoard.getPawns(jcl::Color::Black), blackPawns);
    EXPECT_EQ(mBitBoard.getRooks(jcl::Color::Black), blackRooks);
    EXPECT_EQ(mBitBoard.getKnights(jcl::Color::Black), blackKnights);
    EXPECT_EQ(mBitBoard.getBishops(jcl::Color::Black), blackBishops);
    EXPECT_EQ(mBitBoard.getQueens(jcl::Color::Black), blackQueens);
    EXPECT_EQ(mBitBoard.getKings(jcl::Color::Black), blackKings);
    EXPECT_EQ(mBitBoard.getAll(jcl::Color::Black), blackPieces);
    EXPECT_EQ(mBitBoard.getPawns(jcl::Color::White), whitePawns);
    EXPECT_EQ(mBitBoard.getRooks(jcl::Color::White), whiteRooks);
    EXPECT_EQ(mBitBoard.getKnights(jcl::Color::White), whiteKnights);
    EXPECT_EQ(mBitBoard.getBishops(jcl::Color::White), whiteBishops);
    EXPECT_EQ(mBitBoard.getQueens(jcl::Color::White), whiteQueens);
    EXPECT_EQ(mBitBoard.getKings(jcl::Color::White), whiteKings);
    EXPECT_EQ(mBitBoard.getAll(jcl::Color::White), whitePieces);
    EXPECT_EQ(mBitBoard.getAll(), allPieces);
    EXPECT_EQ(mBitBoard.getNone(), noPieces);
  }

  void testPieces(char * pieces)
  {
    for (int i = 0; i < 8; i++)
    {
      for (int j = 0; j < 8; j++)
      {
        int index = (7-i)*8 + j;
        EXPECT_EQ(mBitBoard.getPieceType(i, j), mCharToPiece[pieces[index]]);
      }
    }
  }

protected:
  jcl::BitBoard mBitBoard;
  std::map<char, jcl::PieceType> mCharToPiece;
};

TEST_F(BitboardTest, TestConstruction)
{
  char pieces[] =
  {
    'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r',
    'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P',
    'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'
  };

  testBitboards(pieces);
  testPieces(pieces);
}

TEST_F(BitboardTest, TestSetPosition)
{
  EXPECT_EQ(mBitBoard.setPosition("rnbqkbnr/pppppppp/8/8/8/P7/1PPPPPPP/RNBQKBNR b KQkq - 0 1"), true);

  char pieces[] =
  {
    'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r',
    'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    'P', '-', '-', '-', '-', '-', '-', '-',
    '-', 'P', 'P', 'P', 'P', 'P', 'P', 'P',
    'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'
  };

  testBitboards(pieces);
  testPieces(pieces);
}

TEST_F(BitboardTest, TestSetPieceType)
{
  char pieces[] =
  {
    'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R',
    'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p',
    'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'
  };

  EXPECT_EQ(mBitBoard.setPosition("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"), true);

  mBitBoard.setPieceType(0, 0, jcl::PieceType::BlackRook);
  mBitBoard.setPieceType(0, 1, jcl::PieceType::BlackKnight);
  mBitBoard.setPieceType(0, 2, jcl::PieceType::BlackBishop);
  mBitBoard.setPieceType(0, 3, jcl::PieceType::BlackQueen);
  mBitBoard.setPieceType(0, 4, jcl::PieceType::BlackKing);
  mBitBoard.setPieceType(0, 5, jcl::PieceType::BlackBishop);
  mBitBoard.setPieceType(0, 6, jcl::PieceType::BlackKnight);
  mBitBoard.setPieceType(0, 7, jcl::PieceType::BlackRook);
  mBitBoard.setPieceType(1, 0, jcl::PieceType::BlackPawn);
  mBitBoard.setPieceType(1, 1, jcl::PieceType::BlackPawn);
  mBitBoard.setPieceType(1, 2, jcl::PieceType::BlackPawn);
  mBitBoard.setPieceType(1, 3, jcl::PieceType::BlackPawn);
  mBitBoard.setPieceType(1, 4, jcl::PieceType::BlackPawn);
  mBitBoard.setPieceType(1, 5, jcl::PieceType::BlackPawn);
  mBitBoard.setPieceType(1, 6, jcl::PieceType::BlackPawn);
  mBitBoard.setPieceType(1, 7, jcl::PieceType::BlackPawn);
  mBitBoard.setPieceType(6, 0, jcl::PieceType::WhitePawn);
  mBitBoard.setPieceType(6, 1, jcl::PieceType::WhitePawn);
  mBitBoard.setPieceType(6, 2, jcl::PieceType::WhitePawn);
  mBitBoard.setPieceType(6, 3, jcl::PieceType::WhitePawn);
  mBitBoard.setPieceType(6, 4, jcl::PieceType::WhitePawn);
  mBitBoard.setPieceType(6, 5, jcl::PieceType::WhitePawn);
  mBitBoard.setPieceType(6, 6, jcl::PieceType::WhitePawn);
  mBitBoard.setPieceType(6, 7, jcl::PieceType::WhitePawn);
  mBitBoard.setPieceType(7, 0, jcl::PieceType::WhiteRook);
  mBitBoard.setPieceType(7, 1, jcl::PieceType::WhiteKnight);
  mBitBoard.setPieceType(7, 2, jcl::PieceType::WhiteBishop);
  mBitBoard.setPieceType(7, 3, jcl::PieceType::WhiteQueen);
  mBitBoard.setPieceType(7, 4, jcl::PieceType::WhiteKing);
  mBitBoard.setPieceType(7, 5, jcl::PieceType::WhiteBishop);
  mBitBoard.setPieceType(7, 6, jcl::PieceType::WhiteKnight);
  mBitBoard.setPieceType(7, 7, jcl::PieceType::WhiteRook);

  testBitboards(pieces);
  testPieces(pieces);
}

TEST_F(BitboardTest, TestClearPieceType)
{
  char pieces[] =
  {
    'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R',
    'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p',
    '-', 'n', 'b', 'q', 'k', 'b', 'n', 'r'
  };

  EXPECT_EQ(mBitBoard.setPosition("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"), true);

  mBitBoard.setPieceType(0, 0, jcl::PieceType::None);
  mBitBoard.setPieceType(0, 1, jcl::PieceType::BlackKnight);
  mBitBoard.setPieceType(0, 2, jcl::PieceType::BlackBishop);
  mBitBoard.setPieceType(0, 3, jcl::PieceType::BlackQueen);
  mBitBoard.setPieceType(0, 4, jcl::PieceType::BlackKing);
  mBitBoard.setPieceType(0, 5, jcl::PieceType::BlackBishop);
  mBitBoard.setPieceType(0, 6, jcl::PieceType::BlackKnight);
  mBitBoard.setPieceType(0, 7, jcl::PieceType::BlackRook);
  mBitBoard.setPieceType(1, 0, jcl::PieceType::BlackPawn);
  mBitBoard.setPieceType(1, 1, jcl::PieceType::BlackPawn);
  mBitBoard.setPieceType(1, 2, jcl::PieceType::BlackPawn);
  mBitBoard.setPieceType(1, 3, jcl::PieceType::BlackPawn);
  mBitBoard.setPieceType(1, 4, jcl::PieceType::BlackPawn);
  mBitBoard.setPieceType(1, 5, jcl::PieceType::BlackPawn);
  mBitBoard.setPieceType(1, 6, jcl::PieceType::BlackPawn);
  mBitBoard.setPieceType(1, 7, jcl::PieceType::BlackPawn);
  mBitBoard.setPieceType(6, 0, jcl::PieceType::WhitePawn);
  mBitBoard.setPieceType(6, 1, jcl::PieceType::WhitePawn);
  mBitBoard.setPieceType(6, 2, jcl::PieceType::WhitePawn);
  mBitBoard.setPieceType(6, 3, jcl::PieceType::WhitePawn);
  mBitBoard.setPieceType(6, 4, jcl::PieceType::WhitePawn);
  mBitBoard.setPieceType(6, 5, jcl::PieceType::WhitePawn);
  mBitBoard.setPieceType(6, 6, jcl::PieceType::WhitePawn);
  mBitBoard.setPieceType(6, 7, jcl::PieceType::WhitePawn);
  mBitBoard.setPieceType(7, 0, jcl::PieceType::WhiteRook);
  mBitBoard.setPieceType(7, 1, jcl::PieceType::WhiteKnight);
  mBitBoard.setPieceType(7, 2, jcl::PieceType::WhiteBishop);
  mBitBoard.setPieceType(7, 3, jcl::PieceType::WhiteQueen);
  mBitBoard.setPieceType(7, 4, jcl::PieceType::WhiteKing);
  mBitBoard.setPieceType(7, 5, jcl::PieceType::WhiteBishop);
  mBitBoard.setPieceType(7, 6, jcl::PieceType::WhiteKnight);
  mBitBoard.setPieceType(7, 7, jcl::PieceType::WhiteRook);

  testBitboards(pieces);
  testPieces(pieces);
}

TEST_F(BitboardTest, TestReset)
{
  EXPECT_EQ(mBitBoard.setPosition("rnbqkbnr/pp1ppppp/8/2pP4/8/8/PPP1PPPP/RNBQKBNR w - c6 0 2"), true);
  mBitBoard.reset();

  char pieces[] =
  {
    'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r',
    'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P',
    'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'
  };

  testBitboards(pieces);
  testPieces(pieces);
}

TEST_F(BitboardTest, TestQuietMove)
{
  char pieces[] =
  {
   'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r',
   'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p',
   '-', '-', '-', '-', '-', '-', '-', '-',
   '-', '-', '-', '-', '-', '-', '-', '-',
   '-', '-', '-', '-', '-', '-', '-', '-',
   '-', '-', 'N', '-', '-', '-', '-', '-',
   'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P',
   'R', '-', 'B', 'Q', 'K', 'B', 'N', 'R'
  };

  uint8_t castlingRights = mBitBoard.getCastlingRights();
  uint8_t enPassantColumn = mBitBoard.getEnpassantColumn();
  uint32_t halfMoveClock = mBitBoard.getHalfMoveClock();
  uint32_t fullMoveCounter = mBitBoard.getFullMoveNumber();
  jcl::Move move(0, 1, 2, 2, castlingRights, enPassantColumn, halfMoveClock, fullMoveCounter, jcl::Piece::Knight, jcl::Move::Type::Quiet);
  mBitBoard.makeMove(&move);

  testBitboards(pieces);
  testPieces(pieces);
}

TEST_F(BitboardTest, TestDoublePushMove)
{
  char pieces[] =
  {
    'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r',
    'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', 'P', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    'P', 'P', 'P', '-', 'P', 'P', 'P', 'P',
    'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'
  };

  uint8_t castlingRights = mBitBoard.getCastlingRights();
  uint8_t enPassantColumn = mBitBoard.getEnpassantColumn();
  uint32_t halfMoveClock = mBitBoard.getHalfMoveClock();
  uint32_t fullMoveCounter = mBitBoard.getFullMoveNumber();
  jcl::Move move(1, 3, 3, 3, castlingRights, enPassantColumn, halfMoveClock, fullMoveCounter, jcl::Piece::Pawn, jcl::Move::Type::DoublePush);
  mBitBoard.makeMove(&move);

  testBitboards(pieces);
  testPieces(pieces);
}

TEST_F(BitboardTest, TestWhitePromotion)
{
  char queenPieces[] =
  {
    'Q', '-', '-', '-', 'k', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', 'K', '-', '-', '-',
  };

  char rookPieces[] =
  {
    'R', '-', '-', '-', 'k', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', 'K', '-', '-', '-',
  };

  char bishopPieces[] =
  {
    'B', '-', '-', '-', 'k', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', 'K', '-', '-', '-',
  };

  char knightPieces[] =
  {
    'N', '-', '-', '-', 'k', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', 'K', '-', '-', '-',
  };

  EXPECT_EQ(mBitBoard.setPosition("4k3/P7/8/8/8/8/8/4K3 w - - 0 2"), true);

  uint8_t castlingRights = mBitBoard.getCastlingRights();
  uint8_t enPassantColumn = mBitBoard.getEnpassantColumn();
  uint32_t halfMoveClock = mBitBoard.getHalfMoveClock();
  uint32_t fullMoveCounter = mBitBoard.getFullMoveNumber();
  jcl::Piece piece = jcl::Piece::Pawn;
  jcl::Piece capturePiece = jcl::Piece::None;
  jcl::Move::Type type = jcl::Move::Type::Promotion;

  jcl::Move queenMove(6, 0, 7, 0, castlingRights, enPassantColumn, halfMoveClock, fullMoveCounter, piece, type, capturePiece, jcl::Piece::Queen);
  mBitBoard.makeMove(&queenMove);
  testBitboards(queenPieces);
  testPieces(queenPieces);

  mBitBoard.setPosition("4k3/P7/8/8/8/8/8/4K3 w - - 0 2");
  jcl::Move rookMove(6, 0, 7, 0, castlingRights, enPassantColumn, halfMoveClock, fullMoveCounter, piece, type, capturePiece, jcl::Piece::Rook);
  mBitBoard.makeMove(&rookMove);
  testBitboards(rookPieces);
  testPieces(rookPieces);

  mBitBoard.setPosition("4k3/P7/8/8/8/8/8/4K3 w - - 0 2");
  jcl::Move bishopMove(6, 0, 7, 0, castlingRights, enPassantColumn, halfMoveClock, fullMoveCounter, piece, type, capturePiece, jcl::Piece::Bishop);
  mBitBoard.makeMove(&bishopMove);
  testBitboards(bishopPieces);
  testPieces(bishopPieces);

  mBitBoard.setPosition("4k3/P7/8/8/8/8/8/4K3 w - - 0 2");
  jcl::Move knightMove(6, 0, 7, 0, castlingRights, enPassantColumn, halfMoveClock, fullMoveCounter, piece, type, capturePiece, jcl::Piece::Knight);
  mBitBoard.makeMove(&knightMove);
  testBitboards(knightPieces);
  testPieces(knightPieces);
}

TEST_F(BitboardTest, TestBlackPromotion)
{
  char queenPieces[] =
  {
    '-', '-', '-', '-', 'k', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', 'K', '-', '-', 'q',
  };

  char rookPieces[] =
  {
    '-', '-', '-', '-', 'k', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', 'K', '-', '-', 'r',
  };

  char bishopPieces[] =
  {
    '-', '-', '-', '-', 'k', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', 'K', '-', '-', 'b',
  };

  char knightPieces[] =
  {
    '-', '-', '-', '-', 'k', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', 'K', '-', '-', 'n',
  };

  EXPECT_EQ(mBitBoard.setPosition("4k3/8/8/8/8/8/7p/4K3 b - - 0 2"), true);

  uint8_t castlingRights = mBitBoard.getCastlingRights();
  uint8_t enPassantColumn = mBitBoard.getEnpassantColumn();
  uint32_t halfMoveClock = mBitBoard.getHalfMoveClock();
  uint32_t fullMoveCounter = mBitBoard.getFullMoveNumber();
  jcl::Piece piece = jcl::Piece::Pawn;
  jcl::Piece capturePiece = jcl::Piece::None;
  jcl::Move::Type type = jcl::Move::Type::Promotion;

  jcl::Move queenMove(1, 7, 0, 7, castlingRights, enPassantColumn, halfMoveClock, fullMoveCounter, piece, type, capturePiece, jcl::Piece::Queen);
  mBitBoard.makeMove(&queenMove);
  testBitboards(queenPieces);
  testPieces(queenPieces);

  EXPECT_EQ(mBitBoard.setPosition("4k3/8/8/8/8/8/7p/4K3 b - - 0 2"), true);
  jcl::Move rookMove(1, 7, 0, 7, castlingRights, enPassantColumn, halfMoveClock, fullMoveCounter, piece, type, capturePiece, jcl::Piece::Rook);
  mBitBoard.makeMove(&rookMove);
  testBitboards(rookPieces);
  testPieces(rookPieces);

  EXPECT_EQ(mBitBoard.setPosition("4k3/8/8/8/8/8/7p/4K3 b - - 0 2"), true);
  jcl::Move bishopMove(1, 7, 0, 7, castlingRights, enPassantColumn, halfMoveClock, fullMoveCounter, piece, type, capturePiece, jcl::Piece::Bishop);
  mBitBoard.makeMove(&bishopMove);
  testBitboards(bishopPieces);
  testPieces(bishopPieces);

  EXPECT_EQ(mBitBoard.setPosition("4k3/8/8/8/8/8/7p/4K3 b - - 0 2"), true);
  jcl::Move knightMove(1, 7, 0, 7, castlingRights, enPassantColumn, halfMoveClock, fullMoveCounter, piece, type, capturePiece, jcl::Piece::Knight);
  mBitBoard.makeMove(&knightMove);
  testBitboards(knightPieces);
  testPieces(knightPieces);
}

TEST_F(BitboardTest, TestStandardCapture)
{
  char pieces[] =
  {
    'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r',
    'p', 'p', 'p', '-', 'p', 'p', 'p', 'p',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', 'P', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    'P', 'P', 'P', 'P', '-', 'P', 'P', 'P',
    'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'
  };

  EXPECT_EQ(mBitBoard.setPosition("rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 2"), true);

  uint8_t castlingRights = mBitBoard.getCastlingRights();
  uint8_t enPassantColumn = mBitBoard.getEnpassantColumn();
  uint32_t halfMoveClock = mBitBoard.getHalfMoveClock();
  uint32_t fullMoveCounter = mBitBoard.getFullMoveNumber();
  jcl::Piece piece = jcl::Piece::Pawn;
  jcl::Piece capturePiece = jcl::Piece::Pawn;
  jcl::Move::Type type = jcl::Move::Type::Capture;

  jcl::Move move(3, 4, 4, 3, castlingRights, enPassantColumn, halfMoveClock, fullMoveCounter, piece, type, capturePiece);
  mBitBoard.makeMove(&move);

  testBitboards(pieces);
  testPieces(pieces);
  EXPECT_EQ(mBitBoard.getSideToMove(), jcl::Color::Black);
}

TEST_F(BitboardTest, TestWhiteEnPassantCapture)
{
  char pieces[] =
  {
    'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r',
    'p', 'p', '-', 'p', 'p', 'p', 'p', 'p',
    '-', '-', 'P', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    'P', 'P', 'P', '-', 'P', 'P', 'P', 'P',
    'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'
  };

  EXPECT_EQ(mBitBoard.setPosition("rnbqkbnr/pp1ppppp/8/2pP4/8/8/PPP1PPPP/RNBQKBNR w KQkq c6 0 1"), true);

  uint8_t castlingRights = mBitBoard.getCastlingRights();
  uint8_t enPassantColumn = mBitBoard.getEnpassantColumn();
  uint32_t halfMoveClock = mBitBoard.getHalfMoveClock();
  uint32_t fullMoveCounter = mBitBoard.getFullMoveNumber();
  jcl::Piece piece = jcl::Piece::Pawn;
  jcl::Piece capturePiece = jcl::Piece::Pawn;
  jcl::Move::Type type = jcl::Move::Type::EpCapture;

  jcl::Move move(4, 3, 5, 2, castlingRights, enPassantColumn, halfMoveClock, fullMoveCounter, piece, type, capturePiece);
  EXPECT_EQ(mBitBoard.makeMove(&move), true);

  testBitboards(pieces);
  testPieces(pieces);
}

TEST_F(BitboardTest, TestBlackEnPassantCapture)
{
  char pieces[] =
  {
    'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r',
    'p', 'p', 'p', '-', 'p', 'p', 'p', 'p',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', 'p', '-', '-', '-', '-', '-',
    'P', 'P', '-', 'P', 'P', 'P', 'P', 'P',
    'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'
  };

  mBitBoard.setPosition("rnbqkbnr/ppp1pppp/8/8/2Pp4/8/PP1PPPPP/RNBQKBNR b KQkq c3 0 1");

  uint8_t castlingRights = mBitBoard.getCastlingRights();
  uint8_t enPassantColumn = mBitBoard.getEnpassantColumn();
  uint32_t halfMoveClock = mBitBoard.getHalfMoveClock();
  uint32_t fullMoveCounter = mBitBoard.getFullMoveNumber();
  jcl::Piece piece = jcl::Piece::Pawn;
  jcl::Piece capturePiece = jcl::Piece::Pawn;
  jcl::Move::Type type = jcl::Move::Type::EpCapture;

  jcl::Move move(3, 3, 2, 2, castlingRights, enPassantColumn, halfMoveClock, fullMoveCounter, piece, type, capturePiece);
  EXPECT_EQ(mBitBoard.makeMove(&move), true);

  testBitboards(pieces);
  testPieces(pieces);
}

TEST_F(BitboardTest, TestWhitePromotionCapture)
{
  char pieces[] =
  {
    '-', '-', '-', '-', 'k', '-', 'Q', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', 'K', '-', '-', '-',
  };

  EXPECT_EQ(mBitBoard.setPosition("4k1b1/7P/8/8/8/8/8/4K3 w - - 0 2"), true);

  uint8_t castlingRights = mBitBoard.getCastlingRights();
  uint8_t enPassantColumn = mBitBoard.getEnpassantColumn();
  uint32_t halfMoveClock = mBitBoard.getHalfMoveClock();
  uint32_t fullMoveCounter = mBitBoard.getFullMoveNumber();
  jcl::Piece piece = jcl::Piece::Pawn;
  jcl::Piece capturePiece = jcl::Piece::Bishop;
  jcl::Piece promotionPiece = jcl::Piece::Queen;
  jcl::Move::Type type = jcl::Move::Type::PromotionCapture;

  jcl::Move move(6, 7, 7, 6, castlingRights, enPassantColumn, halfMoveClock, fullMoveCounter, piece, type, capturePiece, promotionPiece);
  EXPECT_EQ(mBitBoard.makeMove(&move), true);

  testBitboards(pieces);
  testPieces(pieces);
}

TEST_F(BitboardTest, TestBlackPromotionCapture)
{
  char queenPieces[] =
  {
    '-', '-', '-', '-', 'k', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', 'K', '-', 'q', '-',
  };

  EXPECT_EQ(mBitBoard.setPosition("4k3/8/8/8/8/8/7p/4K1B1 b - - 0 2"), true);

  uint8_t castlingRights = mBitBoard.getCastlingRights();
  uint8_t enPassantColumn = mBitBoard.getEnpassantColumn();
  uint32_t halfMoveClock = mBitBoard.getHalfMoveClock();
  uint32_t fullMoveCounter = mBitBoard.getFullMoveNumber();
  jcl::Piece piece = jcl::Piece::Pawn;
  jcl::Piece capturePiece = jcl::Piece::Bishop;
  jcl::Piece promotionPiece = jcl::Piece::Queen;
  jcl::Move::Type type = jcl::Move::Type::PromotionCapture;

  jcl::Move move(1, 7, 0, 6, castlingRights, enPassantColumn, halfMoveClock, fullMoveCounter, piece, type, capturePiece, promotionPiece);
  EXPECT_EQ(mBitBoard.makeMove(&move), true);

  testBitboards(queenPieces);
  testPieces(queenPieces);
}

TEST_F(BitboardTest, TestWhiteKingCastle)
{
  char whiteKingSide[] =
  {
    'r', '-', '-', '-', 'k', '-', '-', 'r',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    'R', '-', '-', '-', '-', 'R', 'K', '-',
  };

  mBitBoard.setPosition("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1");

  uint8_t castlingRights = mBitBoard.getCastlingRights();
  uint8_t enPassantColumn = mBitBoard.getEnpassantColumn();
  uint32_t halfMoveClock = mBitBoard.getHalfMoveClock();
  uint32_t fullMoveCounter = mBitBoard.getFullMoveNumber();
  jcl::Piece piece = jcl::Piece::King;
  jcl::Move::Type type = jcl::Move::Type::Castle;

  jcl::Move whiteKingCastle(0, 4, 0, 6, castlingRights, enPassantColumn, halfMoveClock, fullMoveCounter, piece, type);
  EXPECT_EQ(mBitBoard.makeMove(&whiteKingCastle), true);

  testBitboards(whiteKingSide);
  testPieces(whiteKingSide);
}

TEST_F(BitboardTest, TestWhiteQueenCastle)
{
  char whiteQueenSide[] =
  {
    'r', '-', '-', '-', 'k', '-', '-', 'r',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', 'K', 'R', '-', '-', '-', 'R',
  };

  mBitBoard.setPosition("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1");

  uint8_t castlingRights = mBitBoard.getCastlingRights();
  uint8_t enPassantColumn = mBitBoard.getEnpassantColumn();
  uint32_t halfMoveClock = mBitBoard.getHalfMoveClock();
  uint32_t fullMoveCounter = mBitBoard.getFullMoveNumber();
  jcl::Piece piece = jcl::Piece::King;
  jcl::Move::Type type = jcl::Move::Type::Castle;

  jcl::Move whiteQueenCastle(0, 4, 0, 2, castlingRights, enPassantColumn, halfMoveClock, fullMoveCounter, piece, type);
  EXPECT_EQ(mBitBoard.makeMove(&whiteQueenCastle), true);

  testBitboards(whiteQueenSide);
  testPieces(whiteQueenSide);
}

TEST_F(BitboardTest, TestBlackKingCastle)
{
  char blackKingSide[] =
  {
    'r', '-', '-', '-', '-', 'r', 'k', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    'R', '-', '-', '-', 'K', '-', '-', 'R',
  };

  mBitBoard.setPosition("r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1");

  uint8_t castlingRights = mBitBoard.getCastlingRights();
  uint8_t enPassantColumn = mBitBoard.getEnpassantColumn();
  uint32_t halfMoveClock = mBitBoard.getHalfMoveClock();
  uint32_t fullMoveCounter = mBitBoard.getFullMoveNumber();
  jcl::Piece piece = jcl::Piece::King;
  jcl::Move::Type type = jcl::Move::Type::Castle;

  jcl::Move blackKingCastle(7, 4, 7, 6, castlingRights, enPassantColumn, halfMoveClock, fullMoveCounter, piece, type);
  EXPECT_EQ(mBitBoard.makeMove(&blackKingCastle), true);

  testBitboards(blackKingSide);
  testPieces(blackKingSide);
}

TEST_F(BitboardTest, TestBlackQueenCastle)
{
  char blackQueenSide[] =
  {
    '-', '-', 'k', 'r', '-', '-', '-', 'r',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    'R', '-', '-', '-', 'K', '-', '-', 'R',
  };

  mBitBoard.setPosition("r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1");

  uint8_t castlingRights = mBitBoard.getCastlingRights();
  uint8_t enPassantColumn = mBitBoard.getEnpassantColumn();
  uint32_t halfMoveClock = mBitBoard.getHalfMoveClock();
  uint32_t fullMoveCounter = mBitBoard.getFullMoveNumber();
  jcl::Piece piece = jcl::Piece::King;
  jcl::Move::Type type = jcl::Move::Type::Castle;

  jcl::Move blackQueenCastle(7, 4, 7, 2, castlingRights, enPassantColumn, halfMoveClock, fullMoveCounter, piece, type);
  EXPECT_EQ(mBitBoard.makeMove(&blackQueenCastle), true);

  testBitboards(blackQueenSide);
  testPieces(blackQueenSide);
}

TEST_F(BitboardTest, TestUnmakeQuietMove)
{
  char pieces[] =
  {
    'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r',
    'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P',
    'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'
  };

  EXPECT_EQ(mBitBoard.setPosition("rnbqkbnr/pppppppp/8/8/8/P7/1PPPPPPP/RNBQKBNR b KQkq - 0 1"), true);

  uint8_t castlingRights = mBitBoard.getCastlingRights();
  uint8_t enPassantColumn = mBitBoard.getEnpassantColumn();
  uint32_t halfMoveClock = mBitBoard.getHalfMoveClock();
  uint32_t fullMoveCounter = mBitBoard.getFullMoveNumber();
  jcl::Piece piece = jcl::Piece::Pawn;
  jcl::Move::Type type = jcl::Move::Type::Quiet;

  jcl::Move move(1, 0, 2, 0, castlingRights, enPassantColumn, halfMoveClock, fullMoveCounter, piece, type);
  EXPECT_EQ(mBitBoard.unmakeMove(&move), true);

  testBitboards(pieces);
  testPieces(pieces);
}

TEST_F(BitboardTest, TestUnmakeDoublePushMove)
{
  char pieces[] =
  {
    'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r',
    'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P',
    'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'
  };

  uint8_t castlingRights = mBitBoard.getCastlingRights();
  uint8_t enPassantColumn = mBitBoard.getEnpassantColumn();
  uint32_t halfMoveClock = mBitBoard.getHalfMoveClock();
  uint32_t fullMoveCounter = mBitBoard.getFullMoveNumber();

  EXPECT_EQ(mBitBoard.setPosition("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1"), true);

  jcl::Piece piece = jcl::Piece::Pawn;
  jcl::Move::Type type = jcl::Move::Type::DoublePush;
  jcl::Move move(1, 4, 3, 4, castlingRights, enPassantColumn, halfMoveClock, fullMoveCounter, piece, type);
  EXPECT_EQ(mBitBoard.unmakeMove(&move), true);

  testBitboards(pieces);
  testPieces(pieces);
}

TEST_F(BitboardTest, TestUnmakeWhitePromotion)
{
  char pieces[] =
  {
    '-', '-', '-', '-', 'k', '-', '-', '-',
    'P', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', 'K', '-', '-', '-',
  };

  uint8_t castlingRights = mBitBoard.getCastlingRights();
  uint8_t enPassantColumn = mBitBoard.getEnpassantColumn();
  uint32_t halfMoveClock = mBitBoard.getHalfMoveClock();
  uint32_t fullMoveCounter = mBitBoard.getFullMoveNumber();
  jcl::Piece piece = jcl::Piece::Pawn;
  jcl::Piece capturePiece = jcl::Piece::None;
  jcl::Move::Type type = jcl::Move::Type::Promotion;

  EXPECT_EQ(mBitBoard.setPosition("Q3k3/8/8/8/8/8/8/4K3 b - - 0 2"), true);

  jcl::Move queenMove(6, 0, 7, 0, castlingRights, enPassantColumn, halfMoveClock, fullMoveCounter, piece, type, capturePiece, jcl::Piece::Queen);
  mBitBoard.unmakeMove(&queenMove);
  testBitboards(pieces);
  testPieces(pieces);

  EXPECT_EQ(mBitBoard.setPosition("R3k3/8/8/8/8/8/8/4K3 b - - 0 2"), true);

  jcl::Move rookMove(6, 0, 7, 0, castlingRights, enPassantColumn, halfMoveClock, fullMoveCounter, piece, type, capturePiece, jcl::Piece::Rook);
  mBitBoard.unmakeMove(&rookMove);
  testBitboards(pieces);
  testPieces(pieces);

  EXPECT_EQ(mBitBoard.setPosition("B3k3/8/8/8/8/8/8/4K3 b - - 0 2"), true);

  jcl::Move bishopMove(6, 0, 7, 0, castlingRights, enPassantColumn, halfMoveClock, fullMoveCounter, piece, type, capturePiece, jcl::Piece::Bishop);
  mBitBoard.unmakeMove(&bishopMove);
  testBitboards(pieces);
  testPieces(pieces);

  EXPECT_EQ(mBitBoard.setPosition("N3k3/8/8/8/8/8/8/4K3 b - - 0 2"), true);

  jcl::Move knightMove(6, 0, 7, 0, castlingRights, enPassantColumn, halfMoveClock, fullMoveCounter, piece, type, capturePiece, jcl::Piece::Knight);
  mBitBoard.unmakeMove(&knightMove);
  testBitboards(pieces);
  testPieces(pieces);
}

TEST_F(BitboardTest, TestUnmakeBlackPromotion)
{
  char pieces[] =
  {
    '-', '-', '-', '-', 'k', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', 'p',
    '-', '-', '-', '-', 'K', '-', '-', '-',
  };

  uint8_t castlingRights = mBitBoard.getCastlingRights();
  uint8_t enPassantColumn = mBitBoard.getEnpassantColumn();
  uint32_t halfMoveClock = mBitBoard.getHalfMoveClock();
  uint32_t fullMoveCounter = mBitBoard.getFullMoveNumber();
  jcl::Piece piece = jcl::Piece::Pawn;
  jcl::Piece capturePiece = jcl::Piece::None;
  jcl::Move::Type type = jcl::Move::Type::Promotion;

  EXPECT_EQ(mBitBoard.setPosition("4k3/8/8/8/8/8/8/4K2q w - - 0 2"), true);

  jcl::Move queenMove(1, 7, 0, 7, castlingRights, enPassantColumn, halfMoveClock, fullMoveCounter, piece, type, capturePiece, jcl::Piece::Queen);
  mBitBoard.unmakeMove(&queenMove);
  testBitboards(pieces);
  testPieces(pieces);

  EXPECT_EQ(mBitBoard.setPosition("4k3/8/8/8/8/8/8/4K2r w - - 0 2"), true);

  jcl::Move rookMove(1, 7, 0, 7, castlingRights, enPassantColumn, halfMoveClock, fullMoveCounter, piece, type, capturePiece, jcl::Piece::Rook);
  mBitBoard.unmakeMove(&rookMove);
  testBitboards(pieces);
  testPieces(pieces);

  EXPECT_EQ(mBitBoard.setPosition("4k3/8/8/8/8/8/8/4K2b w - - 0 2"), true);

  jcl::Move bishopMove(1, 7, 0, 7, castlingRights, enPassantColumn, halfMoveClock, fullMoveCounter, piece, type, capturePiece, jcl::Piece::Bishop);
  mBitBoard.unmakeMove(&bishopMove);
  testBitboards(pieces);
  testPieces(pieces);

  EXPECT_EQ(mBitBoard.setPosition("4k3/8/8/8/8/8/8/4K2n w - - 0 2"), true);

  jcl::Move knightMove(1, 7, 0, 7, castlingRights, enPassantColumn, halfMoveClock, fullMoveCounter, piece, type, capturePiece, jcl::Piece::Knight);
  mBitBoard.unmakeMove(&knightMove);
  testBitboards(pieces);
  testPieces(pieces);
}

TEST_F(BitboardTest, TestUnmakeStandardCapture)
{
  char pieces[] =
  {
    'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r',
    'p', 'p', 'p', '-', 'p', 'p', 'p', 'p',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', 'p', '-', '-', '-', '-',
    '-', '-', '-', '-', 'P', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    'P', 'P', 'P', 'P', '-', 'P', 'P', 'P',
    'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'
  };

  uint8_t castlingRights = mBitBoard.getCastlingRights();
  uint8_t enPassantColumn = mBitBoard.getEnpassantColumn();
  uint32_t halfMoveClock = mBitBoard.getHalfMoveClock();
  uint32_t fullMoveCounter = mBitBoard.getFullMoveNumber();

  EXPECT_EQ(mBitBoard.setPosition("rnbqkbnr/ppp1pppp/8/3P4/8/8/PPPP1PPP/RNBQKBNR b KQkq - 0 2"), true);

  jcl::Piece piece = jcl::Piece::Pawn;
  jcl::Piece capturePiece = jcl::Piece::Pawn;
  jcl::Move::Type type = jcl::Move::Type::Capture;

  jcl::Move move(3, 4, 4, 3, castlingRights, enPassantColumn, halfMoveClock, fullMoveCounter, piece, type, capturePiece);
  mBitBoard.unmakeMove(&move);

  testBitboards(pieces);
  testPieces(pieces);
}

TEST_F(BitboardTest, TestUnmakeWhiteEnPassantCapture)
{
  char pieces[] =
  {
    'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r',
    'p', 'p', '-', 'p', 'p', 'p', 'p', 'p',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', 'p', 'P', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    'P', 'P', 'P', '-', 'P', 'P', 'P', 'P',
    'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'
  };

  uint8_t castlingRights = mBitBoard.getCastlingRights();
  uint8_t enPassantColumn = mBitBoard.getEnpassantColumn();
  uint32_t halfMoveClock = mBitBoard.getHalfMoveClock();
  uint32_t fullMoveCounter = mBitBoard.getFullMoveNumber();

  EXPECT_EQ(mBitBoard.setPosition("rnbqkbnr/pp1ppppp/2P5/8/8/8/PPP1PPPP/RNBQKBNR b KQkq c6 0 1"), true);

  jcl::Piece piece = jcl::Piece::Pawn;
  jcl::Piece capturePiece = jcl::Piece::Pawn;
  jcl::Move::Type type = jcl::Move::Type::EpCapture;

  jcl::Move move(4, 3, 5, 2, castlingRights, enPassantColumn, halfMoveClock, fullMoveCounter, piece, type, capturePiece);
  EXPECT_EQ(mBitBoard.unmakeMove(&move), true);

  testBitboards(pieces);
  testPieces(pieces);
}

TEST_F(BitboardTest, TestUnmakeBlackEnPassantCapture)
{
  char pieces[] =
  {
    'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r',
    'p', 'p', 'p', '-', 'p', 'p', 'p', 'p',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', 'P', 'p', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    'P', 'P', '-', 'P', 'P', 'P', 'P', 'P',
    'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'
  };

  uint8_t castlingRights = mBitBoard.getCastlingRights();
  uint8_t enPassantColumn = mBitBoard.getEnpassantColumn();
  uint32_t halfMoveClock = mBitBoard.getHalfMoveClock();
  uint32_t fullMoveCounter = mBitBoard.getFullMoveNumber();

  mBitBoard.setPosition("rnbqkbnr/ppp1pppp/8/8/8/2p5/PP1PPPPP/RNBQKBNR w KQkq c3 0 1");

  jcl::Piece piece = jcl::Piece::Pawn;
  jcl::Piece capturePiece = jcl::Piece::Pawn;
  jcl::Move::Type type = jcl::Move::Type::EpCapture;

  jcl::Move move(3, 3, 2, 2, castlingRights, enPassantColumn, halfMoveClock, fullMoveCounter, piece, type, capturePiece);
  EXPECT_EQ(mBitBoard.unmakeMove(&move), true);

  testBitboards(pieces);
  testPieces(pieces);
}

TEST_F(BitboardTest, TestUnmakeWhitePromotionCapture)
{
  char pieces[] =
  {
    '-', '-', '-', '-', 'k', '-', 'b', '-',
    '-', '-', '-', '-', '-', '-', '-', 'P',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', 'K', '-', '-', '-',
  };

  EXPECT_EQ(mBitBoard.setPosition("4k1Q1/8/8/8/8/8/8/4K3 b - - 0 2"), true);

  uint8_t castlingRights = mBitBoard.getCastlingRights();
  uint8_t enPassantColumn = mBitBoard.getEnpassantColumn();
  uint32_t halfMoveClock = mBitBoard.getHalfMoveClock();
  uint32_t fullMoveCounter = mBitBoard.getFullMoveNumber();
  jcl::Piece piece = jcl::Piece::Pawn;
  jcl::Piece capturePiece = jcl::Piece::Bishop;
  jcl::Piece promotionPiece = jcl::Piece::Queen;
  jcl::Move::Type type = jcl::Move::Type::PromotionCapture;

  jcl::Move move(6, 7, 7, 6, castlingRights, enPassantColumn, halfMoveClock, fullMoveCounter, piece, type, capturePiece, promotionPiece);
  EXPECT_EQ(mBitBoard.unmakeMove(&move), true);

  testBitboards(pieces);
  testPieces(pieces);
}

TEST_F(BitboardTest, TestUnmakeBlackPromotionCapture)
{
  char queenPieces[] =
  {
    '-', '-', '-', '-', 'k', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', 'p',
    '-', '-', '-', '-', 'K', '-', 'B', '-',
  };

  EXPECT_EQ(mBitBoard.setPosition("4k3/8/8/8/8/8/8/4K1q1 w - - 0 2"), true);

  uint8_t castlingRights = mBitBoard.getCastlingRights();
  uint8_t enPassantColumn = mBitBoard.getEnpassantColumn();
  uint32_t halfMoveClock = mBitBoard.getHalfMoveClock();
  uint32_t fullMoveCounter = mBitBoard.getFullMoveNumber();
  jcl::Piece piece = jcl::Piece::Pawn;
  jcl::Piece capturePiece = jcl::Piece::Bishop;
  jcl::Piece promotionPiece = jcl::Piece::Queen;
  jcl::Move::Type type = jcl::Move::Type::PromotionCapture;

  jcl::Move move(1, 7, 0, 6, castlingRights, enPassantColumn, halfMoveClock, fullMoveCounter, piece, type, capturePiece, promotionPiece);
  EXPECT_EQ(mBitBoard.unmakeMove(&move), true);

  testBitboards(queenPieces);
  testPieces(queenPieces);
}

TEST_F(BitboardTest, TestUnmakeWhiteKingCastle)
{
  char whiteKingSide[] =
  {
    'r', '-', '-', '-', 'k', '-', '-', 'r',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    'R', '-', '-', '-', 'K', '-', '-', 'R',
  };

  mBitBoard.setPosition("r3k2r/8/8/8/8/8/8/R4RK1 b KQkq - 0 1");

  uint8_t castlingRights = mBitBoard.getCastlingRights();
  uint8_t enPassantColumn = mBitBoard.getEnpassantColumn();
  uint32_t halfMoveClock = mBitBoard.getHalfMoveClock();
  uint32_t fullMoveCounter = mBitBoard.getFullMoveNumber();
  jcl::Piece piece = jcl::Piece::King;
  jcl::Move::Type type = jcl::Move::Type::Castle;

  jcl::Move whiteKingCastle(0, 4, 0, 6, castlingRights, enPassantColumn, halfMoveClock, fullMoveCounter, piece, type);
  EXPECT_EQ(mBitBoard.unmakeMove(&whiteKingCastle), true);

  testBitboards(whiteKingSide);
  testPieces(whiteKingSide);
}

TEST_F(BitboardTest, TestUnmakeWhiteQueenCastle)
{
  char whiteQueenSide[] =
  {
    'r', '-', '-', '-', 'k', '-', '-', 'r',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    'R', '-', '-', '-', 'K', '-', '-', 'R',
  };

  uint8_t castlingRights = mBitBoard.getCastlingRights();
  uint8_t enPassantColumn = mBitBoard.getEnpassantColumn();
  uint32_t halfMoveClock = mBitBoard.getHalfMoveClock();
  uint32_t fullMoveCounter = mBitBoard.getFullMoveNumber();

  mBitBoard.setPosition("r3k2r/8/8/8/8/8/8/2KR3R b KQkq - 0 1");

  jcl::Piece piece = jcl::Piece::King;
  jcl::Move::Type type = jcl::Move::Type::Castle;

  jcl::Move whiteQueenCastle(0, 4, 0, 2, castlingRights, enPassantColumn, halfMoveClock, fullMoveCounter, piece, type);
  EXPECT_EQ(mBitBoard.unmakeMove(&whiteQueenCastle), true);

  testBitboards(whiteQueenSide);
  testPieces(whiteQueenSide);
}

TEST_F(BitboardTest, TestUnmakeBlackKingCastle)
{
  char blackKingSide[] =
  {
    'r', '-', '-', '-', 'k', '-', '-', 'r',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    'R', '-', '-', '-', 'K', '-', '-', 'R',
  };

  uint8_t castlingRights = mBitBoard.getCastlingRights();
  uint8_t enPassantColumn = mBitBoard.getEnpassantColumn();
  uint32_t halfMoveClock = mBitBoard.getHalfMoveClock();
  uint32_t fullMoveCounter = mBitBoard.getFullMoveNumber();

  mBitBoard.setPosition("r4rk1/8/8/8/8/8/8/R3K2R w KQkq - 0 1");

  jcl::Piece piece = jcl::Piece::King;
  jcl::Move::Type type = jcl::Move::Type::Castle;

  jcl::Move blackKingCastle(7, 4, 7, 6, castlingRights, enPassantColumn, halfMoveClock, fullMoveCounter, piece, type);
  EXPECT_EQ(mBitBoard.unmakeMove(&blackKingCastle), true);

  testBitboards(blackKingSide);
  testPieces(blackKingSide);
}

TEST_F(BitboardTest, TestUnmakeBlackQueenCastle)
{
  char blackQueenSide[] =
  {
    'r', '-', '-', '-', 'k', '-', '-', 'r',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    '-', '-', '-', '-', '-', '-', '-', '-',
    'R', '-', '-', '-', 'K', '-', '-', 'R',
  };

  uint8_t castlingRights = mBitBoard.getCastlingRights();
  uint8_t enPassantColumn = mBitBoard.getEnpassantColumn();
  uint32_t halfMoveClock = mBitBoard.getHalfMoveClock();
  uint32_t fullMoveCounter = mBitBoard.getFullMoveNumber();

  mBitBoard.setPosition("2kr3r/8/8/8/8/8/8/R3K2R w KQkq - 0 1");

  jcl::Piece piece = jcl::Piece::King;
  jcl::Move::Type type = jcl::Move::Type::Castle;

  jcl::Move blackQueenCastle(7, 4, 7, 2, castlingRights, enPassantColumn, halfMoveClock, fullMoveCounter, piece, type);
  EXPECT_EQ(mBitBoard.unmakeMove(&blackQueenCastle), true);

  testBitboards(blackQueenSide);
  testPieces(blackQueenSide);
}
