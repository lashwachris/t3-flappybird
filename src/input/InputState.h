#pragma once

// Hardware-independent snapshot of the controls for one frame. Game code only
// sees this, never GPIO pins.
struct ButtonState {
  bool down = false;     // Currently held.
  bool pressed = false;  // Went down this frame.
};

struct InputState {
  ButtonState top;     // Start / restart.
  ButtonState bottom;  // Flap.
};
