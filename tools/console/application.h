#ifndef APPLICATION_H
#define APPLICATION_H

#include "board.h"

#include <memory>

//class Board;
class ConsoleGame;
class Engine;
class Evaluation;
class Search;

class Application
{
public:
  Application(int argc, char ** argv);
  ~Application();

public:
  void execute();

private:
  void parseCommandLine(int argc, char ** argv);

private:
  std::unique_ptr<Board> mBoard;
  std::unique_ptr<ConsoleGame> mGame;
  std::unique_ptr<Evaluation> mEvaluation;
  std::unique_ptr<Engine> mEngine;
  std::unique_ptr<Search> mSearch;
};

#endif // APPLICATION_H
