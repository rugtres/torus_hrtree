#include <iostream>
#include <random>
#include <torus/torus.hpp>
#include <torus/torus_hrtree.hpp>
#include <torus/torus_grid.hpp>
#include <game_watches.hpp>


using namespace torus;


constexpr size_t N = 10'000;
constexpr size_t G = 10;


int main()
{
  grid_t<float> grid(100, 1.0f);
  auto t = grid({ 0.5f, 0.5f });

  std::vector<aabb_t> pop;
  auto reng = std::default_random_engine(0x12345678);
  auto pdist = std::uniform_real_distribution<float>(0.0f, 1.0f);
  for (size_t i = 0; i < N; ++i) {
    pop.push_back({ {pdist(reng), pdist(reng)}, {0.01f, 0.01f} });
  }

  game_watches::stop_watch bwatch{};
  game_watches::stop_watch qwatch{};
  game_watches::stop_watch bfwatch{};
  hrtree_t hrtree;
  size_t overlaps = 0;
  size_t bfoverlaps = 0;
  for (size_t g = 0; g < G; ++g) {
    bwatch.start();
    hrtree.build(pop.cbegin(), pop.cend(), [](const auto& bbox) { return bbox; });
    bwatch.stop();
    qwatch.start();
    for (size_t i = 0; i < N; ++i) {
      hrtree.query(pop[i], [&](auto idx) { ++overlaps; });
    }
    qwatch.stop();
    bfwatch.start();
    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < N; ++j) {
        if (intersects(pop[i], pop[j])) ++bfoverlaps;
      }
    }
    bfwatch.stop();
  }
  std::cout << bwatch.elapsed<std::chrono::microseconds>().count() / G << " us per build\n";
  std::cout << qwatch.elapsed<std::chrono::microseconds>().count() / G << " us per query round (hrtree)\n";
  std::cout << bfwatch.elapsed<std::chrono::microseconds>().count() / G << " us per query round (brute force)\n";
  std::cout << overlaps / G << ' ' << bfoverlaps / G << '\n';
  return 0;
}
