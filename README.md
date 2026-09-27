# t3-flappybird

A simple Flappy Bird clone for the LilyGo T-Display S3 (ESP32-S3, 320×170 ST7789).

See [PLAN.md](PLAN.md) for the design and the iteration roadmap.

## Controls

Hold the board in landscape with the USB-C port on the left.

| Button | Action |
|---|---|
| Top (GPIO 0 / BOOT) | Start, or restart after game over |
| Bottom (GPIO 14 / KEY) | Flap |

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

## Current state: iteration 1 (hardware bring-up)

The firmware shows a diagnostics screen:

- **Red border:** all four edges must be visible. If one is missing, the panel offset is wrong.
- **"USB" label:** must be next to the USB-C port. If the image is upside down, set
  `cfg::display::kRotation` to `1` in `include/Config.h`.
- **Colour bars, left to right:** white, yellow, cyan, green, magenta, red, blue, black.
- **TOP / BOTTOM indicators:** each lights up while its physical button is held. If they
  are swapped, swap `kButtonTop` and `kButtonBottom` in `include/Config.h`.
- **Stats panel:** actual FPS (capped at 60), render and push times per frame, and the
  uncapped frame rate the hardware could reach.
