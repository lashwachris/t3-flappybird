#include <Arduino.h>

#include "Config.h"
#include "assets/CowSkin.h"
#include "hal/Battery.h"
#include "hal/Button.h"
#include "hal/Display.h"
#include "hal/HighScoreStore.h"
#include "hal/PowerManager.h"
#include "input/InputState.h"
#include "input/SleepGesture.h"
#include "render/PowerOverlay.h"
#include "scenes/DiagnosticsScene.h"
#include "scenes/GameScene.h"
#include "scenes/Scene.h"
#include "util/FrameLimiter.h"
#include "util/FrameStats.h"

namespace {

Display display;
Battery battery;
Button buttonTop(cfg::pins::kButtonTop);
Button buttonBottom(cfg::pins::kButtonBottom);
FrameLimiter frameLimiter(cfg::timing::kTargetFps);
FrameStats frameStats(cfg::timing::kStatsWindowMs);
SleepGesture sleepGesture;
uint32_t lastActivityMs = 0;

HighScoreStore highScoreStore;
GameScene gameScene(kCowSkin, highScoreStore);
DiagnosticsScene diagnosticsScene;
Scene* scene = &gameScene;

InputState readInput() {
  const uint32_t nowMs = millis();
  buttonTop.update(nowMs);
  buttonBottom.update(nowMs);
  return {buttonTop.state(), buttonBottom.state()};
}

[[noreturn]] void halt(const char* message) {
  log_e("%s", message);
  for (;;) {
    delay(1000);
  }
}

}  // namespace

void setup() {
  Serial.begin(115200);

  // The wake press must not count as game input, or as "hold BOTTOM at boot".
  const bool wokeFromSleep = power_manager::wokeFromDeepSleep();
  if (wokeFromSleep) {
    power_manager::finishWake();
  }

  if (!display.begin()) {
    halt("Display init failed: could not allocate frame canvas");
  }
  battery.begin();
  buttonTop.begin();
  buttonBottom.begin();

  // Holding BOTTOM during boot opens the hardware diagnostics screen instead.
  // (TOP can't be used: holding BOOT at reset enters the ROM bootloader.)
  if (!wokeFromSleep && buttonBottom.isDown()) {
    diagnosticsScene.setBattery(&battery);
    scene = &diagnosticsScene;
  } else if (!gameScene.begin(esp_random())) {
    halt("Game init failed: could not allocate game sprites");
  }

  lastActivityMs = millis();
  frameLimiter.reset(micros());
}

void loop() {
  // The frame limiter locks the loop to kTargetFps with ~2x headroom, so one
  // fixed timestep per frame keeps game speed constant and motion smooth.
  const InputState raw = readInput();

  sleepGesture.update(raw.top.down, raw.bottom.down, cfg::timing::kFixedDt);
  if (sleepGesture.triggered()) {
    power_manager::enterDeepSleep(display);
  }

  // Idle auto-sleep: only on the Title / Game Over screens, never mid-round.
  const uint32_t nowMs = millis();
  if (raw.top.down || raw.bottom.down || !scene->idleSleepAllowed()) {
    lastActivityMs = nowMs;
  } else if (cfg::power::kIdleSleepSeconds > 0 &&
             nowMs - lastActivityMs >= cfg::power::kIdleSleepSeconds * 1000UL) {
    power_manager::enterDeepSleep(display);
  }

  // While the sleep gesture is arming, the held buttons must not flap or restart.
  scene->update(sleepGesture.arming() ? InputState{} : raw, cfg::timing::kFixedDt);

  const uint32_t renderStartUs = micros();
  battery.update(millis());
  scene->draw(display.canvas(), frameStats);
  if (scene->showsBatteryOverlay()) {
    power_overlay::drawBattery(display.canvas(), battery, millis());
  }
  if (sleepGesture.arming()) {
    power_overlay::drawSleepProgress(display.canvas(), sleepGesture.progress());
  }
  const uint32_t presentStartUs = micros();
  display.present();
  const uint32_t frameEndUs = micros();

  if (frameStats.record(presentStartUs - renderStartUs, frameEndUs - presentStartUs, frameEndUs)) {
    Serial.printf("fps %.1f  render %.2f ms  push %.2f ms  (uncapped max %.0f fps)\n",
                  frameStats.fps(), frameStats.renderMs(), frameStats.presentMs(),
                  frameStats.maxFps());
  }

  frameLimiter.wait();
}
