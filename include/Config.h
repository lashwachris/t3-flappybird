#pragma once

#include <cstdint>

// Every hardware pin and tuning constant lives here so the rest of the code
// never hard-codes board details.
namespace cfg {

namespace pins {
// Peripheral power rail. Must be driven HIGH or the LCD stays dark on battery.
constexpr int8_t kPowerEnable = 15;

// ST7789 on an 8-bit Intel 8080 parallel bus.
constexpr int8_t kLcdBacklight = 38;
constexpr int8_t kLcdCs = 6;
constexpr int8_t kLcdRst = 5;
constexpr int8_t kLcdDc = 7;
constexpr int8_t kLcdWr = 8;
constexpr int8_t kLcdRd = 9;
constexpr int8_t kLcdData[8] = {39, 40, 41, 42, 45, 46, 47, 48};

// Physical buttons (active-low). Mapping assumes the board is held in
// landscape with the USB-C port on the left. Swap these if it's reversed.
constexpr uint8_t kButtonTop = 0;      // BOOT
constexpr uint8_t kButtonBottom = 14;  // KEY
}  // namespace pins

namespace display {
constexpr int16_t kWidth = 320;
constexpr int16_t kHeight = 170;

// Landscape rotation. 3 = USB-C on the left; use 1 if the image is upside down.
constexpr uint8_t kRotation = 3;

constexpr uint32_t kBusWriteHz = 20000000;
constexpr uint8_t kBrightness = 200;  // 0-255
}  // namespace display

namespace timing {
constexpr uint32_t kTargetFps = 60;
constexpr float kFixedDt = 1.0f / kTargetFps;
constexpr uint32_t kStatsWindowMs = 500;
}  // namespace timing

namespace input {
// After a state change, further changes are ignored for this long. The first
// edge registers immediately, so debouncing adds no input latency.
constexpr uint32_t kDebounceLockoutMs = 30;
}  // namespace input

}  // namespace cfg
