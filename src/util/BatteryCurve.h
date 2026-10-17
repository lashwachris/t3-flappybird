#pragma once

#include <cstdint>

#include "Config.h"

namespace battery {

// Maps a resting LiPo voltage to a charge percentage. The discharge curve is
// flat around 3.7-3.9 V, so this is a piecewise-linear table, not a straight line.
inline uint8_t percentFromMillivolts(uint32_t mv) {
  struct Point {
    uint16_t mv;
    uint8_t percent;
  };
  static constexpr Point kCurve[] = {
      {cfg::power::kBatteryEmptyMv, 0},
      {3610, 5},
      {3690, 10},
      {3730, 20},
      {3770, 30},
      {3800, 40},
      {3840, 50},
      {3870, 60},
      {3910, 65},
      {3950, 70},
      {3980, 75},
      {4020, 80},
      {4080, 85},
      {4110, 90},
      {cfg::power::kBatteryFullMv, 100},
  };
  constexpr int kCount = sizeof(kCurve) / sizeof(kCurve[0]);

  if (mv <= kCurve[0].mv) return 0;
  if (mv >= kCurve[kCount - 1].mv) return 100;
  for (int i = 1; i < kCount; ++i) {
    if (mv <= kCurve[i].mv) {
      const uint32_t span = kCurve[i].mv - kCurve[i - 1].mv;
      const uint32_t into = mv - kCurve[i - 1].mv;
      const uint32_t dp = kCurve[i].percent - kCurve[i - 1].percent;
      return static_cast<uint8_t>(kCurve[i - 1].percent + (into * dp + span / 2) / span);
    }
  }
  return 100;
}

}  // namespace battery
