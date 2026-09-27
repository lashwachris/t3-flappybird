#pragma once

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

#include "Config.h"

// LovyanGFX device description for the T-Display S3: ST7789 170x320 panel on
// the ESP32-S3's i80 LCD peripheral (DMA-driven), with PWM backlight.
class LGFX_TDisplayS3 : public lgfx::LGFX_Device {
 public:
  LGFX_TDisplayS3() {
    {
      auto bus = bus_.config();
      bus.freq_write = cfg::display::kBusWriteHz;
      bus.pin_wr = cfg::pins::kLcdWr;
      bus.pin_rd = cfg::pins::kLcdRd;
      bus.pin_rs = cfg::pins::kLcdDc;
      for (int i = 0; i < 8; ++i) {
        bus.pin_data[i] = cfg::pins::kLcdData[i];
      }
      bus_.config(bus);
      panel_.setBus(&bus_);
    }
    {
      auto panel = panel_.config();
      panel.pin_cs = cfg::pins::kLcdCs;
      panel.pin_rst = cfg::pins::kLcdRst;
      panel.pin_busy = -1;
      panel.panel_width = 170;
      panel.panel_height = 320;
      panel.offset_x = 35;  // The 170-px panel sits in a 240-px wide controller.
      panel.offset_y = 0;
      panel.offset_rotation = 0;
      panel.dummy_read_pixel = 8;
      panel.dummy_read_bits = 1;
      panel.readable = false;
      panel.invert = true;
      panel.rgb_order = false;
      panel.dlen_16bit = false;
      panel.bus_shared = true;
      panel_.config(panel);
    }
    {
      auto light = light_.config();
      light.pin_bl = cfg::pins::kLcdBacklight;
      light.invert = false;
      light.freq = 44100;
      light.pwm_channel = 7;
      light_.config(light);
      panel_.setLight(&light_);
    }
    setPanel(&panel_);
  }

 private:
  lgfx::Bus_Parallel8 bus_;
  lgfx::Panel_ST7789 panel_;
  lgfx::Light_PWM light_;
};
