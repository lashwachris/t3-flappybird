#pragma once

#include <cstdint>

#include "Config.h"
#include "input/InputState.h"

// Debounced, active-low push button. Call update() once per frame.
// Debouncing uses a lockout: the first edge is reported immediately and any
// bounce within the lockout window is ignored, so there is no added latency.
class Button {
 public:
  explicit Button(uint8_t pin, uint32_t lockoutMs = cfg::input::kDebounceLockoutMs)
      : pin_(pin), lockoutMs_(lockoutMs) {}

  void begin();
  void update(uint32_t nowMs);

  bool isDown() const { return down_; }
  bool wasPressed() const { return pressed_; }
  bool wasReleased() const { return released_; }
  ButtonState state() const { return {down_, pressed_}; }

 private:
  uint8_t pin_;
  uint32_t lockoutMs_;
  uint32_t lastChangeMs_ = 0;
  bool down_ = false;
  bool pressed_ = false;
  bool released_ = false;
};
