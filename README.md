# Cypher Game OS

Arduino CLI-only game launcher and 15-game pack for the M5Stack Cardputer ADV.

Cypher Game OS keeps a small launcher resident on the device. Game .bin files live on a FAT32 SD card under /cardputer-game-os/apps. The launcher copies the selected game into the app OTA partition and reboots into it. Games can return to the launcher from their title screen.

## Hardware

- M5Stack Cardputer / Cardputer ADV
- FAT32 microSD card, preferably 32 GB or smaller
- Arduino CLI with the M5Stack ESP32 board package

## Build The Launcher

```bash
cd /Users/cypher/Documents/GitHub/cardputer-game-os
arduino-cli compile --profile adv .
```

Or:

```bash
./tools/build-launcher.sh
```

## Flash The Launcher

Use the touch-first Cardputer flow:

```bash
arduino-cli board list
./tools/flash-launcher.sh /dev/cu.usbmodemXXXX
```

If the port is omitted, the script uses the first /dev/cu.usbmodem* port it finds, touches it at 1200 baud, waits briefly, then uploads with the adv profile.

## Build Games For SD

```bash
./tools/build-games.sh
./tools/package-sd.sh
```

Copy the contents of:

```text
/Users/cypher/Documents/GitHub/cardputer-game-os/dist/sd-card
```

to the root of a FAT32 SD card.

Expected SD layout:

```text
/cardputer-game-os/apps/games.json
/cardputer-game-os/apps/signal-rat-cyberdeck-rpg.bin
/cardputer-game-os/apps/cryptid-park-ranger.bin
...
/cardputer-game-os/saves/<game-slug>/save.sav
```

## Controls

Launcher:

- , or ;: up
- . or /: down
- W/S and K/J: up/down alternatives
- Enter or BtnA: select
- Backtick, Delete, Tab, or Q: back
- R: reload SD catalog

Games:

- 1-5: choose visible actions
- W/S or ;/.: move
- Enter: select
- Backspace/Delete/Q: back
- On each game title screen, Backspace/Q returns to the launcher

## Included Games

Comprehensive game manuals live in [manuals/README.md](manuals/README.md).

- Star Trader: Deep ship-console trading sim with markets, routes, cargo, crew, upgrades, and route events.
- Dungeon Courier: Deep contract delivery RPG with town boards, package integrity, supplies, route choices, and perks.
- Tiny Wasteland: Deep survival road drama with regions, camp care, scavenging risk, party state, trade, and vehicle pressure.
- Pocket Kingdom: Deep seasonal kingdom ledger with workers, buildings, laws, advisors, crises, and yearly reports.
- Monster Ranch: Deep weekly ranch sim with creature care, training, expeditions, facilities, tournaments, and ranch events.
- Guildmaster: Deep quest desk sim with generated contracts, class parties, staged results, hall upgrades, and rivals.
- Haunted Radio: Deep fictional station mystery with tuning, fake signals, decode tools, cases, logs, and power routing.
- Cyberdeck RPG: Deep offline fake node-crawl with job board, simulated nodes, tools, recovered files, market, and reps.
- Pocket Detective: Deep notebook mystery game with cases, maps, interviews, clue book, clue comparison, and accusations.
- Cryptid Ranger: Deep ethical field research game with zones, gear, patrols, evidence tracking, guide entries, and reports.
- Cyber Ranger: Deep fictional cyber-wilderness investigation with park zones, fake anomaly boards, deck commands, guide, cases, and gear.
- Star Trail Ranch: Deep space creature transport sim with contracts, habitats, route decay, market supplies, and discovery logs.
- Waste Guild: Deep settlement command sim with base pressure, expeditions, team roles, staged results, facilities, and threats.
- Signal Rat: The flagship fictional cyberdeck RPG with safehouse, faction jobs, simulated nodes, recovered files, and deck upgrades.
- Poke-Trail: Original creature trail RPG with travel, battles, catching, audio, and region progression. Saves live at `/poke-trail/save.sav`.

## Serial Commands

```text
help
status
games
reload
launch
erase
install <slug>
```

## Notes

- Use sketch app .bin files, not merged flash images.
- The launcher rejects oversized bins and merged images before writing.
- All signal, radio, hacking, investigation, and scanning mechanics are fictional offline game systems.
- The 14 local games use compiled deep content tables and SD-backed save files under `/cardputer-game-os/saves/<game-slug>/save.sav`.
- The design briefs in gameplans/ are preserved as the source gameplan pack.
