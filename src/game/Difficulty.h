#pragma once

#include <algorithm>
#include <cstdint>

#include "Config.h"

// Tunables that change as the score climbs.
struct Difficulty {
  float scrollSpeed;  // px/s
  float pipeGap;      // px

  // Gentle linear ramp: base values until kRampStartScore, then easing towards
  // the caps, which are reached at kRampFullScore and never exceeded.
  static Difficulty forScore(uint32_t score) {
    using namespace cfg::game;
    constexpr float kSpan = static_cast<float>(kRampFullScore - kRampStartScore);
    const float t =
        std::clamp((static_cast<float>(score) - kRampStartScore) / kSpan, 0.0f, 1.0f);
    return {kScrollSpeed + (kMaxScrollSpeed - kScrollSpeed) * t,
            kPipeGap + (kMinPipeGap - kPipeGap) * t};
  }
};
