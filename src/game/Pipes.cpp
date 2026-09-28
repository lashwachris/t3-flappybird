#include "game/Pipes.h"

void Pipes::reset(const Difficulty& difficulty, Random& rng) {
  for (size_t i = 0; i < kCount; ++i) {
    pipes_[i] = spawn(cfg::game::kFirstPipeX + i * cfg::game::kPipeSpacing, difficulty, rng);
  }
}

uint32_t Pipes::update(float dx, float characterX, const Difficulty& difficulty, Random& rng) {
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
      pipe = spawn(rightmostX, difficulty, rng);
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

Pipe Pipes::spawn(float x, const Difficulty& difficulty, Random& rng) {
  const float halfGap = difficulty.pipeGap / 2.0f;
  const float minCenter = cfg::game::kPipeMargin + halfGap;
  const float maxCenter = cfg::game::kGroundY - cfg::game::kPipeMargin - halfGap;
  return {x, rng.range(minCenter, maxCenter), difficulty.pipeGap, false};
}
