#include <iostream>
#include <random>
#include <algorithm>
#include <torus/torus.hpp>
#include <torus/torus_hrtree.hpp>
#include <torus/torus_grid.hpp>
#include <game_watches.hpp>


using namespace torus;


constexpr size_t N = 1'000;
constexpr size_t G = 10;


auto reng = std::default_random_engine(0x12345678);


template <typename SearchObj>
void test(std::vector<aabb_t>& pop)
{
  game_watches::stop_watch bwatch{};
  game_watches::stop_watch qwatch{};
  SearchObj stree;
  size_t overlaps = 0;
  for (size_t g = 0; g < G; ++g) {
    // advect
    auto mdist = std::uniform_real_distribution<float>(-0.001f, +0.001f);
    for (auto& pos : pop) {
      pos.center = wrap(pos.center + vec_t{ mdist(reng), mdist(reng) });
    }
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
