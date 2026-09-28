#pragma once

#include <cstdint>

#include "game/Rect.h"
#include "render/PixelArt.h"

// Everything that defines how the player character looks and how big it is.
// Swapping characters (e.g. bird -> cow) means providing a different skin; no
// game or rendering code needs to change.
struct CharacterSkin {
  const PixelArt* frames;  // Animation frames, all the same size.
  uint8_t frameCount;
  float frameDuration;     // Seconds per animation frame.
  Rect hitbox;             // Collision box relative to the frame's top-left corner.

  uint8_t width() const { return frames[0].width; }
  uint8_t height() const { return frames[0].height; }
};
