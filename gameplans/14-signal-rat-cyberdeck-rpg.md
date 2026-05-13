# Signal Rat: Cyberdeck RPG

## Pitch

Signal Rat is the most implementation-ready cyberdeck game concept in this
pack. The player is a broke freelance hacker with a pocket deck, taking sketchy
fictional jobs from underground clients. The Cardputer becomes the prop: a tiny
terminal where the player scans simulated nodes, decrypts files, manages heat,
upgrades modules, and uncovers ghost signals from abandoned infrastructure.

Everything is fictional and offline. The game never scans, attacks, connects to,
or stores credentials for real networks or devices.

## Target Experience

- Total playtime target: 5 to 10+ hours for one full conspiracy run.
- First playable target: 45 to 90 minutes.
- Session length: 5 to 15 minutes per job.
- Tone: grimy cyberpunk, weird signal horror, practical hustling.
- Player fantasy: "This Cardputer is my deck, job board, terminal, and save
  file."
- Hardware constraints: Arduino CLI, M5Cardputer keyboard, 240x135 text UI,
  SD save/load, fixed compiled tables, compact save state, no internet.

## Core Loop

1. Open the safehouse dashboard.
2. Check credits, heat, deck HP, battery, tools, faction reputation, and active
   story lead.
3. Pick one of up to 3 visible jobs from the board.
4. Enter a simulated node with shield, lock, trace, alarm, and data values.
5. Run fake commands: `scan`, `decrypt`, `spoof`, `inject`, `trace`, and
   `disconnect`.
6. Choose whether to cash out safely, chase optional files, or risk trace.
7. Resolve payout, heat, reputation, story flags, deck damage, and autosave.
8. Buy upgrades and take harder jobs until one ending triggers.

## Controls And UI

- `1`-`5`: choose dashboard actions, jobs, fake commands, and event choices.
- `w/s` and `;/.`: move through job board, log, upgrade list, and faction view.
- `Enter`: confirm highlighted item.
- `Backspace/q`: return or prompt to disconnect during a node.
- Typed command mode is optional. If implemented, it should map typed commands
  to the same action enum as menu choices.
- Main screens for v1:
  - `SAFEHOUSE`: status and main menu.
  - `JOBS`: 3 generated/available contracts.
  - `NODE`: current simulated target and commands.
  - `LOG`: recent results and recovered story files.
  - `UPGRADES`: deck module purchases.
  - `REP`: faction reputation and heat notes.

## Major Systems

- Job board: exactly 12 starter jobs for v1. Each job has client faction,
  target node, payout, base heat, required story step, and optional file.
- Fake node simulation: each node tracks `shield`, `lock`, `trace`, `alarm`,
  `data`, and `ghost`. These are just numbers in a table, not real systems.
- Command set:
  - `scan`: reveals one hidden node stat, low trace gain.
  - `decrypt`: reduces lock or extracts file if lock is low enough.
  - `spoof`: reduces trace or bypasses shield if upgraded.
  - `inject`: extracts payout/data but raises trace and alarm.
  - `trace`: checks how close the player is to being burned.
  - `disconnect`: exits and resolves partial or full job outcome.
- Heat/trace: trace is per-job pressure; heat is campaign pressure. Failed jobs
  add heat, and high heat changes board options and endings.
- Three factions:
  - `Streetline`: underground clients, lower pay, lower heat.
  - `Neon Crown`: corporate brokers, high pay, high reputation risk.
  - `Ghost Relay`: strange signal seekers tied to the conspiracy arc.
- Four deck upgrades:
  - `Cold CPU`: lowers trace gain.
  - `Spoof Chip`: improves `spoof` and unlocks shield bypass.
  - `Battery Stack`: adds job action buffer and reduces disconnect failures.
  - `Blackbox Drive`: stores optional files and unlocks story endings.
- Conspiracy arc: recovered files point to ghost signals coming from abandoned
  infrastructure. The arc should be one compact chain, not a sprawling story.
- Failure states: debt lock, deck burnout, faction betrayal, or heat collapse.

## Progression

Early jobs teach each command and pay enough for the first upgrade. Midgame asks
the player to choose between safe Streetline work, lucrative Neon Crown work, or
weird Ghost Relay leads. Late game unlocks three story jobs that expose the
source of the ghost signal. A careful player can finish cleanly; a greedy player
can win rich but burned; a curious player can uncover the true signal ending.

Recommended pacing:

- Jobs 1 to 3: tutorial rank.
- Jobs 4 to 8: faction identity and first hard traces.
- Jobs 9 to 12: high-risk jobs, optional files, story gate.
- Final arc: 3 chained jobs unlocked by recovered files and Blackbox Drive.

## Data Model

Recommended compiled tables:

- `FactionDef`: id, name, shortName, marketBias, heatBias, endingFlag.
- `NodeDef`: id, name, shield, lock, traceRate, alarmLimit, dataValue,
  ghostLevel.
- `JobDef`: id, name, factionId, nodeId, rank, payout, heat, requiredFlag,
  optionalFileId, successFlag.
- `ToolDef`: id, command, label, baseEffect, traceCost, upgradeId.
- `UpgradeDef`: id, name, cost, maxRank, effectId.
- `FileDef`: id, title, storyStep, unlockFlag, shortTextId.
- `EventDef`: id, trigger, textId, choiceEffects.

