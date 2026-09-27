#pragma once

#include <cstdint>

// Persists the best score in the ESP32's NVS flash so it survives power-off.
// Only written when the value changes, so flash wear is negligible.
class HighScoreStore {
 public:
  uint32_t load();
  void save(uint32_t best);
};
