# Cyber Ranger

## Pitch

Cyber Ranger merges the cryptid field-research game with a fictional cyberdeck
toolkit. The player is a park ranger investigating strange creatures and illegal
corporate infrastructure by decoding signals, tracking tagged wildlife, reading
sensor logs, and triangulating anomalies. It uses hacking as a fictional
tracking and investigation layer, not real network activity.

## Target Experience

- Total playtime target: 5 to 10+ hours per investigation.
- Session length: 5 to 20 minutes per patrol or signal case.
- Tone: wilderness mystery with cyberpunk field tools.
- Player fantasy: "I am a ranger with a pocket signal deck."
- Safety boundary: all scanning, decoding, and signal work is fictional,
  offline, and table-driven.
- Hardware constraints: Arduino CLI, M5Cardputer keyboard, 240x135 text UI,
  SD saves, fixed park/cryptid/signal tables.

## Core Loop

1. Review station reports, gear, signal map, supplies, and active cryptid leads.
2. Choose patrol zone or signal anomaly.
3. Pack survival gear and deck tools.
4. Travel through zone events, collect evidence, and tune fictional signal
   traces.
5. Use fake commands like `scan`, `map`, `tag`, `decode`, and `log`.
6. Resolve whether the anomaly is cryptid behavior, weather, broken equipment,
   or corporate interference.
7. Return to station, update field guide and conspiracy file, unlock tools.

## Controls And UI

- `1`-`5`: choose zone, action, tool, or event response.
- `w/s` and `;/.`: scroll reports, field guide, signal list, and gear.
- `Enter`: confirm highlighted choice or run fake command.
- `Backspace/q`: back, cancel, or retreat.
- Main screens: `STATION`, `PARK`, `SIGNAL`, `DECK`, `GUIDE`, `CASE`, `GEAR`.
- Provide menu shortcuts for all fake commands so typed input is optional.

## Major Systems

- Dual evidence: biological evidence and signal evidence both contribute to
  case progress.
- Park zones: same structure as Cryptid Park Ranger, but each zone also has
  relay towers, sensor ghosts, and interference level.
- Deck tools: scanner, mapper, decoder, tag reader, jammer shield, log scrubber,
  battery saver, signal compass.
- Signal anomalies: fictional pings, corrupted tags, weather echoes, illegal
  sensors, and cryptid bioelectric traces.
- Cryptids: creatures have ecology and signal signatures.
- Corporate conspiracy: flags reveal why sensors are hidden in the preserve.
- Survival: stamina, supplies, exposure, injury, deck battery, heat from using
  suspicious tools near corporate zones.
- Reputation: park service, scientists, local town, corporate sponsor.

## Progression

Early game starts like a ranger patrol with simple signal hints. Midgame adds
hidden relays, corrupted tags, and sponsor pressure. Late game asks whether to
publish evidence, protect cryptid habitats, expose corporate experiments, or
bury the truth to keep the park funded. The best ending requires both field
guide evidence and signal-chain evidence.

## Data Model

Recommended compiled tables:

- `ZoneDef`: id, name, terrain, danger, cryptidMask, relayMask, interference.
- `CryptidDef`: id, name, zoneMask, evidenceRequired, signalSignature,
  temperament.
- `SignalDef`: id, zoneId, type, strength, falseChance, unlockFlag, clueFlag.
- `DeckToolDef`: id, name, command, batteryCost, effectId, upgradeCost.
- `CaseDef`: id, name, bioClueMask, signalClueMask, resolutionFlag.
- `EventDef`: id, zoneMask, signalMask, weight, textId, effects.

Recommended save fields:

- seed, day, currentZone, supplies, stamina, exposure, injury, deckBattery,
  suspicion.
- evidence[cryptidCount], signalClues[16], caseState[8].
- toolLevels[8], zoneUnlockMask, reputation[4], conspiracyFlags[3], ending.

## Content Tables

V1 content target:

- 8 park zones.
- 20 cryptids.
- 32 signal anomalies.
- 8 deck tools.
- 8 investigation cases.
- 56 patrol/signal events.
- 4 reputation groups.
- 6 endings.

## Events And Encounters

- Cryptid track crosses a buried sensor line.
- Fake distress beacon lures the ranger away from real evidence.
- Signal compass points to a storm echo unless decoder is upgraded.
- Corporate drone log mentions a species not in the field guide.
- Tag reader finds two animals with the same impossible ID.
- Deck battery fails during a night patrol.
- Scientist asks for raw data while town wants proof buried.

## Implementation Plan

1. Create a separate `CyberRanger` sketch.
2. Implement station, park zone selection, gear, signal screen, and saving.
3. Add field evidence and signal evidence as two compact bitmasks.
4. Add fake deck commands through a menu-backed action enum.
5. Add patrol events and signal anomalies.
6. Add cryptid guide and investigation case progress.
7. Add corporate conspiracy flags and endings.
8. Balance around 35 to 60 patrols plus 8 cases.

## Testing Checklist

- Compile with Arduino CLI.
- Confirm all fake command actions are playable without typed input.
- Verify no real network, Wi-Fi, Bluetooth, or radio scanning is used.
- Save/load evidence, signal clues, tool levels, and current cases.
- Test low supplies, low deck battery, high suspicion, and injury.
- Solve each case from available bio and signal clues.
- Reach corporate, park, science, town, and failure endings.

## Prompt For Next Codex Session

```text
In /Users/cypher/Documents/GitHub/poke-trail, create a separate Arduino CLI
Cardputer prototype for Cyber Ranger from docs/gameplans/11-cyber-ranger.md.
Use M5Cardputer display/input, 240x135 text UI, SD saves, fixed tables,
cryptid tracking, fictional offline signal tools, fake scan/map/tag/decode/log
actions, deck battery, park survival, field guide evidence, and conspiracy
case progress. Do not use real network or radio features.
```

