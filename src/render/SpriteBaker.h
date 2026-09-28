#pragma once

#include "hal/LGFX_TDisplayS3.h"
#include "render/PixelArt.h"

namespace sprite_baker {

// Colour used for transparent pixels in baked sprites. Art must not use it.
constexpr uint16_t kTransparent = 0xF81F;  // Magenta

// Allocates `out` at the art's size times `scale` and fills it from the
// character rows (each art pixel becomes a scale x scale block).
// Returns false if allocation fails.
bool bake(const PixelArt& art, LGFX_Sprite& out, uint8_t scale = 1);

// Like bake(), but adds a 1-art-pixel border on every side and fills each
// transparent pixel next to an opaque one (including diagonals) with `outline`.
bool bakeOutlined(const PixelArt& art, uint16_t outline, LGFX_Sprite& out, uint8_t scale = 1);

}  // namespace sprite_baker
