#pragma once

#include <cstdint>

#include "game/CharacterSkin.h"
#include "game/Game.h"
#include "render/GameRenderer.h"
#include "scenes/Scene.h"

// The playable game: glues the Game logic to its renderer.
class GameScene : public Scene {
 public:
  explicit GameScene(const CharacterSkin& skin) : skin_(skin), game_(skin) {}

  // Returns false if the character's sprites could not be allocated.
  bool begin(uint32_t seed) {
    game_.seed(seed);
    return renderer_.begin(skin_);
  }

  void update(const InputState& input, float dt) override { game_.update(input, dt); }
  void draw(LGFX_Sprite& canvas, const FrameStats& stats) override {
    renderer_.draw(canvas, game_, stats);
  }

 private:
  const CharacterSkin& skin_;
  Game game_;
  GameRenderer renderer_;
};
