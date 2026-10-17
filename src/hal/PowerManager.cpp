#include "hal/PowerManager.h"

#include <Arduino.h>
#include <driver/rtc_io.h>
#include <esp_sleep.h>

#include "Config.h"

namespace power_manager {

namespace {
constexpr gpio_num_t kWakePin = static_cast<gpio_num_t>(cfg::pins::kButtonBottom);
}

bool wokeFromDeepSleep() { return esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT0; }

void finishWake() {
  rtc_gpio_deinit(kWakePin);
  waitForButtonsReleased(cfg::power::kReleaseTimeoutMs);
}

void waitForButtonsReleased(uint32_t timeoutMs) {
  pinMode(cfg::pins::kButtonTop, INPUT_PULLUP);
  pinMode(cfg::pins::kButtonBottom, INPUT_PULLUP);
  const uint32_t startMs = millis();
  while ((digitalRead(cfg::pins::kButtonTop) == LOW ||
          digitalRead(cfg::pins::kButtonBottom) == LOW) &&
         millis() - startMs < timeoutMs) {
    delay(10);
  }
  delay(50);  // Let contact bounce settle.
}

void enterDeepSleep(Display& display) {
  // Otherwise the still-held buttons would wake the chip immediately.
  waitForButtonsReleased(cfg::power::kReleaseTimeoutMs);

  display.powerDown();

  // The button is active-low; the RTC pull-up keeps it high while the digital
  // domain is off, so only a real press wakes the board.
  rtc_gpio_init(kWakePin);
  rtc_gpio_set_direction(kWakePin, RTC_GPIO_MODE_INPUT_ONLY);
  rtc_gpio_pulldown_dis(kWakePin);
  rtc_gpio_pullup_en(kWakePin);
  esp_sleep_enable_ext0_wakeup(kWakePin, 0);

  Serial.flush();
  esp_deep_sleep_start();
  for (;;) {
  }
}

}  // namespace power_manager
