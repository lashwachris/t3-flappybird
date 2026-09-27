#include "hal/Button.h"

#include <Arduino.h>

void Button::begin() {
  pinMode(pin_, INPUT_PULLUP);
  down_ = digitalRead(pin_) == LOW;
  lastChangeMs_ = millis();
}

void Button::update(uint32_t nowMs) {
  pressed_ = false;
  released_ = false;

  const bool rawDown = digitalRead(pin_) == LOW;
  if (rawDown == down_ || nowMs - lastChangeMs_ < lockoutMs_) {
    return;
  }

  down_ = rawDown;
  lastChangeMs_ = nowMs;
  pressed_ = down_;
  released_ = !down_;
}
