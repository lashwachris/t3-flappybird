#include "hal/Display.h"

#include <Arduino.h>

bool Display::begin() {
  pinMode(cfg::pins::kPowerEnable, OUTPUT);
  digitalWrite(cfg::pins::kPowerEnable, HIGH);

  lcd_.init();
  lcd_.setRotation(cfg::display::kRotation);
  lcd_.setBrightness(cfg::display::kBrightness);
  lcd_.fillScreen(0);

  if (lcd_.width() != cfg::display::kWidth || lcd_.height() != cfg::display::kHeight) {
    log_w("Panel reports %dx%d, expected %dx%d", lcd_.width(), lcd_.height(),
          cfg::display::kWidth, cfg::display::kHeight);
  }

  // 320x170x2 bytes = 108.8 KB. Kept in internal SRAM, which is DMA-capable
  // and much faster to draw into than PSRAM.
  canvas_.setColorDepth(16);
  canvas_.setPsram(false);
  return canvas_.createSprite(lcd_.width(), lcd_.height()) != nullptr;
}

void Display::present() { canvas_.pushSprite(0, 0); }
