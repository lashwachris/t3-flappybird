#include "assets/BirdSkin.h"

#include "render/Colors.h"

namespace {

constexpr PaletteEntry kPalette[] = {
    {'K', colors::rgb565(40, 30, 30)},     // Outline
    {'Y', colors::rgb565(250, 200, 40)},   // Body
    {'W', colors::rgb565(250, 250, 250)},  // Eye white and wing
    {'O', colors::rgb565(240, 110, 40)},   // Beak
};

constexpr const char* kWingUp[] = {
    ".....KKKKKK......",
    "...KKYYYYKWWK....",
    "..KWWKYYKWWWWK...",
    ".KWWWWKYKWWKWK...",
    ".KWWWWKYKWWKWK...",
    ".KYWWYKYYKWWWK...",
    "..KKKKYYYYKKKKKK.",
    "..KYYYYYYKOOOOOOK",
    "..KYYYYYYOKKKKKK.",
    "...KYYYYYKOOOOOK.",
    "....KKYYYYKKKKK..",
    "......KKKKK......",
};

constexpr const char* kWingMid[] = {
    ".....KKKKKK......",
    "...KKYYYYKWWK....",
    "..KYYYYYKWWWWK...",
    ".KYYYYYYKWWKWK...",
    ".KKKKKYYKWWKWK...",
    "KWWWWWKYYKWWWK...",
    "KWWWWWKYYYKKKKKK.",
    ".KKKKKYYYKOOOOOOK",
    "..KYYYYYYOKKKKKK.",
    "...KYYYYYKOOOOOK.",
    "....KKYYYYKKKKK..",
    "......KKKKK......",
};

constexpr const char* kWingDown[] = {
    ".....KKKKKK......",
    "...KKYYYYKWWK....",
    "..KYYYYYKWWWWK...",
    ".KYYYYYYKWWKWK...",
    ".KYYYYYYKWWKWK...",
    ".KKKKKYYYKWWWK...",
    "KWWWWWKYYYKKKKKK.",
    "KWWWWKYYYKOOOOOOK",
    ".KWWKYYYYOKKKKKK.",
    "..KKYYYYYKOOOOOK.",
    "....KKYYYYKKKKK..",
    "......KKKKK......",
};

constexpr uint8_t kPaletteSize = sizeof(kPalette) / sizeof(kPalette[0]);

// Flap cycle: up, mid, down, mid.
constexpr PixelArt kFrames[] = {
    {17, 12, kWingUp, kPalette, kPaletteSize},
    {17, 12, kWingMid, kPalette, kPaletteSize},
    {17, 12, kWingDown, kPalette, kPaletteSize},
    {17, 12, kWingMid, kPalette, kPaletteSize},
};

}  // namespace

const CharacterSkin kBirdSkin = {
    kFrames,
    sizeof(kFrames) / sizeof(kFrames[0]),
    0.09f,
    {2.0f, 2.0f, 13.0f, 8.0f},
};
