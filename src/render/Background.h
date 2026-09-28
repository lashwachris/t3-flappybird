#pragma once

#include <cstdint>

#include "hal/LGFX_TDisplayS3.h"
#include "render/NightSky.h"
#include "render/Theme.h"

// Parallax scenery behind the pipes: sky gradient, moon, clouds, city skyline,
// bats and bushes. Each layer is generated once into a horizontally tileable
// 4-bit palette sprite; every frame only its few palette entries are updated
// from the current theme, so colours can fade without re-drawing the tiles.
class Background {
 public:
  // Generates the layer tiles. Returns false on allocation failure.
  bool begin();

  // `night` is the eased 0..1 night amount; `time` drives the bats.
  void draw(LGFX_Sprite& canvas, double scrollDistance, const Theme& theme, float night,
            float time);

 private:
  bool bakeClouds();
  bool bakeSkyline();
  bool bakeBushes();
  void applyTheme(const Theme& theme);
  void drawLayer(LGFX_Sprite& canvas, LGFX_Sprite& tile, double scrollDistance, float parallax,
                 int32_t y);

  LGFX_Sprite clouds_;
  LGFX_Sprite skyline_;
  LGFX_Sprite bushes_;
  NightSky nightSky_;
};
