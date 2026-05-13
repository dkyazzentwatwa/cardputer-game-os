# Monster Ranch Trail

## Pitch

Monster Ranch Trail is a creature raising and ranch management game set on the
edge of a dangerous frontier. Instead of constantly traveling, the player runs a
ranch, trains creatures, sends them on expeditions, expands facilities, handles
weather, competes in tournaments, and discovers rare variants. It keeps the
creature attachment of Poke-Trail while reducing travel pressure and adding
long-term home-base progression.

## Target Experience

- Total playtime target: 5 to 10+ hours per ranch file.
- Session length: 5 to 15 minutes per week.
- Tone: cozy frontier with risk, humor, and collection goals.
- Player fantasy: "I am building a tiny creature ranch in my pocket."
- Campaign shape: improve the ranch through seasons, collect and train
  creatures, unlock frontier zones, and win league tournaments.
- Hardware constraints: Arduino CLI, M5Cardputer keyboard, 240x135 text UI,
  SD saves, fixed species/facility/event tables, no graphics-heavy systems.

## Core Loop

1. Review the ranch week: money, feed, medicine, morale, weather, facilities,
   and creature condition.
2. Choose weekly actions: train, feed, rest, breed/evolve, send expedition,
   build facility, enter tournament, or handle event.
3. Apply stat growth, fatigue, bond, injuries, and facility bonuses.
4. Resolve expedition or tournament outcomes.
5. Discover items, variants, money, reputation, or story flags.
6. Advance season and unlock new zones or ranch options.

## Controls And UI

- `1`-`5`: choose action or creature slot.
- `w/s` and `;/.`: move through roster, facilities, expeditions, and shop.
- `Enter`: confirm selected action.
- `Backspace/q`: return to ranch overview.
- Main screens: `RANCH`, `ROSTER`, `TRAIN`, `EXPED`, `BUILD`, `SHOP`, `EVENT`.
- Show one creature at a time with compact stats to avoid cramped roster rows.

## Major Systems

- Creature roster: up to 12 creatures in ranch storage, 4 active workers, with
  species, level, HP, power, grit, speed, focus, bond, fatigue, trait, variant.
- Training: choose stat focus. Training improves stats but adds fatigue and
  injury risk if overused.
- Facilities: stable, clinic, training yard, garden, workshop, nursery, lookout,
  weather vane, tournament pen.
- Expeditions: send 1 to 3 creatures to zones for loot, discovery, and risk.
  Results are simulated with stats, traits, weather, and zone danger.
- Breeding/evolution: combine bond, level, seasonal items, and facility unlocks
  to produce variants or growth forms.
- Tournaments: bracket-like simulated battles using stats, stamina, and simple
  type matchups.
- Seasons/weather: affects feed cost, injuries, expedition risk, and available
  events.
- Rival ranchers: recurring opponents with tournament teams, trades, and story
  challenges.

## Progression

Early game starts with one creature, a small stable, and basic training. Midgame
adds expeditions, facility choices, breeding/evolution, and rival tournaments.
Late game unlocks rare frontier zones, legendary creature quests, and league
championships. A long file can become collection-focused, tournament-focused, or
ranch-building-focused.

## Data Model

Recommended compiled tables:

- `SpeciesDef`: id, name, typeA, typeB, baseStats, growth, rarity, zoneMask,
  evolveId, abilityId.
- `Creature`: speciesId, level, xp, stats, bond, fatigue, trait, variant, injury.
- `FacilityDef`: id, name, cost, maxLevel, effectId.
- `ZoneDef`: id, name, danger, rewardMask, speciesMask, unlockRep.
- `TournamentDef`: id, name, rank, entryCost, prize, opponentTable.
- `EventDef`: id, seasonMask, ranchFlag, weight, choiceEffects.

Recommended save fields:

- seed, week, season, money, feed, meds, reputation, leagueRank.
- creatures[12], activeSlots[4], facilityLevels[9].
- discoveredMask, variantMask, rivalState[4], flags[3], ending.

## Content Tables

V1 content target:

- 36 creature species or variants.
- 8 expedition zones.
- 9 facilities with 3 levels.
- 4 seasons and 12 weather states.
- 8 tournaments.
- 5 rival ranchers.
- 45 ranch/expedition events.
- 6 endings.

## Events And Encounters

- Storm damages a facility unless the weather vane is upgraded.
- Creature refuses training and needs rest or favorite feed.
- Expedition finds an egg, broken tool, rare seed, or rival marker.
- Ranch defense event tests active creatures without a full battle screen.
- Traveling judge offers a surprise tournament.
- Sick creature can be treated with meds or time.
- Rival asks for a trade, rematch, or joint expedition.

## Implementation Plan

1. Create a separate `MonsterRanchTrail` sketch.
2. Reuse Poke-Trail-style species tables and compact creature instances.
3. Implement ranch overview, roster, weekly advance, training, and saving.
4. Add fatigue, bond, injury, feed, and money.
5. Add facilities and shop.
6. Add expeditions with simulated outcomes.
7. Add tournaments and rival states.
8. Add breeding/evolution and collection endings.

## Testing Checklist

- Compile with Arduino CLI.
- Confirm creature detail screens fit on 240x135.
- Train, rest, injure, heal, and level creatures.
- Save/load roster, facilities, season, and unlocked zones.
- Run 100 weeks and confirm stats remain capped.
- Complete expeditions in every zone.
- Enter and win/lose tournaments.
- Confirm at least one collection, league, and ranch ending.

## Prompt For Next Codex Session

```text
In /Users/cypher/Documents/GitHub/poke-trail, create a separate Arduino CLI
Cardputer prototype for Monster Ranch Trail from
docs/gameplans/05-monster-ranch-trail.md. Use Poke-Trail-style fixed creature
tables, compact save structs, M5Cardputer controls, SD saves, 240x135 text UI,
ranch weeks, training, fatigue, facilities, expeditions, and tournaments.
```

