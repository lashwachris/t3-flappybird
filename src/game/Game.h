#pragma once

#include <cstdint>

#include "game/Bird.h"
#include "game/CharacterSkin.h"
#include "game/Pipes.h"
#include "input/InputState.h"
#include "util/Random.h"

// Title -> (TOP) -> Ready -> (BOTTOM) -> Playing -> (crash) -> GameOver -> (TOP) -> Ready
enum class GameState : uint8_t { Title, Ready, Playing, GameOver };

// All game rules and state. Has no knowledge of the display or GPIO: it takes
// an InputState and a timestep, and exposes read-only state for the renderer.
class Game {
 public:
  explicit Game(const CharacterSkin& skin);

  void seed(uint32_t seed) { rng_.reseed(seed); }
  void update(const InputState& input, float dt);

  GameState state() const { return state_; }
  const Bird& bird() const { return bird_; }
  const Pipes& pipes() const { return pipes_; }
  uint32_t score() const { return score_; }
  uint32_t best() const { return best_; }
  float time() const { return time_; }            // Seconds since boot (game time).
  float stateTime() const { return stateTime_; }  // Seconds in the current state.
  // Total distance the world has scrolled, in px. Renderers derive each
  // parallax layer's offset from it. Double so it stays exact for days.
  double scrollDistance() const { return scrollDistance_; }
  // True from a crash that beat the previous best until the next round.
  bool isNewBest() const { return newBest_; }
  // Restores a best score persisted from an earlier session.
  void setBest(uint32_t best) { best_ = best; }
  bool canRestart() const;

 private:
  void enter(GameState state);
  void startRound();
  void hover();
  void scroll(float dt);
  void updateTitle(const InputState& input, float dt);
  void updateReady(const InputState& input, float dt);
  void updatePlaying(const InputState& input, float dt);
  void updateGameOver(const InputState& input, float dt);
  void die();

  Bird bird_;
  Pipes pipes_;
  Random rng_;
  GameState state_ = GameState::Title;
  uint32_t score_ = 0;
  uint32_t best_ = 0;
  float time_ = 0.0f;
  float stateTime_ = 0.0f;
  double scrollDistance_ = 0.0;
  bool newBest_ = false;
};
