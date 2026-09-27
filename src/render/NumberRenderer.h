#pragma once

#include <cstdint>

#include "hal/LGFX_TDisplayS3.h"

// Draws numbers with the outlined pixel-art digits at a fixed integer scale.
class NumberRenderer {
 public:
  explicit NumberRenderer(uint8_t scale) : scale_(scale) {}

  bool begin(uint16_t outline);

  // Draws `value` horizontally centred on `cx`, with its top edge at `top`.
  void draw(LGFX_Sprite& canvas, uint32_t value, int32_t cx, int32_t top);

  int32_t height() const;

 private:
  uint8_t scale_;
  LGFX_Sprite digits_[10];
};
