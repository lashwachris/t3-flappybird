#include "hal/HighScoreStore.h"

#include <Preferences.h>

namespace {
constexpr const char* kNamespace = "flappy";
constexpr const char* kBestKey = "best";
}  // namespace

uint32_t HighScoreStore::load() {
  Preferences prefs;
  if (!prefs.begin(kNamespace, /*readOnly=*/true)) {
    return 0;  // Namespace doesn't exist yet: nothing saved.
  }
  const uint32_t best = prefs.getUInt(kBestKey, 0);
  prefs.end();
  return best;
}

void HighScoreStore::save(uint32_t best) {
  Preferences prefs;
  if (!prefs.begin(kNamespace, /*readOnly=*/false)) {
    log_e("Could not open NVS to save the best score");
    return;
  }
  prefs.putUInt(kBestKey, best);
  prefs.end();
}
