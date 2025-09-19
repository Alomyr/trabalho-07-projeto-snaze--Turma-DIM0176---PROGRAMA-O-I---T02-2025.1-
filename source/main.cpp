#include "SnazeSimulation.hpp"

int main(int argc, char* argv[]) {
  // SnazeSimulation is a singleton, meaning only one instance of it can exist.
  // getInstance() provides access to this single instance.
  SnazeSimulation& game = SnazeSimulation::getInstance(); 
  
  game.initialize(argc, argv);
  
  // The Game Loop.
  while (not game.is_over()) {
    game.process_events();
    game.update();
    game.render();
  }

  return 0;
}
