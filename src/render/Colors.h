#pragma once

#include <cstdint>

// RGB565 palette. Values are uint16_t so LovyanGFX treats them as RGB565.
namespace colors {

constexpr uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b) {
  return static_cast<uint16_t>(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
}

constexpr uint16_t kBlack = rgb565(0, 0, 0);
constexpr uint16_t kWhite = rgb565(255, 255, 255);
constexpr uint16_t kRed = rgb565(255, 0, 0);
constexpr uint16_t kGreen = rgb565(0, 255, 0);
constexpr uint16_t kBlue = rgb565(0, 0, 255);
constexpr uint16_t kYellow = rgb565(255, 255, 0);
constexpr uint16_t kCyan = rgb565(0, 255, 255);
constexpr uint16_t kMagenta = rgb565(255, 0, 255);
constexpr uint16_t kOrange = rgb565(255, 140, 0);
constexpr uint16_t kDarkGrey = rgb565(48, 48, 48);
constexpr uint16_t kLightGrey = rgb565(180, 180, 180);

}  // namespace colors
