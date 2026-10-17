#include <Arduino.h>

#include "Config.h"
#include "assets/CowSkin.h"
#include "hal/Battery.h"
#include "hal/Button.h"
#include "hal/Display.h"
#include "hal/HighScoreStore.h"
#include "input/InputState.h"
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

  if (!display.begin()) {
    halt("Display init failed: could not allocate frame canvas");
  }
  battery.begin();
  buttonTop.begin();
  buttonBottom.begin();

  // Holding BOTTOM during boot opens the hardware diagnostics screen instead.
  // (TOP can't be used: holding BOOT at reset enters the ROM bootloader.)
  if (buttonBottom.isDown()) {
    diagnosticsScene.setBattery(&battery);
    scene = &diagnosticsScene;
  } else if (!gameScene.begin(esp_random())) {
    halt("Game init failed: could not allocate game sprites");
  }

  frameLimiter.reset(micros());
}

void loop() {
  // The frame limiter locks the loop to kTargetFps with ~2x headroom, so one
  // fixed timestep per frame keeps game speed constant and motion smooth.
  scene->update(readInput(), cfg::timing::kFixedDt);

  const uint32_t renderStartUs = micros();
  battery.update(millis());
  scene->draw(display.canvas(), frameStats);
  if (scene->showsBatteryOverlay()) {
    power_overlay::drawBattery(display.canvas(), battery, millis());
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
