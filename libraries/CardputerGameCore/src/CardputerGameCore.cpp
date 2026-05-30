#include "CardputerGameCore.h"

#include <Preferences.h>
#include <SD.h>
#include <SPI.h>
#include <esp_ota_ops.h>
#include <esp_partition.h>
#include <esp_system.h>
#include <stdarg.h>

namespace CardputerGameCore {

static bool gSdReady = false;
static lgfx::LovyanGFX* gUi = &M5Cardputer.Display;
static bool gFrameActive = false;
static bool gFrameReady = false;

struct UiTheme {
  uint16_t bg;
  uint16_t panel;
  uint16_t rail;
  uint16_t accent;
  uint16_t glow;
  uint16_t warn;
  const char* tag;
};

static UiTheme gTheme = {COLOR_BG, COLOR_PANEL, 0x10A2, 0x07FF, 0xFFFF, COLOR_WARN, "GAME"};

static M5Canvas& frameCanvas() {
  static M5Canvas frame(&M5Cardputer.Display);
  return frame;
}

static lgfx::LovyanGFX& ui() {
  return *gUi;
}

static bool beginFrame() {
  auto& display = M5Cardputer.Display;
  M5Canvas& frame = frameCanvas();
  if (!gFrameReady) {
    frame.setColorDepth(16);
    gFrameReady = frame.createSprite(display.width(), display.height()) != nullptr;
    if (gFrameReady) {
      frame.setTextDatum(top_left);
      frame.setTextWrap(false);
    }
  }
  gFrameActive = gFrameReady;
  gUi = gFrameReady ? static_cast<lgfx::LovyanGFX*>(&frame) : static_cast<lgfx::LovyanGFX*>(&display);
  return gFrameReady;
}

static void endFrame() {
  if (gFrameActive) {
    frameCanvas().pushSprite(&M5Cardputer.Display, 0, 0);
  }
  gUi = &M5Cardputer.Display;
  gFrameActive = false;
}

static uint8_t pulsePhase() {
  return (millis() / 95) & 0x07;
}

static uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

static uint16_t scale565(uint16_t color, uint8_t percent) {
  uint8_t r = ((color >> 11) & 0x1F) * 255 / 31;
  uint8_t g = ((color >> 5) & 0x3F) * 255 / 63;
  uint8_t b = (color & 0x1F) * 255 / 31;
  r = (uint16_t)r * percent / 100;
  g = (uint16_t)g * percent / 100;
  b = (uint16_t)b * percent / 100;
  return rgb565(r, g, b);
}

static uint16_t blend565(uint16_t a, uint16_t b, uint8_t amount) {
  uint8_t ar = ((a >> 11) & 0x1F) * 255 / 31;
  uint8_t ag = ((a >> 5) & 0x3F) * 255 / 63;
  uint8_t ab = (a & 0x1F) * 255 / 31;
  uint8_t br = ((b >> 11) & 0x1F) * 255 / 31;
  uint8_t bg = ((b >> 5) & 0x3F) * 255 / 63;
  uint8_t bb = (b & 0x1F) * 255 / 31;
  uint8_t inv = 255 - amount;
  return rgb565(((uint16_t)ar * inv + (uint16_t)br * amount) / 255,
                ((uint16_t)ag * inv + (uint16_t)bg * amount) / 255,
                ((uint16_t)ab * inv + (uint16_t)bb * amount) / 255);
}

static uint16_t healthColor(uint8_t health) {
  return health == 2 ? COLOR_BAD : (health == 1 ? COLOR_WARN : COLOR_GOOD);
}

static bool isSlug(const char* slug, const char* expected) {
  return slug && strcmp(slug, expected) == 0;
}

static UiTheme themeForSlug(const char* s, uint16_t accentColor, uint16_t warningColor) {
  if (isSlug(s, "star-trader-pocket-frontier")) return {0x0006, 0x0842, 0x0186, 0x07FF, 0x7DFF, 0xFD20, "STAR"};
  if (isSlug(s, "dungeon-courier")) return {0x1005, 0x210B, 0x4013, 0xB81F, 0xF81F, 0xFD20, "QUEST"};
  if (isSlug(s, "tiny-wasteland")) return {0x1800, 0x30C1, 0x6141, 0xFD20, 0xFFE0, 0xF800, "ROAD"};
  if (isSlug(s, "pocket-kingdom-manager")) return {0x1080, 0x3180, 0x6320, 0xFFE0, 0xFFFF, 0xFD20, "CROWN"};
  if (isSlug(s, "monster-ranch-trail")) return {0x0020, 0x0841, 0x0340, 0x07E0, 0xAFE5, 0xFD20, "HERD"};
  if (isSlug(s, "guildmaster-pocket")) return {0x0808, 0x2128, 0x492B, 0xA145, 0xF81F, 0xFD20, "GUILD"};
  if (isSlug(s, "haunted-radio-operator")) return {0x0000, 0x1082, 0x39E7, 0xBDF7, 0xFFFF, 0xFD20, "GHOST"};
  if (isSlug(s, "cyberdeck-hacker-rpg")) return {0x0005, 0x082A, 0x2013, 0x07FF, 0xF81F, 0xFD20, "NODE"};
  if (isSlug(s, "pocket-detective-agency")) return {0x0000, 0x1082, 0x4208, 0xFFFF, 0xC618, 0xFD20, "CASE"};
  if (isSlug(s, "cryptid-park-ranger")) return {0x0020, 0x0862, 0x02C3, 0x07E0, 0x07FF, 0xFD20, "FIELD"};
  if (isSlug(s, "cyber-ranger")) return {0x0004, 0x0845, 0x018A, 0x07BF, 0x07FF, 0xFD20, "RANGE"};
  if (isSlug(s, "star-trail-rancher")) return {0x1008, 0x2011, 0x4819, 0xF81F, 0x07FF, 0xFD20, "TRAIL"};
  if (isSlug(s, "wasteland-guildmaster")) return {0x1800, 0x30A0, 0x7120, 0xFD20, 0xFFE0, 0xF800, "WASTE"};
  if (isSlug(s, "signal-rat-cyberdeck-rpg")) return {0x0003, 0x0825, 0x01CB, 0x07FF, 0xFFE0, 0xFD20, "SIGNAL"};
  return {COLOR_BG, COLOR_PANEL, 0x10A2, accentColor, warningColor, warningColor, "GAME"};
}

static UiTheme themeFor(const GameDefinition& def) {
  return themeForSlug(def.slug, def.accentColor, def.warningColor);
}

static void setTheme(const GameDefinition& def) {
  gTheme = themeFor(def);
}

void applyGameTheme(const char* slug, uint16_t accentColor, uint16_t warningColor) {
  gTheme = themeForSlug(slug, accentColor, warningColor);
}

static void copyText(char* dest, size_t size, const char* src) {
  if (size == 0) return;
  if (!src) src = "";
  strncpy(dest, src, size - 1);
  dest[size - 1] = '\0';
}

int16_t clampValue(int16_t value, int16_t lo, int16_t hi) {
  if (value < lo) return lo;
  if (value > hi) return hi;
  return value;
}

uint16_t updateChecksum(uint16_t crc, const char* text) {
  if (!text) return crc;
  while (*text) {
    crc = (uint16_t)((crc << 5) ^ (crc >> 11) ^ (uint8_t)*text);
    ++text;
  }
  return (uint16_t)((crc << 5) ^ (crc >> 11) ^ '\n');
}

void initDisplay() {
  M5Cardputer.Display.setRotation(1);
  M5Cardputer.Display.setTextFont(1);
  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setTextWrap(false);
  M5Cardputer.Display.fillScreen(COLOR_BG);
}

void drawHeader(const char* title, uint16_t accentColor) {
  auto& d = ui();
  const uint16_t accent = accentColor ? accentColor : gTheme.accent;
  d.fillScreen(gTheme.bg);
  d.fillRect(0, 0, d.width(), d.height(), gTheme.bg);
  for (int16_t y = 18 + (millis() / 120) % 12; y < d.height() - 14; y += 12) {
    d.drawFastHLine(8, y, d.width() - 16, scale565(gTheme.rail, 55));
  }
  d.fillRoundRect(4, 3, d.width() - 8, 21, 4, gTheme.panel);
  d.drawRoundRect(4, 3, d.width() - 8, 21, 4, accent);
  d.drawFastHLine(9, 26, d.width() - 18, scale565(accent, 70));
  d.setTextSize(1);
  d.setTextColor(COLOR_TEXT, gTheme.panel);
  d.setCursor(6, 6);
  d.print(title);
  d.setTextColor(gTheme.glow, gTheme.panel);
  d.setCursor(d.width() - 88, 6);
  d.print(gTheme.tag);
  d.setTextColor(accent, gTheme.panel);
  d.setCursor(d.width() - 36, 6);
  int battery = M5Cardputer.Power.getBatteryLevel();
  if (battery >= 0) d.printf("%d%%", battery);
  else d.print("--%");
}

void drawFooter(const char* text) {
  auto& d = ui();
  d.fillRoundRect(4, d.height() - 15, d.width() - 8, 13, 3, scale565(gTheme.panel, 80));
  d.drawFastHLine(7, d.height() - 16, d.width() - 14, scale565(gTheme.accent, 65));
  d.setTextSize(1);
  d.setTextColor(COLOR_DIM, scale565(gTheme.panel, 80));
  d.setCursor(6, d.height() - 11);
  d.print(text);
}

void drawRow(uint8_t row, const char* label, const char* detail, bool selected, uint16_t color) {
  auto& d = ui();
  int16_t y = 25 + row * 18;
  uint8_t phase = selected ? pulsePhase() : 0;
  uint16_t bg = selected ? blend565(gTheme.panel, gTheme.accent, 72 + phase * 12) : gTheme.panel;
  uint16_t edge = selected ? gTheme.glow : scale565(gTheme.rail, 80);
  d.fillRoundRect(4, y - 2, d.width() - 8, 16, 4, bg);
  d.drawRoundRect(4, y - 2, d.width() - 8, 16, 4, edge);
  if (selected) {
    d.fillRect(8, y + 1, 3 + phase, 10, gTheme.glow);
  }
  d.setTextSize(1);
  d.setTextColor(selected ? COLOR_BG : color, bg);
  d.setCursor(10, y + 1);
  d.print(selected ? ">" : " ");
  d.print(" ");
  d.print(label);
  if (detail && detail[0]) {
    d.setTextColor(selected ? COLOR_BG : COLOR_DIM, bg);
    int16_t x = d.width() - 8 - (strlen(detail) * 6);
    if (x < 126) x = 126;
    d.setCursor(x, y + 1);
    d.print(detail);
  }
}

void drawWrapped(const char* text, int16_t x, int16_t y, int16_t maxChars, uint8_t maxLines, uint16_t color) {
  auto& d = ui();
  d.setTextSize(1);
  d.setTextColor(color, gTheme.bg);
  const char* p = text ? text : "";
  for (uint8_t line = 0; line < maxLines && *p; ++line) {
    char buf[56];
    uint8_t len = 0;
    const char* start = p;
    const char* lastSpace = nullptr;
    while (*p && *p != '\n' && len < maxChars) {
      if (*p == ' ') lastSpace = p;
      ++p;
      ++len;
    }
    if (*p && *p != '\n' && lastSpace && lastSpace > start) {
      len = (uint8_t)(lastSpace - start);
      p = lastSpace + 1;
    } else if (*p == '\n') {
      ++p;
    }
    if (len >= sizeof(buf)) len = sizeof(buf) - 1;
    memcpy(buf, start, len);
    buf[len] = '\0';
    d.setCursor(x, y + line * 11);
    d.print(buf);
  }
}

void drawProgressBar(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t value, uint16_t maxValue, uint16_t accentColor) {
  auto& d = ui();
  uint16_t accent = accentColor ? accentColor : gTheme.accent;
  int16_t fill = maxValue == 0 ? 0 : (int32_t)(w - 4) * min<uint16_t>(value, maxValue) / maxValue;
  d.fillRoundRect(x, y, w, h, 3, scale565(gTheme.panel, 75));
  d.drawRoundRect(x, y, w, h, 3, scale565(accent, 80));
  if (fill > 0) {
    d.fillRoundRect(x + 2, y + 2, fill, h - 4, 2, accent);
    int16_t shimmer = x + 2 + ((millis() / 70) % max<int16_t>(fill, 1));
    d.drawFastVLine(shimmer, y + 3, h - 6, gTheme.glow);
  }
}

void drawStatusChip(int16_t x, int16_t y, const char* label, uint16_t color) {
  auto& d = ui();
  int16_t w = strlen(label) * 6 + 10;
  d.fillRoundRect(x, y, w, 13, 3, scale565(color, 45));
  d.drawRoundRect(x, y, w, 13, 3, color);
  d.setTextSize(1);
  d.setTextColor(COLOR_TEXT, scale565(color, 45));
  d.setCursor(x + 5, y + 3);
  d.print(label);
}

InputEvent readInput() {
  InputEvent event;
  event.select = M5Cardputer.BtnA.wasClicked();

  if (M5Cardputer.Keyboard.isChange() && M5Cardputer.Keyboard.isPressed()) {
    Keyboard_Class::KeysState keys = M5Cardputer.Keyboard.keysState();
    event.select = event.select || keys.enter;
    event.back = keys.del || keys.tab;
    for (auto c : keys.word) {
      event.key = (char)c;
      if (c >= '1' && c <= '9') event.number = c - '0';
      if (c == ',' || c == ';' || c == 'w' || c == 'W' || c == 'k' || c == 'K') event.up = true;
      if (c == '.' || c == '/' || c == 's' || c == 'S' || c == 'j' || c == 'J') event.down = true;
      if (c == 'a' || c == 'A' || c == 'h' || c == 'H') event.left = true;
      if (c == 'd' || c == 'D' || c == 'l' || c == 'L') event.right = true;
      if (c == 96 || c == 'q' || c == 'Q') event.back = true;
      if (c == ' ' || c == '\r') event.select = true;
    }
  }

  return event;
}

bool mountSd() {
  SD.end();
  pinMode(SD_POWER_HOLD, OUTPUT);
  digitalWrite(SD_POWER_HOLD, HIGH);
  delay(25);
  SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
  gSdReady = SD.begin(SD_CS, SPI, 25000000);
  if (!gSdReady || SD.cardType() == CARD_NONE) {
    gSdReady = false;
    Serial.println("[sd] mount failed");
  } else {
    Serial.printf("[sd] mounted type=%u size=%llu MB\n", SD.cardType(), SD.cardSize() / (1024ULL * 1024ULL));
  }
  return gSdReady;
}

bool sdReady() {
  return gSdReady;
}

void buildGameSavePath(const char* slug, const char* filename, char* out, size_t outSize) {
  snprintf(out, outSize, "/cardputer-game-os/saves/%s/%s", slug ? slug : "game", filename ? filename : "save.sav");
}

bool ensureGameSaveDir(const char* slug) {
  if (!sdReady()) return false;
  if (!SD.exists("/cardputer-game-os")) SD.mkdir("/cardputer-game-os");
  if (!SD.exists("/cardputer-game-os/saves")) SD.mkdir("/cardputer-game-os/saves");
  char dir[96];
  snprintf(dir, sizeof(dir), "/cardputer-game-os/saves/%s", slug ? slug : "game");
  if (!SD.exists(dir)) SD.mkdir(dir);
  return SD.exists(dir);
}

static void siblingSavePath(const char* path, const char* ext, char* out, size_t outSize) {
  if (!path || !ext || outSize == 0) return;
  strncpy(out, path, outSize - 1);
  out[outSize - 1] = '\0';
  char* slash = strrchr(out, '/');
  char* dot = strrchr(out, '.');
  if (dot && (!slash || dot > slash)) {
    *dot = '\0';
  }
  strncat(out, ext, outSize - strlen(out) - 1);
}

bool writeAtomicTextSave(const char* path, const char* body, uint16_t checksum) {
  if (!path || !body || !sdReady()) return false;
  char tmp[144];
  char bak[144];
  siblingSavePath(path, ".tmp", tmp, sizeof(tmp));
  siblingSavePath(path, ".bak", bak, sizeof(bak));
  if (SD.exists(tmp)) SD.remove(tmp);
  File file = SD.open(tmp, FILE_WRITE);
  if (!file) return false;
  file.print(body);
  file.printf("crc=%04X\n", checksum);
  file.flush();
  bool ok = file.getWriteError() == 0;
  file.close();
  if (!ok) {
    SD.remove(tmp);
    return false;
  }
  if (SD.exists(bak)) SD.remove(bak);
  if (SD.exists(path) && !SD.rename(path, bak)) {
    SD.remove(tmp);
    return false;
  }
  if (!SD.rename(tmp, path)) return false;
  return true;
}

bool readTextSave(const char* path, const char* fallbackBakPath, String& out) {
  if (!sdReady()) return false;
  const char* chosen = path && SD.exists(path) ? path : fallbackBakPath;
  if (!chosen || !SD.exists(chosen)) return false;
  File file = SD.open(chosen, FILE_READ);
  if (!file) return false;
  out = file.readString();
  file.close();
  return out.length() > 0;
}

static void markLauncherReturnPrefs(const char* prefsNamespace) {
  Preferences prefs;
  if (prefs.begin(prefsNamespace, false)) {
    prefs.putBool("returnOnce", true);
    prefs.putBool("bootToApp", false);
    prefs.end();
  }
}

void returnToLauncher(uint32_t delayMs) {
  markLauncherReturnPrefs("cardgameos");
  markLauncherReturnPrefs("cyputeros");
  const esp_partition_t* launcher = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_OTA_0, "app0");
  if (!launcher) {
    launcher = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_OTA_0, nullptr);
  }
  if (launcher) {
    esp_ota_set_boot_partition(launcher);
  }
  if (delayMs > 0) delay(delayMs);
  ESP.restart();
}

