# Flappy Bird for LilyGo T-Display S3 — Implementation Plan

## Hardware facts (verified against LilyGo docs)

| Item | Value |
|---|---|
| MCU | ESP32-S3 (16 MB flash, 8 MB OPI PSRAM) |
| Display | 1.9" ST7789, **170 × 320**, 8-bit parallel (Intel 8080) bus |
| LCD pins | D0–D7 = GPIO 39–46, 47, 48 · WR 8 · RD 9 · DC 7 · CS 6 · RST 5 |
| Backlight | GPIO 38 |
| Peripheral power | GPIO 15, **must be driven HIGH** or the LCD stays dark on battery |
| Buttons | GPIO 0 (BOOT) and GPIO 14 (KEY), active-low |
| Panel quirk | 35 px column offset, colour inversion on |

## Key decisions

1. **Orientation: landscape, 320 × 170.** "Top" and "bottom" buttons only make
   sense when the board is held sideways, with the buttons stacked on one short edge.
   The full native resolution is used, with no scaling.
2. **Toolchain: PlatformIO with the Arduino framework.** It gives reproducible builds
   with `platformio.ini` and pinned library versions, can be built in CI, and also
   works from VS Code.
3. **Graphics library: LovyanGFX instead of TFT_eSPI.**
   - TFT_eSPI's ESP32-S3 parallel mode expects data pins in GPIO 0–31. The T-Display
     S3 uses GPIO 39–48, so it needs LilyGo's patched fork and the pinned Arduino core
     2.0.14.
   - LovyanGFX drives the S3's hardware i80 LCD peripheral with DMA, supports any
     pins, and runs on current cores.
4. **Rendering: a full-frame off-screen sprite that is pushed once per frame.**
   - 320 × 170 × 2 B = 108.8 KB, which fits in internal SRAM, so there is no flicker
     and no dirty-rect bookkeeping.
   - Bus budget: the i80 bus at ~20 MHz × 8 bit moves one frame in about 5–6 ms. That
     allows well over 100 fps in theory, so **60 fps is the target and 30 fps is the
     floor**.
   - If profiling shows that drawing plus pushing exceeds 16 ms, move to
     double-buffering (render frame N+1 while DMA sends frame N).
5. **Fixed-timestep game loop at 60 Hz.** Physics uses px/s units scaled by a
   constant `dt`. Game speed stays the same even if a frame drops, and tuning numbers
   don't depend on the frame rate.

## Code structure (small, single-responsibility modules)

```
platformio.ini
include/
  Config.h          // pins, screen size, all gameplay tuning constants
src/
  main.cpp          // setup() + fixed-timestep loop only
  hal/
    Display.h/.cpp  // LGFX device class (bus + panel + backlight config), power-on
    Button.h/.cpp   // reusable debounced button: isDown(), wasPressed() edge detect
  game/
    Game.h/.cpp     // state machine: Title -> Playing -> GameOver -> (restart)
    Bird.h/.cpp     // position, velocity, flap impulse, gravity, rotation
    Pipes.h/.cpp    // fixed-size ring of pipe pairs, spawn/recycle, scoring
    Collision.h     // AABB helpers (bird hitbox vs pipes/ground/ceiling)
    HighScore.h/.cpp// persisted best score (ESP32 Preferences / NVS)
  render/
    Renderer.h/.cpp // draws a GameState into the frame sprite; no game logic
    Sprites.h       // RGB565 pixel-art data (bird frames, pipe caps, digits)
```

Game logic never touches the display, and the renderer never changes state. This
keeps each module testable and easy to swap out.

## Controls

| Button | Title screen | Playing | Game over |
|---|---|---|---|
| **Top** (GPIO 0 / BOOT, USB-C on the left) | start | (ignored) | restart |
| **Bottom** (GPIO 14 / KEY) | — | flap | — |

After TOP starts a round, the game waits in a **Get Ready** state with the character
hovering. The first BOTTOM press begins play. Without this, the character hit the
ground 0.8 s after the start press, before the player could move to the other button.

The top/bottom → GPIO mapping is set in one place (`Config.h`), so it's a one-line
change if the on-device check shows it's swapped. Holding BOOT while pressing reset
puts the ESP32-S3 into download mode. That only happens at reset, so it doesn't
affect gameplay.

A short input lockout (~400 ms) after game over stops an accidental flap from being
read as a restart.

## Scope decisions

- The original build had no battery available for testing, so it shipped without battery
  or power extras (see "Power features" below for the follow-up). GPIO 15 is driven HIGH at
  boot because the LCD needs it on battery. It is harmless on USB.
- PlatformIO was chosen for easy review: every module is its own file, dependencies
  are pinned in `platformio.ini`, and it works from VS Code (PlatformIO extension) or
  the `pio` CLI.

## Iterations (each one ends with a commit and push)

1. **Scaffold and bring-up.** Add `platformio.ini`, the LovyanGFX display config, power
   and backlight init, the Button class, a full-screen sprite test pattern, and an
   on-screen FPS counter plus button-state readout. *Check: display is correct with
   no offset/colour errors, both buttons register, FPS ≥ 60.*
