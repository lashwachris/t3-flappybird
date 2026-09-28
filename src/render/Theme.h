#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "render/Colors.h"

// Colour themes for the scenery. The renderer blends between the day and night
// themes every frame, so every themed colour fades smoothly.

struct Rgb {
  uint8_t r, g, b;

  uint16_t to565() const { return colors::rgb565(r, g, b); }
};

inline Rgb lerp(Rgb a, Rgb b, float t) {
  auto mix = [t](uint8_t x, uint8_t y) {
    return static_cast<uint8_t>(x + (static_cast<int>(y) - x) * t + 0.5f);
  };
  return {mix(a.r, b.r), mix(a.g, b.g), mix(a.b, b.b)};
}

enum class ThemeColor : uint8_t {
  SkyTop,
  SkyHorizon,
  Cloud,
  CloudShade,
  Building,
  WindowLit,  // Windows that glow at night.
  WindowDim,  // Windows that go dark at night.
  Bush,
  BushDark,
  Pipe,
  PipeLight,
  PipeDark,
  GrassLight,
  GrassDark,
  Dirt,
  Count,
};

class Theme {
 public:
  Rgb operator[](ThemeColor c) const { return colors_[static_cast<size_t>(c)]; }
  Rgb& operator[](ThemeColor c) { return colors_[static_cast<size_t>(c)]; }
  uint16_t rgb565(ThemeColor c) const { return (*this)[c].to565(); }

  // Colour-by-colour blend: t = 0 gives `a`, t = 1 gives `b`.
  static Theme blend(const Theme& a, const Theme& b, float t);

 private:
  std::array<Rgb, static_cast<size_t>(ThemeColor::Count)> colors_{};
};

extern const Theme kDayTheme;
extern const Theme kNightTheme;  // Dusky Halloween palette.
