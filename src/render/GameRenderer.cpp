#include "render/GameRenderer.h"

#include <algorithm>
#include <cmath>
#include <cstdio>

#include "Config.h"
#include "render/Colors.h"
#include "render/SpriteBaker.h"
#include "render/Text.h"

namespace {

namespace palette {
constexpr uint16_t kPipe = colors::rgb565(116, 191, 46);
constexpr uint16_t kPipeLight = colors::rgb565(170, 230, 90);
constexpr uint16_t kPipeDark = colors::rgb565(70, 130, 30);
constexpr uint16_t kOutline = colors::rgb565(40, 30, 30);
constexpr uint16_t kGrassLight = colors::rgb565(150, 220, 80);
constexpr uint16_t kGrassDark = colors::rgb565(100, 180, 50);
constexpr uint16_t kDirt = colors::rgb565(222, 216, 149);
constexpr uint16_t kPanel = colors::rgb565(222, 216, 149);
constexpr uint16_t kPanelBorder = colors::rgb565(84, 56, 71);
constexpr uint16_t kText = colors::kWhite;
constexpr uint16_t kTextDark = colors::rgb565(84, 56, 71);
constexpr uint16_t kBadge = colors::rgb565(230, 60, 50);
constexpr uint16_t kFlash = colors::kWhite;
}  // namespace palette

constexpr int16_t kPipeCapHeight = 10;
constexpr int16_t kPipeCapOverhang = 2;
constexpr int16_t kGrassHeight = 4;

// Draws one pipe segment. The cap sits at the end facing the gap.
void drawPipeSegment(LGFX_Sprite& canvas, const Rect& body, bool capAtBottom) {
  const int32_t x = static_cast<int32_t>(body.x);
  const int32_t y = static_cast<int32_t>(body.y);
  const int32_t w = static_cast<int32_t>(body.w);
  const int32_t h = static_cast<int32_t>(body.h);
  if (h <= 0) return;

  canvas.fillRect(x, y, w, h, palette::kPipe);
  canvas.fillRect(x + 3, y, 4, h, palette::kPipeLight);
  canvas.fillRect(x + w - 5, y, 4, h, palette::kPipeDark);
  canvas.drawRect(x, y, w, h, palette::kOutline);

  const int32_t capY = capAtBottom ? y + h - kPipeCapHeight : y;
  const int32_t capX = x - kPipeCapOverhang;
  const int32_t capW = w + 2 * kPipeCapOverhang;
  canvas.fillRect(capX, capY, capW, kPipeCapHeight, palette::kPipe);
  canvas.fillRect(capX + 3, capY, 4, kPipeCapHeight, palette::kPipeLight);
  canvas.fillRect(capX + capW - 5, capY, 4, kPipeCapHeight, palette::kPipeDark);
  canvas.drawRect(capX, capY, capW, kPipeCapHeight, palette::kOutline);
}

float characterTilt(const Game& game) {
  switch (game.state()) {
    case GameState::Title:
    case GameState::Ready:
      return 0.0f;
    case GameState::GameOver:
      return cfg::render::kMaxTilt;
    case GameState::Playing:
      break;
  }
  return std::clamp(game.bird().velocity() * cfg::render::kTiltPerSpeed, cfg::render::kMinTilt,
                    cfg::render::kMaxTilt);
}

}  // namespace

bool GameRenderer::begin(const CharacterSkin& skin) {
  frameCount_ = std::min<uint8_t>(skin.frameCount, kMaxFrames);
  frameDuration_ = skin.frameDuration;
  for (uint8_t i = 0; i < frameCount_; ++i) {
    if (!sprite_baker::bake(skin.frames[i], frames_[i])) {
      return false;
    }
  }
  return frameCount_ > 0 && background_.begin() && scoreDigits_.begin(palette::kOutline) &&
         panelDigits_.begin(palette::kOutline);
}

void GameRenderer::draw(LGFX_Sprite& canvas, const Game& game, const FrameStats& stats) {
  background_.draw(canvas, game.scrollDistance());
  if (game.state() == GameState::Playing || game.state() == GameState::GameOver) {
    drawPipes(canvas, game.pipes());
  }
  drawGround(canvas, game.scrollDistance());
  drawCharacter(canvas, game);

  switch (game.state()) {
    case GameState::Title:
      drawTitle(canvas, game);
      break;
    case GameState::Ready:
      drawReady(canvas, game);
      break;
    case GameState::Playing:
      drawScore(canvas, game.score());
      break;
    case GameState::GameOver:
      drawGameOver(canvas, game);
      break;
  }

  if (game.state() == GameState::GameOver &&
      game.stateTime() < cfg::render::kDeathFlashTime) {
    canvas.fillScreen(palette::kFlash);
  }

  if (cfg::render::kShowFps) {
    drawFps(canvas, stats);
  }
}

void GameRenderer::drawPipes(LGFX_Sprite& canvas, const Pipes& pipes) const {
  for (const Pipe& pipe : pipes.items()) {
    if (pipe.x > canvas.width() || pipe.x + cfg::game::kPipeWidth < -kPipeCapOverhang) {
      continue;
    }
    drawPipeSegment(canvas, pipe.topRect(), true);
    drawPipeSegment(canvas, pipe.bottomRect(), false);
  }
}

