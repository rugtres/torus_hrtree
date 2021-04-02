#ifndef TORUS_GRID_HPP_INCLUDED
#define TORUS_GRID_HPP_INCLUDED

// simple grid class with torus-topology
// 
// all bugs are mine: Hanno 2021


#include <vector>
#include "torus.hpp"


namespace torus {

  template <typename T>
  class grid_t
  {
  public:
    grid_t(grid_t&&) = default;
    grid_t& operator=(grid_t&&) = default;
    grid_t(const grid_t&) = default;
    grid_t& operator=(const grid_t&) = default;

    grid_t(size_t S) : S_(S), data_(S * S) {}
    grid_t(size_t S, T val) : S_(S), data_(S * S, val) {}

    size_t size() const noexcept { return S_ * S_; }
    size_t wh() const noexcept { return S_; }

    const T& operator()(const vec_t& coor) const noexcept
    {
      auto wp = wrap(coor);
      return *(data_.data() + static_cast<ptrdiff_t>((wp[1] * S_ + wp[0]) * S_));
    }

    T& operator()(const vec_t& coor) noexcept
    {
      auto wp = wrap(coor);
      return *(data_.data() + static_cast<ptrdiff_t>((wp[1] * S_ + wp[0]) * S_));
    }

    aabb_t pixel(const vec_t& coor) const noexcept
    {
      const float pr = 0.5f / S_;
      const auto pc = (float(S_) * wrap(coor)) + pr;   // pixel center
      return { pc, { pr, pr } };
    }

    auto cbegin() const noexcept { return data_.cbegin(); }
    auto cend() const noexcept { return data_.end(); }
    auto begin() const noexcept { return data_.cbegin(); }
    auto end() const noexcept { return data_.cend(); }
    auto begin() noexcept { return data_.begin(); }
    auto end() noexcept { return data_.end(); }

    const T* data() const noexcept { return data_.data(); }
    T* data() noexcept { return data_.data(); }

  private:
    size_t S_;
    std::vector<T> data_;
  };

}

#endif
