#include <Arduino.h>
#include <ArduinoJson.h>
#include <M5Cardputer.h>
#include <Preferences.h>
#include <SD.h>
#include <SPI.h>
#include <esp_ota_ops.h>
#include <esp_partition.h>
#include <esp_system.h>

static constexpr uint32_t SERIAL_BAUD = 115200;
static constexpr uint8_t SD_CS = 12;
static constexpr uint8_t SD_SCK = 40;
static constexpr uint8_t SD_MISO = 39;
static constexpr uint8_t SD_MOSI = 14;
static constexpr uint8_t SD_POWER_HOLD = 5;
static constexpr const char* APP_DIR = "/cardputer-game-os/apps";
static constexpr const char* APP_MANIFEST = "/cardputer-game-os/apps/games.json";
static constexpr const char* PREF_NS = "cardgameos";
static constexpr uint32_t AUTO_LAUNCH_MAGIC = 0xC0DEC0DE;
static constexpr uint8_t ESP_APP_IMAGE_MAGIC = 0xE9;
static constexpr uint8_t MAX_APPS = 20;
static constexpr size_t FLASH_CHUNK = 4096;
static constexpr uint32_t BOOT_SPLASH_MS = 1800;
static constexpr uint32_t AUTO_LAUNCH_SPLASH_MS = 1500;
static constexpr uint16_t COLOR_BG = 0x0000;
static constexpr uint16_t COLOR_PANEL = 0x0841;
static constexpr uint16_t COLOR_RAIL = 0x10A2;
static constexpr uint16_t COLOR_TEXT = 0xFFFF;
static constexpr uint16_t COLOR_DIM = 0x9CD3;
static constexpr uint16_t COLOR_ACCENT = 0x07FF;
static constexpr uint16_t COLOR_WARN = 0xFD20;
static constexpr uint16_t COLOR_BAD = 0xF800;
static constexpr uint16_t COLOR_GOOD = 0x07E0;

RTC_DATA_ATTR uint32_t autoLaunchAttempt = 0;

struct AppEntry {
  char name[40];
  char slug[32];
  char binary[72];
  char sourcePath[112];
  char version[24];
  char notes[128];
  char status[32];
  bool installable;
};

struct InputEvent {
  bool up = false;
  bool down = false;
  bool select = false;
  bool back = false;
  bool left = false;
  bool right = false;
  bool reload = false;
  bool any = false;
};

enum ScreenMode {
  SCREEN_HOME,
  SCREEN_APPS,
  SCREEN_SETTINGS,
  SCREEN_INFO,
  SCREEN_CONFIRM_INSTALL,
  SCREEN_CONFIRM_ERASE,
  SCREEN_MESSAGE
};

SPIClass sdSPI(FSPI);
Preferences prefs;
AppEntry apps[MAX_APPS];
uint8_t appCount = 0;
bool sdMounted = false;
bool appPartitionValid = false;
bool bootToApp = true;
bool returnToLauncherOnce = false;
uint8_t brightness = 180;
char lastInstalled[40] = "";
char lastMessage[96] = "";
char pendingTitle[48] = "";
char pendingBody[128] = "";
uint8_t selectedHome = 0;
uint8_t selectedApp = 0;
uint8_t selectedSettings = 0;
uint8_t appScroll = 0;
ScreenMode screenMode = SCREEN_HOME;
bool menuInterruptRequested = false;
lgfx::LovyanGFX* uiTarget = &M5Cardputer.Display;
bool uiFrameReady = false;
bool uiFrameActive = false;
uint32_t nextPulseRedraw = 0;

struct LauncherTheme {
  uint16_t accent;
  uint16_t glow;
  uint16_t panel;
  const char* tag;
};

M5Canvas& uiFrame() {
  static M5Canvas frame(&M5Cardputer.Display);
  return frame;
}

lgfx::LovyanGFX& ui() {
  return *uiTarget;
}

bool beginUiFrame() {
  auto& display = M5Cardputer.Display;
  M5Canvas& frame = uiFrame();
  if (!uiFrameReady) {
    frame.setColorDepth(16);
    uiFrameReady = frame.createSprite(display.width(), display.height()) != nullptr;
    if (uiFrameReady) {
      frame.setTextDatum(top_left);
      frame.setTextWrap(false);
    }
  }
  uiFrameActive = uiFrameReady;
  uiTarget = uiFrameReady ? static_cast<lgfx::LovyanGFX*>(&frame) : static_cast<lgfx::LovyanGFX*>(&display);
  return uiFrameReady;
}

void endUiFrame() {
  if (uiFrameActive) uiFrame().pushSprite(&M5Cardputer.Display, 0, 0);
  uiTarget = &M5Cardputer.Display;
  uiFrameActive = false;
}

uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

uint16_t scale565(uint16_t color, uint8_t percent) {
  uint8_t r = ((color >> 11) & 0x1F) * 255 / 31;
  uint8_t g = ((color >> 5) & 0x3F) * 255 / 63;
  uint8_t b = (color & 0x1F) * 255 / 31;
  return rgb565((uint16_t)r * percent / 100,
                (uint16_t)g * percent / 100,
                (uint16_t)b * percent / 100);
}

