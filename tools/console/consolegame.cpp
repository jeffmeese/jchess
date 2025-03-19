#include "consolegame.h"

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>

#include "jchess_board.h"
#include "jchess_consts.h"
//#include "engine.h"
//#include "evaluation.h"
#include "jchess_fen.h"
//#include "jchess_move.h"
#//include "jchess_movelist.h"
//#include "perft.h"
//#include "search.h"
//#include "timer.h"
#include "jchess_types.h"
//#include "util.h"

ConsoleGame::ConsoleGame(jchess::Board * board)
    : mBoard(board)
{
}

void ConsoleGame::run()
{
  while (true) {
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

    // else if (commandString == "divide")
    //   handleDivide(iss);
    // else if (commandString == "help")
    //   handleHelp();
    // else if (commandString == "print")
    //   handlePrint();
    // else if (commandString == "disp")
    //   handlePrint();
    // else if (commandString == "fen")
    //   handleFen();
    // else if (commandString == "perft")
    //   handlePerft(iss);
    // else if (commandString == "move")
    //   handleMove(iss);
    // else if (commandString == "setboard")
    //   handleSetBoard(iss);
    // else if (commandString == "testmovegen")
    //   handleTestMoveGen();
    // else if (commandString == "eval")
    //   handleEval();
    // else if (commandString == "engine")
    //   handleEngine();
    // else if (commandString == "new")
    //   handleNewGame();
    // else if (commandString == "show")
    //   handleShow();
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

void ConsoleGame::handleHelp() const
{
  std::cout << "quit.................Quits the program\n";
  std::cout << "help.................Displays this help menu\n";
  std::cout << "print................Prints the current board position\n";
  //std::cout << "disp.................Same as print\n";
  //std::cout << "new..................Start a new game\n";
  //std::cout << "eval.................Evaluation the current board position\n";
  //std::cout << "move <smith>.........Performs a move\n";
  //std::cout << "perft <level>........Counts the total number of nodes to depth <level>\n";
  //std::cout << "divide <level>.......Displays the number of child moves\n";
  //std::cout << "table <level>........Displays a table of all perft results from 1 to <level>\n";
  //std::cout << "setboard <fen>.......Sets the board position to <fen>\n";
  //std::cout << "testmovegen..........Tests the move generator\n";
  //std::cout << "undo.................Undoes the last move\n";
}

void ConsoleGame::handlePrint() const
{
  std::map<jchess::PieceType, std::string> pieceMap;
  pieceMap[jchess::PieceType::None] = std::string("   ");
  pieceMap[jchess::PieceType::WhitePawn] = std::string(" P ");
  pieceMap[jchess::PieceType::WhiteRook] = std::string(" R ");
  pieceMap[jchess::PieceType::WhiteKnight] = std::string(" N ");
  pieceMap[jchess::PieceType::WhiteBishop] = std::string(" B ");
  pieceMap[jchess::PieceType::WhiteQueen] = std::string(" Q ");
  pieceMap[jchess::PieceType::WhiteKing] = std::string("[K]");
  pieceMap[jchess::PieceType::BlackPawn] = std::string(" p ");
  pieceMap[jchess::PieceType::BlackRook] = std::string(" r ");
  pieceMap[jchess::PieceType::BlackKnight] = std::string(" n ");
  pieceMap[jchess::PieceType::BlackBishop] = std::string(" b ");
  pieceMap[jchess::PieceType::BlackQueen] = std::string(" q ");
  pieceMap[jchess::PieceType::BlackKing] = std::string("[k]");

  std::ostringstream output;
  output << "\n";
  for (int row = 7; row >= 0; row--) {
    output << " +---+---+---+---+---+---+---+---+\n";
    output << row + 1 << "|";

    for (int col = 0; col < 8; col++) {
      jchess::PieceType type = mBoard->getPieceType(row, col);
      output << pieceMap[type];
      output << "|";

      if (col == 7) {
        if (row == 7)
          output << " To Move: " << ( mBoard->getSideToMove() == jchess::Color::White ? "White" : "Black");

        if (row == 6) {
          uint8_t castling = mBoard->getCastlingRights();
          output << " Castling: ";
          if (castling & jchess::CASTLE_WHITE_KING)
            output << "K";
          if (castling & jchess::CASTLE_WHITE_QUEEN)
            output << "Q";
          if (castling & jchess::CASTLE_BLACK_KING)
            output << "k";
          if (castling & jchess::CASTLE_BLACK_QUEEN)
            output << "q";
        }
      }
    }
    output << "\n";
  }

  output << " +---+---+---+---+---+---+---+---+\n";
  output << "   A   B   C   D   E   F   G   H  \n";
  output << "\n";

  jchess::Fen fen;
  fen.setFromBoard(mBoard);
  std::string fenString = fen.toString();
  output << "FEN: " << fenString << "\n";

  std::cout << output.str() << std::endl;
}

// template <typename T>
// T readValue(std::istream & iss)
// {
//   T value = T();
//   iss >> value;
//   return value;
// }

// ConsoleGame::ConsoleGame(Board * board, Engine * engine)
//   : mBoard(board)
//   , mEngine(engine)
// {
// }

// void ConsoleGame::doPerft(int perftLevel) const
// {
//   Perft perft(mBoard);

//   Timer timer;
//   timer.start();
//   ulonglong totalNodes = perft.execute(perftLevel);
//   timer.stop();
//   std::cout << "Total Nodes: " << totalNodes << " Time: " << timer.elapsed()/1e3 << " milliseconds\n";
// }

// void ConsoleGame::executeEngineMove()
// {
//   Move move = mEngine->generateMove();
//   doMove(move);
// }

// int ConsoleGame::getMoveIndex(uchar srcRow, uchar srcCol, uchar dstRow, uchar dstCol, const MoveList & moveList) const
// {
//   int moveIndex = -1;
//   for (uchar i = 0; i < moveList.size(); i++) {
//     Move move = moveList[i];
//     uchar sourceRow = move.sourceRow();
//     uchar sourceCol = move.sourceCol();
//     uchar destRow = move.destRow();
//     uchar destCol = move.destCol();
//     if (srcRow == sourceRow && dstRow == destRow && srcCol == sourceCol && dstCol == destCol) {
//       moveIndex = static_cast<int>(i);
//       return moveIndex;
//     }
//   }
//   return moveIndex;
// }

// void ConsoleGame::handleDivide(std::istringstream & iss) const
// {
//   uint perftLevel = readLevel(iss);
//   if (perftLevel == 0)
//     return;

//   Perft perft(mBoard);
//   perft.divide(perftLevel);
// }

// void ConsoleGame::handleEngine()
// {
//   Move m = mEngine->generateMove();
//   doMove(m);
//   handlePrint();
// }

// void ConsoleGame::handleEval() const
// {
//   MoveList moveList;
//   mBoard->generateMoves(moveList);

//   for (uchar i = 0; i < moveList.size(); i++) {
//     Move m = moveList[i];
//     std::cout << m.toSmithNotation() << " ";

//     mBoard->makeMove(m);
//     uchar kingRow = mBoard->kingRow(!mBoard->sideToMove());
//     uchar kingCol = mBoard->kingColumn(!mBoard->sideToMove());
//     if (!mBoard->isCellAttacked(kingRow, kingCol, mBoard->sideToMove())) {
//       double score = -mEngine->evaluation()->evaluate(mBoard);
//       std::cout << score << "\n";
//     }
//     mBoard->unmakeMove(m);
//   }
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

// void ConsoleGame::handleMove(std::istringstream & iss)
// {
//   if (isOver()) {
//     std::cout << "The game is over. Type new to start a new game\n";
//     return;
//   }

//   // Get the move string
//   std::string moveString;
//   iss >> moveString;
//   if (moveString.empty()) {
//     std::cout << "A move string must be provided\n";
//     return;
//   }

//   // Parse the move
//   uchar srcRow, srcCol, dstRow, dstCol;
//   if (!parseMovePos(moveString, srcRow, srcCol, dstRow, dstCol))
//     return;

//   // Generate the candidate moves from the board
//   MoveList moveList;
//   mBoard->generateMoves(moveList);

//   // Check that the move is valid
//   int moveIndex = getMoveIndex(srcRow, srcCol, dstRow, dstCol, moveList);
//   if (moveIndex == -1) {
//     std::cout << "Sorry " << moveString << " is not a valid move\n";
//     std::cout << "Type divide 1 to sell all available moves\n";
//     return;
//   }

//   doMove(moveList[moveIndex]);
//   if (isOver()) {
//     showEndGame();
//     return;
//   }

// //  executeEngineMove();
// //  handlePrint();
// //  if (isOver()) {
// //    showEndGame();
// //  }
// }

// void ConsoleGame::handleNewGame()
// {
//   restart();
// }

// void ConsoleGame::handlePerft(std::istringstream & iss) const
// {
//   uint perftLevel = readLevel(iss);
//   if (perftLevel == 0)
//     return;

//   doPerft(perftLevel);
// }

// void ConsoleGame::handlePrint() const
// {
//   std::ostringstream oss;
//   mBoard->print(oss);
//   std::cout << oss.str() << "\n";
// }

// void ConsoleGame::handleSetBoard(std::istringstream & iss)
// {
//   std::string fenString;
//   std::getline(iss, fenString);
//   if (fenString.empty()) {
//     std::cout << "A fen string must be provided\n";
//     return;
//   }

//   setPosition(fenString);
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

// void ConsoleGame::handleTestMoveGen() const
// {
//   std::string fileName("perftsuite.epd");
//   std::ifstream inputStream(fileName.c_str());
//   if (inputStream.fail()) {
//     std::cout << "Could not locate perft test file " << fileName << "\n";
//     return;
//   }

//   while (true) {
//     std::string inputLine;
//     std::getline(inputStream, inputLine);
//     if (inputStream.fail())
//       break;

//     std::vector<std::string> tokens;
//     util::split(tokens, inputLine, ';');
//     if (tokens.size() < 1)
//       continue;

//     std::string fenString = tokens.at(0);
//     if (tokens.size() > 1) {
//       std::cout << "fen: " << fenString << "\n";
//       mBoard->setPosition(fenString);
//     }

//     for (uint i = 1; i < tokens.size(); i++) {
//       std::string perftDepthString = tokens.at(i);

//       std::vector<std::string> perftTokens;
//       util::split(perftTokens, perftDepthString, ' ');
//       if (perftTokens.size() < 2)
//         continue;

//       std::string depthString = perftTokens.at(0);
//       std::string nodesString = perftTokens.at(1);

//       uint depthLevel = atoi(depthString.substr(1).c_str());
//       uint numNodes = atoi(nodesString.c_str());

//       Timer timer;
//       Perft perft(mBoard);

//       timer.start();
//       ulonglong totalNodes = perft.execute(depthLevel);
//       timer.stop();

//       bool success = (totalNodes == numNodes);
//       std::cout << "Perft (" << depthLevel << "): " << totalNodes << " nodes, ";
//       std::cout << "Time: " << timer.elapsed()/1e6 << " s, [" << numNodes << "], ";
//       std::cout << (success ? "OK" : "FAIL") << "\n";

//       //if (!success)
//       //  return;
//     }
//   }
// }

// void ConsoleGame::handleTwoPlayer()
// {
//   //setGameType(HumanVsHuman);
// }

// void ConsoleGame::handleUndo()
// {
//   undoLastMove();
// }

// bool ConsoleGame::parseMovePos(const std::string &moveString, uchar & srcRow, uchar & srcCol, uchar & dstRow, uchar & dstCol) const
// {
//   static std::map<char, uchar> columnMap;
//   static std::map<char, uchar> rowMap;

//   if (columnMap.size() == 0) {
//     columnMap['a'] = 0;
//     columnMap['b'] = 1;
//     columnMap['c'] = 2;
//     columnMap['d'] = 3;
//     columnMap['e'] = 4;
//     columnMap['f'] = 5;
//     columnMap['g'] = 6;
//     columnMap['h'] = 7;

//     rowMap['1'] = 0;
//     rowMap['2'] = 1;
//     rowMap['3'] = 2;
//     rowMap['4'] = 3;
//     rowMap['5'] = 4;
//     rowMap['6'] = 5;
//     rowMap['7'] = 6;
//     rowMap['8'] = 7;

//   }

//   // Check format of move string
//   if (moveString.find_first_not_of("123456789abcdefghpnbrqkEcCNBRQ") != std::string::npos || moveString.length() < 4) {
//     std::cout << "Invalid move string " << moveString << "\n";
//     return false;
//   }

//   srcCol = columnMap[moveString.at(0)];
//   srcRow = rowMap[moveString.at(1)];
//   dstCol = columnMap[moveString.at(2)];
//   dstRow = rowMap[moveString.at(3)];

//   return true;
// }

// uint ConsoleGame::readLevel(std::istream & iss) const
// {
//   uint perftLevel = readValue<uint>(iss);
//   if (iss.fail()) {
//     std::cout << "Invalid perft level\n";
//     return 0;
//   }
//   return perftLevel;
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

