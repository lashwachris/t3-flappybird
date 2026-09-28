#pragma once

#include "hal/LGFX_TDisplayS3.h"

// Owns the panel and a full-screen off-screen canvas. Callers draw the whole
// frame into canvas() and then call present() once, so the screen never shows
// a half-drawn frame.
class Display {
 public:
  // Powers the panel, initialises it and allocates the canvas.
  // Returns false if the canvas could not be allocated.
  bool begin();

  LGFX_Sprite& canvas() { return canvas_; }

  // Pushes the canvas to the panel (DMA over the parallel bus).
  void present();

 private:
  LGFX_TDisplayS3 lcd_;
  LGFX_Sprite canvas_{&lcd_};
};
