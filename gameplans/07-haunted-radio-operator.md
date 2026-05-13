# Haunted Radio Operator

## Pitch

Haunted Radio Operator is a mystery survival game where the Cardputer becomes a
remote radio station. The player tunes fictional frequencies, decodes messages,
manages power, responds to distress calls, logs clues, and survives increasingly
strange supernatural interference. The keyboard makes the fantasy feel native:
the player is literally typing into a small field terminal.

## Target Experience

- Total playtime target: 5 to 10+ hours across one mystery file.
- Session length: 5 to 20 minutes per shift.
- Tone: eerie, quiet, puzzly, tense, not graphics-dependent.
- Player fantasy: "I am the last operator awake, and the radio is lying."
- Campaign shape: survive nightly shifts, solve cases, identify real and fake
  signals, and uncover the source.
- Hardware constraints: Arduino CLI, M5Cardputer keyboard, 240x135 text UI,
  SD saves, offline fictional signals only, no actual radio/network access.

## Core Loop

1. Start a shift and review power, sanity, weather, logs, active cases, and
   station damage.
2. Tune frequency bands or choose from known channels.
3. Receive distorted transmissions, codes, distress calls, number strings, or
   silence.
4. Decode or classify the message using notes, ciphers, maps, and prior clues.
5. Choose a response: relay help, ignore, lie, triangulate, investigate, or
   conserve power.
6. Manage power, generator fuel, interference, fear, and station integrity.
7. End shift, update cases, unlock hidden channels, and move toward an ending.

## Controls And UI

- `1`-`5`: choose action, response, log entry, or decode tool.
- `w/s` and `;/.`: adjust frequency, scroll logs, select cases.
- `Enter`: tune or confirm.
- `Backspace/q`: return to station overview.
- Optional typed commands can be limited to short fake inputs like `LOG`,
  `TUNE`, `PING`, `MAP`, but menus should always be available.
- Main screens: `STATION`, `TUNE`, `SIGNAL`, `LOGS`, `CASES`, `POWER`,
  `DECODE`.

## Major Systems

- Frequency bands: 5 bands with known and hidden channels. Channels unlock via
  case progress, time, and code solutions.
- Signal generator: table-driven transmissions with distortion, reliability,
  source, caseId, clue flags, and false-signal chance.
- Decode tools: Morse-like taps, substitution codes, number stations, reversed
  phrases, grid coordinates, and call-sign matching.
- Power management: generator fuel, battery, antenna draw, floodlights, heater,
  recorder, and emergency beacon.
- Cases: branching mini-mysteries with callers, locations, clues, suspect
  signals, and resolution flags.
- Entity behavior: a fictional presence learns from repeated responses, mimics
  callers, and attacks power or logs.
- Station condition: sanity, fatigue, cold, antenna integrity, door integrity,
  recorder health.
- Endings: truth broadcast, silent evacuation, possessed station, wrong rescue,
  sealed channel, lost operator.

## Progression

Early shifts teach tuning and basic log use. Midgame introduces fake signals,
hidden channels, and cases that cross-reference each other. Late game adds power
scarcity and contradictory transmissions, requiring the player to trust the log
over a single message. The best ending requires solving case links and refusing
the entity's false instructions.

## Data Model

Recommended compiled tables:

- `BandDef`: id, name, minFreq, maxFreq, noise, unlockFlag.
- `SignalDef`: id, band, frequency, caseId, minShift, weight, cipherId, textId,
  truthFlag, falseChance.
- `CaseDef`: id, name, requiredClues, failFlags, resolutionEvent.
- `DecodeToolDef`: id, name, unlockFlag, effectId.
- `EventDef`: id, triggerMask, weight, textId, choices, effects.

Recommended save fields:

- seed, shift, hour, tunedBand, tunedFreq, ending, gameOver.
- power, fuel, battery, sanity, fatigue, cold, antenna, door, recorder.
- caseClues[8], caseState[8], knownChannelsMask, decodeToolMask.
- entityAwareness, falseSignalCount, logFlags[4].

## Content Tables

V1 content target:

- 5 frequency bands.
- 40 signal entries.
- 8 cases.
- 6 decode tools.
- 48 station/entity events.
- 12 hidden channels.
- 6 endings.

## Events And Encounters

- Caller reports a location that does not exist unless a prior clue corrects it.
- Number station repeats coordinates with one digit altered.
- Generator sputters during a critical response.
- Familiar voice mimics an earlier caller but uses the wrong call sign.
- Antenna ice forces a power or repair choice.
- Hidden channel unlocks after three related logs are found.
- Emergency beacon attracts help or the wrong listener depending on flags.

## Implementation Plan

1. Create a separate `HauntedRadioOperator` sketch.
2. Implement station overview, tune screen, signal display, and log screen.
3. Add frequency/channel tables and deterministic signal selection.
4. Add power/fuel/battery drain per shift action.
5. Add case clue flags and decode tools.
6. Add fake-signal/entity behavior.
7. Add branching endings and case resolution.
8. Balance around 18 to 28 shifts per campaign.

## Testing Checklist

- Compile with Arduino CLI.
- Confirm frequency and signal screens fit on 240x135.
- Test tuning with both number menus and cursor aliases.
- Save/load during active shift and between shifts.
- Trigger low power, low sanity, antenna damage, and false signal states.
- Verify each case can be solved from available clues.
- Confirm no mechanics require actual radio, Wi-Fi, Bluetooth, or internet.
- Reach at least three endings.

## Prompt For Next Codex Session

```text
In /Users/cypher/Documents/GitHub/poke-trail, create a separate Arduino CLI
Cardputer prototype for Haunted Radio Operator from
docs/gameplans/07-haunted-radio-operator.md. Use M5Cardputer keyboard/display,
240x135 text screens, SD saves, fixed signal/case/event tables, fictional
offline frequency tuning, power management, logs, decode tools, and branching
mystery cases. Do not use real radio or network access.
```

