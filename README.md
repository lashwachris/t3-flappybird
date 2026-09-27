# t3-flappybird

A simple Flappy Bird clone for the LilyGo T-Display S3 (ESP32-S3, 320×170 ST7789).

See [PLAN.md](PLAN.md) for the design and the iteration roadmap.

## Controls

Hold the board in landscape with the USB-C port on the left.

| Button | Title | Get Ready | Playing | Game over |
|---|---|---|---|---|
| Top (GPIO 0 / BOOT) | start | — | — | restart (after 0.4 s) |
| Bottom (GPIO 14 / KEY) | — | first flap, starts play | flap | — |

The best score is saved to flash and survives power-off.

**Difficulty:** from 10 points, the scroll speed rises (70 → 85 px/s) and new pipes
get narrower gaps (50 → 42 px). Both reach their limits at 50 points and stay there.
All values are in `cfg::game` in `include/Config.h`.

**Diagnostics screen:** hold **BOTTOM** while the board powers up or resets. It shows
the test pattern, button indicators and frame timings.

## Building and flashing

1. Install [VS Code](https://code.visualstudio.com/) and the **PlatformIO IDE** extension.
2. Open this folder in VS Code. PlatformIO downloads the toolchain and libraries on the first build.
3. Connect the board over USB-C and click **Upload** (→) in the PlatformIO status bar,
   or run `pio run -t upload`.
4. Optional: open the **Serial Monitor** (plug icon), or run `pio device monitor`, to see frame timings.

If the upload can't find the board: hold **BOOT**, tap **RST**, release **BOOT**, and upload
again. Tap **RST** afterwards to run the new firmware.

Every push is also built by GitHub Actions. The `firmware` artifact includes
`flappybird-merged.bin`, which can be flashed at offset `0x0` with a browser-based flasher.

## Code layout

| Path | Responsibility |
|---|---|
| `include/Config.h` | All pins and tuning constants (physics, pipes, visuals) |
| `src/hal/` | Board-specific code: display setup, frame canvas, debounced buttons, best-score storage (NVS) |
| `src/input/` | Hardware-independent button snapshot passed to the game |
| `src/game/` | Game rules: state machine, bird physics, pipes, collision. No display or GPIO code |
| `src/render/` | Draws the game state: parallax background, pixel-art baking, score digits, colours, text |
| `src/assets/` | Editable pixel art (character rows + palette): character skins, score digits |
| `src/scenes/` | Full-screen modes: the game and the diagnostics screen |
| `src/util/` | Frame limiter, frame stats, random number generator |

To try different gameplay settings, edit `cfg::game` in `include/Config.h`. The ramp itself
is in `src/game/Difficulty.h`.
