//#include "application.h"

//#include <iostream>
//#include <stdexcept>
#include <memory>

#include "jcl_board8x8.h"
#include "consolegame.h"

int main(int argc, char ** argv)
{
  std::unique_ptr<jcl::Board8x8> board(new jcl::Board8x8);
  std::unique_ptr<ConsoleGame> game(new ConsoleGame(board.get()));
  game->run();

  // try {
  //   Application app(argc, argv);
  //   app.execute();
  // }
  // catch (std::invalid_argument & ex) {
  //   std::cerr << "Caught exception\n";
  //   std::cerr << ex.what() << "\n";
  //   return EXIT_FAILURE;
  // }
  // catch (std::exception & ex) {
  //   std::cerr << "Caught unknown exception\n";
  //   std::cerr << ex.what() << "\n";
  //   return EXIT_FAILURE;
  // }

  return EXIT_SUCCESS;
}
