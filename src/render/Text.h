#pragma once

#include "hal/LGFX_TDisplayS3.h"

namespace text {

// Draws `str` with a 1-px outline so it stays readable on any background.
// Uses the canvas's current font and text datum.
inline void drawOutlined(LGFX_Sprite& canvas, const char* str, int32_t x, int32_t y,
                         uint16_t color, uint16_t outline) {
  canvas.setTextColor(outline);
  for (int dy = -1; dy <= 1; ++dy) {
    for (int dx = -1; dx <= 1; ++dx) {
      if (dx != 0 || dy != 0) {
        canvas.drawString(str, x + dx, y + dy);
      }
    }
  }
  canvas.setTextColor(color);
  canvas.drawString(str, x, y);
}

}  // namespace text
