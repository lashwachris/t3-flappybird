#pragma once

#include <cstdint>

// Pixel art written as rows of characters, so sprites can be read and edited
// directly in source. Each character is looked up in a palette; any character
// not in the palette (by convention '.') is transparent.
struct PaletteEntry {
  char key;
  uint16_t color;  // RGB565
};

struct PixelArt {
  uint8_t width;
  uint8_t height;
  const char* const* rows;  // `height` strings of exactly `width` characters.
  const PaletteEntry* palette;
  uint8_t paletteSize;
};
