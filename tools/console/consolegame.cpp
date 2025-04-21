#include "consolegame.h"

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>

#include "jcl_board.h"
#include "jcl_fen.h"
#include "jcl_perft.h"
#include "jcl_timer.h"
#include "jcl_types.h"
#include "jcl_util.h"

template <typename T>
T readValue(std::istream & iss)
{
  T value = T();
  iss >> value;
  return value;
}

ConsoleGame::ConsoleGame(jcl::Board * board, jcl::Evaluation * evaluation)
    : mBoard(board)
    , mEvaluation(evaluation)
{
}

void ConsoleGame::doMove(const jcl::Move * move)
{
  //if (isOver())
  //  return;
  mBoard->makeMove(move);
  //checkForMate();
//  mCompletedMoves->addMove(move);
}

void ConsoleGame::doPerft(int32_t perftLevel) const
{
  jcl::Perft perft(mBoard);

  jcl::Timer timer;
  timer.start();
  uint64_t totalNodes = perft.execute(perftLevel);
  timer.stop();
  std::cout << "Total Nodes: " << totalNodes << " Time: " << timer.elapsed()/1e3 << " milliseconds\n";
}

void ConsoleGame::run()
{
  while (true)
  {
    std::cout << "\njc: ";

    std::string inputString;
    std::getline(std::cin, inputString);

    std::istringstream iss(inputString);
    std::string commandString;
    iss >> commandString;
    if (commandString.empty())
      continue;

    if (commandString == "quit")
    {
      break;
    }
    else if (commandString == "help")
    {
      handleHelp();
    }
    else if (commandString == "print")
    {
      handlePrint();
    }
    else if (commandString == "show")
    {
      handleShow();
    }
    else if (commandString == "divide")
    {
      handleDivide(iss);
    }
    else if (commandString == "perft")
    {
      handlePerft(iss);
    }
    else if (commandString == "move")
    {
      handleMove(iss);
    }
    else if (commandString == "setboard")
    {
       handleSetBoard(iss);
    }
    else if (commandString == "testmovegen")
    {
      handleTestMoveGen();
    }
    else if (commandString == "eval")
    {
      handleEval();
    }
    // else if (commandString == "engine")
    //   handleEngine();
    else if (commandString == "new")
    {
      handleNewGame();
    }
    // else if (commandString == "undo")
    //   handleUndo();
    // else if (commandString == "twoplayer")
    //   handleTwoPlayer();
    // else if (commandString == "singleplayer")
    //   handleSinglePlayer();
    else {
      std::cout << "Unrecognized command " << commandString << "\n";
      std::cout << "Type help to show command list\n";
    }
  }
}

int32_t ConsoleGame::getMoveIndex(uint8_t srcRow, uint8_t srcCol, uint8_t dstRow, uint8_t dstCol, const jcl::MoveList & moveList) const
{
  int32_t moveIndex = -1;
  for (uint8_t i = 0; i < moveList.size(); i++) {
    const jcl::Move * move = moveList.moveAt(i);
    uint8_t sourceRow = move->getSourceRow();
    uint8_t sourceCol = move->getSourceColumn();
    uint8_t destRow = move->getDestinationRow();
    uint8_t destCol = move->getDestinationColumn();
    if (srcRow == sourceRow && dstRow == destRow && srcCol == sourceCol && dstCol == destCol) {
      moveIndex = static_cast<int>(i);
      return moveIndex;
    }
  }
  return moveIndex;
}

void ConsoleGame::handleDivide(std::istringstream & iss) const
{
  uint8_t perftLevel = readLevel(iss);
  if (perftLevel == 0)
    return;

  jcl::Perft perft(mBoard);
  perft.divide(perftLevel);
}

void ConsoleGame::handleEval() const
{
  jcl::MoveList moveList;
  mBoard->generateMoves(moveList);

  for (uint8_t i = 0; i < moveList.size(); i++)
  {
    const jcl::Move * m = moveList[i];
    std::cout << m->toSmithNotation() << " ";

    mBoard->makeMove(m);
    uint8_t kingRow = mBoard->getKingRow(!mBoard->getSideToMove());
    uint8_t kingCol = mBoard->getKingColumn(!mBoard->getSideToMove());
    if (!mBoard->isCellAttacked(kingRow, kingCol, mBoard->getSideToMove()))
    {
      double score = mEvaluation->evaluateBoard(mBoard);
      std::cout << score << "\n";
    }
    mBoard->unmakeMove(m);
  }
}

