#pragma once

#include <algorithm>
#include <cstdint>

#include "hal/LGFX_TDisplayS3.h"
#include "render/Theme.h"

// Vertical sky gradient drawn as flat bands (cheap, and suits the pixel-art look).
class SkyGradient {
 public:
  static constexpr int kBands = 10;

  SkyGradient(Rgb top, Rgb horizon, int32_t height)
      : top_(top), horizon_(horizon), height_(height) {}

  // Colour of the band covering screen row y.
  Rgb at(int32_t y) const {
    return band(std::clamp<int32_t>(y * kBands / height_, 0, kBands - 1));
  }

  void draw(LGFX_Sprite& canvas) const {
    for (int i = 0; i < kBands; ++i) {
      const int32_t y0 = height_ * i / kBands;
      const int32_t y1 = height_ * (i + 1) / kBands;
      canvas.fillRect(0, y0, canvas.width(), y1 - y0, band(i).to565());
    }
  }

 private:
  Rgb band(int i) const { return lerp(top_, horizon_, static_cast<float>(i) / (kBands - 1)); }

  Rgb top_;
  Rgb horizon_;
  int32_t height_;
};
