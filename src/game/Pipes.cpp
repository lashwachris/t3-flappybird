#include "game/Pipes.h"

void Pipes::reset(Random& rng) {
  for (size_t i = 0; i < kCount; ++i) {
    pipes_[i] = {cfg::game::kFirstPipeX + i * cfg::game::kPipeSpacing, randomGapCenter(rng), false};
  }
}

uint32_t Pipes::update(float dt, float characterX, Random& rng) {
  const float dx = cfg::game::kScrollSpeed * dt;
  uint32_t passed = 0;

  float rightmostX = pipes_[0].x;
  for (Pipe& pipe : pipes_) {
    pipe.x -= dx;
    if (pipe.x > rightmostX) rightmostX = pipe.x;
  }

  for (Pipe& pipe : pipes_) {
    if (!pipe.scored && pipe.x + cfg::game::kPipeWidth / 2.0f < characterX) {
      pipe.scored = true;
      ++passed;
    }
    if (pipe.x + cfg::game::kPipeWidth < 0.0f) {
      rightmostX += cfg::game::kPipeSpacing;
      pipe = {rightmostX, randomGapCenter(rng), false};
    }
  }
  return passed;
}

bool Pipes::collides(const Rect& box) const {
  for (const Pipe& pipe : pipes_) {
    if (box.intersects(pipe.topRect()) || box.intersects(pipe.bottomRect())) {
      return true;
    }
  }
  return false;
}

float Pipes::randomGapCenter(Random& rng) {
  constexpr float kHalfGap = cfg::game::kPipeGap / 2.0f;
  constexpr float kMin = cfg::game::kPipeMargin + kHalfGap;
  constexpr float kMax = cfg::game::kGroundY - cfg::game::kPipeMargin - kHalfGap;
  return rng.range(kMin, kMax);
}
