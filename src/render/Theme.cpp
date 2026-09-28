#include "render/Theme.h"

namespace {

Theme makeDayTheme() {
  Theme t;
  t[ThemeColor::SkyTop] = {84, 180, 236};
  t[ThemeColor::SkyHorizon] = {176, 228, 246};
  t[ThemeColor::Cloud] = {255, 255, 255};
  t[ThemeColor::CloudShade] = {206, 232, 244};
  t[ThemeColor::Building] = {150, 205, 230};
  t[ThemeColor::WindowLit] = {190, 230, 244};
  t[ThemeColor::WindowDim] = {190, 230, 244};
  t[ThemeColor::Bush] = {116, 196, 84};
  t[ThemeColor::BushDark] = {84, 164, 64};
  t[ThemeColor::Pipe] = {116, 191, 46};
  t[ThemeColor::PipeLight] = {170, 230, 90};
  t[ThemeColor::PipeDark] = {70, 130, 30};
  t[ThemeColor::GrassLight] = {150, 220, 80};
  t[ThemeColor::GrassDark] = {100, 180, 50};
  t[ThemeColor::Dirt] = {222, 216, 149};
  return t;
}

Theme makeNightTheme() {
  Theme t;
  t[ThemeColor::SkyTop] = {36, 18, 64};        // Deep purple
  t[ThemeColor::SkyHorizon] = {214, 96, 48};   // Pumpkin-orange dusk
  t[ThemeColor::Cloud] = {104, 78, 124};
  t[ThemeColor::CloudShade] = {66, 48, 90};
  t[ThemeColor::Building] = {34, 24, 46};      // Silhouettes
  t[ThemeColor::WindowLit] = {255, 190, 70};   // Glowing windows
  t[ThemeColor::WindowDim] = {50, 40, 62};
  t[ThemeColor::Bush] = {28, 40, 34};
  t[ThemeColor::BushDark] = {16, 24, 22};
  t[ThemeColor::Pipe] = {78, 98, 58};
  t[ThemeColor::PipeLight] = {116, 138, 78};
  t[ThemeColor::PipeDark] = {44, 58, 36};
  t[ThemeColor::GrassLight] = {70, 84, 46};
  t[ThemeColor::GrassDark] = {48, 60, 34};
  t[ThemeColor::Dirt] = {96, 74, 64};
  return t;
}

}  // namespace

const Theme kDayTheme = makeDayTheme();
const Theme kNightTheme = makeNightTheme();

Theme Theme::blend(const Theme& a, const Theme& b, float t) {
  Theme out;
  for (size_t i = 0; i < out.colors_.size(); ++i) {
    out.colors_[i] = lerp(a.colors_[i], b.colors_[i], t);
  }
  return out;
}
