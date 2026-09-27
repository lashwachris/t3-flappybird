#include "render/Background.h"

#include <cmath>

#include "Config.h"
#include "render/Colors.h"
#include "render/SpriteBaker.h"
#include "util/Random.h"

namespace {

namespace palette {
constexpr uint8_t kSkyTop[3] = {84, 180, 236};
constexpr uint8_t kSkyHorizon[3] = {176, 228, 246};
constexpr uint16_t kCloud = colors::kWhite;
constexpr uint16_t kCloudShade = colors::rgb565(206, 232, 244);
constexpr uint16_t kBuilding = colors::rgb565(150, 205, 230);
constexpr uint16_t kWindow = colors::rgb565(190, 230, 244);
constexpr uint16_t kBush = colors::rgb565(116, 196, 84);
constexpr uint16_t kBushDark = colors::rgb565(84, 164, 64);
}  // namespace palette

// Tile sizes. Widths must divide evenly into the pattern for seamless wrapping.
constexpr int32_t kCloudW = 320;
constexpr int32_t kCloudH = 36;
constexpr int32_t kCloudY = 12;

constexpr int32_t kSkylineW = 160;
constexpr int32_t kSkylineH = 44;
constexpr int32_t kSkylineGap = 6;  // Skyline sits this far above the bushes' base.

constexpr int32_t kBushW = 128;
constexpr int32_t kBushH = 22;

// Fixed seeds so the scenery looks the same on every boot.
constexpr uint32_t kSkylineSeed = 0x5EED0001u;
constexpr uint32_t kBushSeed = 0x5EED0002u;

bool allocateTile(LGFX_Sprite& tile, int32_t w, int32_t h) {
  tile.setColorDepth(16);
  tile.setPsram(false);
  if (tile.createSprite(w, h) == nullptr) {
    return false;
  }
  tile.fillSprite(sprite_baker::kTransparent);
  return true;
}

uint8_t lerp(uint8_t a, uint8_t b, float t) { return static_cast<uint8_t>(a + (b - a) * t); }

// A puffy cloud: three overlapping circles on a flat base, with a shaded underside.
void drawCloud(LGFX_Sprite& tile, int32_t cx, int32_t cy, int32_t s) {
  for (int pass = 0; pass < 2; ++pass) {
    const int32_t dy = pass == 0 ? 2 : 0;
    const uint16_t color = pass == 0 ? palette::kCloudShade : palette::kCloud;
    tile.fillCircle(cx - s, cy + dy, s * 6 / 10, color);
    tile.fillCircle(cx, cy - s * 4 / 10 + dy, s * 8 / 10, color);
    tile.fillCircle(cx + s, cy + dy, s * 6 / 10, color);
    tile.fillRect(cx - s, cy + dy, 2 * s, s * 6 / 10, color);
  }
}

}  // namespace

bool Background::begin() {
  for (int i = 0; i < kSkyBands; ++i) {
    const float t = static_cast<float>(i) / (kSkyBands - 1);
    skyBands_[i] = colors::rgb565(lerp(palette::kSkyTop[0], palette::kSkyHorizon[0], t),
                                  lerp(palette::kSkyTop[1], palette::kSkyHorizon[1], t),
                                  lerp(palette::kSkyTop[2], palette::kSkyHorizon[2], t));
  }
  return bakeClouds() && bakeSkyline() && bakeBushes();
}

void Background::draw(LGFX_Sprite& canvas, double scrollDistance) {
  const int32_t skyH = static_cast<int32_t>(cfg::game::kGroundY);
  for (int i = 0; i < kSkyBands; ++i) {
    const int32_t y0 = skyH * i / kSkyBands;
    const int32_t y1 = skyH * (i + 1) / kSkyBands;
    canvas.fillRect(0, y0, canvas.width(), y1 - y0, skyBands_[i]);
  }

  const int32_t bushY = skyH - kBushH;
  const int32_t skylineY = skyH - kSkylineGap - kSkylineH;
  drawLayer(canvas, clouds_, scrollDistance, cfg::render::kCloudParallax, kCloudY);
  drawLayer(canvas, skyline_, scrollDistance, cfg::render::kSkylineParallax, skylineY);
  drawLayer(canvas, bushes_, scrollDistance, cfg::render::kBushParallax, bushY);
}

void Background::drawLayer(LGFX_Sprite& canvas, LGFX_Sprite& tile, double scrollDistance,
                           float parallax, int32_t y) {
  const int32_t tileW = tile.width();
  const int32_t offset = static_cast<int32_t>(std::fmod(scrollDistance * parallax, tileW));
  for (int32_t x = -offset; x < canvas.width(); x += tileW) {
    tile.pushSprite(&canvas, x, y, sprite_baker::kTransparent);
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

  int32_t x = 0;
  while (x < kSkylineW) {
    int32_t w = static_cast<int32_t>(rng.range(12.0f, 25.0f));
    if (kSkylineW - x < w + 8) w = kSkylineW - x;  // Avoid a sliver at the seam.
    const int32_t h = static_cast<int32_t>(rng.range(14.0f, static_cast<float>(kSkylineH)));
    const int32_t top = kSkylineH - h;

    skyline_.fillRect(x, top, w, h, palette::kBuilding);
    for (int32_t wy = top + 4; wy < kSkylineH - 3; wy += 6) {
      for (int32_t wx = x + 3; wx + 2 <= x + w - 3; wx += 5) {
        skyline_.fillRect(wx, wy, 2, 3, palette::kWindow);
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
    const uint16_t color = pass == 0 ? palette::kBushDark : palette::kBush;
    const int32_t grow = pass == 0 ? 1 : 0;
    for (const Bump& b : bumps) {
      for (int32_t wrap = -kBushW; wrap <= kBushW; wrap += kBushW) {
        bushes_.fillCircle(b.cx + wrap, b.cy, b.r + grow, color);
      }
    }
  }
  bushes_.fillRect(0, kBushH - 6, kBushW, 6, palette::kBush);  // Solid base.
  return true;
}
