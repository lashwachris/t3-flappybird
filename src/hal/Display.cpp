#include "hal/Display.h"

#include <Arduino.h>
#include <driver/gpio.h>

bool Display::begin() {
  // After a deep-sleep wake the pins are still latched off by powerDown().
  gpio_deep_sleep_hold_dis();
  gpio_hold_dis(static_cast<gpio_num_t>(cfg::pins::kPowerEnable));
  gpio_hold_dis(static_cast<gpio_num_t>(cfg::pins::kLcdBacklight));

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

void Display::powerDown() {
  for (int b = cfg::display::kBrightness; b > 0; b -= 8) {
    lcd_.setBrightness(b);
    delay(cfg::power::kFadeStepMs);
  }
  lcd_.setBrightness(0);
  lcd_.sleep();

  digitalWrite(cfg::pins::kPowerEnable, LOW);
  gpio_hold_en(static_cast<gpio_num_t>(cfg::pins::kPowerEnable));
  gpio_hold_en(static_cast<gpio_num_t>(cfg::pins::kLcdBacklight));
  gpio_deep_sleep_hold_en();
}

void Display::present() { canvas_.pushSprite(0, 0); }
