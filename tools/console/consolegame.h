#ifndef CONSOLEGAME_H
#define CONSOLEGAME_H

#include "jcl_board.h"
//#include "engine.h"
#include "jcl_types.h"

class ConsoleGame
    //: public Game
{
public:
  //ConsoleGame(Board * board, Engine * engine);
  ConsoleGame(jcl::Board * board);

public:
  void run();

private:
  // void executeEngineMove();
  // int getMoveIndex(uchar srcRow, uchar srcCol, uchar dstRow, uchar dstCol, const MoveList & moveList) const;
  // void doPerft(int perftLevel) const;
  // void handleDivide(std::istringstream & iss) const;
  // void handleEngine();
  // void handleEval() const;
  // void handleFen() const;
  void handleHelp() const;
  // void handleMove(std::istringstream & iss);
  // void handleNewGame();
  // void handlePerft(std::istringstream & iss) const;
  void handlePrint() const;
  // void handleSetBoard(std::istringstream & iss);
  void handleShow() const;
  // void handleSinglePlayer();
  // void handleTable(std::istringstream & iss) const;
  // void handleTestMoveGen() const;
  // void handleTwoPlayer();
  // void handleUndo();
  // bool parseMovePos(const std::string & moveString, uchar & srcRow, uchar & srcCol, uchar & dstRow, uchar & dstCol) const;
  // uint readLevel(std::istream & iss) const;
  // void showEndGame();

private:
  jcl::Board * mBoard;
  //Engine * mEngine;
};

#endif // CONSOLEGAME_H
