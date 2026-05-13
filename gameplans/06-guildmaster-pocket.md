# Guildmaster Pocket

## Pitch

Guildmaster Pocket is an adventurer guild management sim. The player recruits
adventurers, builds parties, assigns quests, buys gear, treats injuries, grows a
guild hall, and competes with rival guilds. Missions resolve through readable
text simulation rather than real-time combat, which makes the design ideal for
Cardputer menus and long-play strategy.

## Target Experience

- Total playtime target: 5 to 10+ hours per guild.
- Session length: 5 to 20 minutes per quest cycle.
- Tone: strategic, character-driven, dangerous, occasionally silly.
- Player fantasy: "I manage a roster of flawed heroes from a tiny command desk."
- Campaign shape: take low-rank jobs, build reputation, unlock regions, survive
  injuries and deaths, complete boss contracts.
- Hardware constraints: Arduino CLI, M5Cardputer keyboard, 240x135 text UI,
  SD saves, fixed class/quest/town tables, no graphics dependency.

## Core Loop

1. Check guild status: gold, fame, hall level, region rank, roster condition.
2. Review available quests with risk, reward, length, enemy type, and class
   recommendations.
3. Build a party from available adventurers.
4. Equip or rest members.
5. Launch quest and resolve stages with text events and skill checks.
6. Return with gold, loot, injuries, trauma, fame, rival changes, or deaths.
7. Recruit, upgrade hall, unlock harder quests, and pursue boss contracts.

## Controls And UI

- `1`-`5`: choose quest, action, roster slot, or event option.
- `w/s` and `;/.`: scroll roster, party, gear, quest board.
- `Enter`: confirm highlighted selection.
- `Backspace/q`: back out of party selection or menus.
- Main screens: `GUILD`, `QUESTS`, `ROSTER`, `PARTY`, `GEAR`, `RESULT`, `HALL`.
- Show adventurers in compact rows: `Name Cls Lvl HP Stress Trait`.

## Major Systems

- Adventurer roster: up to 18 recruits, 4 per quest. Each has class, level, HP,
  stress, injury, trait, flaw, loyalty, and gear tier.
- Classes: fighter, scout, medic, mage, guard, bard, engineer, mystic. Classes
  provide event bonuses and quest role coverage.
- Traits/flaws: brave, careful, greedy, loyal, reckless, medic-minded, dramatic,
  cowardly, pyromaniac, snack thief. Traits alter risks and rewards.
- Quest board: generated from region, rank, enemy type, length, objective, and
  sponsor faction.
- Expedition simulation: 3 to 7 stages with checks for combat, traps, social
  problems, travel, and boss encounters.
- Gear and hall upgrades: weapons, armor, kits, infirmary, barracks, library,
  blacksmith, map room, trophy wall.
- Rival guilds: steal jobs, underbid contracts, challenge reputation, or help in
  disasters.
- Consequences: injuries, death, retirement, stress breaks, loyalty events, and
  chain quests.

## Progression

Early game has small errands with forgiving injuries. Midgame adds longer quests,
boss contracts, class synergy, rival guild pressure, and expensive upgrades.
Late game demands specialized parties and accepts that some heroes may retire or
die. Endings can reward a beloved guild, ruthless mercenary machine, scholarly
guild, town protector, bankrupt hall, or cursed legacy.

## Data Model

Recommended compiled tables:

- `ClassDef`: id, name, hpBase, attack, defense, skillTags, gearMask.
- `RecruitTemplate`: id, name, classId, traitId, flawId, hireCost.
- `QuestDef`: id, name, rank, region, length, enemyTag, reward, risk, chainId.
- `StageEventDef`: id, tagMask, weight, checkType, textId, effects.
- `UpgradeDef`: id, name, cost, maxLevel, effectId.
- `RivalDef`: id, name, style, aggression, eventMask.

Recommended save fields:

- seed, week, gold, fame, guildRank, hallLevel, gameOver, ending.
- adventurers[18]: templateId, classId, level, xp, hp, stress, injury, trait,
  flaw, gearTier, loyalty, status.
- upgradeLevels[8], regionUnlockMask, rivalRep[3], questChainFlags[3].

## Content Tables

V1 content target:

- 8 classes.
- 48 recruit templates.
- 60 quest templates.
- 64 stage events.
- 8 hall upgrades.
- 3 rival guilds.
- 10 boss contracts.
- 6 endings.

## Events And Encounters

- Trap corridor tests scout or engineer.
- Injured villager tests medic or compassion.
- Treasure room tempts greedy members.
- Ambush tests front-line durability.
- Ancient door tests mage, mystic, or map room upgrade.
- Party argument increases stress unless bard or loyal trait helps.
- Boss contract checks party composition and accumulated chain flags.

## Implementation Plan

1. Create a separate `GuildmasterPocket` sketch.
2. Implement guild overview, roster, quest board, party selection, and saving.
3. Add class and recruit tables.
4. Add generated quest resolution with staged text events.
5. Add XP, injuries, stress, death, and rest.
6. Add gear and hall upgrades.
7. Add rivals and boss contract chains.
8. Balance around 40 to 70 completed quests for a long campaign.

## Testing Checklist

- Compile with Arduino CLI.
- Confirm roster and party screens stay readable.
- Recruit, dismiss, rest, injure, level, and lose adventurers.
- Run quests with empty, partial, and full parties.
- Save/load during normal guild state and after quest results.
- Test every class in at least one beneficial event.
- Simulate 75 quests and verify no roster overflow.
- Reach victory and failure endings.

## Prompt For Next Codex Session

```text
In /Users/cypher/Documents/GitHub/poke-trail, build the first playable
Guildmaster Pocket prototype from docs/gameplans/06-guildmaster-pocket.md as a
separate Arduino CLI Cardputer sketch. Use M5Cardputer input/display, SD saves,
fixed tables, compact roster structs, quest board, party selection, simulated
quest stages, injuries, XP, gear, hall upgrades, and a 240x135 text UI.
```

