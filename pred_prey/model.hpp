#ifndef PRED_PREY_MODEL_HPP_INCLUDED
#define PRED_PREY_MODEL_HPP_INCLUDED

#include <random>
#include <torus/torus.hpp>
#include <torus/torus_grid.hpp>
#include <torus/torus_hrtree.hpp>

namespace model {


  extern std::default_random_engine /* thread_local */ greng;


  struct Param
  {
    size_t N_prey = 40'000;     // # prey
    size_t N_pred = 4'000;      // # pred
    size_t GS = 200;         // grid size
    float grow_back = 0.01f;  // resource grow back (increment)

    float prey_step = 1.0f;   // step length [grid cells] 
    float pred_step = 1.5f;   // step length [grid cells] 
    float pred_sr = 0.5;     // search radius [grid cells]
  };


  struct Prey
  {
    torus::vec_t pos = { 0,0 };
    double uptake = 0.0;      // resource uptake; -1.0: dead
  };


  struct Pred
  {
    torus::aabb_t pos_sr = { {0,0,},{0,0} };    // position & search-radius
    int catches = 0;
  };


  class Simulation
  {
    using search_tree_t = torus::hrtree_t;

  public:
    Simulation(const Param& param);
    size_t run();

  private:
    void single_step();
    void random_walks();
    void graze();
    void hunt();

    torus::grid_t<float> grid_;
    std::vector<Prey> prey_;
    std::vector<Pred> pred_;
    search_tree_t prey_tree_;
    search_tree_t pred_tree_;
    Param param_;
  };

}

#endif
