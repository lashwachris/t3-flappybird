#pragma once

#include <cstdint>

// Collects per-frame timings and publishes averages once per window.
class FrameStats {
 public:
  explicit FrameStats(uint32_t windowMs) : windowUs_(windowMs * 1000UL) {}

  // Record one finished frame. Returns true when a new window of averages has
  // just been published.
  bool record(uint32_t renderUs, uint32_t presentUs, uint32_t nowUs) {
    if (frames_ == 0 && windowStartUs_ == 0) {
      windowStartUs_ = nowUs;
    }
    ++frames_;
    renderSumUs_ += renderUs;
    presentSumUs_ += presentUs;

    const uint32_t elapsedUs = nowUs - windowStartUs_;
    if (elapsedUs < windowUs_) {
      return false;
    }

    fps_ = frames_ * 1e6f / elapsedUs;
    renderMs_ = renderSumUs_ / 1000.0f / frames_;
    presentMs_ = presentSumUs_ / 1000.0f / frames_;

    windowStartUs_ = nowUs;
    frames_ = 0;
    renderSumUs_ = 0;
    presentSumUs_ = 0;
    return true;
  }

  float fps() const { return fps_; }
  float renderMs() const { return renderMs_; }
  float presentMs() const { return presentMs_; }
  float workMs() const { return renderMs_ + presentMs_; }
  // Frame rate the hardware could reach if the loop were not capped.
  float maxFps() const { return workMs() > 0.0f ? 1000.0f / workMs() : 0.0f; }

 private:
  uint32_t windowUs_;
  uint32_t windowStartUs_ = 0;
  uint32_t frames_ = 0;
  uint32_t renderSumUs_ = 0;
  uint32_t presentSumUs_ = 0;
  float fps_ = 0.0f;
  float renderMs_ = 0.0f;
  float presentMs_ = 0.0f;
};