GameRuntime::GameRuntime(const GameDefinition& definition) : def(definition) {}

void GameRuntime::setStatus(const char* text) {
  copyText(status, sizeof(status), text);
}

void GameRuntime::begin() {
  auto cfg = M5.config();
  M5Cardputer.begin(cfg, true);
  Serial.begin(115200);
  Serial.printf("[%s] boot\n", def.slug);
  setTheme(def);
  initDisplay();
  randomSeed((uint32_t)esp_random());
  mountSd();
  hasSave = saveExists();
  setStatus(sdReady() ? "Ready." : "SD missing. Saves disabled.");
  dirty = true;
  nextAnimMs = millis() + 100;
}

void GameRuntime::loop() {
  draw();
  M5Cardputer.update();
  InputEvent input = readInput();
  if (!input.any()) {
    if (gFrameReady && millis() >= nextAnimMs) {
      dirty = true;
      nextAnimMs = millis() + 110;
    }
    delay(12);
    return;
  }
  handleInput(input);
}

void GameRuntime::savePath(char* out, size_t outSize, const char* suffix) const {
  snprintf(out, outSize, "/cardputer-game-os/saves/%s/%s", def.slug, suffix);
}

bool GameRuntime::ensureSaveDir() {
  if (!sdReady()) return false;
  if (!SD.exists("/cardputer-game-os")) SD.mkdir("/cardputer-game-os");
  if (!SD.exists("/cardputer-game-os/saves")) SD.mkdir("/cardputer-game-os/saves");
  char dir[96];
  snprintf(dir, sizeof(dir), "/cardputer-game-os/saves/%s", def.slug);
  if (!SD.exists(dir)) SD.mkdir(dir);
  return SD.exists(dir);
}

