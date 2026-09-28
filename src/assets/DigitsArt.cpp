#include "assets/DigitsArt.h"

#include "render/Colors.h"

namespace {

constexpr PaletteEntry kPalette[] = {{'W', colors::kWhite}};
constexpr uint8_t kPaletteSize = sizeof(kPalette) / sizeof(kPalette[0]);

// clang-format off
constexpr const char* k0[] = {".WWW.", "WW.WW", "WW.WW", "WW.WW", "WW.WW", "WW.WW", "WW.WW", ".WWW."};
constexpr const char* k1[] = {"..WW.", ".WWW.", "..WW.", "..WW.", "..WW.", "..WW.", "..WW.", ".WWWW"};
constexpr const char* k2[] = {".WWW.", "WW.WW", "...WW", "..WW.", ".WW..", "WW...", "WW...", "WWWWW"};
constexpr const char* k3[] = {"WWWW.", "...WW", "...WW", ".WWW.", "...WW", "...WW", "...WW", "WWWW."};
constexpr const char* k4[] = {"...WW", "..WWW", ".WWWW", "WW.WW", "WWWWW", "...WW", "...WW", "...WW"};
constexpr const char* k5[] = {"WWWWW", "WW...", "WW...", "WWWW.", "...WW", "...WW", "WW.WW", ".WWW."};
constexpr const char* k6[] = {".WWW.", "WW...", "WW...", "WWWW.", "WW.WW", "WW.WW", "WW.WW", ".WWW."};
constexpr const char* k7[] = {"WWWWW", "...WW", "...WW", "..WW.", "..WW.", ".WW..", ".WW..", ".WW.."};
constexpr const char* k8[] = {".WWW.", "WW.WW", "WW.WW", ".WWW.", "WW.WW", "WW.WW", "WW.WW", ".WWW."};
constexpr const char* k9[] = {".WWW.", "WW.WW", "WW.WW", "WW.WW", ".WWWW", "...WW", "...WW", ".WWW."};
// clang-format on

}  // namespace

const PixelArt kDigitsArt[10] = {
    {5, 8, k0, kPalette, kPaletteSize}, {5, 8, k1, kPalette, kPaletteSize},
    {5, 8, k2, kPalette, kPaletteSize}, {5, 8, k3, kPalette, kPaletteSize},
    {5, 8, k4, kPalette, kPaletteSize}, {5, 8, k5, kPalette, kPaletteSize},
    {5, 8, k6, kPalette, kPaletteSize}, {5, 8, k7, kPalette, kPaletteSize},
    {5, 8, k8, kPalette, kPaletteSize}, {5, 8, k9, kPalette, kPaletteSize},
};
