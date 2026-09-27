#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "Config.h"
#include "game/Rect.h"
#include "util/Random.h"

struct Pipe {
  float x = 0.0f;           // Left edge.
  float gapCenterY = 0.0f;  // Vertical centre of the opening.
  bool scored = false;

  Rect topRect() const {
    return {x, 0.0f, cfg::game::kPipeWidth, gapCenterY - cfg::game::kPipeGap / 2.0f};
  }
  Rect bottomRect() const {
    const float top = gapCenterY + cfg::game::kPipeGap / 2.0f;
    return {x, top, cfg::game::kPipeWidth, cfg::game::kGroundY - top};
  }
};

// Fixed pool of pipe pairs. Pipes that scroll off the left edge are recycled to
// the right with a new random gap, so nothing is allocated during play.
class Pipes {
 public:
  // Enough pipes to cover the screen plus one entering from the right.
  static constexpr size_t kCount =
      static_cast<size_t>(cfg::display::kWidth / cfg::game::kPipeSpacing) + 2;

  void reset(Random& rng);

  // Scrolls all pipes. Returns how many pipes the character passed this step.
  uint32_t update(float dt, float characterX, Random& rng);

  bool collides(const Rect& box) const;

  const std::array<Pipe, kCount>& items() const { return pipes_; }

 private:
  static float randomGapCenter(Random& rng);

  std::array<Pipe, kCount> pipes_{};
};
