# Cardputer Gameplan Pack

This folder contains the source design briefs for the 14 local deep-game apps.
The first playable implementations now live under `games/`, and each plan still
serves as the expansion brief for future content passes. Each plan assumes the
same practical target as Poke-Trail:

- M5Stack Cardputer / Cardputer-Adv.
- Arduino CLI only.
- 240x135 display with compact text-first screens.
- M5Cardputer keyboard input with number keys, `w/s`, `;/.`, `Enter`, and
  `Backspace/q`.
- Offline gameplay with fake/simulated systems only.
- SD-card save/load support.
- Fixed compiled C++ tables for content, plus compact save structs.

The files are meant to be handed to another Codex session one at a time. They
favor concrete first-build scope, data shapes, content counts, and acceptance
tests over vague brainstorming. The implemented games now use
`CardputerGameCore::DeepGameRuntime` with compiled content tables, compact
state, and SD-backed saves.

## Deep Runtime Status

All 14 local plans have a first deep playable pass in this repo:

- Each game has bespoke screens and content tables in its own `games/<slug>/`
  folder.
- The apps remain Arduino CLI-only and launch through the SD catalog.
- Cyber, radio, signal, and investigation mechanics are fictional offline table
  systems only.
- Poke-Trail remains a sibling app and is not part of this local deep-runtime
  conversion.

## Recommended Next Expansions

| Rank | Gameplan | Fit | Difficulty | Reuse From Poke-Trail |
|---:|---|---|---|---|
| 1 | [Signal Rat: Cyberdeck RPG](14-signal-rat-cyberdeck-rpg.md) | Best hardware fantasy | Medium | Menus, saves, events |
| 2 | [Cryptid Park Ranger](10-cryptid-park-ranger.md) | Best Poke-Trail cousin | Medium | Regions, creature tables |
| 3 | [Star Trader: Pocket Frontier](01-star-trader-pocket-frontier.md) | Best systems depth | Medium-high | Travel, events, resources |
| 4 | [Guildmaster Pocket](06-guildmaster-pocket.md) | Strong long-play sim | Medium | Tables, roster logic |
| 5 | [Tiny Wasteland](03-tiny-wasteland.md) | Strong survival drama | Medium | Trail loop, resources |

## Full Index

1. [Star Trader: Pocket Frontier](01-star-trader-pocket-frontier.md)
2. [Dungeon Courier](02-dungeon-courier.md)
3. [Tiny Wasteland](03-tiny-wasteland.md)
4. [Pocket Kingdom Manager](04-pocket-kingdom-manager.md)
5. [Monster Ranch Trail](05-monster-ranch-trail.md)
6. [Guildmaster Pocket](06-guildmaster-pocket.md)
7. [Haunted Radio Operator](07-haunted-radio-operator.md)
8. [Cyberdeck Hacker RPG](08-cyberdeck-hacker-rpg.md)
9. [Pocket Detective Agency](09-pocket-detective-agency.md)
10. [Cryptid Park Ranger](10-cryptid-park-ranger.md)
11. [Cyber Ranger](11-cyber-ranger.md)
12. [Star Trail Rancher](12-star-trail-rancher.md)
13. [Wasteland Guildmaster](13-wasteland-guildmaster.md)
14. [Signal Rat: Cyberdeck RPG](14-signal-rat-cyberdeck-rpg.md)

## Shared Implementation Defaults

- Start as a separate sketch unless the next session explicitly chooses to
  modify Poke-Trail in place.
- Keep UI text short enough for the 240x135 screen.
- Prefer numbered menus over cursor-heavy interfaces.
- Store long descriptions in compiled lookup tables, not in save files.
- Save only IDs, counters, resource values, flags, and compact state.
- Keep all hacking, radio, signal, and investigation mechanics fictional and
  offline.
- Use deterministic random generation from a saved seed so long runs can be
  resumed cleanly.
