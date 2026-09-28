#include "scenes/DiagnosticsScene.h"

#include <cstdio>

#include "Config.h"
#include "render/Colors.h"

namespace {

constexpr int16_t kIndicatorX = 6;
constexpr int16_t kIndicatorW = 84;
constexpr int16_t kIndicatorH = 40;

// Moves a coordinate and reflects its velocity off [0, max].
void bounce(float& pos, float& vel, float dt, float max) {
  pos += vel * dt;
  if (pos < 0.0f) {
    pos = -pos;
    vel = -vel;
  } else if (pos > max) {
    pos = 2.0f * max - pos;
    vel = -vel;
  }
}

}  // namespace

void DiagnosticsScene::update(const InputState& input, float dt) {
  input_ = input;
  if (input.top.pressed) ++topPresses_;
  if (input.bottom.pressed) ++bottomPresses_;

  bounce(boxX_, boxVx_, dt, cfg::display::kWidth - kBoxSize);
  bounce(boxY_, boxVy_, dt, cfg::display::kHeight - kBoxSize);
}

void DiagnosticsScene::draw(LGFX_Sprite& canvas, const FrameStats& stats) {
  drawBackground(canvas);

  canvas.fillRect(static_cast<int32_t>(boxX_), static_cast<int32_t>(boxY_), kBoxSize, kBoxSize,
                  colors::kOrange);

  drawButtonIndicator(canvas, 6, "TOP", "GPIO0", input_.top.down, topPresses_);
  drawButtonIndicator(canvas, cfg::display::kHeight - kIndicatorH - 6, "BOTTOM", "GPIO14",
                      input_.bottom.down, bottomPresses_);
  drawStats(canvas, stats);
}

void DiagnosticsScene::drawBackground(LGFX_Sprite& canvas) const {
  static constexpr uint16_t kBars[] = {colors::kWhite, colors::kYellow,  colors::kCyan,
                                       colors::kGreen, colors::kMagenta, colors::kRed,
                                       colors::kBlue,  colors::kBlack};
  constexpr int kBarCount = sizeof(kBars) / sizeof(kBars[0]);
  const int16_t w = canvas.width();
  const int16_t h = canvas.height();
  const int16_t barW = w / kBarCount;

  for (int i = 0; i < kBarCount; ++i) {
    canvas.fillRect(i * barW, 0, barW, h, kBars[i]);
  }

  // Outermost pixels in red, next ring in black, so the border shows against
  // every bar. If an edge is missing, the panel offset is wrong.
  canvas.drawRect(0, 0, w, h, colors::kRed);
  canvas.drawRect(1, 1, w - 2, h - 2, colors::kBlack);

  // Marks the side where the USB-C port should be (left, per Config.h).
  canvas.setFont(&fonts::Font0);
  canvas.setTextDatum(textdatum_t::middle_left);
  canvas.fillRect(2, h / 2 - 8, 26, 16, colors::kBlack);
  canvas.setTextColor(colors::kWhite);
  canvas.drawString("USB", 5, h / 2);
}

void DiagnosticsScene::drawButtonIndicator(LGFX_Sprite& canvas, int16_t y, const char* label,
                                           const char* pin, bool down, uint32_t presses) const {
  const uint16_t fill = down ? colors::kGreen : colors::kDarkGrey;
  const uint16_t text = down ? colors::kBlack : colors::kWhite;

  canvas.fillRoundRect(kIndicatorX, y, kIndicatorW, kIndicatorH, 6, fill);
  canvas.drawRoundRect(kIndicatorX, y, kIndicatorW, kIndicatorH, 6, colors::kWhite);

  canvas.setTextColor(text);
  canvas.setTextDatum(textdatum_t::top_center);
  canvas.setFont(&fonts::Font2);
  canvas.drawString(label, kIndicatorX + kIndicatorW / 2, y + 4);
  canvas.setFont(&fonts::Font0);
  char line[24];
  snprintf(line, sizeof(line), "%s x%lu", pin, static_cast<unsigned long>(presses));
  canvas.drawString(line, kIndicatorX + kIndicatorW / 2, y + 26);
}

void DiagnosticsScene::drawStats(LGFX_Sprite& canvas, const FrameStats& stats) const {
  constexpr int16_t kPanelW = 176;
  constexpr int16_t kPanelH = 70;
  const int16_t x = canvas.width() - kPanelW - 8;
  const int16_t y = (canvas.height() - kPanelH) / 2;

  canvas.fillRoundRect(x, y, kPanelW, kPanelH, 6, colors::kBlack);
  canvas.drawRoundRect(x, y, kPanelW, kPanelH, 6, colors::kLightGrey);

  canvas.setTextColor(colors::kWhite);
  canvas.setTextDatum(textdatum_t::top_left);
  canvas.setFont(&fonts::Font2);

  char line[40];
  snprintf(line, sizeof(line), "FPS %.1f / %lu", stats.fps(),
           static_cast<unsigned long>(cfg::timing::kTargetFps));
  canvas.drawString(line, x + 8, y + 6);
  snprintf(line, sizeof(line), "render %.2f ms", stats.renderMs());
  canvas.drawString(line, x + 8, y + 26);
  snprintf(line, sizeof(line), "push %.2f ms (max %.0f)", stats.presentMs(), stats.maxFps());
  canvas.drawString(line, x + 8, y + 46);
}
