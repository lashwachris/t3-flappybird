#pragma once

#include <cstdint>

// Reads the battery voltage on the sense pin once per sample interval and
// keeps a smoothed value. Call update() every frame; it only touches the ADC
// when the interval has elapsed.
class Battery {
 public:
  void begin();
  void update(uint32_t nowMs);

  uint32_t millivolts() const { return mv_; }
  uint32_t pinMillivolts() const { return pinMv_; }
  uint8_t percent() const;
  // True when the reading is too high to be the cell alone (USB-fed).
  bool usbPowered() const;
  bool low() const;

 private:
  uint32_t readPinMillivolts() const;

  uint32_t lastSampleMs_ = 0;
  uint32_t pinMv_ = 0;
  float filteredMv_ = 0.0f;
  uint32_t mv_ = 0;
};
