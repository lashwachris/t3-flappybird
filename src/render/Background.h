#pragma once

#include <cstdint>

#include "hal/LGFX_TDisplayS3.h"

// Parallax scenery behind the pipes: sky gradient, clouds, city skyline and
// bushes. Each layer is generated once into a horizontally tileable sprite,
// then drawn every frame at an offset derived from the world scroll distance.
class Background {
 public:
  // Generates the layer tiles. Returns false on allocation failure.
  bool begin();

  void draw(LGFX_Sprite& canvas, double scrollDistance);

 private:
  static constexpr int kSkyBands = 10;

  bool bakeClouds();
  bool bakeSkyline();
  bool bakeBushes();
  void drawLayer(LGFX_Sprite& canvas, LGFX_Sprite& tile, double scrollDistance, float parallax,
                 int32_t y);

  uint16_t skyBands_[kSkyBands] = {};
  LGFX_Sprite clouds_;
  LGFX_Sprite skyline_;
  LGFX_Sprite bushes_;
};
