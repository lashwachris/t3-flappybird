#pragma once

#include <algorithm>
#include <cstdint>

#include "Config.h"

// Tracks how far the world has faded towards night (0 = full day, 1 = full
// night). The target flips every kNightCycleScore points; the amount eases
// towards it at a fixed rate so each fade takes kThemeFadeTime seconds.
class DayNightCycle {
 public:
  // Night on the odd blocks of kNightCycleScore points: 15-29, 45-59, ...
  static bool isNightScore(uint32_t score) {
    return (score / cfg::game::kNightCycleScore) % 2 == 1;
  }

  void update(bool night, float dt) {
    const float step = dt / cfg::game::kThemeFadeTime;
    amount_ = night ? std::min(1.0f, amount_ + step) : std::max(0.0f, amount_ - step);
  }

  // Linear fade progress. Renderers apply their own easing.
  float amount() const { return amount_; }

 private:
  float amount_ = 0.0f;
};
