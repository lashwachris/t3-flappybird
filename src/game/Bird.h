#pragma once

#include "Config.h"
#include "game/CharacterSkin.h"
#include "game/Rect.h"

// The player character's physics. Horizontal position is fixed (the world
// scrolls instead); only the vertical axis moves.
class Bird {
 public:
  explicit Bird(const CharacterSkin& skin) : skin_(skin) {}

  void reset(float y) {
    y_ = y;
    vy_ = 0.0f;
  }

  void flap() { vy_ = cfg::game::kFlapVelocity; }

  // Applies gravity and moves. Stops at the ceiling and rests on the ground.
  void step(float dt);

  // Places the bird without physics (used for the title-screen bob).
  void setY(float y) { y_ = y; }

  float x() const { return cfg::game::kBirdX; }
  float y() const { return y_; }
  float velocity() const { return vy_; }
  bool onGround() const { return hitbox().bottom() >= cfg::game::kGroundY; }

  // Collision box in screen coordinates.
  Rect hitbox() const;

  const CharacterSkin& skin() const { return skin_; }

 private:
  const CharacterSkin& skin_;
  float y_ = 0.0f;
  float vy_ = 0.0f;
};
