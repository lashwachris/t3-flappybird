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

// Gameplay tuning. Distances are in pixels, speeds in px/s, times in seconds.
namespace game {
constexpr float kGroundHeight = 16.0f;
constexpr float kGroundY = display::kHeight - kGroundHeight;

constexpr float kBirdX = 80.0f;  // Horizontal centre of the character.
constexpr float kGravity = 600.0f;
constexpr float kFlapVelocity = -150.0f;
constexpr float kMaxFallSpeed = 260.0f;

constexpr float kScrollSpeed = 70.0f;
constexpr float kPipeWidth = 26.0f;
constexpr float kPipeGap = 50.0f;
constexpr float kPipeSpacing = 120.0f;   // Distance between consecutive pipes.
constexpr float kPipeMargin = 18.0f;     // Minimum distance of a gap from the top or ground.
constexpr float kFirstPipeX = display::kWidth + 40.0f;

constexpr float kRestartLockout = 0.4f;  // Ignore restart this long after dying.
}  // namespace game

// Purely visual settings.
namespace render {
constexpr float kTiltPerSpeed = 0.3f;  // Degrees of tilt per px/s of vertical speed.
constexpr float kMinTilt = -25.0f;     // Nose up.
constexpr float kMaxTilt = 70.0f;      // Nose down.
constexpr float kTitleBobAmplitude = 4.0f;
constexpr float kTitleBobSpeed = 4.0f;  // rad/s
constexpr bool kShowFps = false;        // Small FPS readout in the corner.
constexpr float kGroundPatternWidth = 16.0f;  // Ground texture repeats every N px.
}  // namespace render

namespace input {
// After a state change, further changes are ignored for this long. The first
// edge registers immediately, so debouncing adds no input latency.
constexpr uint32_t kDebounceLockoutMs = 30;
}  // namespace input

}  // namespace cfg