void ConsoleGame::handleHelp() const
{
  std::cout << "quit.................Quits the program\n";
  std::cout << "help.................Displays this help menu\n";
  std::cout << "print................Prints the current board position\n";
  //std::cout << "disp.................Same as print\n";
  std::cout << "new..................Start a new game\n";
  std::cout << "eval.................Evaluation the current board position\n";
  std::cout << "move <smith>.........Performs a move\n";
  std::cout << "perft <level>........Counts the total number of nodes to depth <level>\n";
  //std::cout << "divide <level>.......Displays the number of child moves\n";
  //std::cout << "table <level>........Displays a table of all perft results from 1 to <level>\n";
  std::cout << "setboard <fen>.......Sets the board position to <fen>\n";
  std::cout << "testmovegen..........Tests the move generator\n";
  //std::cout << "undo.................Undoes the last move\n";
}

void ConsoleGame::handleMove(std::istringstream & iss)
{
  //if (isOver()) {
  //  std::cout << "The game is over. Type new to start a new game\n";
  //  return;
  //}

  // Get the move string
  std::string moveString;
  iss >> moveString;
  if (moveString.empty()) {
    std::cout << "A move string must be provided\n";
    return;
  }

  // Parse the move
  uint8_t srcRow, srcCol, dstRow, dstCol;
  if (!parseMovePos(moveString, srcRow, srcCol, dstRow, dstCol))
    return;

  jcl::Move move(srcRow, srcCol, dstRow, dstCol,
                 mBoard->getCastlingRights(), mBoard->getEnpassantColumn(), mBoard->getHalfMoveClock(), mBoard->getFullMoveNumber(),
                 jcl::Piece::Pawn, jcl::Move::Type::Quiet);
  doMove(&move);
  return;

  // // Generate the candidate moves from the board
  // jcl::MoveList moveList;
  // mBoard->generateMoves(moveList);

  // // Check that the move is valid
  // int moveIndex = getMoveIndex(srcRow, srcCol, dstRow, dstCol, moveList);
  // if (moveIndex == -1) {
  //   std::cout << "Sorry " << moveString << " is not a valid move\n";
  //   std::cout << "Type divide 1 to sell all available moves\n";
  //   return;
  // }

  // doMove(moveList.moveAt(moveIndex));
  //if (isOver()) {
  //  showEndGame();
  //  return;
  //}

//  executeEngineMove();
//  handlePrint();
//  if (isOver()) {
//    showEndGame();
//  }
}

void ConsoleGame::handleNewGame()
{
  mBoard->reset();
}

void ConsoleGame::handlePerft(std::istringstream & iss) const
{
  int32_t perftLevel = readLevel(iss);
  if (perftLevel == 0)
    return;

  doPerft(perftLevel);
  //mBoard->printPerformanceMetrics();
}

void ConsoleGame::handlePrint() const
{
  std::map<jcl::PieceType, std::string> pieceMap;
  pieceMap[jcl::PieceType::None] = std::string("   ");
  pieceMap[jcl::PieceType::WhitePawn] = std::string(" P ");
  pieceMap[jcl::PieceType::WhiteRook] = std::string(" R ");
  pieceMap[jcl::PieceType::WhiteKnight] = std::string(" N ");
  pieceMap[jcl::PieceType::WhiteBishop] = std::string(" B ");
  pieceMap[jcl::PieceType::WhiteQueen] = std::string(" Q ");
  pieceMap[jcl::PieceType::WhiteKing] = std::string("[K]");
  pieceMap[jcl::PieceType::BlackPawn] = std::string(" p ");
  pieceMap[jcl::PieceType::BlackRook] = std::string(" r ");
  pieceMap[jcl::PieceType::BlackKnight] = std::string(" n ");
  pieceMap[jcl::PieceType::BlackBishop] = std::string(" b ");
  pieceMap[jcl::PieceType::BlackQueen] = std::string(" q ");
  pieceMap[jcl::PieceType::BlackKing] = std::string("[k]");

  std::ostringstream output;
  output << "\n";
  for (int row = 7; row >= 0; row--) {
    output << " +---+---+---+---+---+---+---+---+\n";
    output << row + 1 << "|";

    for (int col = 0; col < 8; col++) {
      jcl::PieceType type = mBoard->getPieceType(row, col);
      output << pieceMap[type];
      output << "|";

      if (col == 7) {
        if (row == 7)
          output << " To Move: " << ( mBoard->getSideToMove() == jcl::Color::White ? "White" : "Black");

        if (row == 6) {
          uint8_t castling = mBoard->getCastlingRights();
          output << " Castling: ";
          if (castling & jcl::Board::CASTLE_WHITE_KING)
            output << "K";
          if (castling & jcl::Board::CASTLE_WHITE_QUEEN)
            output << "Q";
          if (castling & jcl::Board::CASTLE_BLACK_KING)
            output << "k";
          if (castling & jcl::Board::CASTLE_BLACK_QUEEN)
            output << "q";
        }
      }
    }
    output << "\n";
  }

  output << " +---+---+---+---+---+---+---+---+\n";
  output << "   A   B   C   D   E   F   G   H  \n";
  output << "\n";

  jcl::Fen fen;
  fen.setFromBoard(mBoard);
  std::string fenString = fen.toString();
  output << "FEN: " << fenString << "\n";

  std::cout << output.str() << std::endl;
}

