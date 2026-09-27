#pragma once

#include <cstdint>

#include "scenes/Scene.h"

// Hardware bring-up screen. It checks:
//  - panel offset and orientation: a 1-px border must be visible on all four
//    edges, and the "USB" label must be next to the USB-C port;
//  - colours: the bars must read white, yellow, cyan, green, magenta, red, blue, black;
//  - button mapping: each physical button must light up its own indicator;
//  - performance: FPS and per-frame render/present times.
// A bouncing square forces a full-frame redraw every frame.
class DiagnosticsScene : public Scene {
 public:
  void update(const InputState& input, float dt) override;
  void draw(LGFX_Sprite& canvas, const FrameStats& stats) override;

 private:
  void drawBackground(LGFX_Sprite& canvas) const;
  void drawButtonIndicator(LGFX_Sprite& canvas, int16_t y, const char* label, const char* pin,
                           bool down, uint32_t presses) const;
  void drawStats(LGFX_Sprite& canvas, const FrameStats& stats) const;

  static constexpr int16_t kBoxSize = 20;

  InputState input_;
  uint32_t topPresses_ = 0;
  uint32_t bottomPresses_ = 0;
  float boxX_ = 120.0f;
  float boxY_ = 60.0f;
  float boxVx_ = 140.0f;  // px/s
  float boxVy_ = 95.0f;   // px/s
};