bool GameRuntime::saveExists() {
  if (!sdReady()) return false;
  char save[128];
  char bak[128];
  savePath(save, sizeof(save), "save.sav");
  savePath(bak, sizeof(bak), "save.bak");
  return SD.exists(save) || SD.exists(bak);
}

static bool writeLine(File& file, uint16_t& crc, const char* fmt, ...) {
  char line[160];
  va_list args;
  va_start(args, fmt);
  vsnprintf(line, sizeof(line), fmt, args);
  va_end(args);
  file.println(line);
  crc = updateChecksum(crc, line);
  return file.getWriteError() == 0;
}

bool GameRuntime::saveGame() {
  if (!ensureSaveDir()) {
    setStatus("SD missing. Save skipped.");
    return false;
  }
  char save[128];
  char tmp[128];
  char bak[128];
  savePath(save, sizeof(save), "save.sav");
  savePath(tmp, sizeof(tmp), "save.tmp");
  savePath(bak, sizeof(bak), "save.bak");
  if (SD.exists(tmp)) SD.remove(tmp);
  File file = SD.open(tmp, FILE_WRITE);
  if (!file) {
    setStatus("Could not open save temp.");
    return false;
  }
  uint16_t crc = 0xBEEF;
  writeLine(file, crc, "%s", def.saveMagic);
  writeLine(file, crc, "schema=1");
  writeLine(file, crc, "seed=%lu", (unsigned long)state.seed);
  writeLine(file, crc, "day=%u", state.day);
  writeLine(file, crc, "progress=%u", state.progress);
  writeLine(file, crc, "location=%u", state.location);
  writeLine(file, crc, "resources=%d,%d,%d,%d,%d,%d,%d,%d", state.resources[0], state.resources[1], state.resources[2], state.resources[3], state.resources[4], state.resources[5], state.resources[6], state.resources[7]);
  writeLine(file, crc, "trackers=%u,%u,%u,%u,%u,%u,%u,%u", state.trackers[0], state.trackers[1], state.trackers[2], state.trackers[3], state.trackers[4], state.trackers[5], state.trackers[6], state.trackers[7]);
  writeLine(file, crc, "rep=%d,%d,%d,%d", state.reputation[0], state.reputation[1], state.reputation[2], state.reputation[3]);
  writeLine(file, crc, "flags=%08lX", (unsigned long)state.flags);
  writeLine(file, crc, "last=%u,%u", state.lastEvent, state.lastActor);
  writeLine(file, crc, "end=%u,%u,%u", state.gameOver ? 1 : 0, state.victory ? 1 : 0, state.ending);
  file.printf("crc=%04X\n", crc);
  file.flush();
  file.close();
  if (SD.exists(bak)) SD.remove(bak);
  if (SD.exists(save) && !SD.rename(save, bak)) {
    SD.remove(tmp);
    setStatus("Save backup failed.");
    return false;
  }
  if (!SD.rename(tmp, save)) {
    setStatus("Save rename failed.");
    return false;
  }
  hasSave = true;
  setStatus("Game saved to SD.");
  return true;
}

static bool parseCsv(const String& payload, int* values, uint8_t count) {
  int start = 0;
  for (uint8_t i = 0; i < count; ++i) {
    int comma = payload.indexOf(',', start);
    if (comma < 0 && i < count - 1) return false;
    String part = (i == count - 1) ? payload.substring(start) : payload.substring(start, comma);
    part.trim();
    values[i] = part.toInt();
    start = comma + 1;
  }
  return true;
}

bool GameRuntime::loadGame() {
  if (!sdReady()) {
    setStatus("SD missing. Cannot load.");
    return false;
  }
  char save[128];
  char bak[128];
  savePath(save, sizeof(save), "save.sav");
  savePath(bak, sizeof(bak), "save.bak");
  const char* path = SD.exists(save) ? save : bak;
  File file = SD.open(path, FILE_READ);
  if (!file) {
    setStatus("No save found.");
    return false;
  }
  String first = file.readStringUntil('\n');
  first.trim();
  if (first != def.saveMagic) {
    file.close();
    setStatus("Save belongs to another game.");
    return false;
  }
  GenericGameState loaded;
  uint16_t crc = 0xBEEF;
  crc = updateChecksum(crc, def.saveMagic);
  uint16_t expected = 0;
  bool sawCrc = false;
  while (file.available()) {
    String line = file.readStringUntil('\n');
    line.trim();
    if (line.length() == 0) continue;
    if (line.startsWith("crc=")) {
      expected = (uint16_t)strtoul(line.substring(4).c_str(), nullptr, 16);
      sawCrc = true;
      continue;
    }
    crc = updateChecksum(crc, line.c_str());
    if (line.startsWith("seed=")) loaded.seed = (uint32_t)line.substring(5).toInt();
    else if (line.startsWith("day=")) loaded.day = (uint16_t)line.substring(4).toInt();
    else if (line.startsWith("progress=")) loaded.progress = (uint16_t)line.substring(9).toInt();
    else if (line.startsWith("location=")) loaded.location = (uint8_t)line.substring(9).toInt();
    else if (line.startsWith("resources=")) {
      int values[8];
      if (parseCsv(line.substring(10), values, 8)) {
        for (uint8_t i = 0; i < MAX_RESOURCES; ++i) loaded.resources[i] = values[i];
      }
    } else if (line.startsWith("trackers=")) {
      int values[8];
      if (parseCsv(line.substring(9), values, 8)) {
        for (uint8_t i = 0; i < MAX_TRACKERS; ++i) loaded.trackers[i] = constrain(values[i], 0, 255);
      }
    } else if (line.startsWith("rep=")) {
      int values[4];
      if (parseCsv(line.substring(4), values, 4)) {
        for (uint8_t i = 0; i < MAX_REPUTATION; ++i) loaded.reputation[i] = constrain(values[i], -99, 99);
      }
    } else if (line.startsWith("flags=")) loaded.flags = strtoul(line.substring(6).c_str(), nullptr, 16);
    else if (line.startsWith("last=")) {
      int values[2];
      if (parseCsv(line.substring(5), values, 2)) {
        loaded.lastEvent = values[0];
        loaded.lastActor = values[1];
      }
    } else if (line.startsWith("end=")) {
      int values[3];
      if (parseCsv(line.substring(4), values, 3)) {
        loaded.gameOver = values[0] != 0;
        loaded.victory = values[1] != 0;
        loaded.ending = values[2];
      }
    }
  }
  file.close();
  if (sawCrc && crc != expected) {
    setStatus("Save checksum failed.");
    return false;
  }
  if (loaded.location >= def.locationCount) loaded.location = 0;
  if (loaded.ending >= def.endingCount) loaded.ending = 0;
  state = loaded;
  setStatus("Loaded save from SD.");
  screen = state.gameOver || state.victory ? SCREEN_ENDING : SCREEN_DASH;
  dirty = true;
  return true;
}

