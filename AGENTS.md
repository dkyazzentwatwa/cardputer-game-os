# Repository Guidelines

This repo is Arduino CLI only. Do not add PlatformIO, ESP-IDF project files, online OTA catalogs, or multi-board ports unless the user explicitly changes scope.

## Commands

Build the launcher:

```bash
arduino-cli compile --profile adv /Users/cypher/Documents/GitHub/cardputer-game-os
# or
/Users/cypher/Documents/GitHub/cardputer-game-os/tools/build-launcher.sh
```

Build and package every game app:

```bash
/Users/cypher/Documents/GitHub/cardputer-game-os/tools/build-games.sh
/Users/cypher/Documents/GitHub/cardputer-game-os/tools/package-sd.sh
```

Flash the launcher with the touch-first Cardputer flow:

```bash
/Users/cypher/Documents/GitHub/cardputer-game-os/tools/flash-launcher.sh /dev/cu.usbmodemXXXX
```

## Scope

- Target hardware: M5Stack Cardputer / Cardputer ADV.
- Supported launch model: SD catalog installs game sketch .bin files into the app partition, then reboots.
- SD root: /cardputer-game-os.
- Save root: /cardputer-game-os/saves.
- Keep all cyber/radio/signal mechanics fictional and offline. No real network, Wi-Fi, Bluetooth, radio scanning, credentials, or internet calls in games.
