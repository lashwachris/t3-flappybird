#pragma once

#include <cstdint>

// Small, fast xorshift32 PRNG. Game logic uses this instead of a platform RNG so
// it stays hardware-independent and a round can be reproduced from its seed.
class Random {
 public:
  explicit Random(uint32_t seed = kDefaultSeed) { reseed(seed); }

  void reseed(uint32_t seed) { state_ = seed != 0 ? seed : kDefaultSeed; }

  uint32_t next() {
    state_ ^= state_ << 13;
    state_ ^= state_ >> 17;
    state_ ^= state_ << 5;
    return state_;
  }

  // Uniform float in [lo, hi).
  float range(float lo, float hi) {
    return lo + (hi - lo) * static_cast<float>(next() >> 8) * (1.0f / 16777216.0f);
  }

 private:
  static constexpr uint32_t kDefaultSeed = 0x9E3779B9u;
  uint32_t state_ = kDefaultSeed;
};