void GameRuntime::newGame() {
  memset(&state, 0, sizeof(state));
  state.seed = (uint32_t)random(1, 2147483647);
  state.day = 1;
  state.location = def.startingLocation < def.locationCount ? def.startingLocation : 0;
  for (uint8_t i = 0; i < MAX_RESOURCES; ++i) {
    state.resources[i] = i < def.resourceCount ? def.resources[i].start : 0;
  }
  for (uint8_t i = 0; i < MAX_REPUTATION; ++i) state.reputation[i] = 10;
  state.lastEvent = 0;
  state.lastActor = 0;
  setStatus("New run started.");
  screen = SCREEN_DASH;
  saveGame();
  dirty = true;
}

void GameRuntime::continueGame() {
  if (!loadGame()) screen = SCREEN_TITLE;
  dirty = true;
}

void GameRuntime::drawTitle() {
  drawHeader(def.title, def.accentColor);
  auto& d = ui();
  d.setTextColor(gTheme.glow, gTheme.bg);
  d.setCursor(8, 29);
  d.print(def.subtitle);
  drawStatusChip(8, 43, sdReady() ? "SD READY" : "NO SD", sdReady() ? gTheme.accent : COLOR_WARN);
  drawStatusChip(74, 43, hasSave ? "SAVE" : "NEW", hasSave ? COLOR_GOOD : gTheme.rail);
  if (hasSave) {
    drawRow(2, "1 Continue", "save", selected == 0);
    drawRow(3, "2 New Run", "reset", selected == 1);
    drawRow(4, "3 Launcher", "home", selected == 2, COLOR_WARN);
  } else {
    drawRow(2, "1 New Run", "start", selected == 0);
    drawRow(3, "2 Launcher", "home", selected == 1, COLOR_WARN);
  }
  d.setTextColor(COLOR_WARN, gTheme.bg);
  d.setCursor(8, 112);
  d.print(status);
  drawFooter("W/S ;/. move  Enter select  Q home");
}

void GameRuntime::drawDash() {
  drawHeader(def.title, def.accentColor);
  auto& d = ui();
  d.setTextColor(COLOR_TEXT, gTheme.bg);
  d.setCursor(8, 30);
  d.printf("Day %-3u %s", state.day, def.locations[state.location].name);
  drawProgressBar(8, 43, d.width() - 16, 11, state.progress, def.targetProgress, gTheme.accent);
  int16_t x = 8;
  int16_t y = 61;
  for (uint8_t i = 0; i < def.resourceCount && i < 6; ++i) {
    d.setTextColor(i % 2 == 0 ? COLOR_TEXT : COLOR_DIM, gTheme.bg);
    d.setCursor(x, y);
    d.printf("%s %d", def.resources[i].label, state.resources[i]);
    x += 75;
    if (x > 160) {
      x = 8;
      y += 12;
    }
  }
  drawRow(3, "1", def.primaryVerb, selected == 0);
  drawRow(4, "2", def.safeVerb, selected == 1);
  d.setTextColor(COLOR_DIM, gTheme.bg);
  d.setCursor(8, 116);
  d.print("3 Info  4 Save/Menu  5 Home");
  drawFooter("Numbers choose actions");
}

void GameRuntime::drawEvent() {
  drawHeader("Result", def.accentColor);
  const NamedValue& ev = def.events[state.lastEvent % def.eventCount];
  const NamedValue& actor = def.actors[state.lastActor % def.actorCount];
  auto& d = ui();
  d.setTextColor(gTheme.glow, gTheme.bg);
  d.setCursor(8, 30);
  d.print(actor.name);
  drawWrapped(ev.name, 8, 46, 37, 3, COLOR_TEXT);
  drawWrapped(ev.note, 8, 80, 37, 2, COLOR_DIM);
  d.setTextColor(COLOR_WARN, gTheme.bg);
  d.setCursor(8, 104);
  d.print(status);
  drawFooter("Enter continues  Q status");
}

void GameRuntime::drawRoster() {
  drawHeader(def.rosterLabel, def.accentColor);
  for (uint8_t i = 0; i < def.actorCount && i < 5; ++i) {
    char detail[18];
    snprintf(detail, sizeof(detail), "%u", state.trackers[i]);
    drawRow(i, def.actors[i].name, detail, selected == i);
  }
  drawWrapped("Each row tracks this game's crew, clues, files, creatures, or party state.", 8, 112, 37, 1, COLOR_DIM);
  drawFooter("Back returns");
}

void GameRuntime::drawStatus() {
  drawHeader("Status", def.accentColor);
  auto& d = ui();
  for (uint8_t i = 0; i < def.resourceCount && i < 8; ++i) {
    d.setTextColor(i % 2 == 0 ? COLOR_TEXT : COLOR_DIM, gTheme.bg);
    d.setCursor(8 + (i % 2) * 110, 28 + (i / 2) * 13);
    d.printf("%s %d", def.resources[i].label, state.resources[i]);
  }
  drawProgressBar(8, 76, d.width() - 16, 11, state.progress, def.targetProgress, gTheme.accent);
  d.setTextColor(COLOR_DIM, gTheme.bg);
  d.setCursor(8, 86);
  d.printf("Seed %lu", (unsigned long)state.seed);
  d.setCursor(8, 99);
  d.printf("Save: %s", sdReady() ? "SD" : "disabled");
  drawFooter("Back returns");
}

void GameRuntime::drawEnding() {
  drawHeader(state.victory ? "Victory" : "Run Ended", def.accentColor);
  uint8_t ending = state.ending < def.endingCount ? state.ending : 0;
  auto& d = ui();
  d.setTextColor(state.victory ? COLOR_GOOD : COLOR_BAD, gTheme.bg);
  d.setCursor(8, 30);
  d.print(def.endings[ending].name);
  drawWrapped(def.endings[ending].note, 8, 48, 37, 4, COLOR_TEXT);
  d.setTextColor(COLOR_DIM, gTheme.bg);
  d.setCursor(8, 96);
  d.printf("Day %u Progress %u", state.day, state.progress);
  drawFooter("Enter title  Q launcher");
}

void GameRuntime::draw() {
  if (!dirty) return;
  setTheme(def);
  beginFrame();
  switch (screen) {
    case SCREEN_TITLE: drawTitle(); break;
    case SCREEN_DASH: drawDash(); break;
    case SCREEN_EVENT: drawEvent(); break;
    case SCREEN_ROSTER: drawRoster(); break;
    case SCREEN_STATUS: drawStatus(); break;
    case SCREEN_ENDING: drawEnding(); break;
  }
  endFrame();
  dirty = false;
}

