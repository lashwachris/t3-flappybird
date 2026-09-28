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

constexpr float kPlayerX = 80.0f;  // Horizontal centre of the character.
constexpr float kGravity = 600.0f;
constexpr float kFlapVelocity = -150.0f;
constexpr float kMaxFallSpeed = 260.0f;

constexpr float kScrollSpeed = 70.0f;
constexpr float kPipeWidth = 26.0f;
// Pipe gaps are sized relative to the character's hitbox:
//   gap = hitbox height + clearance + hitbox width * kPipeGapPerWidth
// A wider character spends longer inside each pipe, so it gets a little more
// room. Keeps difficulty equal across skins: the bird (13x8) gets the 50 -> 42 px
// gaps that play-tested well; the cow (20x11) gets 54 -> 46 px, which matched the
// bird's survival rate in simulation.
constexpr float kPipeGapClearance = 40.0f;
constexpr float kPipeGapPerWidth = 0.15f;
constexpr float kPipeSpacing = 120.0f;   // Distance between consecutive pipes.
constexpr float kPipeMargin = 18.0f;     // Minimum distance of a gap from the top or ground.
constexpr float kFirstPipeX = display::kWidth + 40.0f;

// Difficulty ramp: from kRampStartScore the speed rises and the gap clearance
// narrows, reaching the limits below at kRampFullScore. Gaps are fixed when each
// pipe spawns, so a pipe never changes while it's on screen.
constexpr uint32_t kRampStartScore = 10;
constexpr uint32_t kRampFullScore = 50;
constexpr float kMaxScrollSpeed = 85.0f;
constexpr float kMinPipeGapClearance = 32.0f;

constexpr float kRestartLockout = 0.4f;  // Ignore restart this long after dying.

// Day/night cycle: the world fades to the Halloween night theme every
// kNightCycleScore points and back again (night at 15-29, 45-59, ...).
constexpr uint32_t kNightCycleScore = 15;
constexpr float kThemeFadeTime = 1.5f;  // Seconds per fade (requested max: 2 s).
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
constexpr float kDeathFlashTime = 0.08f;      // White flash when crashing.

// Parallax: fraction of the world scroll speed each background layer moves at.
constexpr float kCloudParallax = 0.1f;
constexpr float kSkylineParallax = 0.25f;
constexpr float kBushParallax = 0.5f;
}  // namespace render

namespace input {
// After a state change, further changes are ignored for this long. The first
// edge registers immediately, so debouncing adds no input latency.
constexpr uint32_t kDebounceLockoutMs = 30;
}  // namespace input

}  // namespace cfg
