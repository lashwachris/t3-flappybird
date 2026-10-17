#pragma once

#include <cstdint>

#include "hal/Battery.h"
#include "hal/LGFX_TDisplayS3.h"

// Status drawn on top of whatever scene is active, just before present().
namespace power_overlay {

// Battery icon with a percentage in the top-right corner. Shows a bolt instead
// of a number when the board looks USB-powered, and blinks red when low.
void drawBattery(LGFX_Sprite& canvas, const Battery& battery, uint32_t nowMs);

// "Hold to sleep" panel with a fill bar, centred on screen. progress is 0-1.
void drawSleepProgress(LGFX_Sprite& canvas, float progress);

}  // namespace power_overlay