void GameRuntime::applyPrimaryAction() {
  state.day++;
  uint16_t gain = random(5, 15);
  state.progress = state.progress + gain > def.targetProgress ? def.targetProgress : state.progress + gain;
  if (def.locationCount > 1) {
    uint32_t scaled = (uint32_t)state.progress * def.locationCount / (def.targetProgress + 1);
    state.location = scaled >= def.locationCount ? def.locationCount - 1 : scaled;
  }
  state.lastEvent = random(0, def.eventCount);
  state.lastActor = random(0, def.actorCount);
  for (uint8_t i = 0; i < def.resourceCount && i < MAX_RESOURCES; ++i) {
    int16_t drift = def.resources[i].primaryDelta + random(-2, 3);
    state.resources[i] = clampValue(state.resources[i] + drift, def.resources[i].minValue, def.resources[i].maxValue);
  }
  if (state.lastActor < MAX_TRACKERS) {
    state.trackers[state.lastActor] = constrain((int)state.trackers[state.lastActor] + 1, 0, 99);
  }
  snprintf(status, sizeof(status), "+%u progress. Autosaved.", gain);
  finishIfNeeded();
  saveGame();
  screen = state.gameOver || state.victory ? SCREEN_ENDING : SCREEN_EVENT;
  dirty = true;
}

void GameRuntime::applySafeAction() {
  state.day++;
  for (uint8_t i = 0; i < def.resourceCount && i < MAX_RESOURCES; ++i) {
    int16_t drift = def.resources[i].safeDelta;
    state.resources[i] = clampValue(state.resources[i] + drift, def.resources[i].minValue, def.resources[i].maxValue);
  }
  setStatus("Recovered and saved.");
  finishIfNeeded();
  saveGame();
  screen = state.gameOver || state.victory ? SCREEN_ENDING : SCREEN_EVENT;
  dirty = true;
}

void GameRuntime::finishIfNeeded() {
  if (state.progress >= def.targetProgress) {
    state.victory = true;
    state.gameOver = false;
    state.ending = state.resources[0] > 60 ? 0 : (def.endingCount > 1 ? 1 : 0);
    return;
  }
  bool failed = false;
  for (uint8_t i = 0; i < def.resourceCount && i < 4; ++i) {
    if (state.resources[i] <= def.resources[i].minValue && def.resources[i].primaryDelta < 0) failed = true;
  }
  if (failed) {
    state.gameOver = true;
    state.victory = false;
    state.ending = def.endingCount > 0 ? def.endingCount - 1 : 0;
  }
}

void GameRuntime::handleTitle(const InputEvent& input) {
  uint8_t count = hasSave ? 3 : 2;
  if (input.back) returnToLauncher();
  if (input.up) {
    selected = selected == 0 ? count - 1 : selected - 1;
    dirty = true;
  }
  if (input.down) {
    selected = (selected + 1) % count;
    dirty = true;
  }
  bool chosen = input.select || input.number != 0;
  if (input.number) selected = input.number - 1;
  if (!chosen) return;
  if (hasSave && selected == 0) continueGame();
  else if ((hasSave && selected == 1) || (!hasSave && selected == 0)) newGame();
  else returnToLauncher();
}

void GameRuntime::handleDash(const InputEvent& input) {
  if (input.number == 1) applyPrimaryAction();
  else if (input.number == 2) applySafeAction();
  else if (input.number == 3) { screen = SCREEN_STATUS; dirty = true; }
  else if (input.number == 4) { saveGame(); screen = SCREEN_TITLE; hasSave = saveExists(); dirty = true; }
  else if (input.number == 5) returnToLauncher();
  else if (input.up || input.down) { selected = input.down ? (selected + 1) % 2 : (selected == 0 ? 1 : 0); dirty = true; }
  else if (input.select) { selected == 0 ? applyPrimaryAction() : applySafeAction(); }
  else if (input.back) { screen = SCREEN_TITLE; hasSave = saveExists(); dirty = true; }
}

void GameRuntime::handleInput(const InputEvent& input) {
  switch (screen) {
    case SCREEN_TITLE: handleTitle(input); break;
    case SCREEN_DASH: handleDash(input); break;
    case SCREEN_EVENT:
      if (input.back) screen = SCREEN_STATUS;
      else screen = SCREEN_DASH;
      dirty = true;
      break;
    case SCREEN_ROSTER:
    case SCREEN_STATUS:
      if (input.back || input.select || input.number) { screen = SCREEN_DASH; dirty = true; }
      break;
    case SCREEN_ENDING:
      if (input.back) returnToLauncher();
      else if (input.select || input.number) { screen = SCREEN_TITLE; selected = 0; hasSave = saveExists(); dirty = true; }
      break;
  }
}

enum DeepView : uint8_t {
  DEEP_TITLE,
  DEEP_HUB,
  DEEP_DETAIL,
  DEEP_RESULT,
  DEEP_STATUS,
  DEEP_ENDING
};

DeepGameRuntime::DeepGameRuntime(const DeepGameDefinition& definition) : def(definition) {}

void DeepGameRuntime::setStatus(const char* text) {
  copyText(status, sizeof(status), text);
}

void DeepGameRuntime::begin() {
  auto cfg = M5.config();
  M5Cardputer.begin(cfg, true);
  Serial.begin(115200);
  Serial.printf("[%s] deep boot\n", def.slug);
  applyGameTheme(def.slug, def.accentColor, def.warningColor);
  initDisplay();
  randomSeed((uint32_t)esp_random());
  mountSd();
  hasSave = saveExists();
  setStatus(sdReady() ? "Deep run ready." : "SD missing. Saves disabled.");
  dirty = true;
  nextAnimMs = millis() + 100;
}

void DeepGameRuntime::loop() {
  draw();
  M5Cardputer.update();
  InputEvent input = readInput();
  if (!input.any()) {
    if (gFrameReady && millis() >= nextAnimMs) {
      dirty = true;
      nextAnimMs = millis() + 130;
    }
    delay(12);
    return;
  }
  handleInput(input);
}

bool DeepGameRuntime::saveExists() {
  if (!sdReady()) return false;
  char save[128];
  char bak[128];
  buildGameSavePath(def.slug, "save.sav", save, sizeof(save));
  buildGameSavePath(def.slug, "save.bak", bak, sizeof(bak));
  return SD.exists(save) || SD.exists(bak);
}

void DeepGameRuntime::newGame() {
  memset(&state, 0, sizeof(state));
  state.seed = (uint32_t)random(1, 2147483647);
  state.day = 1;
  for (uint8_t i = 0; i < MAX_RESOURCES; ++i) {
    state.resources[i] = i < def.resourceCount ? def.resources[i].start : 0;
  }
  for (uint8_t i = 0; i < MAX_REPUTATION; ++i) state.reputation[i] = 10;
  state.location = 0;
  state.progress = 0;
  activeScreen = 0;
  selected = 0;
  page = 0;
  setStatus("New deep run started.");
  view = DEEP_HUB;
  saveGame();
  dirty = true;
}

bool DeepGameRuntime::saveGame() {
  if (!ensureGameSaveDir(def.slug)) {
    setStatus("SD missing. Save skipped.");
    return false;
  }
  char body[760];
  int len = snprintf(body, sizeof(body),
                     "%s\n"
                     "schema=2\n"
                     "seed=%lu\n"
                     "day=%u\n"
                     "progress=%u\n"
                     "location=%u\n"
                     "resources=%d,%d,%d,%d,%d,%d,%d,%d\n"
                     "trackers=%u,%u,%u,%u,%u,%u,%u,%u\n"
                     "rep=%d,%d,%d,%d\n"
                     "flags=%08lX\n"
                     "last=%u,%u\n"
                     "ui=%u,%u,%u\n"
                     "end=%u,%u,%u\n",
                     def.saveMagic,
                     (unsigned long)state.seed,
                     state.day,
                     state.progress,
                     state.location,
                     state.resources[0], state.resources[1], state.resources[2], state.resources[3],
                     state.resources[4], state.resources[5], state.resources[6], state.resources[7],
                     state.trackers[0], state.trackers[1], state.trackers[2], state.trackers[3],
                     state.trackers[4], state.trackers[5], state.trackers[6], state.trackers[7],
                     state.reputation[0], state.reputation[1], state.reputation[2], state.reputation[3],
                     (unsigned long)state.flags,
                     state.lastEvent, state.lastActor,
                     activeScreen, selected, page,
                     state.gameOver ? 1 : 0, state.victory ? 1 : 0, state.ending);
  if (len <= 0 || len >= (int)sizeof(body)) {
    setStatus("Save body too large.");
    return false;
  }
  uint16_t crc = 0xBEEF;
  const char* p = body;
  while (*p) {
    const char* eol = strchr(p, '\n');
    if (!eol) break;
    char line[160];
    size_t n = min<size_t>((size_t)(eol - p), sizeof(line) - 1);
    memcpy(line, p, n);
    line[n] = '\0';
    crc = updateChecksum(crc, line);
    p = eol + 1;
  }
  char save[128];
  buildGameSavePath(def.slug, "save.sav", save, sizeof(save));
  if (!writeAtomicTextSave(save, body, crc)) {
    setStatus("Save write failed.");
    return false;
  }
  hasSave = true;
  setStatus("Deep state saved.");
  return true;
}

