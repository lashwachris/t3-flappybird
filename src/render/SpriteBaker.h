#pragma once

#include "hal/LGFX_TDisplayS3.h"
#include "render/PixelArt.h"

namespace sprite_baker {

// Colour used for transparent pixels in baked sprites. Art must not use it.
constexpr uint16_t kTransparent = 0xF81F;  // Magenta

// Allocates `out` at the art's size and fills it from the character rows.
// Returns false if allocation fails.
bool bake(const PixelArt& art, LGFX_Sprite& out);

}  // namespace sprite_baker
