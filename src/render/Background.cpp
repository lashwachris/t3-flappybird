#include "render/Background.h"

#include <cmath>

#include "Config.h"
#include "render/SkyGradient.h"
#include "util/Random.h"

namespace {

// Palette indices used inside the 4-bit layer tiles. Index 0 is transparent in
// every tile; the rest are filled from the current theme each frame.
namespace ink {
constexpr uint8_t kClear = 0;
constexpr uint8_t kCloud = 1;
constexpr uint8_t kCloudShade = 2;
constexpr uint8_t kBuilding = 1;
constexpr uint8_t kWindowLit = 2;
constexpr uint8_t kWindowDim = 3;
constexpr uint8_t kBush = 1;
constexpr uint8_t kBushDark = 2;
}  // namespace ink

// Tile sizes. Widths must divide evenly into the pattern for seamless wrapping.
constexpr int32_t kCloudW = 320;
constexpr int32_t kCloudH = 36;
constexpr int32_t kCloudY = 12;

constexpr int32_t kSkylineW = 160;
constexpr int32_t kSkylineH = 44;
constexpr int32_t kSkylineGap = 6;  // Skyline sits this far above the bushes' base.
constexpr float kLitWindowShare = 0.55f;  // Fraction of windows that glow at night.

constexpr int32_t kBushW = 128;
constexpr int32_t kBushH = 22;

// Fixed seeds so the scenery looks the same on every boot.
constexpr uint32_t kSkylineSeed = 0x5EED0001u;
constexpr uint32_t kBushSeed = 0x5EED0002u;
constexpr uint32_t kWindowSeed = 0x5EED0003u;  // Separate, so lighting doesn't move buildings.

bool allocateTile(LGFX_Sprite& tile, int32_t w, int32_t h) {
  tile.setColorDepth(4);
  tile.setPsram(false);
  if (tile.createSprite(w, h) == nullptr || !tile.createPalette()) {
    return false;
  }
  tile.fillSprite(ink::kClear);
  return true;
}

void setInk(LGFX_Sprite& tile, uint8_t index, Rgb color) {
  tile.setPaletteColor(index, color.r, color.g, color.b);
}

// A puffy cloud: three overlapping circles on a flat base, with a shaded underside.
void drawCloud(LGFX_Sprite& tile, int32_t cx, int32_t cy, int32_t s) {
  for (int pass = 0; pass < 2; ++pass) {
    const int32_t dy = pass == 0 ? 2 : 0;
    const uint8_t color = pass == 0 ? ink::kCloudShade : ink::kCloud;
    tile.fillCircle(cx - s, cy + dy, s * 6 / 10, color);
    tile.fillCircle(cx, cy - s * 4 / 10 + dy, s * 8 / 10, color);
    tile.fillCircle(cx + s, cy + dy, s * 6 / 10, color);
    tile.fillRect(cx - s, cy + dy, 2 * s, s * 6 / 10, color);
  }
}

}  // namespace

bool Background::begin() { return bakeClouds() && bakeSkyline() && bakeBushes(); }

void Background::draw(LGFX_Sprite& canvas, double scrollDistance, const Theme& theme,
                      float night, float time) {
  applyTheme(theme);

  const int32_t skyH = static_cast<int32_t>(cfg::game::kGroundY);
  const SkyGradient sky(theme[ThemeColor::SkyTop], theme[ThemeColor::SkyHorizon], skyH);
  sky.draw(canvas);

  const int32_t bushY = skyH - kBushH;
  const int32_t skylineY = skyH - kSkylineGap - kSkylineH;
  nightSky_.drawMoon(canvas, sky, night);
  drawLayer(canvas, clouds_, scrollDistance, cfg::render::kCloudParallax, kCloudY);
  drawLayer(canvas, skyline_, scrollDistance, cfg::render::kSkylineParallax, skylineY);
  nightSky_.drawBats(canvas, sky, night, time);
  drawLayer(canvas, bushes_, scrollDistance, cfg::render::kBushParallax, bushY);
}

