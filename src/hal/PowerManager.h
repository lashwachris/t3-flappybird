#pragma once

#include <cstdint>

#include "hal/Display.h"

// Deep sleep entry and wake handling.
namespace power_manager {

// True if this boot is a wake from deep sleep via the wake button. Call first
// thing in setup().
bool wokeFromDeepSleep();

// Returns the wake pin to normal GPIO use and waits for the wake press to end.
// Call in setup() when wokeFromDeepSleep() is true, before reading buttons.
void finishWake();

// Blocks until both buttons are released (or the timeout passes). Used so the
// press that put the board to sleep, or woke it, isn't read as game input.
void waitForButtonsReleased(uint32_t timeoutMs);

// Waits for release, powers the display down and enters deep sleep. The wake
// button (BOTTOM) restarts the firmware from setup(). Does not return.
[[noreturn]] void enterDeepSleep(Display& display);

}  // namespace power_manager
