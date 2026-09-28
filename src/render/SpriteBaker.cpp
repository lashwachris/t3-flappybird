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

bool isOpaque(const PixelArt& art, int x, int y) {
  if (x < 0 || y < 0 || x >= art.width || y >= art.height) return false;
  bool found = false;
  lookup(art, art.rows[y][x], found);
  return found;
}

bool allocate(LGFX_Sprite& out, int w, int h) {
  out.setColorDepth(16);
  out.setPsram(false);
  if (out.createSprite(w, h) == nullptr) {
    return false;
  }
  out.fillSprite(kTransparent);
  return true;
}

// Draws the art's opaque pixels, scaled, with the art's top-left pixel at
// art-pixel position (ox, oy) of the sprite.
void drawArt(const PixelArt& art, LGFX_Sprite& out, int ox, int oy, uint8_t scale) {
  for (uint8_t y = 0; y < art.height; ++y) {
    const char* row = art.rows[y];
    for (uint8_t x = 0; x < art.width && row[x] != '\0'; ++x) {
      bool found = false;
      const uint16_t color = lookup(art, row[x], found);
      if (found) {
        out.fillRect((ox + x) * scale, (oy + y) * scale, scale, scale, color);
      }
    }
  }
}

}  // namespace

bool bake(const PixelArt& art, LGFX_Sprite& out, uint8_t scale) {
  if (!allocate(out, art.width * scale, art.height * scale)) {
    return false;
  }
  drawArt(art, out, 0, 0, scale);
  return true;
}

bool bakeOutlined(const PixelArt& art, uint16_t outline, LGFX_Sprite& out, uint8_t scale) {
  if (!allocate(out, (art.width + 2) * scale, (art.height + 2) * scale)) {
    return false;
  }
  // Sprite pixel (sx, sy) corresponds to art pixel (sx - 1, sy - 1).
  for (int sy = 0; sy < art.height + 2; ++sy) {
    for (int sx = 0; sx < art.width + 2; ++sx) {
      if (isOpaque(art, sx - 1, sy - 1)) continue;
      bool touches = false;
      for (int dy = -1; dy <= 1 && !touches; ++dy) {
        for (int dx = -1; dx <= 1 && !touches; ++dx) {
          touches = isOpaque(art, sx - 1 + dx, sy - 1 + dy);
        }
      }
      if (touches) {
        out.fillRect(sx * scale, sy * scale, scale, scale, outline);
      }
    }
  }
  drawArt(art, out, 1, 1, scale);
  return true;
}

}  // namespace sprite_baker