bool DeepGameRuntime::loadGame() {
  if (!sdReady()) {
    setStatus("SD missing. Cannot load.");
    return false;
  }
  char save[128];
  char bak[128];
  buildGameSavePath(def.slug, "save.sav", save, sizeof(save));
  buildGameSavePath(def.slug, "save.bak", bak, sizeof(bak));
  String text;
  if (!readTextSave(save, bak, text)) {
    setStatus("No save found.");
    return false;
  }
  int firstBreak = text.indexOf('\n');
  String first = firstBreak >= 0 ? text.substring(0, firstBreak) : text;
  first.trim();
  if (first != def.saveMagic) {
    setStatus("Save belongs to another game.");
    return false;
  }
  GenericGameState loaded;
  uint8_t loadedScreen = 0;
  uint8_t loadedSelected = 0;
  uint8_t loadedPage = 0;
  uint16_t crc = 0xBEEF;
  uint16_t expected = 0;
  bool sawCrc = false;
  int start = 0;
  while (start < text.length()) {
    int end = text.indexOf('\n', start);
    if (end < 0) end = text.length();
    String line = text.substring(start, end);
    line.trim();
    start = end + 1;
    if (line.length() == 0) continue;
    if (line.startsWith("crc=")) {
      expected = (uint16_t)strtoul(line.substring(4).c_str(), nullptr, 16);
      sawCrc = true;
      continue;
    }
    crc = updateChecksum(crc, line.c_str());
    if (line == def.saveMagic || line.startsWith("schema=")) continue;
    if (line.startsWith("seed=")) loaded.seed = (uint32_t)line.substring(5).toInt();
    else if (line.startsWith("day=")) loaded.day = (uint16_t)line.substring(4).toInt();
    else if (line.startsWith("progress=")) loaded.progress = (uint16_t)line.substring(9).toInt();
    else if (line.startsWith("location=")) loaded.location = (uint8_t)line.substring(9).toInt();
    else if (line.startsWith("resources=")) {
      int values[8];
      if (parseCsv(line.substring(10), values, 8)) {
        for (uint8_t i = 0; i < MAX_RESOURCES; ++i) loaded.resources[i] = values[i];
      }
    } else if (line.startsWith("trackers=")) {
      int values[8];
      if (parseCsv(line.substring(9), values, 8)) {
        for (uint8_t i = 0; i < MAX_TRACKERS; ++i) loaded.trackers[i] = constrain(values[i], 0, 255);
      }
    } else if (line.startsWith("rep=")) {
      int values[4];
      if (parseCsv(line.substring(4), values, 4)) {
        for (uint8_t i = 0; i < MAX_REPUTATION; ++i) loaded.reputation[i] = constrain(values[i], -99, 99);
      }
    } else if (line.startsWith("flags=")) loaded.flags = strtoul(line.substring(6).c_str(), nullptr, 16);
    else if (line.startsWith("last=")) {
      int values[2];
      if (parseCsv(line.substring(5), values, 2)) {
        loaded.lastEvent = values[0];
        loaded.lastActor = values[1];
      }
    } else if (line.startsWith("ui=")) {
      int values[3];
      if (parseCsv(line.substring(3), values, 3)) {
        loadedScreen = constrain(values[0], 0, 255);
        loadedSelected = constrain(values[1], 0, 255);
        loadedPage = constrain(values[2], 0, 255);
      }
    } else if (line.startsWith("end=")) {
      int values[3];
      if (parseCsv(line.substring(4), values, 3)) {
        loaded.gameOver = values[0] != 0;
        loaded.victory = values[1] != 0;
        loaded.ending = values[2];
      }
    }
  }
  if (sawCrc && crc != expected) {
    setStatus("Save checksum failed.");
    return false;
  }
  if (loaded.ending >= def.endingCount) loaded.ending = 0;
  state = loaded;
  activeScreen = def.screenCount == 0 ? 0 : loadedScreen % def.screenCount;
  selected = loadedSelected;
  page = loadedPage;
  view = state.gameOver || state.victory ? DEEP_ENDING : DEEP_HUB;
  setStatus("Loaded deep save.");
  dirty = true;
  return true;
}

void DeepGameRuntime::drawTitle() {
  drawHeader(def.title, def.accentColor);
  auto& d = ui();
  d.setTextColor(gTheme.glow, gTheme.bg);
  d.setCursor(8, 29);
  d.print(def.subtitle);
  drawStatusChip(8, 43, sdReady() ? "SD READY" : "NO SD", sdReady() ? gTheme.accent : COLOR_WARN);
  drawStatusChip(74, 43, hasSave ? "DEEP SAVE" : "NEW", hasSave ? COLOR_GOOD : gTheme.rail);
  if (hasSave) {
    drawRow(2, "1 Continue", "save", selected == 0);
    drawRow(3, "2 New Run", "reset", selected == 1);
    drawRow(4, "3 Launcher", "home", selected == 2, COLOR_WARN);
  } else {
    drawRow(2, "1 New Run", "start", selected == 0);
    drawRow(3, "2 Launcher", "home", selected == 1, COLOR_WARN);
  }
  d.setTextColor(COLOR_WARN, gTheme.bg);
  d.setCursor(8, 112);
  d.print(status);
  drawFooter("W/S move  Enter select  Q home");
}

void DeepGameRuntime::drawHub() {
  drawHeader(def.title, def.accentColor);
  auto& d = ui();
  static constexpr uint8_t hubRowsPerPage = 3;
  d.setTextColor(COLOR_TEXT, gTheme.bg);
  d.setCursor(8, 29);
  const char* primaryLabel = (def.trackerLabels && def.primaryTracker < def.trackerLabelCount)
                                 ? def.trackerLabels[def.primaryTracker]
                                 : nullptr;
  if (primaryLabel) {
    d.printf("D%-3u P%u/%u  %s %u", state.day, state.progress, def.targetProgress,
             primaryLabel, state.trackers[def.primaryTracker]);
  } else {
    d.printf("Day %-3u Prog %u/%u", state.day, state.progress, def.targetProgress);
  }
  // Up to four key resources on one line, color-coded by health. The full set of
  // eight (and the tracker labels) lives on the Status screen.
  int16_t x = 8;
  for (uint8_t i = 0; i < def.resourceCount && i < 4; ++i) {
    d.setTextColor(healthColor(resourceHealth(i)), gTheme.bg);
    d.setCursor(x, 43);
    d.printf("%s %d", def.resources[i].label, state.resources[i]);
    x += 58;
  }
  uint8_t maxPage = def.screenCount == 0 ? 0 : (def.screenCount - 1) / hubRowsPerPage;
  if (page > maxPage) page = maxPage;
  uint8_t start = page * hubRowsPerPage;
  uint8_t count = start >= def.screenCount ? 0 : min<uint8_t>(hubRowsPerPage, def.screenCount - start);
  if (count == 0) selected = 0;
  else if (selected >= count) selected = count - 1;
  for (uint8_t i = 0; i < count; ++i) {
    const DeepScreenDef& screen = def.screens[start + i];
    char label[28];
    snprintf(label, sizeof(label), "%u %s", i + 1, screen.title);
    drawRow(i + 2, label, screen.verb, selected == i);
  }
  d.setTextColor(COLOR_DIM, gTheme.bg);
  d.setCursor(8, 116);
  d.printf("Pg %u/%u  5 Status  Q title", page + 1, max<uint8_t>((def.screenCount + hubRowsPerPage - 1) / hubRowsPerPage, 1));
  drawFooter(def.objective ? def.objective : "Pick a deep system");
}

void DeepGameRuntime::drawDetail() {
  const DeepScreenDef& screen = def.screens[activeScreen % def.screenCount];
  drawHeader(screen.title, def.accentColor);
  auto& d = ui();
  uint8_t start = page * 5;
  for (uint8_t i = 0; i < 5; ++i) {
    uint8_t idx = start + i;
    if (idx >= screen.rowCount) break;
    char label[28];
    snprintf(label, sizeof(label), "%u %s", i + 1, screen.rows[idx].name);
    drawRow(i, label, screen.verb, selected == i);
  }
  d.setTextColor(COLOR_DIM, gTheme.bg);
  d.setCursor(8, 116);
  d.printf("Pg %u/%u  Enter acts  Q hub", page + 1, max<uint8_t>((screen.rowCount + 4) / 5, 1));
  drawFooter(screen.objective ? screen.objective : "W/S row  A/D page");
}