uint16_t blend565(uint16_t a, uint16_t b, uint8_t amount) {
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

bool slugIs(const char* slug, const char* expected) {
  return slug && strcmp(slug, expected) == 0;
}

LauncherTheme themeForSlug(const char* slug) {
  if (slugIs(slug, "star-trader-pocket-frontier")) return {0x07FF, 0x7DFF, 0x0842, "STAR"};
  if (slugIs(slug, "dungeon-courier")) return {0xB81F, 0xF81F, 0x210B, "QUEST"};
  if (slugIs(slug, "tiny-wasteland")) return {0xFD20, 0xFFE0, 0x30C1, "ROAD"};
  if (slugIs(slug, "pocket-kingdom-manager")) return {0xFFE0, 0xFFFF, 0x3180, "CROWN"};
  if (slugIs(slug, "monster-ranch-trail")) return {0x07E0, 0xAFE5, 0x0841, "HERD"};
  if (slugIs(slug, "guildmaster-pocket")) return {0xA145, 0xF81F, 0x2128, "GUILD"};
  if (slugIs(slug, "haunted-radio-operator")) return {0xBDF7, 0xFFFF, 0x1082, "GHOST"};
  if (slugIs(slug, "cyberdeck-hacker-rpg")) return {0x07FF, 0xF81F, 0x082A, "NODE"};
  if (slugIs(slug, "pocket-detective-agency")) return {0xFFFF, 0xC618, 0x1082, "CASE"};
  if (slugIs(slug, "cryptid-park-ranger")) return {0x07E0, 0x07FF, 0x0862, "FIELD"};
  if (slugIs(slug, "cyber-ranger")) return {0x07BF, 0x07FF, 0x0845, "RANGE"};
  if (slugIs(slug, "star-trail-rancher")) return {0xF81F, 0x07FF, 0x2011, "TRAIL"};
  if (slugIs(slug, "wasteland-guildmaster")) return {0xFD20, 0xFFE0, 0x30A0, "WASTE"};
  if (slugIs(slug, "signal-rat-cyberdeck-rpg")) return {0x07FF, 0xFFE0, 0x0825, "SIGNAL"};
  if (slugIs(slug, "poke-trail")) return {COLOR_DIM, COLOR_TEXT, COLOR_PANEL, "EXT"};
  return {COLOR_ACCENT, COLOR_TEXT, COLOR_PANEL, "OS"};
}

uint8_t pulsePhase() {
  return (millis() / 95) & 0x07;
}

bool hasInstalledName() {
  return lastInstalled[0] != '\0';
}

const char* installedLabel() {
  if (!appPartitionValid) return "none";
  return hasInstalledName() ? lastInstalled : "unknown";
}

void copyField(char* dest, size_t destSize, const char* source) {
  if (destSize == 0) return;
  if (!source) source = "";
  strncpy(dest, source, destSize - 1);
  dest[destSize - 1] = '\0';
}

void setMessage(const char* title, const char* body) {
  copyField(pendingTitle, sizeof(pendingTitle), title);
  copyField(pendingBody, sizeof(pendingBody), body);
  screenMode = SCREEN_MESSAGE;
}

void loadPrefs() {
  prefs.begin(PREF_NS, false);
  brightness = prefs.getUChar("brightness", 180);
  bootToApp = prefs.getBool("bootToApp", false);
  returnToLauncherOnce = prefs.getBool("returnOnce", false);
  if (returnToLauncherOnce) {
    prefs.putBool("returnOnce", false);
    bootToApp = false;
    prefs.putBool("bootToApp", false);
  }
  String installed = prefs.getString("lastApp", "");
  copyField(lastInstalled, sizeof(lastInstalled), installed.c_str());
}

void savePrefs() {
  prefs.putUChar("brightness", brightness);
  prefs.putBool("bootToApp", bootToApp);
  prefs.putString("lastApp", lastInstalled);
}

const esp_partition_t* appPartition() {
  const esp_partition_t* part = esp_partition_find_first(
      ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_OTA_1, "app1");
  if (!part) {
    part = esp_partition_find_first(
        ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_OTA_1, nullptr);
  }
  return part;
}

bool setBootPartition(const esp_partition_t* part) {
  if (!part) return false;
  esp_err_t err = esp_ota_set_boot_partition(part);
  if (err != ESP_OK) {
    Serial.printf("[boot] set boot partition failed: %s\n", esp_err_to_name(err));
    return false;
  }
  Serial.printf("[boot] next boot: %s @ 0x%06X\n", part->label, part->address);
  return true;
}

bool readAppMagic() {
  const esp_partition_t* part = appPartition();
  if (!part) return false;
  uint8_t magic = 0;
  if (esp_partition_read(part, 0, &magic, 1) != ESP_OK) return false;
  return magic == ESP_APP_IMAGE_MAGIC;
}

void drawHeader(const char* title, uint16_t accent = COLOR_ACCENT, const char* tag = "OS") {
  auto& d = ui();
  d.fillScreen(COLOR_BG);
  d.fillRect(0, 0, d.width(), d.height(), COLOR_BG);
  for (int16_t y = 18 + (millis() / 120) % 12; y < d.height() - 14; y += 12) {
    d.drawFastHLine(8, y, d.width() - 16, scale565(COLOR_RAIL, 55));
  }
  d.fillRoundRect(4, 3, d.width() - 8, 21, 4, COLOR_PANEL);
  d.drawRoundRect(4, 3, d.width() - 8, 21, 4, accent);
  d.drawFastHLine(8, 26, d.width() - 16, scale565(accent, 75));
  d.setTextSize(1);
  d.setTextColor(COLOR_TEXT, COLOR_PANEL);
  d.setCursor(6, 6);
  d.print(title);
  d.setTextColor(accent, COLOR_PANEL);
  d.setCursor(d.width() - 84, 6);
  d.print(tag);
  d.setCursor(d.width() - 36, 6);
  int battery = M5Cardputer.Power.getBatteryLevel();
  if (battery >= 0) {
    d.printf("%d%%", battery);
  } else {
    d.print("--%");
  }
}

void drawFooter(const char* text) {
  auto& d = ui();
  d.fillRoundRect(4, d.height() - 15, d.width() - 8, 13, 3, scale565(COLOR_PANEL, 80));
  d.drawFastHLine(7, d.height() - 16, d.width() - 14, scale565(COLOR_ACCENT, 55));
  d.setTextSize(1);
  d.setTextColor(COLOR_DIM, scale565(COLOR_PANEL, 80));
  d.setCursor(6, d.height() - 11);
  d.print(text);
}

void drawRow(uint8_t row, const char* label, const char* detail, bool selected, uint16_t color = COLOR_TEXT, uint16_t accent = COLOR_ACCENT) {
  auto& d = ui();
  int16_t y = 25 + row * 20;
  uint8_t phase = selected ? pulsePhase() : 0;
  uint16_t bg = selected ? blend565(COLOR_PANEL, accent, 72 + phase * 12) : COLOR_PANEL;
  d.fillRoundRect(4, y - 2, d.width() - 8, 18, 4, bg);
  d.drawRoundRect(4, y - 2, d.width() - 8, 18, 4, selected ? accent : scale565(COLOR_RAIL, 70));
  if (selected) d.fillRect(8, y + 2, 3 + phase, 10, COLOR_TEXT);
  d.setTextSize(1);
  d.setTextColor(selected ? COLOR_BG : color, bg);
  d.setCursor(10, y + 2);
  d.print(selected ? ">" : " ");
  d.print(" ");
  d.print(label);
  if (detail && detail[0]) {
    d.setTextColor(selected ? COLOR_BG : COLOR_DIM, bg);
    int16_t x = d.width() - 8 - (strlen(detail) * 6);
    if (x < 120) x = 120;
    d.setCursor(x, y + 2);
    d.print(detail);
  }
}

void drawWrapped(const char* text, int16_t x, int16_t y, int16_t maxChars, uint8_t maxLines, uint16_t color) {
  auto& d = ui();
  d.setTextSize(1);
  d.setTextColor(color, COLOR_BG);
  const char* p = text;
  for (uint8_t line = 0; line < maxLines && *p; ++line) {
    char buf[48];
    uint8_t len = 0;
    const char* start = p;
    const char* lastSpace = nullptr;
    while (*p && *p != '\n' && len < maxChars) {
      if (*p == ' ') lastSpace = p;
      ++p;
      ++len;
    }
    if (*p && *p != '\n' && lastSpace && lastSpace > start) {
      len = lastSpace - start;
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

uint8_t homeCount() {
  return 5;
}

void drawHome() {
  drawHeader("Cypher Game OS", COLOR_ACCENT, "MENU");
  char detail[24];
  snprintf(detail, sizeof(detail), "%u", appCount);
  drawRow(0, "Games on SD", detail, selectedHome == 0, sdMounted ? COLOR_TEXT : COLOR_WARN);
  drawRow(1, "Launch Installed", installedLabel(), selectedHome == 1,
          appPartitionValid ? COLOR_TEXT : COLOR_DIM);
  drawRow(2, "Settings", bootToApp ? "auto" : "menu", selectedHome == 2);
  drawRow(3, "Erase Installed", appPartitionValid ? "ready" : "empty", selectedHome == 3,
          appPartitionValid ? COLOR_WARN : COLOR_DIM);
  drawRow(4, "Info", "status", selectedHome == 4);

  if (lastMessage[0]) {
    ui().setTextColor(COLOR_DIM, COLOR_BG);
    ui().setCursor(8, 127);
    ui().print(lastMessage);
  }
  drawFooter(".,/ down  ,/; up  Enter select  ` back");
}

void drawApps() {
  AppEntry* current = appCount ? &apps[selectedApp < appCount ? selectedApp : 0] : nullptr;
  LauncherTheme headerTheme = current ? themeForSlug(current->slug) : LauncherTheme{COLOR_ACCENT, COLOR_TEXT, COLOR_PANEL, "SD"};
  drawHeader("SD Game Catalog", headerTheme.accent, headerTheme.tag);
  if (!sdMounted) {
    drawWrapped("No SD card mounted. Put games.json and .bin files under /cardputer-game-os/apps.",
                8, 32, 36, 6, COLOR_WARN);
    drawFooter("` back  r reload");
    return;
  }
  if (appCount == 0) {
    drawWrapped("No apps found. Run tools/package-sd.sh, then copy dist/sd-card to a FAT32 SD card.",
                8, 32, 36, 6, COLOR_WARN);
    drawFooter("` back  r reload");
    return;
  }

  if (selectedApp < appScroll) appScroll = selectedApp;
  if (selectedApp >= appScroll + 4) appScroll = selectedApp - 3;
  uint8_t visible = min<uint8_t>(4, appCount - appScroll);
  for (uint8_t i = 0; i < visible; ++i) {
    AppEntry& app = apps[appScroll + i];
    LauncherTheme theme = themeForSlug(app.slug);
    const char* status = app.installable ? app.version : "needs port";
    drawRow(i, app.name, status, selectedApp == appScroll + i,
            app.installable ? theme.accent : COLOR_DIM, theme.accent);
  }

  current = &apps[selectedApp];
  ui().fillRoundRect(5, 106, ui().width() - 10, 14, 3, COLOR_BG);
  drawWrapped(current->notes, 8, 108, 37, 1, current->installable ? headerTheme.glow : COLOR_WARN);
  drawFooter("Enter install  ` back  r reload");
}

void drawSettings() {
  drawHeader("Settings", COLOR_ACCENT, "CFG");
  char brightBuf[12];
  snprintf(brightBuf, sizeof(brightBuf), "%u", brightness);
  drawRow(0, "Boot behavior", bootToApp ? "auto app" : "menu", selectedSettings == 0);
  drawRow(1, "Brightness", brightBuf, selectedSettings == 1);
  drawRow(2, "Reload SD catalog", "", selectedSettings == 2);
  drawFooter("Enter toggle/change  ` back");
}

void drawInfo() {
  drawHeader("System Info", COLOR_ACCENT, "SYS");
  auto& d = ui();
  const esp_partition_t* part = appPartition();
  d.setTextSize(1);
  d.setTextColor(COLOR_TEXT, COLOR_BG);
  d.setCursor(8, 28);
  d.printf("SD: %s", sdMounted ? "mounted" : "missing");
  d.setCursor(8, 42);
  d.printf("Catalog: %u apps", appCount);
  d.setCursor(8, 56);
  d.printf("Installed: %s", installedLabel());
  d.setCursor(8, 70);
  d.printf("Heap: %u", ESP.getFreeHeap());
  d.setCursor(8, 84);
  if (part) {
    d.printf("App slot: 0x%06X %u KB", part->address, part->size / 1024);
  } else {
    d.print("App slot: missing");
  }
  d.setCursor(8, 98);
  d.printf("Build: %s %s", __DATE__, __TIME__);
  drawFooter("` back");
}

void drawConfirmInstall() {
  AppEntry& app = apps[selectedApp];
  LauncherTheme theme = themeForSlug(app.slug);
  drawHeader("Install Game?", theme.accent, theme.tag);
  ui().setTextSize(1);
  ui().setTextColor(theme.glow, COLOR_BG);
  ui().setCursor(8, 30);
  ui().print(app.name);
  drawWrapped(app.notes, 8, 48, 37, 3, COLOR_DIM);
  drawWrapped("This copies the SD .bin into the app partition and reboots into it.",
              8, 84, 37, 3, COLOR_WARN);
  drawFooter("Enter install  ` cancel");
}

void drawConfirmErase() {
  drawHeader("Erase Installed?", COLOR_WARN, "ERASE");
  drawWrapped("This invalidates the app partition. The launcher stays installed.",
              8, 34, 37, 4, COLOR_WARN);
  drawWrapped(lastInstalled, 8, 84, 37, 2, COLOR_DIM);
  drawFooter("Enter erase  ` cancel");
}

void drawMessage() {
  drawHeader(pendingTitle, COLOR_ACCENT, "MSG");
  drawWrapped(pendingBody, 8, 32, 37, 7, COLOR_TEXT);
  drawFooter("Enter/` back");
}

void redraw() {
  beginUiFrame();
  switch (screenMode) {
    case SCREEN_HOME: drawHome(); break;
    case SCREEN_APPS: drawApps(); break;
    case SCREEN_SETTINGS: drawSettings(); break;
    case SCREEN_INFO: drawInfo(); break;
    case SCREEN_CONFIRM_INSTALL: drawConfirmInstall(); break;
    case SCREEN_CONFIRM_ERASE: drawConfirmErase(); break;
    case SCREEN_MESSAGE: drawMessage(); break;
  }
  endUiFrame();
}

bool mountSd() {
  SD.end();
  sdSPI.end();
  pinMode(SD_POWER_HOLD, OUTPUT);
  digitalWrite(SD_POWER_HOLD, HIGH);
  delay(25);
  sdSPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
  sdMounted = SD.begin(SD_CS, sdSPI, 25000000);
  if (!sdMounted) {
    copyField(lastMessage, sizeof(lastMessage), "SD card not mounted");
    Serial.println("[sd] mount failed");
    return false;
  }
  Serial.printf("[sd] mounted type=%u size=%llu MB\n", SD.cardType(),
                SD.cardSize() / (1024ULL * 1024ULL));
  return true;
}

bool loadCatalog() {
  appCount = 0;
  if (!sdMounted && !mountSd()) return false;

  File file = SD.open(APP_MANIFEST, FILE_READ);
  if (!file) {
    copyField(lastMessage, sizeof(lastMessage), "games.json missing");
    Serial.printf("[catalog] missing %s\n", APP_MANIFEST);
    return false;
  }

  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, file);
  file.close();
  if (err) {
    copyField(lastMessage, sizeof(lastMessage), "games.json parse failed");
    Serial.printf("[catalog] parse failed: %s\n", err.c_str());
    return false;
  }

  JsonArray list;
  if (doc.is<JsonArray>()) {
    list = doc.as<JsonArray>();
  } else {
    list = doc["games"].as<JsonArray>();
    if (list.isNull()) {
      list = doc["apps"].as<JsonArray>();
    }
  }
  if (list.isNull()) {
    copyField(lastMessage, sizeof(lastMessage), "games.json has no games");
    Serial.println("[catalog] no apps array");
    return false;
  }

  for (JsonObject item : list) {
    if (appCount >= MAX_APPS) break;
    AppEntry& app = apps[appCount];
    copyField(app.name, sizeof(app.name), item["name"] | "Unnamed");
    copyField(app.slug, sizeof(app.slug), item["slug"] | app.name);
    copyField(app.binary, sizeof(app.binary), item["binary"] | "");
    copyField(app.sourcePath, sizeof(app.sourcePath), item["source_path"] | "");
    copyField(app.version, sizeof(app.version), item["version"] | "local");
    copyField(app.notes, sizeof(app.notes), item["notes"] | "");
    copyField(app.status, sizeof(app.status), item["status"] | "ready");
    app.installable = app.binary[0] && strcmp(app.status, "ready") == 0;
    ++appCount;
  }

  selectedApp = 0;
  appScroll = 0;
  snprintf(lastMessage, sizeof(lastMessage), "Loaded %u apps", appCount);
  Serial.printf("[catalog] loaded %u apps\n", appCount);
  return appCount > 0;
}

bool fileLooksMerged(File& file) {
  uint8_t sig[3] = {0};
  if (file.size() <= 0x8003) return false;
  if (!file.seek(0x8000)) return false;
  if (file.read(sig, sizeof(sig)) != sizeof(sig)) return false;
  return sig[0] == 0xAA && sig[1] == 0x50 && sig[2] == 0x01;
}

void drawInstallProgress(const char* name, size_t written, size_t total, const char* phase) {
  beginUiFrame();
  auto& d = ui();
  drawHeader("Installing", COLOR_ACCENT, "FLASH");
  d.setTextColor(COLOR_TEXT, COLOR_BG);
  d.setCursor(8, 34);
  d.print(name);
  d.setTextColor(COLOR_DIM, COLOR_BG);
  d.setCursor(8, 50);
  d.print(phase);
  int16_t width = d.width() - 20;
  int16_t fill = total == 0 ? 0 : (width * written) / total;
  d.drawRect(10, 76, width, 12, COLOR_DIM);
  d.fillRect(11, 77, max<int16_t>(0, fill - 2), 10, COLOR_ACCENT);
  d.fillRect(11 + max<int16_t>(0, fill - 2), 77, 3, 10, COLOR_TEXT);
  d.setCursor(8, 98);
  d.printf("%u / %u KB", written / 1024, total / 1024);
  endUiFrame();
}

bool eraseInstalledApp(bool showMessage) {
  const esp_partition_t* part = appPartition();
  if (!part) {
    if (showMessage) setMessage("Erase Failed", "No app partition was found.");
    return false;
  }
  esp_err_t err = esp_partition_erase_range(part, 0, 4096);
  if (err != ESP_OK) {
    Serial.printf("[erase] failed: %s\n", esp_err_to_name(err));
    if (showMessage) setMessage("Erase Failed", esp_err_to_name(err));
    return false;
  }
  appPartitionValid = false;
  lastInstalled[0] = '\0';
  savePrefs();
  copyField(lastMessage, sizeof(lastMessage), "Installed app erased");
  if (showMessage) setMessage("Erased", "The app partition is now empty. The launcher remains installed.");
  return true;
}

bool installApp(uint8_t index) {
  if (index >= appCount) return false;
  AppEntry& app = apps[index];
  if (!app.installable) {
    setMessage("Not Installable", "This catalog entry is marked as needing a Cardputer port.");
    return false;
  }

  char path[128];
  snprintf(path, sizeof(path), "%s/%s", APP_DIR, app.binary);
  File file = SD.open(path, FILE_READ);
  if (!file) {
    setMessage("Install Failed", "The binary file was not found on the SD card.");
    return false;
  }

  const esp_partition_t* part = appPartition();
  if (!part) {
    file.close();
    setMessage("Install Failed", "No OTA app partition exists in the current partition table.");
    return false;
  }

  size_t total = file.size();
  if (total < 16 || total > part->size) {
    file.close();
    setMessage("Install Failed", "The .bin is empty or too large for the Cardputer app slot.");
    return false;
  }

  uint8_t header[16] = {0};
  if (file.read(header, sizeof(header)) != sizeof(header) || header[0] != ESP_APP_IMAGE_MAGIC) {
    file.close();
    setMessage("Install Failed", "Use the sketch app .bin, not a data file.");
    return false;
  }
  if (fileLooksMerged(file)) {
    file.close();
    setMessage("Install Failed", "This looks like a merged flash image. Use the sketch .bin only.");
    return false;
  }
  file.seek(0);

  Serial.printf("[install] %s from %s size=%u slot=%s 0x%06X/%u\n",
                app.name, path, total, part->label, part->address, part->size);
  drawInstallProgress(app.name, 0, total, "Erasing app slot");
  esp_err_t err = esp_partition_erase_range(part, 0, part->size);
  if (err != ESP_OK) {
    file.close();
    setMessage("Install Failed", esp_err_to_name(err));
    return false;
  }

  uint8_t buffer[FLASH_CHUNK];
  uint8_t firstBlock[16] = {0};
  bool savedFirstBlock = false;
  size_t written = 0;
  uint32_t lastUi = 0;
  while (written < total) {
    size_t want = min<size_t>(sizeof(buffer), total - written);
    size_t got = file.read(buffer, want);
    if (got == 0) {
      file.close();
      setMessage("Install Failed", "SD read stopped before the binary was complete.");
      return false;
    }

    size_t offset = 0;
    if (!savedFirstBlock) {
      memcpy(firstBlock, buffer, sizeof(firstBlock));
      savedFirstBlock = true;
      offset = sizeof(firstBlock);
    }

    if (got > offset) {
      err = esp_partition_write(part, written + offset, buffer + offset, got - offset);
      if (err != ESP_OK) {
        file.close();
        setMessage("Install Failed", esp_err_to_name(err));
        return false;
      }
    }

    written += got;
    if (millis() - lastUi > 150 || written == total) {
      drawInstallProgress(app.name, written, total, "Writing app binary");
      lastUi = millis();
    }
    yield();
  }
  file.close();

  drawInstallProgress(app.name, total, total, "Finalizing boot header");
  err = esp_partition_write(part, 0, firstBlock, sizeof(firstBlock));
  if (err != ESP_OK) {
    setMessage("Install Failed", esp_err_to_name(err));
    return false;
  }

  uint8_t verify = 0;
  err = esp_partition_read(part, 0, &verify, 1);
  if (err != ESP_OK || verify != ESP_APP_IMAGE_MAGIC) {
    setMessage("Install Failed", "The app slot did not verify after writing.");
    return false;
  }

  copyField(lastInstalled, sizeof(lastInstalled), app.name);
  savePrefs();
  appPartitionValid = true;
  autoLaunchAttempt = AUTO_LAUNCH_MAGIC;
  if (!setBootPartition(part)) {
    setMessage("Install Failed", "Could not select the app boot partition.");
    return false;
  }
  drawInstallProgress(app.name, total, total, "Complete. Rebooting");
  delay(900);
  ESP.restart();
  return true;
}

void launchInstalled() {
  if (!appPartitionValid) {
    setMessage("No Installed Game", "Install a game .bin from the SD catalog first.");
    return;
  }
  autoLaunchAttempt = AUTO_LAUNCH_MAGIC;
  if (!setBootPartition(appPartition())) {
    setMessage("Launch Failed", "Could not select the app boot partition.");
    return;
  }
  beginUiFrame();
  drawHeader("Launching", COLOR_ACCENT, "BOOT");
  drawWrapped(lastInstalled, 8, 40, 37, 2, COLOR_ACCENT);
  drawWrapped("Rebooting into the installed app partition.", 8, 70, 37, 3, COLOR_DIM);
  endUiFrame();
  delay(600);
  ESP.restart();
}

InputEvent readInput() {
  InputEvent event;
  event.select = M5Cardputer.BtnA.wasClicked();

  if (M5Cardputer.Keyboard.isChange() && M5Cardputer.Keyboard.isPressed()) {
    Keyboard_Class::KeysState keys = M5Cardputer.Keyboard.keysState();
    event.select = event.select || keys.enter;
    event.back = keys.del || keys.tab;
    for (auto c : keys.word) {
      if (c == ',' || c == ';' || c == 'w' || c == 'W' || c == 'k' || c == 'K') event.up = true;
      if (c == '.' || c == '/' || c == 's' || c == 'S' || c == 'j' || c == 'J') event.down = true;
      if (c == 'a' || c == 'A' || c == 'h' || c == 'H') event.left = true;
      if (c == 'd' || c == 'D' || c == 'l' || c == 'L') event.right = true;
      if (c == '`' || c == 'q' || c == 'Q') event.back = true;
      if (c == 'r' || c == 'R') event.reload = true;
    }
  }

  event.any = event.up || event.down || event.select || event.back || event.left || event.right || event.reload;
  return event;
}

void handleHome(InputEvent event) {
  if (event.up) selectedHome = selectedHome == 0 ? homeCount() - 1 : selectedHome - 1;
  if (event.down) selectedHome = (selectedHome + 1) % homeCount();
  if (!event.select) return;

  switch (selectedHome) {
    case 0:
      screenMode = SCREEN_APPS;
      break;
    case 1:
      launchInstalled();
      break;
    case 2:
      screenMode = SCREEN_SETTINGS;
      break;
    case 3:
      if (appPartitionValid) screenMode = SCREEN_CONFIRM_ERASE;
      else setMessage("Nothing To Erase", "The app partition is already empty.");
      break;
    case 4:
      screenMode = SCREEN_INFO;
      break;
  }
}

void handleApps(InputEvent event) {
  if (event.back) {
    screenMode = SCREEN_HOME;
    return;
  }
  if (event.reload) {
    mountSd();
    loadCatalog();
    return;
  }
  if (appCount == 0) return;
  if (event.up) selectedApp = selectedApp == 0 ? appCount - 1 : selectedApp - 1;
  if (event.down) selectedApp = (selectedApp + 1) % appCount;
  if (event.select) {
    if (apps[selectedApp].installable) screenMode = SCREEN_CONFIRM_INSTALL;
    else setMessage("Needs Port", apps[selectedApp].notes);
  }
}

void handleSettings(InputEvent event) {
  if (event.back) {
    screenMode = SCREEN_HOME;
    return;
  }
  if (event.up) selectedSettings = selectedSettings == 0 ? 2 : selectedSettings - 1;
  if (event.down) selectedSettings = (selectedSettings + 1) % 3;
  if (!event.select && !event.left && !event.right) return;

  switch (selectedSettings) {
    case 0:
      bootToApp = !bootToApp;
      savePrefs();
      break;
    case 1:
      if (event.left) brightness = brightness <= 35 ? 20 : brightness - 15;
      else brightness = brightness >= 240 ? 255 : brightness + 15;
      M5Cardputer.Display.setBrightness(brightness);
      savePrefs();
      break;
    case 2:
      mountSd();
      loadCatalog();
      break;
  }
}

void handleInput(InputEvent event) {
  if (!event.any) return;
  switch (screenMode) {
    case SCREEN_HOME:
      handleHome(event);
      break;
    case SCREEN_APPS:
      handleApps(event);
      break;
    case SCREEN_SETTINGS:
      handleSettings(event);
      break;
    case SCREEN_INFO:
      if (event.back || event.select) screenMode = SCREEN_HOME;
      break;
    case SCREEN_CONFIRM_INSTALL:
      if (event.back) screenMode = SCREEN_APPS;
      else if (event.select) installApp(selectedApp);
      break;
    case SCREEN_CONFIRM_ERASE:
      if (event.back) screenMode = SCREEN_HOME;
      else if (event.select) eraseInstalledApp(true);
      break;
    case SCREEN_MESSAGE:
      if (event.back || event.select) screenMode = SCREEN_HOME;
      break;
  }
  redraw();
}

void printStatus(Print& out) {
  const esp_partition_t* part = appPartition();
  out.println("Cypher Game OS");
  out.printf("sd=%s games=%u installed=%s bootToApp=%s brightness=%u\n",
             sdMounted ? "mounted" : "missing", appCount,
             installedLabel(),
             bootToApp ? "true" : "false", brightness);
  if (part) {
    out.printf("app_partition=%s address=0x%06X size=%u\n", part->label, part->address, part->size);
  }
}

void handleSerial() {
  if (!Serial.available()) return;
  String line = Serial.readStringUntil('\n');
  line.trim();
  if (line == "help") {
    Serial.println("commands: help, status, games, reload, launch, erase, install <slug>");
  } else if (line == "status") {
    printStatus(Serial);
  } else if (line == "apps" || line == "games") {
    for (uint8_t i = 0; i < appCount; ++i) {
      Serial.printf("%u %s slug=%s status=%s bin=%s\n", i, apps[i].name, apps[i].slug,
                    apps[i].status, apps[i].binary);
    }
  } else if (line == "reload") {
    mountSd();
    loadCatalog();
    redraw();
  } else if (line == "launch") {
    launchInstalled();
  } else if (line == "erase") {
    eraseInstalledApp(true);
    redraw();
  } else if (line.startsWith("install ")) {
    String slug = line.substring(8);
    for (uint8_t i = 0; i < appCount; ++i) {
      if (slug == apps[i].slug) {
        installApp(i);
        return;
      }
    }
    Serial.println("No app with that slug.");
  } else {
    Serial.println("Unknown command. Type help.");
  }
}

template <typename GfxT>
void drawBootText(GfxT& gfx, const char* text, int16_t y, uint8_t size,
                  uint16_t color, uint16_t bg, int16_t width) {
  gfx.setTextSize(size);
  gfx.setTextColor(color, bg);
  const int16_t textWidth = strlen(text) * 6 * size;
  int16_t x = (width - textWidth) / 2;
  if (x < 0) x = 0;
  gfx.setCursor(x, y);
  gfx.print(text);
}

template <typename GfxT>
void drawBootSplashFrame(GfxT& gfx, int16_t width, int16_t height, uint32_t elapsed,
                         uint32_t durationMs, bool allowInterrupt) {
  static const uint8_t starX[] = {12, 29, 47, 66, 91, 117, 143, 169, 197, 224};
  static const uint8_t starY[] = {17, 93, 34, 113, 24, 86, 49, 15, 104, 68};
  static const uint8_t starSpeed[] = {1, 2, 1, 3, 2, 1, 2, 1, 3, 2};

  const uint32_t safeDuration = durationMs == 0 ? 1 : durationMs;
  const int16_t sweep = (elapsed / 16) % height;
  const int16_t pulse = (elapsed / 90) % 18;
  const int16_t barWidth = width - 44;
  const int16_t progress = min<int16_t>(barWidth, (barWidth * elapsed) / safeDuration);

  gfx.fillScreen(TFT_BLACK);
  gfx.fillRect(0, 0, width, height, 0x0008);

  for (int16_t y = 10; y < height; y += 16) {
    gfx.drawFastHLine(12, y, width - 24, 0x0841);
  }
  for (int16_t x = 18; x < width; x += 24) {
    gfx.drawFastVLine(x, 8, height - 16, 0x0821);
  }

  for (uint8_t i = 0; i < sizeof(starX); ++i) {
    const int16_t x = (starX[i] + (elapsed / (42 / starSpeed[i]))) % width;
    const int16_t y = starY[i];
    const uint16_t color = (i % 3 == 0) ? TFT_CYAN : ((i % 3 == 1) ? TFT_DARKCYAN : TFT_BLUE);
    gfx.drawPixel(x, y, color);
    if (i % 4 == 0) gfx.drawPixel((x + 1) % width, y, color);
  }

  gfx.drawRect(5, 5, width - 10, height - 10, TFT_DARKCYAN);
  gfx.drawRect(8, 8, width - 16, height - 16, TFT_BLUE);
  gfx.drawFastHLine(12, 12, 34 + pulse, TFT_CYAN);
  gfx.drawFastHLine(width - 46 - pulse, 12, 34 + pulse, TFT_CYAN);
  gfx.drawFastHLine(12, height - 13, 34 + pulse, TFT_CYAN);
  gfx.drawFastHLine(width - 46 - pulse, height - 13, 34 + pulse, TFT_CYAN);
  gfx.drawFastVLine(12, 12, 24, TFT_CYAN);
  gfx.drawFastVLine(width - 13, 12, 24, TFT_CYAN);
  gfx.drawFastVLine(12, height - 36, 24, TFT_CYAN);
  gfx.drawFastVLine(width - 13, height - 36, 24, TFT_CYAN);

  gfx.fillRect(10, sweep, width - 20, 2, 0x03FF);
  if (sweep > 0) gfx.drawFastHLine(14, sweep - 1, width - 28, 0x01D7);

  gfx.setTextSize(2);
  gfx.setTextColor(TFT_BLACK, TFT_BLACK);
  gfx.setCursor((width - 72) / 2 + 2, 35);
  gfx.print("CYPHER");
  gfx.setTextColor(TFT_CYAN, TFT_BLACK);
  gfx.setCursor((width - 72) / 2, 33);
  gfx.print("CYPHER");

  drawBootText(gfx, "GAME OS", 57, 2, TFT_WHITE, TFT_BLACK, width);
  drawBootText(gfx, "SD GAME LAUNCHER", 82, 1, COLOR_DIM, TFT_BLACK, width);
  drawBootText(gfx, allowInterrupt ? "ANY KEY: MENU" : "CARDPUTER ADV",
               96, 1, TFT_DARKCYAN, TFT_BLACK, width);

  gfx.drawRect(21, height - 25, width - 42, 8, TFT_DARKCYAN);
  gfx.fillRect(23, height - 23, progress, 4, TFT_CYAN);
  gfx.fillRect(23 + progress, height - 23, 3, 4, TFT_WHITE);
}

bool bootSplash(uint32_t durationMs, bool allowInterrupt) {
  auto& display = M5Cardputer.Display;
  const int16_t width = display.width();
  const int16_t height = display.height();
  const uint32_t started = millis();
  M5Canvas frame;
  frame.setColorDepth(16);
  const bool buffered = frame.createSprite(width, height) != nullptr;

  display.setTextDatum(top_left);
  display.setTextWrap(false);
  if (buffered) {
    frame.setTextDatum(top_left);
    frame.setTextWrap(false);
  }

  while (millis() - started < durationMs) {
    const uint32_t elapsed = millis() - started;
    M5Cardputer.update();
    if (allowInterrupt && readInput().any) {
      if (buffered) frame.deleteSprite();
      return true;
    }

    if (buffered) {
      drawBootSplashFrame(frame, width, height, elapsed, durationMs, allowInterrupt);
      frame.pushSprite(&display, 0, 0);
    } else {
      drawBootSplashFrame(display, width, height, elapsed, durationMs, allowInterrupt);
    }

    delay(33);
    yield();
  }

  if (buffered) frame.deleteSprite();
  return false;
}

void maybeAutoLaunch() {
  if (!appPartitionValid || !bootToApp || !hasInstalledName()) {
    autoLaunchAttempt = 0;
    return;
  }
  if (returnToLauncherOnce) {
    autoLaunchAttempt = 0;
    setMessage("Returned To Launcher", "Select another app, or use Launch Installed to resume the current app.");
    return;
  }
  if (autoLaunchAttempt == AUTO_LAUNCH_MAGIC) {
    autoLaunchAttempt = 0;
    setMessage("Auto Launch Paused", "Launcher returned after a launch attempt. Use Launch Installed manually.");
    return;
  }
  if (menuInterruptRequested || bootSplash(AUTO_LAUNCH_SPLASH_MS, true)) {
    autoLaunchAttempt = 0;
    return;
  }
  autoLaunchAttempt = AUTO_LAUNCH_MAGIC;
  ESP.restart();
}

void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(150);
  Serial.println();
  Serial.println("Cypher Game OS boot");

  auto cfg = M5.config();
  M5Cardputer.begin(cfg, true);
  M5Cardputer.Display.setRotation(1);
  M5Cardputer.Display.setTextDatum(top_left);
  M5Cardputer.Display.setTextWrap(false);

  loadPrefs();
  M5Cardputer.Display.setBrightness(brightness);
  menuInterruptRequested = bootSplash(BOOT_SPLASH_MS, true);

  appPartitionValid = readAppMagic();
  if (appPartitionValid && !hasInstalledName()) {
    copyField(lastMessage, sizeof(lastMessage), "Unknown app in slot");
  }
  mountSd();
  loadCatalog();
  maybeAutoLaunch();
  redraw();
  nextPulseRedraw = millis() + 120;
  printStatus(Serial);
}

void loop() {
  M5Cardputer.update();
  handleInput(readInput());
  handleSerial();
  if (uiFrameReady && millis() >= nextPulseRedraw) {
    redraw();
    nextPulseRedraw = millis() + 120;
  }
  delay(20);
}
