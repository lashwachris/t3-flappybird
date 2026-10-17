// Host-side simulation of the game logic (src/game). Runs on a desktop with a
// plain C++17 compiler, no board or PlatformIO needed. It drives Game with
// scripted inputs and an autopilot, and checks the rules that were verified
// during development. Exit code is non-zero if any check fails.
//
// Build and run from the repository root (one line):
//   g++ -std=c++17 -O2 -Wall -Iinclude -Isrc tools/host-sim/sim.cpp src/game/*.cpp
//       src/assets/BirdSkin.cpp src/assets/CowSkin.cpp -o /tmp/sim && /tmp/sim

#include <cstdio>

#include "assets/BirdSkin.h"
#include "assets/CowSkin.h"
#include "game/Game.h"

namespace {

constexpr float kDt = cfg::timing::kFixedDt;
int failures = 0;

void check(bool ok, const char* what) {
  std::printf("  [%s] %s\n", ok ? "PASS" : "FAIL", what);
  if (!ok) ++failures;
}

InputState press(bool top, bool bottom) {
  InputState in{};
  in.top = {top, top};
  in.bottom = {bottom, bottom};
  return in;
}

const InputState kNone{};
const InputState kTop = press(true, false);
const InputState kBottom = press(false, true);

// Flaps when below the next gap's centre and falling faster than `minFall`.
// Raising `slack`, `minFall` and the cooldown makes it sloppier, so gap size
// measurably affects survival.
InputState autopilot(const Game& g, float slack, float minFall, int& cooldown,
                     int cooldownFrames) {
  float target = cfg::game::kGroundY / 2.0f;
  float nearest = 1e9f;
  for (const Pipe& p : g.pipes().items()) {
    if (p.x + cfg::game::kPipeWidth > g.player().x() - 10.0f && p.x < nearest) {
      nearest = p.x;
      target = p.gapCenterY;
    }
  }
  if (cooldown > 0) {
    --cooldown;
    return kNone;
  }
  if (g.player().y() > target + slack && g.player().velocity() > minFall) {
    cooldown = cooldownFrames;
    return kBottom;
  }
  return kNone;
}

void untilState(Game& g, GameState s, int maxFrames = 60 * 30) {
  for (int i = 0; i < maxFrames && g.state() != s; ++i) g.update(kNone, kDt);
}

void testStateFlow() {
  std::printf("State flow\n");
  Game g(kCowSkin);
  g.seed(7);
  g.update(kBottom, kDt);
  check(g.state() == GameState::Title, "BOTTOM on title does nothing");
  g.update(kTop, kDt);
  check(g.state() == GameState::Ready, "TOP on title -> Get Ready");
  for (int i = 0; i < 600; ++i) g.update(kNone, kDt);
  check(g.state() == GameState::Ready, "Get Ready waits indefinitely for a flap");
  g.update(kBottom, kDt);
  check(g.state() == GameState::Playing, "BOTTOM on Get Ready -> Playing");
  untilState(g, GameState::GameOver);
  check(g.state() == GameState::GameOver, "no input -> crash");
  g.update(kTop, kDt);
  check(g.state() == GameState::GameOver, "TOP during restart lockout is ignored");
  for (int i = 0; i < 30; ++i) g.update(kNone, kDt);
  check(g.player().onGround(), "character drops to the ground after a crash");
  g.update(kTop, kDt);
  check(g.state() == GameState::Ready, "TOP after lockout -> Get Ready");
}

void testBestScore() {
  std::printf("Best score\n");
  Game g(kCowSkin);
  g.seed(5);
  g.setBest(3);
  g.update(kTop, kDt);
  g.update(kBottom, kDt);
  untilState(g, GameState::GameOver);
  check(g.best() == 3 && !g.isNewBest(), "scoring 0 keeps the saved best, no NEW badge");

  for (int i = 0; i < 30; ++i) g.update(kNone, kDt);
  g.update(kTop, kDt);
  g.update(kBottom, kDt);
  int cooldown = 0;
  for (int f = 0; f < 60 * 20 && g.state() == GameState::Playing; ++f) {
    g.update(autopilot(g, 4.0f, 0.0f, cooldown, 0), kDt);
  }
  untilState(g, GameState::GameOver);
  check(g.best() == g.score() && g.score() > 3 && g.isNewBest(), "beating the best sets NEW");
  for (int i = 0; i < 30; ++i) g.update(kNone, kDt);
  g.update(kTop, kDt);
  check(!g.isNewBest(), "NEW clears on the next round");
}

void testDayNight() {
  std::printf("Day/night cycle\n");
  bool switchesOk = true;
  for (uint32_t s = 0; s < 120; ++s) {
    const bool expected = (s / 15) % 2 == 1;
    if (DayNightCycle::isNightScore(s) != expected) switchesOk = false;
  }
  check(switchesOk, "night on 15-29, 45-59, ... repeating");

  Game g(kCowSkin);
  g.seed(42);
  g.update(kTop, kDt);
  g.update(kBottom, kDt);
  int cooldown = 0;
  int fadeStart = -1;
  int fadeFrames = -1;
  for (int f = 0; f < 60 * 60 && g.state() == GameState::Playing; ++f) {
    const float before = g.nightAmount();
    g.update(autopilot(g, 4.0f, 0.0f, cooldown, 0), kDt);
    if (before == 0.0f && g.nightAmount() > 0.0f) fadeStart = f;
    if (before < 1.0f && g.nightAmount() == 1.0f && fadeStart >= 0) {
      fadeFrames = f - fadeStart + 1;
      break;
    }
  }
  const float fadeSeconds = fadeFrames * kDt;
  std::printf("  fade to night took %.2f s\n", fadeSeconds);
  check(fadeFrames > 0 && fadeSeconds <= 2.0f, "fade completes within 2 s");
}

// Compares survival between skins with a sloppy autopilot, to confirm the
// hitbox-based gap sizing keeps every character equally hard.
void testSkinFairness() {
  std::printf("Skin fairness (200 runs each)\n");
  auto run = [](const char* name, const CharacterSkin& skin) {
    constexpr int kRuns = 200;
    uint32_t total = 0;
    for (int r = 0; r < kRuns; ++r) {
      Game g(skin);
      g.seed(5000 + r);
      g.update(kTop, kDt);
      g.update(kBottom, kDt);
      int cooldown = 0;
      for (int f = 0; f < 60 * 240 && g.state() == GameState::Playing; ++f) {
        g.update(autopilot(g, 9.0f, 40.0f, cooldown, 10), kDt);
      }
      total += g.score();
    }
    const Difficulty easy = Difficulty::forScore(0, skin.hitbox);
    const Difficulty hard = Difficulty::forScore(1000, skin.hitbox);
    const float avg = static_cast<float>(total) / kRuns;
    std::printf("  %-5s gap %.0f -> %.0f px, avg score %.1f\n", name, easy.pipeGap, hard.pipeGap,
                avg);
    return avg;
  };
  const float bird = run("bird", kBirdSkin);
  const float cow = run("cow", kCowSkin);
  check(cow > bird * 0.9f && cow < bird * 1.1f, "cow and bird average scores within 10%");
}

}  // namespace

int main() {
  testStateFlow();
  testBestScore();
  testDayNight();
  testSkinFairness();
  std::printf("%s (%d failure%s)\n", failures == 0 ? "ALL PASSED" : "FAILED", failures,
              failures == 1 ? "" : "s");
  return failures == 0 ? 0 : 1;
}
