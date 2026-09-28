#include "render/NightSky.h"

#include <cmath>

#include "Config.h"

namespace {

// Below this, decorations are invisible anyway; skip drawing them.
constexpr float kVisibleThreshold = 0.02f;

constexpr Rgb kMoon = {248, 238, 200};
constexpr Rgb kMoonCrater = {214, 200, 160};
constexpr Rgb kMoonGlow = {255, 214, 150};
constexpr float kGlowStrength = 0.35f;
constexpr int32_t kMoonX = 256;
constexpr int32_t kMoonY = 34;
constexpr int32_t kMoonR = 11;
constexpr int32_t kGlowR = kMoonR + 5;

constexpr Rgb kBat = {22, 12, 28};

// 11x5 bat, two wing positions. 'X' = bat, '.' = transparent.
constexpr int kBatW = 11;
constexpr int kBatH = 5;
constexpr const char* kBatWingsUp[kBatH] = {
    "X.........X",
    "XX.......XX",
    ".XXX.X.XXX.",
    "..XXXXXXX..",
    "....XXX....",
};
constexpr const char* kBatWingsDown[kBatH] = {
    "....X.X....",
    "...XXXXX...",
    ".XXXXXXXXX.",
    "XX..XXX..XX",
    "X.........X",
};

// Each bat loops right-to-left across the sky on a gentle sine path.
struct BatPath {
  float speed;    // px/s leftwards
  float offset;   // px, staggers the bats
  float baseY;    // centre line
  float amplitude;
  float bobRate;  // rad/s
};
constexpr BatPath kBats[] = {
    {34.0f, 0.0f, 40.0f, 6.0f, 2.2f},
    {42.0f, 150.0f, 64.0f, 5.0f, 2.9f},
    {28.0f, 260.0f, 26.0f, 4.0f, 1.7f},
};
constexpr float kBatFlapRate = 7.0f;  // Wing changes per second.

void drawBat(LGFX_Sprite& canvas, int32_t x, int32_t y, bool wingsUp, uint16_t color) {
  const char* const* rows = wingsUp ? kBatWingsUp : kBatWingsDown;
  for (int row = 0; row < kBatH; ++row) {
    for (int col = 0; col < kBatW; ++col) {
      if (rows[row][col] == 'X') {
        canvas.drawPixel(x + col, y + row, color);
      }
    }
  }
}

}  // namespace

void NightSky::drawMoon(LGFX_Sprite& canvas, const SkyGradient& sky, float night) const {
  if (night < kVisibleThreshold) return;

  const Rgb behind = sky.at(kMoonY);
  canvas.fillCircle(kMoonX, kMoonY, kGlowR,
                    lerp(behind, kMoonGlow, kGlowStrength * night).to565());
  canvas.fillCircle(kMoonX, kMoonY, kMoonR, lerp(behind, kMoon, night).to565());

  const uint16_t crater = lerp(behind, kMoonCrater, night).to565();
  canvas.fillCircle(kMoonX - 4, kMoonY - 3, 2, crater);
  canvas.fillCircle(kMoonX + 3, kMoonY + 4, 3, crater);
  canvas.fillCircle(kMoonX + 4, kMoonY - 5, 1, crater);
}

void NightSky::drawBats(LGFX_Sprite& canvas, const SkyGradient& sky, float night,
                        float time) const {
  if (night < kVisibleThreshold) return;

  const float loop = static_cast<float>(canvas.width() + kBatW);
  for (size_t i = 0; i < sizeof(kBats) / sizeof(kBats[0]); ++i) {
    const BatPath& bat = kBats[i];
    const float travelled = std::fmod(time * bat.speed + bat.offset, loop);
    const int32_t x = static_cast<int32_t>(canvas.width() - travelled);
    const int32_t y =
        static_cast<int32_t>(bat.baseY + bat.amplitude * std::sin(time * bat.bobRate + i));
    const bool wingsUp = static_cast<int>(time * kBatFlapRate + i) % 2 == 0;
    drawBat(canvas, x, y, wingsUp, lerp(sky.at(y), kBat, night).to565());
  }
}
