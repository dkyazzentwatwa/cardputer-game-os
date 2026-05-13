# Cypher Game OS Manuals

These manuals cover the 14 local deep-v2 games included with Cypher Game OS for the M5Stack Cardputer / Cardputer ADV. Poke-Trail is a sibling app and is not documented in this manual pack.

## Global Controls

| Action | Keys |
|---|---|
| Move up | `W`, `K`, `;`, or `,` |
| Move down | `S`, `J`, `.`, or `/` |
| Change page where available | `A` / `D` or left / right |
| Select | `Enter`, `Space`, or BtnA |
| Pick visible action | `1` through `5` |
| Back | `Backspace`, `Delete`, `Tab`, backtick, or `Q` |
| Return from title screen | `Backspace` or `Q` |

## SD Layout And Saves

Game binaries live under `/cardputer-game-os/apps` on the SD card. The 14 local deep games save under:

```text
/cardputer-game-os/saves/<game-slug>/save.sav
```

Each game also uses temporary and backup save files while writing, so avoid powering off during an active save message.

## Game Manuals

| Game | Slug | Genre | Manual | Save Path | Offline Scope Note |
|---|---|---|---|---|---|
| Star Trader | `star-trader-pocket-frontier` | Ship trading sim | [Manual](star-trader-pocket-frontier.md) | `/cardputer-game-os/saves/star-trader-pocket-frontier/save.sav` | Scanner, customs, pirate, and salvage scenes are fictional table events. |
| Dungeon Courier | `dungeon-courier` | Delivery RPG | [Manual](dungeon-courier.md) | `/cardputer-game-os/saves/dungeon-courier/save.sav` | Routes, packages, curses, and hazards are fictional gameplay. |
| Tiny Wasteland | `tiny-wasteland` | Survival road drama | [Manual](tiny-wasteland.md) | `/cardputer-game-os/saves/tiny-wasteland/save.sav` | Signal references are narrative only. |
| Pocket Kingdom | `pocket-kingdom-manager` | Kingdom ledger sim | [Manual](pocket-kingdom-manager.md) | `/cardputer-game-os/saves/pocket-kingdom-manager/save.sav` | Politics, raids, faith, magic, and crises are fictional simulation tables. |
| Monster Ranch | `monster-ranch-trail` | Creature ranch sim | [Manual](monster-ranch-trail.md) | `/cardputer-game-os/saves/monster-ranch-trail/save.sav` | Creature care is fictional game content, not real animal guidance. |
| Guildmaster | `guildmaster-pocket` | Quest desk sim | [Manual](guildmaster-pocket.md) | `/cardputer-game-os/saves/guildmaster-pocket/save.sav` | Fantasy quest and signal-ruin language is fictional. |
| Haunted Radio | `haunted-radio-operator` | Station mystery | [Manual](haunted-radio-operator.md) | `/cardputer-game-os/saves/haunted-radio-operator/save.sav` | Tuning, bands, calls, and decoding are fictional offline systems. |
| Cyberdeck RPG | `cyberdeck-hacker-rpg` | Fictional node-crawl | [Manual](cyberdeck-hacker-rpg.md) | `/cardputer-game-os/saves/cyberdeck-hacker-rpg/save.sav` | Nodes, scans, injects, traces, and files are compiled fiction. |
| Pocket Detective | `pocket-detective-agency` | Notebook mystery | [Manual](pocket-detective-agency.md) | `/cardputer-game-os/saves/pocket-detective-agency/save.sav` | Cases and investigations are fictional offline puzzles. |
| Cryptid Ranger | `cryptid-park-ranger` | Field research sim | [Manual](cryptid-park-ranger.md) | `/cardputer-game-os/saves/cryptid-park-ranger/save.sav` | Evidence, reports, radio, and field work are fictional game systems. |
| Cyber Ranger | `cyber-ranger` | Cyber-wilderness mystery | [Manual](cyber-ranger.md) | `/cardputer-game-os/saves/cyber-ranger/save.sav` | Signals and deck tools never touch real devices or networks. |
| Star Trail Ranch | `star-trail-rancher` | Space creature transport | [Manual](star-trail-rancher.md) | `/cardputer-game-os/saves/star-trail-rancher/save.sav` | Transport, quarantine, and care are fictional offline systems. |
| Waste Guild | `wasteland-guildmaster` | Settlement command sim | [Manual](wasteland-guildmaster.md) | `/cardputer-game-os/saves/wasteland-guildmaster/save.sav` | Survival, medical, radio, and defense content is fictional. |
| Signal Rat | `signal-rat-cyberdeck-rpg` | Flagship cyberdeck RPG | [Manual](signal-rat-cyberdeck-rpg.md) | `/cardputer-game-os/saves/signal-rat-cyberdeck-rpg/save.sav` | Every contract, node, trace, signal, and file is compiled fiction. |

## Manual Conventions

- Screen names are written exactly as they appear in the game data.
- Resource abbreviations match the compiled tables.
- A "run" means one saved campaign for that game.
- Progress generally advances by taking meaningful actions from a game screen.
- Endings are influenced by progress, resources, flags, and the last major choices.

## Fictional Offline Scope

Cypher Game OS games are offline tabletop-like simulations running from compiled data tables. Cyber, radio, signal, survival, investigation, creature-care, and settlement systems are entertainment mechanics only. They do not provide real networking, radio reception, scanning, credential access, GPS tracking, medical advice, legal advice, tactical guidance, emergency guidance, animal care, or internet services.

## Maintainer Checklist

When adding or revising a manual, include controls, save path, resource table, screen guide, core loop, first-run tips, endings, and a clear fictional/offline scope note.
