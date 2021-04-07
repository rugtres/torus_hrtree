#include <iostream>
#include <game_watches.hpp>
#include "model.hpp"



int main()
{
  model::Simulation sim(model::Param{});
  game_watches::stop_watch<> watch{};
  watch.start();
  const auto t = sim.run();
  std::cout << "prey went extinct after " << t << " steps (";
  std::cout << watch.elapsed_seconds() << " s)\n";
  return 0;
}
