#pragma once

#include "Config.h"

// Detects "both buttons held for kSleepHoldSeconds". Pure logic, no hardware.
// Feed it the held state every frame.
class SleepGesture {
 public:
  void update(bool topDown, bool bottomDown, float dt) {
    if (topDown && bottomDown) {
      heldSeconds_ += dt;
    } else {
      heldSeconds_ = 0.0f;
    }
  }

  // True while both buttons are held and the hold has not completed.
  bool arming() const { return heldSeconds_ > 0.0f && !triggered(); }
  bool triggered() const { return heldSeconds_ >= cfg::power::kSleepHoldSeconds; }
  float progress() const {
    const float p = heldSeconds_ / cfg::power::kSleepHoldSeconds;
    return p > 1.0f ? 1.0f : p;
  }

 private:
  float heldSeconds_ = 0.0f;
};