void DeepGameRuntime::drawResult() {
  drawHeader("Result", def.accentColor);
  auto& d = ui();
  const DeepScreenDef& screen = def.screens[activeScreen % def.screenCount];
  uint8_t row = selected + page * 5;
  if (row >= screen.rowCount) row = 0;
  const NamedValue& item = screen.rows[row];

  d.setTextColor(gTheme.glow, gTheme.bg);
  d.setCursor(8, 30);
  d.print(item.name);
  drawWrapped(item.note, 8, 44, 37, 2, COLOR_TEXT);

  if (lastActionFailed) {
    d.setTextColor(COLOR_BAD, gTheme.bg);
    d.setCursor(8, 70);
    d.print(status);
    drawFooter("Enter back  Q status");
    return;
  }

  // Per-turn resource change summary.
  d.setTextColor(COLOR_DIM, gTheme.bg);
  d.setCursor(8, 66);
  d.printf("Day %u", state.day);
  drawWrapped(lastResultLine, 8, 78, 37, 1, COLOR_TEXT);

  // Event outcome, colored by tone.
  if (def.eventCount) {
    const EventDef& ev = def.events[state.lastEvent % def.eventCount];
    uint16_t toneColor = ev.tone > 0 ? COLOR_GOOD : (ev.tone < 0 ? COLOR_BAD : gTheme.glow);
    d.setTextColor(toneColor, gTheme.bg);
    d.setCursor(8, 90);
    d.print(ev.name);
    drawWrapped(ev.note, 8, 102, 37, 1, COLOR_DIM);
  }
  drawFooter("Enter continues  Q status");
}

void DeepGameRuntime::drawStatus() {
  drawHeader("Status", def.accentColor);
  auto& d = ui();
  // All resources, color-coded by health.
  for (uint8_t i = 0; i < def.resourceCount && i < 8; ++i) {
    d.setTextColor(healthColor(resourceHealth(i)), gTheme.bg);
    d.setCursor(8 + (i % 2) * 110, 28 + (i / 2) * 13);
    d.printf("%s %d", def.resources[i].label, state.resources[i]);
  }
  drawProgressBar(8, 72, d.width() - 16, 9, state.progress, def.targetProgress, gTheme.accent);
  // Named tracker progress, two per row.
  int16_t ty = 84;
  uint8_t shown = 0;
  for (uint8_t i = 0; i < def.trackerLabelCount && i < MAX_TRACKERS; ++i) {
    const char* lbl = def.trackerLabels ? def.trackerLabels[i] : nullptr;
    if (!lbl) continue;
    d.setTextColor(COLOR_DIM, gTheme.bg);
    d.setCursor(8 + (shown % 2) * 110, ty);
    d.printf("%s %u", lbl, state.trackers[i]);
    if (shown % 2 == 1) ty += 12;
    ++shown;
  }
  if (shown % 2 == 1) ty += 12;
  d.setTextColor(COLOR_DIM, gTheme.bg);
  d.setCursor(8, ty < 110 ? ty : 110);
  d.printf("Save %s  Seed %lu", sdReady() ? "SD" : "--", (unsigned long)state.seed);
  drawFooter("Back returns  4 saves");
}

void DeepGameRuntime::drawEnding() {
  drawHeader(state.victory ? "Victory" : "Run Ended", def.accentColor);
  uint8_t ending = state.ending < def.endingCount ? state.ending : 0;
  auto& d = ui();
  d.setTextColor(state.victory ? COLOR_GOOD : COLOR_BAD, gTheme.bg);
  d.setCursor(8, 30);
  d.print(def.endings[ending].name);
  drawWrapped(def.endings[ending].note, 8, 48, 37, 4, COLOR_TEXT);
  d.setTextColor(COLOR_DIM, gTheme.bg);
  d.setCursor(8, 96);
  d.printf("Day %u Progress %u", state.day, state.progress);
  drawFooter("Enter title  Q launcher");
}

void DeepGameRuntime::draw() {
  if (!dirty) return;
  applyGameTheme(def.slug, def.accentColor, def.warningColor);
  beginFrame();
  switch (view) {
    case DEEP_TITLE: drawTitle(); break;
    case DEEP_HUB: drawHub(); break;
    case DEEP_DETAIL: drawDetail(); break;
    case DEEP_RESULT: drawResult(); break;
    case DEEP_STATUS: drawStatus(); break;
    case DEEP_ENDING: drawEnding(); break;
  }
  endFrame();
  dirty = false;
}

void DeepGameRuntime::applyResourceDelta(uint8_t index, int16_t delta) {
  if (index >= def.resourceCount || index >= MAX_RESOURCES) return;
  int16_t before = state.resources[index];
  state.resources[index] = clampValue(before + delta, def.resources[index].minValue,
                                      def.resources[index].maxValue);
  // Record the effective (post-clamp) change so the result screen is honest.
  lastDeltas[index] += (state.resources[index] - before);
}

uint8_t DeepGameRuntime::selectEvent(uint8_t rowIndex) const {
  if (def.eventCount == 0) return 0;
  // Deterministic per-turn mix of the saved seed and turn context.
  uint32_t r = state.seed ^ ((uint32_t)state.day * 2654435761u) ^
               ((uint32_t)activeScreen << 8) ^ rowIndex;
  uint32_t total = 0;
  for (uint8_t i = 0; i < def.eventCount; ++i) {
    const EventDef& e = def.events[i];
    if (e.requireFlags && (state.flags & e.requireFlags) != e.requireFlags) continue;
    if (e.forbidFlags && (state.flags & e.forbidFlags)) continue;
    total += e.weight ? e.weight : 1;
  }
  if (total == 0) return (rowIndex + activeScreen + state.day) % def.eventCount;
  uint32_t pick = r % total;
  for (uint8_t i = 0; i < def.eventCount; ++i) {
    const EventDef& e = def.events[i];
    if (e.requireFlags && (state.flags & e.requireFlags) != e.requireFlags) continue;
    if (e.forbidFlags && (state.flags & e.forbidFlags)) continue;
    uint32_t w = e.weight ? e.weight : 1;
    if (pick < w) return i;
    pick -= w;
  }
  return 0;
}

void DeepGameRuntime::composeResultLine() {
  lastResultLine[0] = '\0';
  char* p = lastResultLine;
  size_t left = sizeof(lastResultLine);
  for (uint8_t i = 0; i < def.resourceCount && i < MAX_RESOURCES; ++i) {
    if (lastDeltas[i] == 0) continue;
    int n = snprintf(p, left, "%s%s%+d", (p == lastResultLine) ? "" : "  ",
                     def.resources[i].label, lastDeltas[i]);
    if (n <= 0 || (size_t)n >= left) break;
    p += n;
    left -= n;
  }
  if (lastResultLine[0] == '\0') copyText(lastResultLine, sizeof(lastResultLine), "No change.");
}