void ConsoleGame::handleSetBoard(std::istringstream & iss)
{
  std::string fenString;
  std::getline(iss, fenString);
  if (fenString.empty()) {
    std::cout << "A fen string must be provided\n";
    return;
  }

  mBoard->setPosition(fenString);
}

void ConsoleGame::handleTestMoveGen() const
{
  std::string fileName("perftsuite.epd");
  std::ifstream inputStream(fileName.c_str());
  if (inputStream.fail())
  {
    std::cout << "Could not locate perft test file " << fileName << "\n";
    return;
  }

  while (true)
  {
    std::string inputLine;
    std::getline(inputStream, inputLine);
    if (inputStream.fail())
    {
      break;
    }

    std::vector<std::string> tokens;
    jcl::split(tokens, inputLine, ';');
    if (tokens.size() < 1)
    {
      continue;
    }

    std::string fenString = tokens.at(0);
    if (tokens.size() > 1)
    {
      std::cout << "fen: " << fenString << "\n";
      mBoard->setPosition(fenString);
    }

    for (uint8_t i = 1; i < tokens.size(); i++)
    {
      std::string perftDepthString = tokens.at(i);

      std::vector<std::string> perftTokens;
      jcl::split(perftTokens, perftDepthString, ' ');
      if (perftTokens.size() < 2)
      {
        continue;
      }

      std::string depthString = perftTokens.at(0);
      std::string nodesString = perftTokens.at(1);

      int32_t depthLevel = atoi(depthString.substr(1).c_str());
      uint64_t numNodes = atoi(nodesString.c_str());

      // void ConsoleGame::doPerft(int32_t perftLevel) const
      // {
      //   jcl::Perft perft(mBoard);

      //   jcl::Timer timer;
      //   timer.start();
      //   uint64_t totalNodes = perft.execute(perftLevel);
      //   timer.stop();
      //   std::cout << "Total Nodes: " << totalNodes << " Time: " << timer.elapsed()/1e3 << " milliseconds\n";
      // }

      jcl::Timer timer;
      jcl::Perft perft(mBoard);

      timer.start();
      uint64_t totalNodes = perft.execute(depthLevel);
      timer.stop();

      bool success = (totalNodes == numNodes);
      std::cout << "Perft (" << static_cast<int>(depthLevel) << "): " << totalNodes << " nodes, ";
      std::cout << "Time: " << timer.elapsed()/1e6 << " s, [" << static_cast<int>(numNodes) << "], ";
      std::cout << (success ? "OK" : "FAIL") << "\n";

      //if (!success)
      //  return;
    }
  }
}

void ConsoleGame::handleShow() const
{
  jcl::Timer timer;
  timer.start();

  jcl::MoveList moveList;
  mBoard->generateMoves(moveList);
  std::cout << timer.elapsed() << std::endl;

  for (uint8_t i = 0; i < moveList.size(); i++)
  {
    const jcl::Move * move = moveList.moveAt(i);
    bool validMove = false;
    mBoard->makeMove(move);
    uint8_t kingRow = mBoard->getKingRow(!mBoard->getSideToMove());
    uint8_t kingCol = mBoard->getKingColumn(!mBoard->getSideToMove());

    bool attacked = mBoard->isCellAttacked(kingRow, kingCol, mBoard->getSideToMove());
    if (!attacked)
    {
      validMove = true;
    }
    mBoard->unmakeMove(move);

    if (validMove)
    {
      std::cout << moveList.moveAt(i)->toSmithNotation() << "\n";
    }
  }
}

bool ConsoleGame::parseMovePos(const std::string &moveString, uint8_t & srcRow, uint8_t & srcCol, uint8_t & dstRow, uint8_t & dstCol) const
{
  static std::map<char, uint8_t> columnMap;
  static std::map<char, uint8_t> rowMap;

  if (columnMap.size() == 0) {
    columnMap['a'] = 0;
    columnMap['b'] = 1;
    columnMap['c'] = 2;
    columnMap['d'] = 3;
    columnMap['e'] = 4;
    columnMap['f'] = 5;
    columnMap['g'] = 6;
    columnMap['h'] = 7;

    rowMap['1'] = 0;
    rowMap['2'] = 1;
    rowMap['3'] = 2;
    rowMap['4'] = 3;
    rowMap['5'] = 4;
    rowMap['6'] = 5;
    rowMap['7'] = 6;
    rowMap['8'] = 7;

  }

  // Check format of move string
  if (moveString.find_first_not_of("123456789abcdefghpnbrqkEcCNBRQ") != std::string::npos || moveString.length() < 4) {
    std::cout << "Invalid move string " << moveString << "\n";
    return false;
  }

  srcCol = columnMap[moveString.at(0)];
  srcRow = rowMap[moveString.at(1)];
  dstCol = columnMap[moveString.at(2)];
  dstRow = rowMap[moveString.at(3)];

  return true;
}

