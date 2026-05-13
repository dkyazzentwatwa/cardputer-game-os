# Cyberdeck Hacker RPG

## Pitch

Cyberdeck Hacker RPG turns the Cardputer into a fictional cyberdeck for a
cyberpunk contract game. The player accepts jobs, scans simulated networks, runs
fake programs, manages heat and trace pressure, upgrades deck modules, and
uncovers faction conspiracies. It is a perfect hardware fantasy because typing
and reading logs on the Cardputer feels like the game world.

## Target Experience

- Total playtime target: 5 to 10+ hours per hacker career.
- Session length: 5 to 20 minutes per job.
- Tone: neon noir, underground, tense, stylish, fictional.
- Player fantasy: "This little device is my deck."
- Safety boundary: all hacking is fake, offline, table-driven, and never touches
  real networks, credentials, Wi-Fi targets, Bluetooth targets, or devices.
- Hardware constraints: Arduino CLI, M5Cardputer keyboard, 240x135 text UI,
  SD saves, fixed job/tool/faction tables.

## Core Loop

1. Check safehouse status: credits, heat, deck condition, tools, reputation.
2. Choose a contract from clients and factions.
3. Enter a simulated target node.
4. Use fake commands or menus: `scan`, `spoof`, `decrypt`, `inject`, `trace`,
   `disconnect`.
5. Trade time and heat for data, money, secrets, or clean exit.
6. Resolve trace consequences, faction changes, and story flags.
7. Upgrade deck modules and unlock tougher jobs.

## Controls And UI

- `1`-`5`: choose menu actions, contract rows, or command shortcuts.
- `w/s` and `;/.`: scroll job board, tool list, logs, and black market.
- `Enter`: run highlighted command.
- `Backspace/q`: back or disconnect prompt.
- Optional command input should accept short fake commands but mirror them to
  menu choices for reliability.
- Main screens: `SAFEHOUSE`, `JOBS`, `NODE`, `TOOLS`, `LOG`, `MARKET`, `REP`.

## Major Systems

- Job board: contracts with client, target type, payout, heat, required tool,
  story flag, and faction effect.
- Simulated nodes: fake systems with shield, lock, trace rate, data value,
  alarm threshold, and clue flags.
- Tools: scanner, spoofer, decryptor, injector, firewall, scrubber, sniffer,
  ghost module. Tools have levels and cooldowns.
- Heat and trace: actions increase trace; leaving early saves the run but may
  reduce payout. High heat changes job availability and ending paths.
- Deck upgrades: CPU, memory, battery, stealth, cooling, storage, exploit kit,
  trace shield.
- Factions: street clients, corporate handlers, signal cult, civic netwatch.
  Reputation determines job types and black market prices.
- Story missions: a conspiracy arc told through recovered files, fake emails,
  corrupted logs, and recurring clients.
- Consequences: busted deck, debt, locked safehouse, faction betrayal, or clean
  ghost reputation.

## Progression

Early jobs are short tutorial nodes with low trace. Midgame forces tool choices
and faction commitments. Late game introduces multi-stage jobs where the player
must decide whether to chase story files or leave with the payout. Endings
reflect identity: ethical leaker, paid ghost, corporate asset, burned runner,
signal believer, or vanished legend.

## Data Model

Recommended compiled tables:

- `JobDef`: id, name, clientFaction, targetType, rank, payout, heat, requiredTool,
  storyFlag, nodeId.
- `NodeDef`: id, name, shield, lock, traceRate, alarm, dataValue, clueFlag.
- `ToolDef`: id, name, command, basePower, cooldown, upgradeCost.
- `UpgradeDef`: id, name, cost, maxRank, effectId.
- `FactionDef`: id, name, marketBias, heatBias, endingFlag.
- `FileDef`: id, title, unlockFlag, textId, conspiracyStep.

Recommended save fields:

- seed, day, credits, heat, debt, deckHp, battery, currentJob, currentNode.
- toolLevels[8], upgradeRanks[8], factionRep[4].
- storyFlags[3], recoveredFilesMask, blackMarketUnlocks, ending.

## Content Tables

V1 content target:

- 32 jobs.
- 20 simulated nodes.
- 8 tools.
- 8 deck upgrades.
- 4 factions.
- 24 recovered files.
- 12 black market offers.
- 6 endings.

## Events And Encounters

- Client offers bonus for stealing an extra file.
- Trace spike forces a choice between scrubber, disconnect, or risk.
- Fake admin login is a trap unless spoofer is upgraded.
- Corporate handler pays well but raises civic heat.
- Signal cult job unlocks strange corrupted logs.
- Deck overheats and disables one tool for a day.
- Rival hacker leaks your handle unless paid or exposed.

## Implementation Plan

1. Create a separate `CyberdeckRPG` sketch.
2. Implement safehouse, job board, node screen, tool list, and save/load.
3. Add short command aliases backed by the same action enum as menus.
4. Add job resolution with node stats, trace, heat, and payout.
5. Add tool upgrades and deck upgrades.
6. Add faction reputation and black market.
7. Add recovered file story flags and endings.
8. Balance a full career around 25 to 45 completed jobs.

## Testing Checklist

- Compile with Arduino CLI.
- Verify all command names and logs fit on 240x135.
- Test menu-only play with no typed commands.
- Test typed fake commands if implemented.
- Save/load before job, during job, after disconnect, and after upgrade.
- Confirm no code scans, attacks, or touches real networks or credentials.
- Trigger low deck HP, high heat, failed trace, and clean success.
- Reach at least three endings.

## Prompt For Next Codex Session

```text
In /Users/cypher/Documents/GitHub/poke-trail, build a separate Arduino CLI
Cardputer prototype for Cyberdeck Hacker RPG from
docs/gameplans/08-cyberdeck-hacker-rpg.md. Use M5Cardputer display/input, a
240x135 fake terminal/menu UI, SD saves, fixed tables, fictional offline jobs,
fake scan/spoof/decrypt/inject/trace/disconnect actions, heat, tools, deck
upgrades, factions, and story files. Do not implement any real hacking,
network scanning, Wi-Fi targeting, credential handling, or internet features.
```

