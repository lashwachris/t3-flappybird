#include "render/NumberRenderer.h"

#include <cstdio>

#include "assets/DigitsArt.h"
#include "render/SpriteBaker.h"

bool NumberRenderer::begin(uint16_t outline) {
  for (int d = 0; d < 10; ++d) {
    if (!sprite_baker::bakeOutlined(kDigitsArt[d], outline, digits_[d], scale_)) {
      return false;
    }
  }
  return true;
}

int32_t NumberRenderer::height() const { return digits_[0].height(); }

void NumberRenderer::draw(LGFX_Sprite& canvas, uint32_t value, int32_t cx, int32_t top) {
  char buf[11];
  const int count = snprintf(buf, sizeof(buf), "%lu", static_cast<unsigned long>(value));

  // Neighbouring digits share their 1-art-pixel outline column.
  const int32_t digitW = digits_[0].width();
  const int32_t advance = digitW - scale_;
  const int32_t totalW = count * advance + scale_;

  int32_t x = cx - totalW / 2;
  for (int i = 0; i < count; ++i) {
    digits_[buf[i] - '0'].pushSprite(&canvas, x, top, sprite_baker::kTransparent);
    x += advance;
  }
}