2. **Core gameplay.** Bird physics, flap, scrolling pipes with a random gap height,
   collision, score, and the Title/Playing/GameOver state machine. Graphics are plain
   primitives.
3. **Polish.** Pixel-art bird with flap animation and tilt, pipe caps, scrolling
   ground, parallax background, large score digits, and a best score saved to NVS.
4. **Tuning and cleanup.** Base physics kept as they were after play-testing. Added a
   gentle difficulty ramp: from score 10 to 50, speed goes 70 → 85 px/s and the gap
   narrows 50 → 42 px, then both stay capped. A pipe's gap is fixed when it spawns.
   Updated the README. (The GitHub Actions build was moved forward into iteration 1.)
5. **Winged cow character (final).** Replace the bird with a pixel-art flying cow,
   based on the concept image: a black-and-white cow with a pink muzzle, small horns
   and white wings. Scale it to about 30 × 24 px for the 170-px-tall screen. It gets
   2–3 wing-flap frames and a tilt driven by vertical speed. The collision hitbox is
   a tight rectangle around the body, so wingtips and horns don't cause unfair
   deaths. The cow's hitbox is taller than the bird's (8 px), so re-check the gap
   limits (`kPipeGap`, `kMinPipeGap`) against it and adjust if needed.
   **Requested:** resize the pipe gaps to make up for the larger sprite.
   **Done:** a 28×20 cow with a 20×11 hitbox. Gaps now come from the hitbox:
   `gap = h + 0.15·w + clearance` (clearance 40 → 32 over the ramp). The bird keeps
   50 → 42 px; the cow gets 54 → 46 px. Over 200 simulated runs each, both characters
   had the same survival rate. `Bird` was renamed `Player`.
6. **Day/night cycle (Halloween theme).** Updated request: switch every **15** points
   and keep repeating (night at 15–29, 45–59, ...). Each fade takes ≤ 2 s (set to 1.5 s,
   smoothstep-eased). Also add a moon, bats and glowing windows. Planned approach: bake the background layers as palette-indexed
   sprites, so each frame only interpolates a small palette of colours between the
   two themes instead of re-drawing the tiles. Pipes, ground and panel colours come
   from the same interpolated theme. Optional background extras from the concept: clouds, a city skyline,
   trees, and a grass-and-dirt ground strip, as parallax layers.

## Power features (battery indicator + deep sleep) — planned

Goal: tell the player how much charge is left, and let them put the board into deep
sleep with a deliberate button gesture, then wake it again. Both features stay out of
the `game/` and `scenes/` code. They are hooked into `main.cpp` and drawn as an overlay.

### Hardware facts to rely on (verify on the device)

| Item | Value |
|---|---|
| Battery sense | GPIO 4 (ADC1) through a ÷2 divider, so battery mV = 2 × the pin's mV. Community-sourced; confirm against a multimeter. |
| USB caveat | With USB plugged in, GPIO 4 reads a USB-influenced voltage, not the pure battery voltage. Voltage alone is a poor USB-detect, so the indicator should not claim "charging" with certainty. |
| Wake-capable pins | GPIO 0 and 14 are both RTC IOs on the ESP32-S3, so either can wake from deep sleep. |
| Strapping | GPIO 0 is a boot strapping pin (holding it at reset enters download mode). Prefer GPIO 14 as the wake button. |
| Peripheral rail | GPIO 15 powers the LCD. Driving it LOW (and holding it through sleep) removes that load. |

**Battery in use:** 3.7 V nominal, 500 mAh LiPo (4.2 V full). The percentage is based on
voltage only, so the capacity doesn't change the table. It does matter for sleep: at 500 mAh,
a sleep draw of even 1 mA is about 20 days to empty, so the sleep current is worth measuring.

### Decision: how to trigger sleep

- **Reset button: rejected.** RST is wired to the chip's EN pin. It reboots or halts the
  SoC and is not readable by firmware, so it can't be turned into a "sleep" action.
- **Recommended trigger: hold TOP + BOTTOM together for 1.5 s.** Normal play never
  presses both at once (TOP only starts or restarts, BOTTOM flaps), so there is no
  accidental trigger. It works in every game state. While both are held, a "hold to
  sleep" progress bar fills along the bottom edge. Releasing early cancels.
- **Wake: press BOTTOM (GPIO 14).** The wake press must not also count as a game input.
- **Optional extra: auto-sleep after N minutes idle** on the Title or Game Over screen
  (default 3 min, `0` disables), because forgetting the board on is the main way a small
  battery gets drained.

### Iteration 7 — Battery indicator

1. `Config.h`: `kBatterySense = 4`, divider ratio, full/empty millivolts (4150/3300),
   sample interval (1 s), low-battery threshold (15 %).
2. `hal/Battery.h/.cpp`: reads with `analogReadMilliVolts()` (uses the eFuse ADC
   calibration), takes a median of ~8 samples, then low-pass filters. This avoids the
   percentage flickering when the WiFi-off ADC is noisy or when the LCD bus is busy.
   Sampling happens once per second, not once per frame.
