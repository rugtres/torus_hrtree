#include <iostream>
#include <random>
#include <torus/torus.hpp>
#include <torus/torus_hrtree.hpp>
#include <torus/torus_grid.hpp>
#include <game_watches.hpp>


using namespace torus;


constexpr size_t N = 20'000;
constexpr size_t G = 10;


template <typename SearchObj>
void test(const std::vector<aabb_t>& pop)
{
  game_watches::stop_watch bwatch{};
  game_watches::stop_watch qwatch{};
  SearchObj stree;
  size_t overlaps = 0;
  for (size_t g = 0; g < G; ++g) {
    bwatch.start();
    stree.build(pop.cbegin(), pop.cend(), [](const auto& bbox) { return bbox; });
    bwatch.stop();
    qwatch.start();
    for (size_t i = 0; i < N; ++i) {
      stree.query(pop[i], [&](auto idx) { ++overlaps; });
    }
    qwatch.stop();
  }
  std::cout << bwatch.elapsed<std::chrono::microseconds>().count() / G << " us per build\n";
  std::cout << qwatch.elapsed<std::chrono::microseconds>().count() / G << " us per query round\n";
  std::cout << overlaps / G << " overlaps per gen\n";
}


int main()
{
  std::vector<aabb_t> pop;
  auto reng = std::default_random_engine(0x12345678);
  auto pdist = std::uniform_real_distribution<float>(0.0f, 1.0f);
  for (size_t i = 0; i < N; ++i) {
    pop.push_back({ {pdist(reng), pdist(reng)}, {0.01f, 0.01f} });
  }

  std::cout << "hrtree_t\n";
  test<hrtree_t>(pop);
  std::cout << "\nbrute_force_t\n";
  test<brute_force_t>(pop);

  return 0;
}
