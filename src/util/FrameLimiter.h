#pragma once

#include <Arduino.h>

// Paces the main loop to a fixed frame rate. Sleeps (yielding to FreeRTOS) for
// most of the wait, then spins for the last millisecond for accurate timing.
class FrameLimiter {
 public:
  explicit FrameLimiter(uint32_t fps) : periodUs_(1000000UL / fps) {}

  void reset(uint32_t nowUs) { nextFrameUs_ = nowUs + periodUs_; }

  void wait() {
    const int32_t remainingUs = static_cast<int32_t>(nextFrameUs_ - micros());
    if (remainingUs > 2000) {
      delay((remainingUs - 1000) / 1000);
    }
    while (static_cast<int32_t>(nextFrameUs_ - micros()) > 0) {
    }

    nextFrameUs_ += periodUs_;
    // If we've fallen more than a frame behind, resync instead of bursting
    // several frames to catch up.
    const uint32_t nowUs = micros();
    if (static_cast<int32_t>(nowUs - nextFrameUs_) > static_cast<int32_t>(periodUs_)) {
      nextFrameUs_ = nowUs + periodUs_;
    }
  }

 private:
  uint32_t periodUs_;
  uint32_t nextFrameUs_ = 0;
};
