#include "game/Player.h"

#include <algorithm>

void Player::step(float dt) {
  vy_ = std::min(vy_ + cfg::game::kGravity * dt, cfg::game::kMaxFallSpeed);
  y_ += vy_ * dt;

  const Rect box = hitbox();
  if (box.y < 0.0f) {
    y_ -= box.y;
    vy_ = 0.0f;
  } else if (box.bottom() > cfg::game::kGroundY) {
    y_ -= box.bottom() - cfg::game::kGroundY;
    vy_ = 0.0f;
  }
}

Rect Player::hitbox() const {
  const Rect& local = skin_.hitbox;
  return {x() - skin_.width() / 2.0f + local.x, y_ - skin_.height() / 2.0f + local.y, local.w,
          local.h};
}