void GameRenderer::drawGround(LGFX_Sprite& canvas, double scrollDistance) const {
  const int32_t groundY = static_cast<int32_t>(cfg::game::kGroundY);
  const int32_t w = canvas.width();
  const int32_t pattern = static_cast<int32_t>(cfg::render::kGroundPatternWidth);
  const int32_t half = pattern / 2;
  const int32_t scroll = static_cast<int32_t>(std::fmod(scrollDistance, pattern));

  canvas.fillRect(0, groundY, w, canvas.height() - groundY, palette::kDirt);
  for (int32_t x = -scroll; x < w; x += pattern) {
    canvas.fillRect(x, groundY, half, kGrassHeight, palette::kGrassLight);
    canvas.fillRect(x + half, groundY, half, kGrassHeight, palette::kGrassDark);
  }
  canvas.drawFastHLine(0, groundY, w, palette::kOutline);
  canvas.drawFastHLine(0, groundY + kGrassHeight, w, palette::kGrassDark);
}

void GameRenderer::drawCharacter(LGFX_Sprite& canvas, const Game& game) {
  uint8_t frame = 0;
  if (game.state() != GameState::GameOver && frameCount_ > 1) {
    frame = static_cast<uint32_t>(game.time() / frameDuration_) % frameCount_;
  }
  const Bird& bird = game.bird();
  frames_[frame].pushRotateZoom(&canvas, bird.x(), bird.y(), characterTilt(game), 1.0f, 1.0f,
                                sprite_baker::kTransparent);
}

void GameRenderer::drawScore(LGFX_Sprite& canvas, uint32_t score) {
  scoreDigits_.draw(canvas, score, canvas.width() / 2, 6);
}

void GameRenderer::drawTitle(LGFX_Sprite& canvas, const Game& game) const {
  const int32_t cx = canvas.width() / 2;

  canvas.setFont(&fonts::Font4);
  canvas.setTextDatum(textdatum_t::top_center);
  canvas.setTextSize(1.5f);
  text::drawOutlined(canvas, "FLAPPY", cx, 18, palette::kText, palette::kOutline);
  canvas.setTextSize(1.0f);

  canvas.setFont(&fonts::Font2);
  if (std::fmod(game.stateTime(), 1.0f) < 0.7f) {
    text::drawOutlined(canvas, "Press TOP to start", cx, 104, palette::kText, palette::kOutline);
  }
  text::drawOutlined(canvas, "BOTTOM to flap", cx, 124, palette::kText, palette::kOutline);
}

void GameRenderer::drawReady(LGFX_Sprite& canvas, const Game& game) {
  const int32_t cx = canvas.width() / 2;
  drawScore(canvas, game.score());

  canvas.setFont(&fonts::Font4);
  canvas.setTextDatum(textdatum_t::top_center);
  text::drawOutlined(canvas, "GET READY", cx, 44, palette::kText, palette::kOutline);

  canvas.setFont(&fonts::Font2);
  if (std::fmod(game.stateTime(), 1.0f) < 0.7f) {
    text::drawOutlined(canvas, "Press BOTTOM to flap", cx, 112, palette::kText,
                       palette::kOutline);
  }
}

void GameRenderer::drawGameOver(LGFX_Sprite& canvas, const Game& game) {
  constexpr int32_t kPanelW = 170;
  constexpr int32_t kPanelH = 106;
  constexpr int32_t kColumnOffset = 40;  // Score and best columns, either side of centre.
  const int32_t x = (canvas.width() - kPanelW) / 2;
  const int32_t y = (static_cast<int32_t>(cfg::game::kGroundY) - kPanelH) / 2;
  const int32_t cx = canvas.width() / 2;
  const int32_t scoreX = cx - kColumnOffset;
  const int32_t bestX = cx + kColumnOffset;

  canvas.fillRoundRect(x, y, kPanelW, kPanelH, 6, palette::kPanel);
  canvas.drawRoundRect(x, y, kPanelW, kPanelH, 6, palette::kPanelBorder);
  canvas.drawRoundRect(x + 1, y + 1, kPanelW - 2, kPanelH - 2, 5, palette::kPanelBorder);

  canvas.setTextDatum(textdatum_t::top_center);
  canvas.setFont(&fonts::Font4);
  text::drawOutlined(canvas, "GAME OVER", cx, y + 6, palette::kText, palette::kOutline);

  canvas.setFont(&fonts::Font2);
  canvas.setTextColor(palette::kTextDark);
  canvas.drawString("SCORE", scoreX, y + 36);
  canvas.drawString("BEST", bestX, y + 36);
  panelDigits_.draw(canvas, game.score(), scoreX, y + 54);
  panelDigits_.draw(canvas, game.best(), bestX, y + 54);

  if (game.isNewBest()) {
    constexpr int32_t kBadgeW = 26;
    constexpr int32_t kBadgeH = 11;
    const int32_t bx = bestX + 17;
    const int32_t by = y + 37;
    canvas.fillRoundRect(bx, by, kBadgeW, kBadgeH, 3, palette::kBadge);
    canvas.setFont(&fonts::Font0);
    canvas.setTextColor(palette::kText);
    canvas.drawString("NEW", bx + kBadgeW / 2, by + 2);
  }

  if (game.canRestart()) {
    canvas.setFont(&fonts::Font2);
    canvas.setTextColor(palette::kTextDark);
    canvas.drawString("TOP to restart", cx, y + kPanelH - 22);
  }
}

void GameRenderer::drawFps(LGFX_Sprite& canvas, const FrameStats& stats) const {
  char buf[16];
  snprintf(buf, sizeof(buf), "%.0f fps", stats.fps());
  canvas.setFont(&fonts::Font0);
  canvas.setTextDatum(textdatum_t::top_left);
  canvas.setTextColor(palette::kText);
  canvas.drawString(buf, 2, 2);
}
