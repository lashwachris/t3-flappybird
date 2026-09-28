#pragma once

#include <cstdint>

#include "hal/LGFX_TDisplayS3.h"
#include "render/SkyGradient.h"

// Night-only decorations: a glowing moon and a few flapping bats. They fade in
// with the night theme by blending from the sky colour behind them, so they
// appear and disappear smoothly with the palette. `night` is the eased 0..1
// night amount.
class NightSky {
 public:
  void drawMoon(LGFX_Sprite& canvas, const SkyGradient& sky, float night) const;
  void drawBats(LGFX_Sprite& canvas, const SkyGradient& sky, float night, float time) const;
};
