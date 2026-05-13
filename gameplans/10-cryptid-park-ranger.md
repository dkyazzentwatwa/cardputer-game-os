# Cryptid Park Ranger

## Pitch

Cryptid Park Ranger is a wilderness survival and monster research game where the
player patrols a huge national park, documents strange creatures, helps campers,
sets cameras and traps, survives weather, and builds a cryptid field guide. It
keeps the discovery and collection appeal of Poke-Trail without making combat
the center. The player is trying to understand creatures, not defeat them.

## Target Experience

- Total playtime target: 5 to 10+ hours per ranger season.
- Session length: 5 to 20 minutes per patrol.
- Tone: mysterious wilderness, field research, light survival.
- Player fantasy: "My Cardputer is a ranger field terminal."
- Campaign shape: patrol zones, collect evidence, unlock research tools, solve
  missing-person and habitat mysteries, identify rare cryptids.
- Hardware constraints: Arduino CLI, M5Cardputer keyboard, 240x135 text UI,
  SD saves, fixed zone/cryptid/event tables, no graphics dependency.

## Core Loop

1. Check ranger station: supplies, stamina, weather, camera batteries,
   reputation, active reports.
2. Pick a park zone and patrol objective.
3. Choose gear: camera, bait, medkit, rope, radio battery, sample jars, thermal
   sensor.
4. Travel through zone segments and resolve weather, camper, terrain, and clue
   events.
5. Track cryptids through signs, sounds, prints, sightings, and samples.
6. Decide whether to document, retreat, rescue, tag, or observe.
7. Return to station, update field guide, earn funding, and unlock zones/tools.

## Controls And UI

- `1`-`5`: choose action, gear, zone, or event response.
- `w/s` and `;/.`: scroll zones, field guide, reports, and gear.
- `Enter`: confirm highlighted choice.
- `Backspace/q`: return to station or cancel.
- Main screens: `STATION`, `ZONES`, `GEAR`, `PATROL`, `TRACK`, `GUIDE`,
  `REPORTS`.
- Field guide entries should use compact evidence meters: `Print`, `Photo`,
  `Audio`, `Sample`, `Sight`.

## Major Systems

- Park zones: forest, lake, ridge, caves, old campground, fire road, bog,
  summit, abandoned lodge, deep preserve. Each has weather, terrain, and
  cryptid masks.
- Cryptid research: each species has required evidence types. Discovery is not
  binary; better evidence improves research rating and funding.
- Gear: cameras, bait, field recorder, sample kit, thermal sensor, drone-like
  fictional mapper, medkit, rope, extra battery.
- Survival: stamina, supplies, weather exposure, injuries, fear, radio battery,
  station funding.
- Patrol reports: missing camper, strange tracks, damaged fence, illegal
  logging, odd lights, injured animal, unknown call.
- Reputation: campers, scientists, park service, local town. Reputation changes
  funding and access.
- Creature behavior: curiosity, aggression, shyness, territory, weather
  preference, active time.
- Rescue and ethics: help people, protect habitats, avoid over-tagging, refuse
  exploitative sponsors.

## Progression

Early patrols teach tracking with common cryptids and safe zones. Midgame opens
night patrols, harsher terrain, rare evidence, and conflicting stakeholder
requests. Late game ties missing-person reports to a preserve-wide mystery.
Endings depend on field guide completion, ethics, rescue success, and who the
player gives the final evidence to.

## Data Model

Recommended compiled tables:

- `ZoneDef`: id, name, terrain, weatherBias, danger, cryptidMask, unlockRep.
- `CryptidDef`: id, name, rarity, behavior, zoneMask, activeWeather,
  requiredEvidenceMask, temperament.
- `GearDef`: id, name, cost, uses, effectId, evidenceBonus.
- `ReportDef`: id, zoneId, type, minDay, reward, chainId.
- `TrackEventDef`: id, zoneMask, cryptidMask, weight, evidenceType, effects.
- `ReputationDef`: id, name, fundingBias, endingFlag.

Recommended save fields:

- seed, day, currentZone, stationFunding, stamina, supplies, exposure, injury,
  fear, radioBattery.
- evidence[cryptidCount]: bitmask and rating.
- gearOwnedMask, gearUses[8], reportsState[12], reputation[4].
- zoneUnlockMask, mysteryFlags[3], ending.

## Content Tables

V1 content target:

- 10 zones.
- 30 cryptids.
- 8 gear types.
- 36 patrol reports.
- 72 track/events.
- 4 reputation groups.
- 6 endings.

## Events And Encounters

- Fresh tracks split toward a safe route or a stormy ridge.
- Camera battery dies unless spare battery was packed.
- Camper panic can ruin a sighting unless calmed.
- Bait attracts the wrong cryptid.
- Thermal reading reveals hidden den but risks disturbing habitat.
- Town sponsor offers funding for sensational proof.
- Rare cryptid appears only if weather, zone, and prior evidence align.

## Implementation Plan

1. Create a separate `CryptidRanger` sketch.
2. Implement station overview, zone selection, gear selection, patrol travel,
   and saving.
3. Add cryptid evidence model and field guide.
4. Add patrol events and reports.
5. Add survival resources and weather.
6. Add reputation/funding choices.
7. Add rare cryptid behavior and final mystery flags.
8. Balance a ranger season around 45 to 80 patrols.

## Testing Checklist

- Compile with Arduino CLI.
- Confirm station, gear, and guide screens fit on 240x135.
- Track and document common, rare, and dangerous cryptids.
- Save/load evidence ratings, gear uses, reports, and reputation.
- Test weather exposure, injury, low supplies, and radio battery states.
- Verify every cryptid has at least one possible evidence path.
- Complete a field guide target and at least one rescue chain.
- Reach science, park, town, failed, and mystery endings.

## Prompt For Next Codex Session

```text
In /Users/cypher/Documents/GitHub/poke-trail, build a separate Arduino CLI
Cardputer prototype for Cryptid Park Ranger from
docs/gameplans/10-cryptid-park-ranger.md. Reuse Poke-Trail-style input,
display, SD saves, fixed tables, and compact state. Implement station, zones,
gear, patrols, cryptid evidence, field guide, reports, weather, reputation, and
no combat-first creature discovery.
```