Recommended runtime structs:

```cpp
struct SignalRatState {
  uint32_t seed;
  uint16_t day;
  int16_t credits;
  uint8_t heat;
  uint8_t deckHp;
  uint8_t battery;
  int8_t factionRep[3];
  uint8_t upgradeRank[4];
  uint16_t completedJobsMask;
  uint16_t recoveredFilesMask;
  uint32_t storyFlags;
  uint8_t currentJobId;
  uint8_t currentNodeId;
  uint8_t nodeTrace;
  uint8_t nodeAlarm;
  bool inJob;
  bool gameOver;
  uint8_t ending;
};
```

Recommended save fields:

- `schema`, `seed`, `day`, `credits`, `heat`, `deckHp`, `battery`.
- `rep=street,neon,ghost`.
- `upgrades=cpu,spoof,battery,blackbox`.
- `jobsMask`, `filesMask`, `storyFlags`.
- `currentJob`, `currentNode`, `nodeTrace`, `nodeAlarm`, `inJob`.
- `ending`, `crc`.

## Content Tables

V1 content target:

- 3 factions.
- 12 starter jobs.
- 8 simulated nodes.
- 6 fake commands.
- 4 deck upgrades with 2 ranks each.
- 10 recovered files.
- 12 normal node events.
- 6 heat/faction events.
- 3 final story jobs.
- 6 endings.

Starter job sketch:

| Job | Faction | Purpose |
|---|---|---|
| Rat Trap Receipt | Streetline | Tutorial scan/decrypt |
| Laundromat Ledger | Streetline | First optional file |
| Crown Demo Cache | Neon Crown | Higher pay, higher heat |
| Relay 17 Echo | Ghost Relay | First ghost clue |
| Clinic Queue Patch | Streetline | Ethical low-pay job |
| Executive Calendar | Neon Crown | Spoof tutorial |
| Dead Mall Repeater | Ghost Relay | Ghost node event |
| Servo Pawn Audit | Streetline | Inject risk |
| Private Weather Feed | Neon Crown | Trace pressure |
| Underpass Choir | Ghost Relay | Requires file clue |
| Vault Without Walls | Neon Crown | Hard lock job |
| The Old Signal | Ghost Relay | Story gate |

## Events And Encounters

- Trace spike: alarm jumps unless `spoof` succeeds.
- Ghost echo: recovered file appears, but heat rises if Blackbox Drive is not
  installed.
- Client changes terms mid-job: disconnect for partial pay or continue for rep.
- Deck heat: lose deck HP unless Cold CPU is upgraded.
- False root: `inject` gives money but blocks optional file.
- Civic sweep: high campaign heat removes one job from board.
- Relay whisper: story flag unlocks final arc after enough files.

## Implementation Plan

1. Create a separate `SignalRat` sketch folder.
2. Port the known-good Poke-Trail style for display init, input events, dirty
   redraws, SD init, save temp/backup flow, and compact screens.
3. Implement `SAFEHOUSE`, `JOBS`, `NODE`, `LOG`, `UPGRADES`, and `REP` screens.
4. Add `SignalRatState` and text save/load at `/signal-rat/save.sav`.
5. Add fixed faction, node, job, tool, upgrade, and file tables.
6. Implement job start, command actions, trace/alarm math, disconnect, success,
   fail, payout, heat, and autosave.
7. Add 12 starter jobs and 8 nodes.
8. Add upgrade purchases and rank effects.
9. Add recovered files and simple story flags.
10. Add final 3-job conspiracy chain and endings.
11. Balance until a normal win takes about 25 to 40 completed jobs including
    repeats or optional jobs.

## Testing Checklist

- Compile with Arduino CLI for the M5Stack Cardputer FQBN.
- Confirm all screens fit on 240x135 with no important wrapped labels.
- Confirm `1`-`5`, `w/s`, `;/.`, `Enter`, `Backspace`, and `q` work on every
  screen.
- Start and finish all 12 starter jobs.
- Test each fake command on at least 3 node types.
- Verify trace, alarm, heat, deck HP, battery, payout, and reputation stay in
  valid ranges.
- Buy each upgrade and verify its effect.
- Save/load from safehouse and while inside a job.
- Reboot/load after a failed job and after a recovered file.
- Confirm no code performs real network scans, Wi-Fi attacks, Bluetooth scans,
  credential handling, or internet calls.
- Reach clean ghost ending, rich corporate ending, street legend ending, and
  heat collapse ending.

## Prompt For Next Codex Session

```text
In /Users/cypher/Documents/GitHub/poke-trail, implement Signal Rat as a
separate Arduino CLI Cardputer sketch using
docs/gameplans/14-signal-rat-cyberdeck-rpg.md as the source of truth. Reuse the
Poke-Trail style for M5Cardputer display, keyboard input, dirty redraws, SD
save/load with temp/backup files, fixed C++ content tables, and compact
240x135 screens. Build the v1 scope: safehouse, job board, node screen, fake
commands scan/decrypt/spoof/inject/trace/disconnect, heat/trace, 3 factions,
12 starter jobs, 8 simulated nodes, 4 upgrades, recovered files, autosave, and
one conspiracy arc. All hacking must be fictional and offline; do not implement
real network, Wi-Fi, Bluetooth, credential, or internet functionality.
```

