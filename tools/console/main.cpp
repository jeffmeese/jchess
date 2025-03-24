//#include "application.h"

//#include <iostream>
//#include <stdexcept>
#include <memory>

#include "jcl_bitboard.h"
#include "jcl_board8x8.h"
#include "jcl_fastboard8x8.h"
#include "jcl_evaluation.h"
#include "consolegame.h"

int main(int argc, char ** argv)
{
  //std::unique_ptr<jcl::Board8x8> board(new jcl::Board8x8);
  //std::unique_ptr<jcl::FastBoard8x8> board(new jcl::FastBoard8x8);
  std::unique_ptr<jcl::BitBoard> board(new jcl::BitBoard);
  std::unique_ptr<jcl::Evaluation> evaluation(new jcl::Evaluation);
  std::unique_ptr<ConsoleGame> game(new ConsoleGame(board.get(), evaluation.get()));
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