int32_t ConsoleGame::readLevel(std::istream & iss) const
{
  int32_t perftLevel = readValue<int32_t>(iss);
  if (iss.fail()) {
    std::cout << "Invalid perft level\n";
    return 0;
  }
  return perftLevel;
}



// ConsoleGame::ConsoleGame(Board * board, Engine * engine)
//   : mBoard(board)
//   , mEngine(engine)
// {
// }



// void ConsoleGame::executeEngineMove()
// {
//   Move move = mEngine->generateMove();
//   doMove(move);
// }





// void ConsoleGame::handleEngine()
// {
//   Move m = mEngine->generateMove();
//   doMove(m);
//   handlePrint();
// }

// void ConsoleGame::handleFen() const
// {
//   std::cout << mBoard->writeFenString() << "\n";
// }

// void ConsoleGame::handleHelp() const
// {
//   std::cout << "quit.................Quits the program\n";
//   std::cout << "help.................Displays this help menu\n";
//   std::cout << "print................Prints the current board position\n";
//   std::cout << "disp.................Same as print\n";
//   std::cout << "new..................Start a new game\n";
//   std::cout << "eval.................Evaluation the current board position\n";
//   std::cout << "move <smith>.........Performs a move\n";
//   std::cout << "perft <level>........Counts the total number of nodes to depth <level>\n";
//   std::cout << "divide <level>.......Displays the number of child moves\n";
//   std::cout << "table <level>........Displays a table of all perft results from 1 to <level>\n";
//   std::cout << "setboard <fen>.......Sets the board position to <fen>\n";
//   std::cout << "testmovegen..........Tests the move generator\n";
//   std::cout << "undo.................Undoes the last move\n";
// }







// void ConsoleGame::handlePrint() const
// {
//   std::ostringstream oss;
//   mBoard->print(oss);
//   std::cout << oss.str() << "\n";
// }



// void ConsoleGame::handleShow() const
// {
//   MoveList moveList;
//   mBoard->generateMoves(moveList);
//   for (uchar i = 0; i < moveList.size(); i++)
//     std::cout << moveList[i].toSmithNotation() << "\n";
// }

// void ConsoleGame::handleSinglePlayer()
// {
//   //setGameType(HumanVsComputer);
// }



// void ConsoleGame::handleTwoPlayer()
// {
//   //setGameType(HumanVsHuman);
// }

// void ConsoleGame::handleUndo()
// {
//   undoLastMove();
// }





// void ConsoleGame::run()
// {
//   while (true) {
//     std::cout << "\njc: ";

//     std::string inputString;
//     std::getline(std::cin, inputString);

//     std::istringstream iss(inputString);
//     std::string commandString;
//     iss >> commandString;
//     if (commandString.empty())
//       continue;

//     if (commandString == "quit")
//       break;
//     else if (commandString == "divide")
//       handleDivide(iss);
//     else if (commandString == "help")
//       handleHelp();
//     else if (commandString == "print")
//       handlePrint();
//     else if (commandString == "disp")
//       handlePrint();
//     else if (commandString == "fen")
//       handleFen();
//     else if (commandString == "perft")
//       handlePerft(iss);
//     else if (commandString == "move")
//       handleMove(iss);
//     else if (commandString == "setboard")
//       handleSetBoard(iss);
//     else if (commandString == "testmovegen")
//       handleTestMoveGen();
//     else if (commandString == "eval")
//       handleEval();
//     else if (commandString == "engine")
//       handleEngine();
//     else if (commandString == "new")
//       handleNewGame();
//     else if (commandString == "show")
//       handleShow();
//     else if (commandString == "undo")
//       handleUndo();
//     else if (commandString == "twoplayer")
//       handleTwoPlayer();
//     else if (commandString == "singleplayer")
//       handleSinglePlayer();
//     else {
//       std::cout << "Unrecognized command " << commandString << "\n";
//       std::cout << "Type help to show command list\n";
//     }
//   }
// }

// void ConsoleGame::showEndGame()
// {
//   if (isBlackCheckmate())
//     std::cout << "Checkmate. White wins\n";

//   if (isWhiteCheckmate())
//     std::cout << "Checkmate. Black wins\n";

//   if (isStalemate())
//     std::cout << "Stalemate\n";
// }

