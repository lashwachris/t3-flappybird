# Handoff: Flappy Cow on the LilyGo T-Display S3

Status as of 2026-10-17. All six planned iterations are done, play-tested on the board,
and merged into `main` through PR #1.

## 1. Where things stand

| Area | State |
|---|---|
| Gameplay | Finished: Title → Get Ready → Playing → Game Over, best score saved in NVS |
| Character | Winged cow (28×20, three wing positions, speed-based tilt) |
| Difficulty | Gentle ramp from score 10 to 50, then capped |
| Visuals | Parallax background, pixel-art digits, Halloween day/night cycle every 15 points |
| Performance | Locked 60 fps on the board (iteration 1 measured 2.6 ms render + 5.5 ms push) |
| CI | GitHub Actions builds every push; all runs green |
| Tests | Host simulation of the game logic, `tools/host-sim/sim.cpp` (all checks pass) |

Everything below was confirmed on the real board by the user: orientation, colours,
button mapping, 60 fps, game feel, the ramp, the cow's look and animation, and the
day/night cycle.

## 2. Resuming locally with Claude Code

1. **Get the code**
   ```bash
   git clone https://github.com/lashwachris/t3-flappybird
   cd t3-flappybird
   git checkout main && git pull
   git checkout -b <your-feature-branch>
   ```
2. **Tools:** VS Code with the PlatformIO IDE extension (or `pip install platformio` for
   the `pio` CLI), a C++17 compiler for the host simulation, and Claude Code.
3. **Start Claude Code in the repo root** (`claude`). It reads `CLAUDE.md` automatically,
   which covers commands, architecture rules and gotchas.
4. **Suggested first prompt:**
   > Read HANDOFF.md and CLAUDE.md, run the host simulation and `pio run` to confirm a
   > clean baseline, then let's plan <next feature>.

**What's better locally than in the cloud session:** the cloud environment blocked
PlatformIO's package registry, so every firmware build had to go through GitHub
Actions. Locally, Claude Code can run `pio run` directly. With the board plugged in, it
can also flash (`pio run -t upload`) and read the serial log
(`timeout 15 pio device monitor`). You still have to look at the screen and press the
buttons.

## 3. Project history

| Iteration | What was added | Key commit |
|---|---|---|
| 0 | Plan (`PLAN.md`) | `5dcb77f` |
| 1 | PlatformIO + LovyanGFX setup, full-frame canvas, debounced buttons, frame limiter, diagnostics screen, CI | `9ee896c` |
| 2 | Game state machine, physics, pooled pipes, collision, data-driven `CharacterSkin`, Get Ready state | `92997fe` |
| 3 | Parallax background, outlined pixel-art digits, crash flash, NEW badge, NVS best score | `ffc4983` |
| 4 | Difficulty ramp (speed 70→85 px/s, narrower gaps, score 10→50) | `24641cd` |
| 5 | Winged cow; gaps derived from hitbox; `Bird` renamed `Player` | `7191e8c` |
| 6 | Halloween day/night cycle (moon, bats, glowing windows), palette-sprite background | `041892c` |

`PLAN.md` is the original design document with notes added per iteration. Its "Code
structure" section shows the first sketch; the README's **Code layout** table is current.

## 4. Decisions and the reasons for them

- **LovyanGFX, not TFT_eSPI.** TFT_eSPI's ESP32-S3 parallel mode needs data pins below
  GPIO 32. This board uses 39–48, so TFT_eSPI would need LilyGo's patched fork and an old
  Arduino core.
- **Full-frame off-screen canvas pushed once per frame.** No flicker and no dirty-rect
  bookkeeping. There's roughly 2× headroom, so double-buffering was never needed.
- **One fixed timestep per frame at 60 Hz.** Smoother than an accumulator when the loop is
  already locked, and the tuning numbers don't depend on frame rate.
- **Get Ready state.** Without it, the character hit the ground 0.8 s after TOP was
  pressed, before the player could reach the BOTTOM button.
- **Lockout debouncing.** The first edge registers immediately, so debouncing adds no
  input latency.
- **Gap sizing from the hitbox: `gap = h + 0.15·w + clearance`** (clearance 40 → 32 over
  the ramp). Height alone left the wider cow about 5% harder in simulation; the width term
  made the two characters equal (96.4 vs 96.8 average score). The bird keeps the
  50 → 42 px gaps that were play-tested.
- **Day/night through palette sprites.** The background tiles are 4-bit palette sprites.
  Each frame only about 7 palette entries change, instead of the tiles being redrawn. The
  fade is 1.5 s with smoothstep easing (the user's limit was 2 s). Title and Get Ready
  always fade back to day; Game Over keeps the current theme.
- **Window lighting uses its own random seed,** so turning windows on didn't change the
  approved skyline layout.

## 5. Tuning cheat sheet

Unless noted, these are in `include/Config.h`.

| Want to change | Where |
|---|---|
| Gravity, flap strength, fall speed | `cfg::game::kGravity`, `kFlapVelocity`, `kMaxFallSpeed` |
| Base and max speed, ramp start/end | `cfg::game::kScrollSpeed`, `kMaxScrollSpeed`, `kRampStartScore`, `kRampFullScore` |
| Gap size | `cfg::game::kPipeGapClearance`, `kMinPipeGapClearance`, `kPipeGapPerWidth` |
| Day/night interval, fade time | `cfg::game::kNightCycleScore`, `kThemeFadeTime` |
| Theme colours | `src/render/Theme.cpp` (`makeDayTheme`, `makeNightTheme`) |
| Moon and bats | `src/render/NightSky.cpp` |
| Character art and hitbox | `src/assets/CowSkin.cpp` (switch skins in `src/main.cpp`) |
| FPS overlay | `cfg::render::kShowFps` |
| Button mapping, rotation | `cfg::pins::kButtonTop` / `kButtonBottom`, `cfg::display::kRotation` |

Tip: to check night quickly, temporarily set `kNightCycleScore` to `3`.

## 6. How changes were verified (keep doing this)

1. **Game logic:** run the host simulation (command in `CLAUDE.md`). Add a check to
   `tools/host-sim/sim.cpp` for any new rule.
2. **Firmware:** `pio run` locally, and CI on push.
3. **Art:** pixel art was previewed by rendering the character rows to an enlarged PNG
   before committing. This is worth repeating for new sprites.
4. **On device:** the user flashes and checks look, feel and FPS. Nothing visual counts as
   done until then.

## 7. Known limitations and loose ends

- **Host simulation isn't in CI yet.** Adding a job that compiles and runs
  `tools/host-sim/sim.cpp` would be a cheap safety net.
- **GitHub Actions warns that Node 20 is deprecated** for `actions/checkout@v4`, `cache@v4`,
  `setup-python@v5` and `upload-artifact@v4`. The builds still pass. Bump the versions when
  newer majors are available.
- **`BirdSkin`** (the original placeholder) is still compiled and kept as a ready
  alternative skin.
- **Writing a new best to NVS** takes a few ms on the game-over frame. The user didn't
  notice a hitch.
- **No battery features yet:** the user had no battery when this was built. GPIO 15 is
  already driven HIGH for battery operation.

## 8. Ideas not yet requested

None of these are committed to; they're options to discuss.

- Battery level indicator, once a battery is available (check LilyGo's docs for the
  battery-sense pin before implementing).
- A pause, e.g. hold TOP during play.
- Medals on the game-over panel (bronze/silver/gold thresholds).
- Extra character skins, using the same `CharacterSkin` pipeline.
- More night decorations (jack-o'-lanterns on the ground, twinkling stars), going through
  `NightSky` and `Theme`.
