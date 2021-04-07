#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>
#include "model.hpp"


using namespace torus;


namespace model {


  std::default_random_engine /* thread_local */ greng{};


  Simulation::Simulation(const Param& param) :
    grid_(param.GS, 0.0f),
    prey_(param.N_prey),
    pred_(param.N_pred),
    param_(param)
  {
    auto& reng = greng;
    auto pdist = std::uniform_real_distribution<float>(0.0f, 1.0f);
    for (auto& prey : prey_) {
      prey.pos = { pdist(reng), pdist(reng) };
    }

    const auto psr = param.pred_sr * grid_.pixel_radii();   // search radius
    for (auto& pred : pred_) {
      pred.pos_sr = { { pdist(reng), pdist(reng) }, psr };
    }
  }


  size_t Simulation::run()
  {
    size_t t = 0;
    for (; !prey_.empty(); ++t) {
      single_step();
    }
    return t;
  }


  void Simulation::single_step()
  {
    // grow back
    for (auto& c : grid_) {
      c = std::min(1.0f, c + param_.grow_back);
    }

    // move our critters around on the torus
    random_walks();

    // build the search trees
    prey_tree_.build(prey_.cbegin(), prey_.cend(), [](auto& prey) { return aabb_t{ prey.pos, {0,0} }; });
    pred_tree_.build(pred_.cbegin(), pred_.cend(), [](auto& pred) { return pred.pos_sr; });

    graze();
    hunt();

    // remove dead prey
    prey_.erase(
      std::partition(prey_.begin(), prey_.end(), [](const auto& prey) { return prey.uptake >= 0.0; }),
      prey_.end()
    );
  }


  void Simulation::random_walks()
  {
    auto& reng = greng;
    auto adist = std::uniform_real_distribution<float>(0.f, static_cast<float>(2.0 * M_PI));
    auto sdist = [&]() {
      const auto a = adist(reng);
      return vec_t{ std::cos(a), std::sin(a) };
    };
    for (auto& prey : prey_) {
      prey.pos = wrap(prey.pos + param_.prey_step * sdist());
    }
    for (auto& pred : pred_) {
      pred.pos_sr.center = wrap(pred.pos_sr.center + param_.pred_step * sdist());
    }
  }


  void Simulation::graze()
  {
    // fair share between prey on same cell
    for (size_t i = 0; i < prey_.size(); ++i) {
      int on_cell = 0;          // # prey on cell
      prey_tree_.query(grid_.pixel(prey_[i].pos), [&](size_t /* j */) {
        ++on_cell;    // including 'i'
        });
      prey_[i].uptake += grid_(prey_[i].pos) / static_cast<double>(on_cell);
    }
    for (size_t i = 0; i < prey_.size(); ++i) {
      grid_(prey_[i].pos) = 0.0f;
    }
  }


  void Simulation::hunt()
  {
    // closest predator wins 
    for (size_t i = 0; i < prey_.size(); ++i) {
      float min_dd = param_.pred_sr * param_.pred_sr;  // min distance2 so far
      size_t jpred = -1;
      const auto pos = prey_[i].pos;
      pred_tree_.query({ pos, {0,0} }, [&](size_t j) {
        const auto dd = distance2(pos, pred_[j].pos_sr.center);
        if (dd < min_dd) {
          min_dd = dd;
          jpred = j;
          prey_[i].uptake = -1.0;   // doomed
        }
      });
      if (jpred != -1) {
        ++pred_[jpred].catches;
      }
    }
  }

}
