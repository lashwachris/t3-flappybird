#pragma once

#include <cstdint>

#include "game/Game.h"
#include "hal/LGFX_TDisplayS3.h"
#include "render/Background.h"
#include "render/NumberRenderer.h"
#include "render/Theme.h"
#include "util/FrameStats.h"

// Draws a Game into the frame canvas. Reads game state only; never changes it.
class GameRenderer {
 public:
  // Bakes the character frames, background tiles and digits.
  // Returns false on allocation failure.
  bool begin(const CharacterSkin& skin);

  void draw(LGFX_Sprite& canvas, const Game& game, const FrameStats& stats);

 private:
  static constexpr uint8_t kMaxFrames = 4;

  void drawPipes(LGFX_Sprite& canvas, const Pipes& pipes, const Theme& theme) const;
  void drawGround(LGFX_Sprite& canvas, double scrollDistance, const Theme& theme) const;
  void drawCharacter(LGFX_Sprite& canvas, const Game& game);
  void drawScore(LGFX_Sprite& canvas, uint32_t score);
  void drawTitle(LGFX_Sprite& canvas, const Game& game) const;
  void drawReady(LGFX_Sprite& canvas, const Game& game);
  void drawGameOver(LGFX_Sprite& canvas, const Game& game);
  void drawFps(LGFX_Sprite& canvas, const FrameStats& stats) const;

  Background background_;
  NumberRenderer scoreDigits_{3};
  NumberRenderer panelDigits_{2};
  LGFX_Sprite frames_[kMaxFrames];
  uint8_t frameCount_ = 0;
  float frameDuration_ = 0.1f;
};
