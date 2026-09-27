#include "game/Game.h"

#include <cmath>

#include "Config.h"

namespace {
constexpr float kStartY = cfg::game::kGroundY / 2.0f;
}  // namespace

Game::Game(const CharacterSkin& skin) : bird_(skin) { bird_.reset(kStartY); }

void Game::update(const InputState& input, float dt) {
  time_ += dt;
  stateTime_ += dt;

  switch (state_) {
    case GameState::Title:
      updateTitle(input, dt);
      break;
    case GameState::Ready:
      updateReady(input, dt);
      break;
    case GameState::Playing:
      updatePlaying(input, dt);
      break;
    case GameState::GameOver:
      updateGameOver(input, dt);
      break;
  }
}

bool Game::canRestart() const {
  return state_ == GameState::GameOver && stateTime_ >= cfg::game::kRestartLockout;
}

void Game::enter(GameState state) {
  state_ = state;
  stateTime_ = 0.0f;
}

// Resets the round and waits for the first flap, so the player has time to
// move from the TOP button to the BOTTOM one.
void Game::startRound() {
  score_ = 0;
  newBest_ = false;
  bird_.reset(kStartY);
  pipes_.reset(rng_);
  enter(GameState::Ready);
}

void Game::scroll(float dt) { scrollDistance_ += cfg::game::kScrollSpeed * dt; }

// Gentle idle bob used while waiting for the player.
void Game::hover() {
  bird_.setY(kStartY + cfg::render::kTitleBobAmplitude *
                           std::sin(stateTime_ * cfg::render::kTitleBobSpeed));
}

void Game::updateTitle(const InputState& input, float dt) {
  scroll(dt);
  hover();

  if (input.top.pressed) {
    startRound();
  }
}

void Game::updateReady(const InputState& input, float dt) {
  scroll(dt);

  if (input.bottom.pressed) {
    bird_.reset(bird_.y());
    bird_.flap();
    enter(GameState::Playing);
  } else {
    hover();
  }
}

void Game::updatePlaying(const InputState& input, float dt) {
  scroll(dt);

  if (input.bottom.pressed) {
    bird_.flap();
  }
  bird_.step(dt);
  score_ += pipes_.update(dt, bird_.x(), rng_);

  if (bird_.onGround() || pipes_.collides(bird_.hitbox())) {
    die();
  }
}

void Game::updateGameOver(const InputState& input, float dt) {
  // The world freezes; the character drops to the ground.
  if (!bird_.onGround()) {
    bird_.step(dt);
  }

  if (canRestart() && input.top.pressed) {
    startRound();
  }
}

void Game::die() {
  if (score_ > best_) {
    best_ = score_;
    newBest_ = true;
  }
  enter(GameState::GameOver);
}
