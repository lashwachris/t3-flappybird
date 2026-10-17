# CLAUDE.md

Guidance for Claude Code working in this repository. Read `HANDOFF.md` for project
history, decisions and open ideas. Read `README.md` for player-facing behaviour.

## Project

Flappy Cow: a Flappy Bird-style game for the **LilyGo T-Display S3** (ESP32-S3, 1.9"
ST7789 at 320×170 landscape, 8-bit i80 parallel bus). It is written in C++17 with the
Arduino framework on PlatformIO, and uses LovyanGFX for graphics. The game runs at a
locked 60 fps.

## Commands

```bash
pio run                      # build firmware (first run downloads the toolchain)
pio run -t upload            # build + flash (board on USB-C)
timeout 15 pio device monitor   # serial log; prints fps/render/push times every 0.5 s
                             # (the monitor is interactive: always run it with a timeout)

# Host simulation of the game logic. No board or PlatformIO needed. Run after any
# change in src/game/ or src/assets/*Skin*. Exit code 0 = all checks pass.
g++ -std=c++17 -O2 -Wall -Wextra -Iinclude -Isrc tools/host-sim/sim.cpp src/game/*.cpp \
    src/assets/BirdSkin.cpp src/assets/CowSkin.cpp -o /tmp/sim && /tmp/sim
```

If `pio run -t upload` can't find the board: hold BOOT, tap RST, release BOOT, upload
again, then tap RST. The user has to do this by hand.

CI (`.github/workflows/build.yml`) builds every push and uploads `firmware.bin` plus a
merged image (`flappybird-merged.bin`, flash at 0x0).

## Architecture rules

These are deliberate. Keep them.

- **`include/Config.h` holds every pin and tuning constant** (`cfg::pins`, `cfg::display`,
  `cfg::game`, `cfg::render`, ...). Don't hard-code numbers elsewhere.
- **`src/game/` is pure logic:** no LovyanGFX, Arduino or GPIO includes. It takes an
  `InputState` and a fixed `dt` and exposes read-only state. This is what makes the host
  simulation possible. Keep it compiling with plain `g++`.
- **`src/render/` only reads game state.** It never changes it.
- **`src/hal/` is the only board-specific code** (display config, buttons, NVS).
- **The character is data.** A `CharacterSkin` (`src/game/CharacterSkin.h`) holds the
  frames, frame timing and hitbox. Skins live in `src/assets/`, and switching skins is one
  line in `src/main.cpp`. Pipe gaps are derived from the hitbox, so new skins are
  automatically fair.
- **Pixel art is written as character rows plus a palette** (`src/render/PixelArt.h`) and
  baked into sprites at startup (`SpriteBaker`). Every row must be exactly `width`
  characters long.
- **One fixed timestep per frame.** `FrameLimiter` locks the loop to 60 Hz and
  `cfg::timing::kFixedDt` is used for every update. There is no variable `dt`.

## Gotchas

- **Board:** GPIO 15 must be driven HIGH (LCD power on battery). Panel `offset_x = 35`,
  `invert = true`, rotation 3 = USB-C on the left. All confirmed on hardware.
- **Buttons:** TOP = GPIO 0 (BOOT), BOTTOM = GPIO 14. Holding BOOT at reset enters the ROM
  bootloader, which is why the diagnostics screen uses BOTTOM-at-boot instead.
- **TFT_eSPI doesn't work here** without LilyGo's patched fork, because its S3 parallel
  mode needs data pins below 32. Stay on LovyanGFX (`^1.2.0`; CI resolves 1.2.30 on
  `espressif32` 6.13.0 / Arduino core 2.0.17).
- **16-bit sprites** use magenta `0xF81F` (`sprite_baker::kTransparent`) as the
  transparent key. No art may use that colour.
- **4-bit palette sprites** (the background layers in `Background.cpp`) take **palette
  indices** as drawing colours, and the `transp` argument of `pushSprite` is also an index
  (index 0 = clear). This was verified in LovyanGFX 1.2.30 `colortype.hpp`. They are
  recoloured every frame with `setPaletteColor`.
- **Memory:** the 320×170×2 frame canvas (108.8 KB) and all sprites live in internal SRAM
  (`setPsram(false)`) for speed. Watch the heap if you add large sprites; PSRAM is the
  fallback.
- **Frame budget:** about 8 ms of the 16.7 ms frame was used before the day/night work.
  It still holds 60 fps on the board. Check with `cfg::render::kShowFps = true` or the
  serial log after any rendering change.
- **NVS write:** a new best score writes to flash once, at game over.

## Code style

Match the existing code: Google-ish C++ (2-space indent, 100 columns, `kConstant`,
`member_`, `camelCase()` methods). Keep header comments short and explain why, not what.
Small single-purpose files.

## Working with this user

- They prefer clean, scalable, reusable solutions. Never fix a bug with a hack or by
  rewriting large parts of foundational code.
- Ask for clarification when instructions are ambiguous. Research things outside your
  knowledge rather than guessing (e.g. LovyanGFX APIs: read the actual source).
- Work iteratively: plan first, then each iteration ends with a commit and push. Do
  on-device checks with the user before moving on.
- If they're working past midnight EST, gently mention the time and favour quick but
  solid solutions. Don't discourage them from continuing.
