#include "application.h"

#include <cstdio>
#include <iostream>
#include <stdexcept>

int main(int argc, char ** argv)
{
  try {
    Application app(argc, argv);
    app.execute();
  }
  catch (std::invalid_argument & ex) {
    std::cerr << "Caught exception\n";
    std::cerr << ex.what() << "\n";
    return EXIT_FAILURE;
  }
  catch (std::exception & ex) {
    std::cerr << "Caught unknown exception\n";
    std::cerr << ex.what() << "\n";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
