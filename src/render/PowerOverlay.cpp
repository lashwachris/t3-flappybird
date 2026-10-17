#include "render/PowerOverlay.h"

#include <cstdio>

#include "Config.h"
#include "render/Colors.h"
#include "render/Text.h"

namespace power_overlay {

namespace {

constexpr int16_t kMargin = 4;
constexpr int16_t kBodyW = 22;
constexpr int16_t kBodyH = 11;
constexpr int16_t kNubW = 2;
constexpr uint16_t kOutline = colors::rgb565(40, 30, 30);
constexpr uint32_t kLowBlinkPeriodMs = 800;

void drawBolt(LGFX_Sprite& canvas, int16_t cx, int16_t cy, uint16_t color) {
  canvas.fillTriangle(cx + 1, cy - 4, cx - 3, cy + 1, cx, cy + 1, color);
  canvas.fillTriangle(cx, cy + 1, cx + 3, cy - 1, cx - 1, cy + 4, color);
}

}  // namespace

void drawBattery(LGFX_Sprite& canvas, const Battery& battery, uint32_t nowMs) {
  const int16_t x = canvas.width() - kMargin - kBodyW - kNubW;
  const int16_t y = kMargin;
  const bool usb = battery.usbPowered();
  const bool low = battery.low();
  const bool blinkOff = low && (nowMs % kLowBlinkPeriodMs) >= kLowBlinkPeriodMs / 2;

  uint16_t fillColor = colors::kGreen;
  if (low) {
    fillColor = colors::kRed;
  } else if (battery.percent() < 40) {
    fillColor = colors::kYellow;
  }

  canvas.fillRect(x, y, kBodyW, kBodyH, kOutline);
  canvas.fillRect(x + kBodyW, y + 3, kNubW, kBodyH - 6, kOutline);
  canvas.fillRect(x + 1, y + 1, kBodyW - 2, kBodyH - 2, colors::kDarkGrey);

  if (usb) {
    drawBolt(canvas, x + kBodyW / 2, y + kBodyH / 2, colors::kYellow);
    return;
  }

  if (!blinkOff) {
    const int16_t innerW = kBodyW - 4;
    int16_t w = innerW * battery.percent() / 100;
    if (w < 1) w = 1;
    canvas.fillRect(x + 2, y + 2, w, kBodyH - 4, fillColor);
  }

  char text[8];
  snprintf(text, sizeof(text), "%u%%", battery.percent());
  canvas.setFont(&fonts::Font0);
  canvas.setTextDatum(textdatum_t::middle_right);
  text::drawOutlined(canvas, text, x - 3, y + kBodyH / 2, low ? colors::kRed : colors::kWhite,
                     kOutline);
}

void drawSleepProgress(LGFX_Sprite& canvas, float progress) {
  constexpr int16_t kW = 150;
  constexpr int16_t kH = 40;
  const int16_t x = (canvas.width() - kW) / 2;
  const int16_t y = (canvas.height() - kH) / 2;

  canvas.fillRoundRect(x, y, kW, kH, 6, colors::kBlack);
  canvas.drawRoundRect(x, y, kW, kH, 6, colors::kLightGrey);

  canvas.setFont(&fonts::Font2);
  canvas.setTextDatum(textdatum_t::top_center);
  canvas.setTextColor(colors::kWhite);
  canvas.drawString("HOLD TO SLEEP", x + kW / 2, y + 5);

  constexpr int16_t kBarW = kW - 20;
  canvas.drawRect(x + 10, y + 26, kBarW, 8, colors::kLightGrey);
  canvas.fillRect(x + 11, y + 27, static_cast<int16_t>((kBarW - 2) * progress), 6,
                  colors::kCyan);
}

}  // namespace power_overlay
