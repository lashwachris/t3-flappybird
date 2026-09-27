#include "render/SpriteBaker.h"

namespace sprite_baker {

namespace {

uint16_t lookup(const PixelArt& art, char key, bool& found) {
  for (uint8_t i = 0; i < art.paletteSize; ++i) {
    if (art.palette[i].key == key) {
      found = true;
      return art.palette[i].color;
    }
  }
  found = false;
  return kTransparent;
}

}  // namespace

bool bake(const PixelArt& art, LGFX_Sprite& out) {
  out.setColorDepth(16);
  out.setPsram(false);
  if (out.createSprite(art.width, art.height) == nullptr) {
    return false;
  }
  out.fillSprite(kTransparent);

  for (uint8_t y = 0; y < art.height; ++y) {
    const char* row = art.rows[y];
    for (uint8_t x = 0; x < art.width && row[x] != '\0'; ++x) {
      bool found = false;
      const uint16_t color = lookup(art, row[x], found);
      if (found) {
        out.drawPixel(x, y, color);
      }
    }
  }
  return true;
}

}  // namespace sprite_baker