void Background::applyTheme(const Theme& theme) {
  setInk(clouds_, ink::kCloud, theme[ThemeColor::Cloud]);
  setInk(clouds_, ink::kCloudShade, theme[ThemeColor::CloudShade]);
  setInk(skyline_, ink::kBuilding, theme[ThemeColor::Building]);
  setInk(skyline_, ink::kWindowLit, theme[ThemeColor::WindowLit]);
  setInk(skyline_, ink::kWindowDim, theme[ThemeColor::WindowDim]);
  setInk(bushes_, ink::kBush, theme[ThemeColor::Bush]);
  setInk(bushes_, ink::kBushDark, theme[ThemeColor::BushDark]);
}

void Background::drawLayer(LGFX_Sprite& canvas, LGFX_Sprite& tile, double scrollDistance,
                           float parallax, int32_t y) {
  const int32_t tileW = tile.width();
  const int32_t offset = static_cast<int32_t>(std::fmod(scrollDistance * parallax, tileW));
  for (int32_t x = -offset; x < canvas.width(); x += tileW) {
    tile.pushSprite(&canvas, x, y, ink::kClear);
  }
}

bool Background::bakeClouds() {
  if (!allocateTile(clouds_, kCloudW, kCloudH)) return false;
  drawCloud(clouds_, 50, 18, 10);
  drawCloud(clouds_, 168, 22, 8);
  drawCloud(clouds_, 262, 16, 11);
  return true;
}

bool Background::bakeSkyline() {
  if (!allocateTile(skyline_, kSkylineW, kSkylineH)) return false;
  Random rng(kSkylineSeed);
  Random windowRng(kWindowSeed);

  int32_t x = 0;
  while (x < kSkylineW) {
    int32_t w = static_cast<int32_t>(rng.range(12.0f, 25.0f));
    if (kSkylineW - x < w + 8) w = kSkylineW - x;  // Avoid a sliver at the seam.
    const int32_t h = static_cast<int32_t>(rng.range(14.0f, static_cast<float>(kSkylineH)));
    const int32_t top = kSkylineH - h;

    skyline_.fillRect(x, top, w, h, ink::kBuilding);
    for (int32_t wy = top + 4; wy < kSkylineH - 3; wy += 6) {
      for (int32_t wx = x + 3; wx + 2 <= x + w - 3; wx += 5) {
        const bool lit = windowRng.range(0.0f, 1.0f) < kLitWindowShare;
        skyline_.fillRect(wx, wy, 2, 3, lit ? ink::kWindowLit : ink::kWindowDim);
      }
    }
    x += w + 1;  // 1-px gap between buildings.
  }
  return true;
}

bool Background::bakeBushes() {
  if (!allocateTile(bushes_, kBushW, kBushH)) return false;

  struct Bump {
    int32_t cx, cy, r;
  };
  constexpr int32_t kStep = 16;
  Bump bumps[kBushW / kStep];

  Random rng(kBushSeed);
  for (int32_t i = 0; i < kBushW / kStep; ++i) {
    const int32_t r = static_cast<int32_t>(rng.range(9.0f, 14.0f));
    bumps[i] = {i * kStep + static_cast<int32_t>(rng.range(0.0f, 8.0f)), kBushH - r / 2, r};
  }

  // Dark rims first, then fills, so bumps overlap cleanly. Each bump is also
  // drawn one tile-width left and right so the tile wraps seamlessly.
  for (int pass = 0; pass < 2; ++pass) {
    const uint8_t color = pass == 0 ? ink::kBushDark : ink::kBush;
    const int32_t grow = pass == 0 ? 1 : 0;
    for (const Bump& b : bumps) {
      for (int32_t wrap = -kBushW; wrap <= kBushW; wrap += kBushW) {
        bushes_.fillCircle(b.cx + wrap, b.cy, b.r + grow, color);
      }
    }
  }
  bushes_.fillRect(0, kBushH - 6, kBushW, 6, ink::kBush);  // Solid base.
  return true;
}
