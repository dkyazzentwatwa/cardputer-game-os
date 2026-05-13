# Wasteland Guildmaster

## Pitch

Wasteland Guildmaster blends scavenger expedition management with settlement
survival. The player runs a small post-collapse base, recruits scavengers,
builds teams, sends them into ruins, treats wounds, expands facilities, and
defends the settlement. It takes the best parts of Guildmaster Pocket and Tiny
Wasteland, but anchors them in a visible home base.

## Target Experience

- Total playtime target: 5 to 10+ hours per settlement.
- Session length: 5 to 20 minutes per expedition cycle.
- Tone: gritty survival management with tense choices.
- Player fantasy: "I am running the last useful outpost."
- Campaign shape: grow from a scrap camp to a defended settlement while
  unlocking regions and deciding what kind of society survives.
- Hardware constraints: Arduino CLI, M5Cardputer keyboard, 240x135 text UI,
  SD saves, fixed survivor/mission/facility tables, offline simulation.

## Core Loop

1. Review settlement: food, water, meds, ammo, scrap, power, morale, defense,
   population.
2. Check available expeditions and threats.
3. Build a scavenger team and assign gear.
4. Resolve expedition stages with loot, wounds, radiation, faction contact, and
   moral choices.
5. Return, treat survivors, distribute supplies, and update base facilities.
6. Handle settlement events or attacks.
7. Unlock regions, recruit specialists, and pursue an endgame survival plan.

## Controls And UI

- `1`-`5`: choose base action, mission, team slot, or event response.
- `w/s` and `;/.`: scroll survivors, facilities, missions, and inventory.
- `Enter`: confirm highlighted selection.
- `Backspace/q`: return to base overview.
- Main screens: `BASE`, `MISSIONS`, `TEAM`, `ROSTER`, `GEAR`, `RESULT`,
  `BUILD`.
- Compact survivor rows: `Name Role Lvl HP Rad Stress Trait`.

## Major Systems

- Settlement resources: food, water, meds, ammo, scrap, power, morale, defense,
  population, radiation.
- Survivor roster: up to 20 survivors with role, health, radiation, stress,
  injury, loyalty, trait, and skill.
- Expedition missions: ruins, clinics, depots, radio towers, farms, tunnels,
  faction camps, rescue signals.
- Team composition: 1 to 4 scavengers; role coverage affects outcomes.
- Facilities: garden, purifier, clinic, workshop, barracks, watchtower, radio,
  school, walls, storage.
- Threat clock: raiders, sickness, storms, starvation, faction demands.
- Factions: river medics, scrap barons, free camps, signal cult, old authority.
- Endgame plans: fortified town, mobile convoy, alliance, radio beacon,
  isolation bunker, collapse.

## Progression

Early game is about food/water stability. Midgame adds base upgrades, factions,
and harder expeditions. Late game forces settlement identity: who gets rescued,
who gets turned away, and what long-term plan gets funded. Difficulty ramps via
larger population needs, region danger, threat clock, and survivor trauma.

## Data Model

Recommended compiled tables:

- `SurvivorTemplate`: id, name, role, trait, baseStats, recruitRegion.
- `MissionDef`: id, name, region, rank, length, lootMask, dangerTags,
  requiredRoleHint.
- `StageEventDef`: id, tagMask, weight, checkRole, textId, effects.
- `FacilityDef`: id, name, cost, maxLevel, upkeep, effectId.
- `ThreatDef`: id, name, threshold, eventId, cooldown.
- `FactionDef`: id, name, tradeBias, demandType, endingFlag.

Recommended save fields:

- seed, week, regionUnlockMask, ending, gameOver.
- resources: food, water, meds, ammo, scrap, power, morale, defense, population,
  radiation.
- survivors[20], facilityLevels[10], factionRep[5].
- threatMeters[5], missionChainFlags[3], settlementFlags[4].

## Content Tables

V1 content target:

- 20 survivor templates.
- 50 mission templates.
- 70 stage events.
- 10 facilities.
- 5 threat clocks.
- 5 factions.
- 8 region unlocks.
- 6 endings.

## Events And Encounters

- Clinic run finds meds and an infected survivor.
- Radio tower reveals new region but draws attention.
- Raider scouts test stealth, ammo, or diplomacy.
- Water purifier breaks before a heat wave.
- Survivor refuses a mission after too much stress.
- Faction asks for food in exchange for future protection.
- Base attack triggers defense and roster injury checks.

## Implementation Plan

1. Create a separate `WastelandGuildmaster` sketch.
2. Implement base overview, mission board, roster, team selection, and saving.
3. Add resource upkeep and weekly settlement advance.
4. Add mission stage simulation with role checks.
5. Add injuries, radiation, stress, rest, and treatment.
6. Add facilities and threat clocks.
7. Add factions and endgame plan flags.
8. Balance around 40 to 70 expeditions for a full settlement arc.

## Testing Checklist

- Compile with Arduino CLI.
- Confirm base, roster, team, and build screens fit.
- Run missions with all team sizes.
- Save/load with active resources, roster, facilities, and threats.
- Trigger starvation, low water, sickness, attack, radiation, and morale crises.
- Build every facility and verify its effect.
- Simulate 80 weeks and confirm no overflow or impossible resource spiral.
- Reach alliance, fortress, convoy, bunker, and collapse endings.

## Prompt For Next Codex Session

```text
In /Users/cypher/Documents/GitHub/poke-trail, create a separate Arduino CLI
Cardputer prototype for Wasteland Guildmaster from
docs/gameplans/13-wasteland-guildmaster.md. Use M5Cardputer input/display,
240x135 text UI, SD saves, fixed survivor/mission/facility tables, settlement
resources, team expeditions, injuries, base upgrades, threats, factions, and
endings.
```