void DeepGameRuntime::applyRow(uint8_t rowIndex) {
  if (def.screenCount == 0) return;
  const DeepScreenDef& screen = def.screens[activeScreen % def.screenCount];
  if (rowIndex >= screen.rowCount) return;

  // Gate: locked screen.
  if (screen.requireFlags && (state.flags & screen.requireFlags) != screen.requireFlags) {
    lastActionFailed = true;
    setStatus("Locked: requirements not met yet.");
    view = DEEP_RESULT;
    dirty = true;
    return;
  }
  // Gate: action cost.
  if (screen.costResource >= 0 && screen.costResource < def.resourceCount) {
    if (state.resources[screen.costResource] < screen.costAmount) {
      lastActionFailed = true;
      snprintf(status, sizeof(status), "Need %d %s.", screen.costAmount,
               def.resources[screen.costResource].label);
      view = DEEP_RESULT;
      dirty = true;
      return;
    }
  }

  lastActionFailed = false;
  for (uint8_t i = 0; i < MAX_RESOURCES; ++i) lastDeltas[i] = 0;

  state.day++;
  uint8_t progGain = screen.progressDelta + 1 + (rowIndex % 3);
  state.progress = min<uint16_t>(state.progress + progGain, def.targetProgress);
  state.location = activeScreen;

  // Pick the consequence before applying anything else.
  uint8_t ev = selectEvent(rowIndex);
  state.lastEvent = ev;
  state.lastActor = activeScreen;
  const EventDef& E = def.events[def.eventCount == 0 ? 0 : ev];

  // Pay the action cost (recorded as a delta).
  if (screen.costResource >= 0) applyResourceDelta(screen.costResource, -screen.costAmount);

  // Screen deltas (+ small jitter) then event deltas, all clamped and recorded.
  for (uint8_t i = 0; i < def.resourceCount && i < MAX_RESOURCES; ++i) {
    int16_t drift = screen.resourceDeltas[i] + (int8_t)((rowIndex % 3) - 1);
    if (def.eventCount) drift += E.resourceDeltas[i];
    applyResourceDelta(i, drift);
  }

  // Tracker bump from the screen, plus optional event tracker.
  uint8_t tracker = screen.trackerIndex % MAX_TRACKERS;
  int16_t tdelta = screen.trackerDelta ? screen.trackerDelta : 1;
  state.trackers[tracker] = constrain((int)state.trackers[tracker] + tdelta + (rowIndex % 2), 0, 255);
  if (def.eventCount && E.trackerIndex >= 0) {
    uint8_t et = E.trackerIndex % MAX_TRACKERS;
    state.trackers[et] = constrain((int)state.trackers[et] + E.trackerDelta, 0, 255);
  }

  // Reputation from the event.
  if (def.eventCount && E.repIndex >= 0) {
    uint8_t rp = E.repIndex % MAX_REPUTATION;
    state.reputation[rp] = constrain((int)state.reputation[rp] + E.repDelta, -99, 99);
  }

  // Flags: the screen sets its flag cleanly (so gating is deterministic), plus
  // any flags the rolled event advances.
  if (screen.flagMask) state.flags |= screen.flagMask;
  if (def.eventCount) state.flags |= E.setFlags;

  composeResultLine();
  snprintf(status, sizeof(status), "%s +%u progress.", screen.verb, progGain);

  finishIfNeeded();
  saveGame();
  view = state.gameOver || state.victory ? DEEP_ENDING : DEEP_RESULT;
  dirty = true;
}

uint8_t DeepGameRuntime::selectEnding(bool victory) const {
  for (uint8_t i = 0; i < def.endingCount; ++i) {
    const EndingCond& c = def.endings[i].cond;
    if (c.requiresVictory != victory) continue;
    if (c.requireFlags && (state.flags & c.requireFlags) != c.requireFlags) continue;
    if (c.forbidFlags && (state.flags & c.forbidFlags)) continue;
    if (c.resourceIndex >= 0 && c.resourceIndex < def.resourceCount) {
      int16_t v = state.resources[c.resourceIndex];
      if (v < c.resourceMin || v > c.resourceMax) continue;
    }
    if (c.trackerIndex >= 0 && state.trackers[c.trackerIndex % MAX_TRACKERS] < c.trackerMin) continue;
    if (c.repIndex >= 0 && state.reputation[c.repIndex % MAX_REPUTATION] < c.repMin) continue;
    return i;
  }
  // Guaranteed fallback: first victory ending or last loss ending.
  return victory ? 0 : (def.endingCount ? def.endingCount - 1 : 0);
}

void DeepGameRuntime::finishIfNeeded() {
  bool won = state.progress >= def.targetProgress;
  bool lost = false;
  if (!won) {
    for (uint8_t i = 0; i < def.resourceCount && i < 4; ++i) {
      if (state.resources[i] <= def.resources[i].minValue && def.resources[i].primaryDelta < 0) {
        lost = true;
        break;
      }
    }
  }
  if (!won && !lost) return;
  state.victory = won;
  state.gameOver = !won;
  state.ending = selectEnding(won);
}

uint8_t DeepGameRuntime::resourceHealth(uint8_t index) const {
  if (index >= def.resourceCount) return 0;
  const ResourceDef& r = def.resources[index];
  int16_t v = state.resources[index];
  if (!r.inverted) {
    if (v <= r.badBelow) return 2;
    if (v <= r.warnBelow) return 1;
  } else {
    if (v >= r.badBelow) return 2;
    if (v >= r.warnBelow) return 1;
  }
  return 0;
}

void DeepGameRuntime::handleTitle(const InputEvent& input) {
  uint8_t count = hasSave ? 3 : 2;
  if (input.back) returnToLauncher();
  if (input.up) selected = selected == 0 ? count - 1 : selected - 1;
  if (input.down) selected = (selected + 1) % count;
  bool chosen = input.select || input.number != 0;
  if (input.number) selected = input.number - 1;
  if (!chosen) {
    dirty = true;
    return;
  }
  if (hasSave && selected == 0) loadGame();
  else if ((hasSave && selected == 1) || (!hasSave && selected == 0)) newGame();
  else returnToLauncher();
}

void DeepGameRuntime::handleHub(const InputEvent& input) {
  static constexpr uint8_t hubRowsPerPage = 3;
  uint8_t maxPage = def.screenCount == 0 ? 0 : (def.screenCount - 1) / hubRowsPerPage;
  if (page > maxPage) page = maxPage;
  uint8_t start = page * hubRowsPerPage;
  uint8_t count = start >= def.screenCount ? 0 : min<uint8_t>(hubRowsPerPage, def.screenCount - start);
  if (count == 0) selected = 0;
  else if (selected >= count) selected = count - 1;
  if (input.back) {
    view = DEEP_TITLE;
    hasSave = saveExists();
    selected = 0;
    dirty = true;
    return;
  }
  if (input.number >= 1 && input.number <= count) {
    activeScreen = start + input.number - 1;
    selected = 0;
    page = 0;
    view = DEEP_DETAIL;
  } else if (input.number == 5) {
    view = DEEP_STATUS;
  } else if (input.left && page > 0) {
    page--;
    selected = 0;
  } else if (input.right && page < maxPage) {
    page++;
    selected = 0;
  } else if (input.up && count > 0) {
    selected = selected == 0 ? count - 1 : selected - 1;
  } else if (input.down && count > 0) {
    selected = (selected + 1) % count;
  } else if (input.select && count > 0) {
    activeScreen = start + selected;
    selected = 0;
    page = 0;
    view = DEEP_DETAIL;
  }
  dirty = true;
}

void DeepGameRuntime::handleDetail(const InputEvent& input) {
  const DeepScreenDef& screen = def.screens[activeScreen % def.screenCount];
  uint8_t maxPage = screen.rowCount == 0 ? 0 : (screen.rowCount - 1) / 5;
  uint8_t visible = min<uint8_t>(5, screen.rowCount - page * 5);
  if (input.back) {
    view = DEEP_HUB;
    static constexpr uint8_t hubRowsPerPage = 3;
    page = activeScreen / hubRowsPerPage;
    selected = activeScreen % hubRowsPerPage;
    dirty = true;
    return;
  }
  if (input.left && page > 0) {
    page--;
    selected = 0;
  } else if (input.right && page < maxPage) {
    page++;
    selected = 0;
  } else if (input.up && visible > 0) {
    selected = selected == 0 ? visible - 1 : selected - 1;
  } else if (input.down && visible > 0) {
    selected = (selected + 1) % visible;
  } else if (input.number >= 1 && input.number <= visible) {
    selected = input.number - 1;
    applyRow(page * 5 + selected);
    return;
  } else if (input.select && visible > 0) {
    applyRow(page * 5 + selected);
    return;
  }
  dirty = true;
}

void DeepGameRuntime::handleInput(const InputEvent& input) {
  switch (view) {
    case DEEP_TITLE: handleTitle(input); break;
    case DEEP_HUB: handleHub(input); break;
    case DEEP_DETAIL: handleDetail(input); break;
    case DEEP_RESULT:
      if (input.back) view = DEEP_STATUS;
      else view = DEEP_HUB;
      dirty = true;
      break;
    case DEEP_STATUS:
      if (input.number == 4) saveGame();
      view = DEEP_HUB;
      dirty = true;
      break;
    case DEEP_ENDING:
      if (input.back) returnToLauncher();
      else if (input.select || input.number) {
        view = DEEP_TITLE;
        selected = 0;
        hasSave = saveExists();
        dirty = true;
      }
      break;
  }
}

}  // namespace CardputerGameCore
