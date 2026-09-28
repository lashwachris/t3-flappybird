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

- No battery is available for testing, so there are no battery or power extras. GPIO 15
  is still driven HIGH at boot because the LCD needs it on battery. It is harmless on USB.
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
