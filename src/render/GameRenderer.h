#pragma once

#include <cstdint>

#include "game/Game.h"
#include "hal/LGFX_TDisplayS3.h"
#include "util/FrameStats.h"

// Draws a Game into the frame canvas. Reads game state only; never changes it.
class GameRenderer {
 public:
  // Bakes the character's animation frames. Returns false on allocation failure.
  bool begin(const CharacterSkin& skin);

  void draw(LGFX_Sprite& canvas, const Game& game, const FrameStats& stats);

 private:
  static constexpr uint8_t kMaxFrames = 4;

  void drawSky(LGFX_Sprite& canvas) const;
  void drawPipes(LGFX_Sprite& canvas, const Pipes& pipes) const;
  void drawGround(LGFX_Sprite& canvas, float scroll) const;
  void drawCharacter(LGFX_Sprite& canvas, const Game& game);
  void drawScore(LGFX_Sprite& canvas, uint32_t score) const;
  void drawTitle(LGFX_Sprite& canvas, const Game& game) const;
  void drawReady(LGFX_Sprite& canvas, const Game& game) const;
  void drawGameOver(LGFX_Sprite& canvas, const Game& game) const;
  void drawFps(LGFX_Sprite& canvas, const FrameStats& stats) const;

  LGFX_Sprite frames_[kMaxFrames];
  uint8_t frameCount_ = 0;
  float frameDuration_ = 0.1f;
};
