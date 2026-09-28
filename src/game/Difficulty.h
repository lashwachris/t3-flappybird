#pragma once

#include <algorithm>
#include <cstdint>

#include "Config.h"
#include "game/Rect.h"

// Tunables that change as the score climbs.
struct Difficulty {
  float scrollSpeed;  // px/s
  float pipeGap;      // px

  // Gentle linear ramp: base values until kRampStartScore, then easing towards
  // the caps, which are reached at kRampFullScore and never exceeded.
  // Gaps are sized around the player's `hitbox` (see kPipeGapClearance).
  static Difficulty forScore(uint32_t score, const Rect& hitbox) {
    using namespace cfg::game;
    constexpr float kSpan = static_cast<float>(kRampFullScore - kRampStartScore);
    const float t =
        std::clamp((static_cast<float>(score) - kRampStartScore) / kSpan, 0.0f, 1.0f);
    return {kScrollSpeed + (kMaxScrollSpeed - kScrollSpeed) * t,
            hitbox.h + hitbox.w * kPipeGapPerWidth + kPipeGapClearance +
                (kMinPipeGapClearance - kPipeGapClearance) * t};
  }
};
