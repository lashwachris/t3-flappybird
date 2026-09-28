#pragma once

// Axis-aligned rectangle in screen pixels.
struct Rect {
  float x = 0.0f;
  float y = 0.0f;
  float w = 0.0f;
  float h = 0.0f;

  float right() const { return x + w; }
  float bottom() const { return y + h; }

  bool intersects(const Rect& o) const {
    return x < o.right() && o.x < right() && y < o.bottom() && o.y < bottom();
  }
};
