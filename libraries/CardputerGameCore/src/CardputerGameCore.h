#pragma once

#include <Arduino.h>
#include <M5Cardputer.h>

namespace CardputerGameCore {

static constexpr uint8_t SD_CS = 12;
static constexpr uint8_t SD_SCK = 40;
static constexpr uint8_t SD_MISO = 39;
static constexpr uint8_t SD_MOSI = 14;
static constexpr uint8_t SD_POWER_HOLD = 5;
static constexpr uint8_t MAX_RESOURCES = 8;
static constexpr uint8_t MAX_TRACKERS = 8;
static constexpr uint8_t MAX_REPUTATION = 4;
static constexpr uint16_t COLOR_BG = 0x0000;
static constexpr uint16_t COLOR_PANEL = 0x0841;
static constexpr uint16_t COLOR_PANEL_HOT = 0x1126;
static constexpr uint16_t COLOR_TEXT = 0xFFFF;
static constexpr uint16_t COLOR_DIM = 0x9CD3;
static constexpr uint16_t COLOR_WARN = 0xFD20;
static constexpr uint16_t COLOR_BAD = 0xF800;
static constexpr uint16_t COLOR_GOOD = 0x07E0;

struct InputEvent {
  bool up = false;
  bool down = false;
  bool left = false;
  bool right = false;
  bool select = false;
  bool back = false;
  uint8_t number = 0;
  char key = 0;

  bool any() const {
    return up || down || left || right || select || back || number != 0;
  }
};

struct ResourceDef {
  const char* label;
  int16_t start;
  int16_t minValue;
  int16_t maxValue;
  int8_t primaryDelta;
  int8_t safeDelta;
};

struct NamedValue {
  const char* name;
  const char* note;
};

struct GameDefinition {
  const char* title;
  const char* slug;
  const char* subtitle;
  const char* saveMagic;
  const char* primaryVerb;
  const char* safeVerb;
  const char* rosterLabel;
  const char* shopLabel;
  const ResourceDef* resources;
  uint8_t resourceCount;
  const NamedValue* locations;
  uint8_t locationCount;
  const NamedValue* actors;
  uint8_t actorCount;
  const NamedValue* events;
  uint8_t eventCount;
  const NamedValue* endings;
  uint8_t endingCount;
  uint16_t targetProgress;
  uint8_t startingLocation;
  uint16_t accentColor;
  uint16_t warningColor;
};

struct DeepScreenDef {
  const char* title;
  const char* verb;
  const NamedValue* rows;
  uint8_t rowCount;
  int8_t resourceDeltas[MAX_RESOURCES];
  uint8_t progressDelta;
  uint8_t trackerIndex;
  uint32_t flagMask;
};

struct DeepGameDefinition {
  const char* title;
  const char* slug;
  const char* subtitle;
  const char* saveMagic;
  const ResourceDef* resources;
  uint8_t resourceCount;
  const DeepScreenDef* screens;
  uint8_t screenCount;
  const NamedValue* events;
  uint8_t eventCount;
  const NamedValue* endings;
  uint8_t endingCount;
  uint16_t targetProgress;
  uint16_t accentColor;
  uint16_t warningColor;
};

struct GenericGameState {
  uint32_t seed = 0;
  uint16_t day = 1;
  uint16_t progress = 0;
  uint8_t location = 0;
  int16_t resources[MAX_RESOURCES] = {0};
  uint8_t trackers[MAX_TRACKERS] = {0};
  int8_t reputation[MAX_REPUTATION] = {0};
  uint32_t flags = 0;
  uint8_t lastEvent = 0;
  uint8_t lastActor = 0;
  bool gameOver = false;
  bool victory = false;
  uint8_t ending = 0;
};

enum RuntimeScreen : uint8_t {
  SCREEN_TITLE,
  SCREEN_DASH,
  SCREEN_EVENT,
  SCREEN_ROSTER,
  SCREEN_STATUS,
  SCREEN_ENDING
};

void initDisplay();
void drawHeader(const char* title, uint16_t accentColor);
void drawFooter(const char* text);
void drawRow(uint8_t row, const char* label, const char* detail, bool selected, uint16_t color = COLOR_TEXT);
void drawWrapped(const char* text, int16_t x, int16_t y, int16_t maxChars, uint8_t maxLines, uint16_t color = COLOR_TEXT);
void drawProgressBar(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t value, uint16_t maxValue, uint16_t accentColor);
void drawStatusChip(int16_t x, int16_t y, const char* label, uint16_t color);
InputEvent readInput();
bool mountSd();
bool sdReady();
void returnToLauncher(uint32_t delayMs = 250);
int16_t clampValue(int16_t value, int16_t lo, int16_t hi);
uint16_t updateChecksum(uint16_t crc, const char* text);
void applyGameTheme(const char* slug, uint16_t accentColor, uint16_t warningColor);
void buildGameSavePath(const char* slug, const char* filename, char* out, size_t outSize);
bool ensureGameSaveDir(const char* slug);
bool writeAtomicTextSave(const char* path, const char* body, uint16_t checksum);
bool readTextSave(const char* path, const char* fallbackBakPath, String& out);

class GameRuntime {
 public:
  explicit GameRuntime(const GameDefinition& definition);
  void begin();
  void loop();

 private:
  const GameDefinition& def;
  GenericGameState state;
  RuntimeScreen screen = SCREEN_TITLE;
  bool dirty = true;
  bool hasSave = false;
  uint8_t selected = 0;
  char status[112] = "";
  uint32_t nextAnimMs = 0;

  void setStatus(const char* text);
  void newGame();
  void continueGame();
  void draw();
  void drawTitle();
  void drawDash();
  void drawEvent();
  void drawRoster();
  void drawStatus();
  void drawEnding();
  void handleInput(const InputEvent& input);
  void handleTitle(const InputEvent& input);
  void handleDash(const InputEvent& input);
  void applyPrimaryAction();
  void applySafeAction();
  void finishIfNeeded();
  void savePath(char* out, size_t outSize, const char* suffix) const;
  bool ensureSaveDir();
  bool saveGame();
  bool loadGame();
  bool saveExists();
};

class DeepGameRuntime {
 public:
  explicit DeepGameRuntime(const DeepGameDefinition& definition);
  void begin();
  void loop();

 private:
  const DeepGameDefinition& def;
  GenericGameState state;
  uint8_t view = 0;
  uint8_t selected = 0;
  uint8_t activeScreen = 0;
  uint8_t page = 0;
  bool dirty = true;
  bool hasSave = false;
  char status[112] = "";
  uint32_t nextAnimMs = 0;

  void setStatus(const char* text);
  void newGame();
  bool saveExists();
  bool saveGame();
  bool loadGame();
  void draw();
  void drawTitle();
  void drawHub();
  void drawDetail();
  void drawResult();
  void drawStatus();
  void drawEnding();
  void handleInput(const InputEvent& input);
  void handleTitle(const InputEvent& input);
  void handleHub(const InputEvent& input);
  void handleDetail(const InputEvent& input);
  void applyRow(uint8_t rowIndex);
  void finishIfNeeded();
};

}  // namespace CardputerGameCore

inline void cardputerGameReturnToLauncher(uint32_t delayMs = 250) {
  CardputerGameCore::returnToLauncher(delayMs);
}
