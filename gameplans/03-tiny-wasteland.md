# Tiny Wasteland

## Pitch

Tiny Wasteland is a post-apocalyptic survival RPG about guiding a small
scavenger group across ruined territory toward a rumored safe zone. It is the
most direct dramatic cousin to an Oregon Trail structure: travel, scavenge,
repair gear, ration food and water, treat sickness, recruit survivors, and make
hard choices. It fits the Cardputer because the tension comes from resource
numbers, short events, survivor names, and compact travel decisions.

## Target Experience

- Total playtime target: 5 to 10+ hours per campaign.
- Session length: 5 to 20 minutes.
- Tone: survival drama with dry humor and occasional hope.
- Player fantasy: "I am keeping this fragile group alive one bad day at a
  time."
- Campaign shape: a long westward route through regions, settlements, ruins,
  and faction-controlled zones.
- Hardware constraints: Arduino CLI, M5Cardputer keyboard, 240x135 text UI,
  SD saves, offline events, fixed tables, compact state.

## Core Loop

1. Review party health, food, water, meds, fuel, morale, radiation, and vehicle
   durability.
2. Choose daily action: travel, scavenge, rest, repair, trade, manage party, or
   save/menu.
3. Spend resources and advance time.
4. Resolve events: weather, raiders, ruins, sickness, settlement choices,
   vehicle damage, survivor conflicts.
5. Collect or lose supplies, injuries, recruits, faction reputation, or clues.
6. Reach a new region or settlement.
7. Decide whether to trust factions and what kind of safe zone to build or find.

## Controls And UI

- `1`-`5`: choose visible actions.
- `w/s` and `;/.`: move through party, inventory, settlement, and event choices.
- `Enter`: confirm highlighted choice.
- `Backspace/q`: return.
- Main screens: `ROAD`, `PARTY`, `CAMP`, `SCAV`, `TRADE`, `EVENT`, `ENDING`.
- Keep stats abbreviated: `Fd`, `Wat`, `Med`, `Fuel`, `Mor`, `Rad`, `Veh`.

## Major Systems

- Travel regions: 10 zones with hazard modifiers for thirst, radiation, fuel
  use, encounter odds, and settlement density.
- Party survival: up to 5 survivors with health, morale, skill, wound, sickness,
  loyalty, and trait.
- Resources: food, water, meds, ammo, scrap, fuel, filters, morale, vehicle
  durability, radiation.
- Scavenging: choose location type and risk level. Higher risk gives better
  loot but increases wounds, radiation, raiders, and noise.
- Injuries and sickness: wounds, infection, dehydration, radiation sickness,
  exhaustion, panic.
- Factions: settlement league, raider barons, machine cult, river medics, free
  scavengers. Reputation changes trade prices and event outcomes.
- Vehicle: engine, tires, armor, storage, purifier, radio. Broken vehicle slows
  travel and increases resource burn.
- Moral decisions: share food, abandon cargo, rescue strangers, raid supplies,
  hide sickness, use scarce medicine.

## Progression

Early game is about learning rationing and basic repair. Midgame introduces
recruits, settlement politics, radiation zones, and vehicle upgrades. Late game
asks what "safe" means: join a fortified settlement, found a camp, trust a radio
signal, sell out to a faction, or walk alone. Difficulty rises through longer
routes, harsher weather, resource scarcity, and party trauma.

## Data Model

Recommended compiled tables:

- `RegionDef`: id, name, distance, foodCost, waterCost, radRisk, eventMask.
- `SurvivorTemplate`: id, name, skill, trait, maxHealth, loyalty.
- `LocationDef`: id, name, lootMask, risk, minRegion.
- `EventDef`: id, regionMask, weight, textId, choiceCount, effectIds.
- `FactionDef`: id, name, tradeBias, hostility, endingFlag.
- `UpgradeDef`: id, name, scrapCost, effectId.

Recommended save fields:

- seed, day, region, regionMiles, totalMiles, gameOver, ending.
- resources: food, water, meds, ammo, scrap, fuel, filters, morale, vehicle,
  radiation.
- survivors[5]: templateId, health, wound, sickness, loyalty, xp.
- factionRep[5], vehicleUpgradeMask, flags[3], currentEvent.

## Content Tables

V1 content target:

- 10 regions.
- 24 survivor templates.
- 10 scavenging location types.
- 60 events.
- 5 factions.
- 8 vehicle upgrades.
- 12 settlement trade events.
- 7 endings.

## Events And Encounters

- Dust storm: lose time, water, or vehicle durability.
- Pharmacy ruin: medicine reward with infection or raider risk.
- Stranger with a Geiger counter offers a shortcut through hot ground.
- Survivor hides a bite or sickness.
- Settlement demands payment, service, or faction loyalty.
- Vehicle axle cracks; repair with scrap or abandon cargo.
- Radio safe-zone signal repeats coordinates that may be real or a trap.

## Implementation Plan

1. Build a separate `TinyWasteland` sketch with Poke-Trail-style screens.
2. Implement road status, daily actions, travel, rest, repair, and save.
3. Add region progression and resource burn.
4. Add survivor roster with health, sickness, loyalty, and traits.
5. Add scavenging locations and loot tables.
6. Add events with moral choices and faction reputation.
7. Add vehicle upgrades and late-game ending checks.
8. Balance a full route around 90 to 130 in-game days.

## Testing Checklist

- Compile with Arduino CLI.
- Verify road and party screens fit without wrapping.
- Travel across all 10 regions.
- Trigger hunger, thirst, radiation, vehicle damage, sickness, and morale lows.
- Recruit, injure, heal, and lose survivors.
- Save/load with party roster and vehicle upgrades intact.
- Simulate 150 days and check no overflow or impossible negative resources.
- Reach safe-zone, faction, collapse, and lone-survivor endings.

## Prompt For Next Codex Session

```text
In /Users/cypher/Documents/GitHub/poke-trail, create a separate Arduino CLI
Cardputer sketch for Tiny Wasteland using docs/gameplans/03-tiny-wasteland.md.
Reuse Poke-Trail-style display/input/save patterns. Implement travel, resource
survival, party roster, scavenging, region events, vehicle durability, SD saves,
and a compact 240x135 UI. Do not modify the existing PokeTrail sketch.
```

