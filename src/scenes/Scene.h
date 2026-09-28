#pragma once

#include "hal/LGFX_TDisplayS3.h"
#include "input/InputState.h"
#include "util/FrameStats.h"

// A full-screen mode of the app (the game, the diagnostics screen, ...).
// The main loop calls update() once per fixed timestep, then draw() once per frame.
class Scene {
 public:
  virtual ~Scene() = default;
  virtual void update(const InputState& input, float dt) = 0;
  virtual void draw(LGFX_Sprite& canvas, const FrameStats& stats) = 0;
};