3. `util/BatteryCurve.h`: a pure function from millivolts to percent using a LiPo
   discharge table (a piecewise-linear table, not a straight line, since a LiPo is flat
   around 3.7–3.9 V). It has no hardware code, so it is easy to check in a host-side test.
4. `render/PowerOverlay.h/.cpp`: draws a small battery icon plus a percentage in the
   top-right corner, after the scene draws and before `present()`, so scenes are not
   modified. It is shown on Title and Game Over only, and hidden during play to keep the
   playfield clear (a `kShowBatteryDuringPlay` flag allows changing this). Under 15 %
   the icon turns red and blinks. When the reading looks like USB-powered (voltage above
   ~4.15 V), the icon shows a bolt instead of a misleading number.
5. The Diagnostics screen also prints raw pin mV, battery mV and percent. That is the
   calibration aid for the on-device check.

**Status:** implemented and compiles (not yet checked on hardware). Deviation: the USB
threshold is `kUsbPoweredMv = 4250`, not ~4.15 V, because a fully charged cell rests at
4.2 V and would otherwise show a bolt. Tune it from the Diagnostics readout. Scenes expose
`showsBatteryOverlay()` so the overlay is hidden during play.

*Check: with a battery, the reading is within ~50 mV of a multimeter and the percentage
drops monotonically over a play session. On USB only, the display shows the USB state
rather than a stale percentage.*

### Iteration 8 — Deep sleep

1. `input/SleepGesture.h`: a small pure state machine fed `(topDown, bottomDown, dt)`.
   It reports `idle`, `arming` (with 0–1 progress) and `triggered`. It is separate from
   the game input, so the same logic can be tested on a PC.
2. `hal/PowerManager.h/.cpp`:
   - `enterDeepSleep()`: wait for both buttons to be released (otherwise the chip wakes
     at once), fade the backlight out, `lcd.sleep()`, set the backlight pin LOW, drive
     GPIO 15 LOW and latch it with `gpio_hold_en()` plus `gpio_deep_sleep_hold_en()`.
     Then enable RTC pull-up on GPIO 14, call `esp_sleep_enable_ext0_wakeup(GPIO_NUM_14, 0)`
     and `esp_deep_sleep_start()`.
   - `wokeFromSleep()` and the wake cause, read at the top of `setup()`.
   - `Display::begin()` calls `gpio_hold_dis(GPIO_NUM_15)` before driving it HIGH, or the
     rail would stay latched off after a wake.
3. `main.cpp`: feed `SleepGesture` before `scene->update`. While it is arming, pass an
   empty `InputState` to the scene so the held buttons don't flap or restart. Draw the
   progress bar through `PowerOverlay`.
4. Wake handling: after a sleep wake, skip the "hold BOTTOM at boot opens Diagnostics"
   check (the wake press would otherwise always trigger it), and wait for the button to be
   released before reading input. The game starts at the Title screen; the best score is
   already in NVS, so nothing needs saving first.
5. Optional idle timer in `GameScene` or `main.cpp` (Title/Game Over only), using
   `cfg::power::kIdleSleepSeconds`.

**Status:** implemented and compiles (not yet checked on hardware). Sleep entry lives in
`Display::powerDown()` (fade, panel sleep, GPIO 15 and 38 latched off) and
`power_manager::enterDeepSleep()`; the idle timer is in `main.cpp` and scenes opt in with
`idleSleepAllowed()`.

*Check: the 1.5 s hold sleeps the board and the screen goes fully dark. BOTTOM wakes it to
the Title screen with the correct best score, working display and no diagnostics screen.
A short or single-button press does nothing. Then measure the sleep current with a USB
meter or multimeter. I have not verified a figure for this board, because the regulator
and charger IC may dominate it.*

Notes and risks:
- Deep sleep resets the chip, so the USB serial port disappears. To re-flash, press RST
  (or BOOT+RST) as in the README.
- If GPIO 14 turns out to be unreliable as a wake source on a given board, fall back to
  `esp_sleep_enable_ext1_wakeup` with GPIO 0 and 14, and handle the GPIO 0 strapping
  caveat by requiring the button to be released before sleeping.
- Both features need the real hardware (and a LiPo for iteration 7). CI only proves they
  compile.

### Keeping the character swappable

From iteration 2 on, the player character is described by data, not code. A
`CharacterSkin` holds the animation frames (RGB565 with a transparent key colour),
the frame size, the frame timing and the hitbox inset. The renderer and the collision
code read only that description. Swapping the bird for the cow in iteration 5 is then
a new asset file plus one line, with no changes to physics, collision or rendering
code.

## Verification

- I can't flash hardware from this environment. Each iteration will be checked by
  compiling it with PlatformIO in the container, and later in CI.
- On-device checks (orientation, button mapping, measured FPS) need you to flash the
  board. Iteration 1 is designed to make those checks trivial.
