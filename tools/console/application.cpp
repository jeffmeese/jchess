#include "application.h"

#include "board.h"
#include "board8x8.h"
#include "consolegame.h"
#include "engine.h"
#include "evaluation.h"
#include "alphabetasearch.h"
#include "negamaxsearch.h"

#include <iostream>

Application::Application(int argc, char ** argv)
{
  parseCommandLine(argc, argv);
}

Application::~Application()
{

}

void Application::execute()
{
  mGame->run();
}

void Application::parseCommandLine(int argc, char **argv)
{
  mBoard.reset(new Board8x8);
  mEvaluation.reset(new Evaluation);
  mSearch.reset(new NegamaxSearch);
  mEngine.reset(new Engine(mBoard.get(), mSearch.get(), mEvaluation.get()));
  mGame.reset(new ConsoleGame(mBoard.get(), mEngine.get()));

  // This is where we could choose different boards, engines, ...
  //mBoard.reset(new Board);
  //mEvaluation.reset(new Evaluation);
  //mSearch.reset(new NegamaxSearch);
  //mEngine.reset(new Engine(mBoard.get(), mSearch.get(), mEvaluation.get()));
  //mGame.reset(new ConsoleGame(mBoard.get(), mEngine.get()));
}
