#include <Arduino.h>

#include "Config.h"
#include "hal/Button.h"
#include "hal/Display.h"
#include "input/InputState.h"
#include "scenes/DiagnosticsScene.h"
#include "util/FrameLimiter.h"
#include "util/FrameStats.h"

namespace {

Display display;
Button buttonTop(cfg::pins::kButtonTop);
Button buttonBottom(cfg::pins::kButtonBottom);
FrameLimiter frameLimiter(cfg::timing::kTargetFps);
FrameStats frameStats(cfg::timing::kStatsWindowMs);
DiagnosticsScene scene;

InputState readInput() {
  const uint32_t nowMs = millis();
  buttonTop.update(nowMs);
  buttonBottom.update(nowMs);
  return {buttonTop.state(), buttonBottom.state()};
}

}  // namespace

void setup() {
  Serial.begin(115200);

  if (!display.begin()) {
    log_e("Display init failed: could not allocate frame canvas");
    for (;;) {
      delay(1000);
    }
  }
  buttonTop.begin();
  buttonBottom.begin();

  frameLimiter.reset(micros());
}

void loop() {
  scene.update(readInput(), cfg::timing::kFixedDt);

  const uint32_t renderStartUs = micros();
  scene.draw(display.canvas(), frameStats);
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
