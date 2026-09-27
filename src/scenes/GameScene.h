#pragma once

#include <cstdint>

#include "game/CharacterSkin.h"
#include "game/Game.h"
#include "hal/HighScoreStore.h"
#include "render/GameRenderer.h"
#include "scenes/Scene.h"

// The playable game: glues the Game logic to its renderer and to persistent
// storage for the best score.
class GameScene : public Scene {
 public:
  GameScene(const CharacterSkin& skin, HighScoreStore& store)
      : skin_(skin), store_(store), game_(skin) {}

  // Returns false if the renderer's sprites could not be allocated.
  bool begin(uint32_t seed) {
    game_.seed(seed);
    savedBest_ = store_.load();
    game_.setBest(savedBest_);
    return renderer_.begin(skin_);
  }

  void update(const InputState& input, float dt) override {
    game_.update(input, dt);
    if (game_.best() != savedBest_) {
      savedBest_ = game_.best();
      store_.save(savedBest_);
    }
  }
  void draw(LGFX_Sprite& canvas, const FrameStats& stats) override {
    renderer_.draw(canvas, game_, stats);
  }

 private:
  const CharacterSkin& skin_;
  HighScoreStore& store_;
  Game game_;
  uint32_t savedBest_ = 0;
  GameRenderer renderer_;
};
