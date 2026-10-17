#include "hal/Battery.h"

#include <Arduino.h>

#include <algorithm>

#include "Config.h"
#include "util/BatteryCurve.h"

void Battery::begin() {
  pinMode(cfg::pins::kBatterySense, INPUT);
  analogSetPinAttenuation(cfg::pins::kBatterySense, ADC_11db);
  lastSampleMs_ = millis();
  pinMv_ = readPinMillivolts();
  filteredMv_ = pinMv_ * cfg::power::kBatteryDividerRatio;
  mv_ = static_cast<uint32_t>(filteredMv_ + 0.5f);
}

void Battery::update(uint32_t nowMs) {
  if (nowMs - lastSampleMs_ < cfg::power::kBatterySampleMs) return;
  lastSampleMs_ = nowMs;

  pinMv_ = readPinMillivolts();
  const float sample = pinMv_ * cfg::power::kBatteryDividerRatio;
  filteredMv_ += (sample - filteredMv_) * cfg::power::kBatteryFilter;
  mv_ = static_cast<uint32_t>(filteredMv_ + 0.5f);
}

uint32_t Battery::readPinMillivolts() const {
  uint32_t samples[cfg::power::kBatterySamples];
  for (auto& s : samples) {
    s = analogReadMilliVolts(cfg::pins::kBatterySense);
  }
  std::nth_element(samples, samples + cfg::power::kBatterySamples / 2,
                   samples + cfg::power::kBatterySamples);
  return samples[cfg::power::kBatterySamples / 2];
}

uint8_t Battery::percent() const { return battery::percentFromMillivolts(mv_); }
bool Battery::usbPowered() const { return mv_ >= cfg::power::kUsbPoweredMv; }
bool Battery::low() const { return !usbPowered() && percent() < cfg::power::kLowBatteryPercent; }
