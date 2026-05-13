# Pocket Kingdom Manager

## Pitch

Pocket Kingdom Manager is a compact kingdom sim where the player rules through
seasons of famine, war, construction, laws, diplomacy, and magical trouble. It
plays like a tiny council desk: assign workers, review advisors, choose decrees,
fund buildings, survive crises, and guide a dynasty. It fits the Cardputer
because it is all turn decisions, stats, short advisor text, and table-driven
events.

## Target Experience

- Total playtime target: 5 to 10+ hours per dynasty.
- Session length: 5 to 15 minutes per year or crisis.
- Tone: strategic, dry, occasionally absurd, but readable.
- Player fantasy: "This tiny keyboard is my royal ledger."
- Campaign shape: survive 20 to 40 seasons while steering toward military,
  prosperity, faith, magic, diplomacy, or stability victories.
- Hardware constraints: Arduino CLI, M5Cardputer keyboard, 240x135 text UI,
  SD saves, deterministic turn simulation, compact tables.

## Core Loop

1. Review seasonal stats: food, gold, people, soldiers, morale, faith, magic,
   stability, threat.
2. Assign workers to farms, mines, walls, study, temples, markets, or army.
3. Choose one decree or building project.
4. Hear advisor warnings and disagreements.
5. Resolve production, upkeep, unrest, weather, raids, diplomacy, and magic
   events.
6. Update laws, advisor loyalty, class tension, and victory progress.
7. Advance to next season or handle emergency decisions.

## Controls And UI

- `1`-`5`: pick action, building, decree, or advisor response.
- `w/s` and `;/.`: move through resources, building list, laws, and advisors.
- `Enter`: confirm highlighted command.
- `Backspace/q`: return to kingdom overview.
- Main screens: `REALM`, `WORK`, `BUILD`, `LAW`, `COURT`, `EVENT`, `YEAR`.
- Use compact status rows: `Food`, `Gold`, `Pop`, `Sold`, `Mor`, `Faith`,
  `Magic`, `Stable`.

## Major Systems

- Seasonal simulation: spring, summer, autumn, winter with distinct production
  and event weights.
- Worker assignment: population is divided among jobs; jobs produce resources
  or reduce risks.
- Buildings: farms, granary, barracks, walls, market, library, shrine, tower,
  roads, infirmary, court.
- Laws: rationing, taxes, conscription, trade rights, temple tithe, magic
  licensing, militia charter.
- Advisors: 5 advisors with agenda, competence, loyalty, and hidden flaw.
  Listening to one can upset another.
- Class tension: nobles, commoners, soldiers, clergy, mages. Low trust causes
  unrest or blocked laws.
- Crisis deck: famine, plague, border war, dragon rumor, succession dispute,
  magical leak, harvest festival, merchant strike.
- Victory paths: golden age, iron crown, holy mandate, wizard compact, people's
  charter, exile, collapse.

## Progression

The first years teach food, gold, and unrest. Midgame unlocks buildings and
larger political tradeoffs. Late game stacks seasonal pressure with external
threat and class demands. The player can specialize, but over-specialization
creates vulnerabilities. A military kingdom may starve; a rich kingdom may be
invaded; a magical kingdom may destabilize reality.

## Data Model

Recommended compiled tables:

- `SeasonDef`: id, name, foodMod, taxMod, threatMod, eventMask.
- `BuildingDef`: id, name, costGold, costWood, upkeep, effectId, maxLevel.
- `LawDef`: id, name, unlockYear, resourceEffect, classEffects[5].
- `AdvisorDef`: id, name, agenda, flaw, skill, adviceMask.
- `EventDef`: id, seasonMask, minYear, weight, textId, choiceEffects.
- `VictoryDef`: id, name, requiredStats, requiredFlags.

Recommended save fields:

- seed, year, season, rulerAge, heirState, ending.
- resources: food, gold, wood, people, soldiers, morale, faith, magic,
  stability, threat.
- workers[7], buildingLevels[10], activeLawMask.
- advisorLoyalty[5], classTrust[5], flags[3], crisisCooldowns.

## Content Tables

V1 content target:

- 4 seasons.
- 10 buildings with 3 levels each.
- 12 laws.
- 5 advisors.
- 5 population classes.
- 50 seasonal events.
- 8 crisis chains.
- 7 endings.

## Events And Encounters

- Famine: open granary, buy grain, ration, or ignore warnings.
- Advisor scandal: protect them, investigate, exile, or exploit it.
- Border raid: spend soldiers, gold, diplomacy, or walls.
- Magic leak: use mages, temples, workers, or seal the tower.
- Harvest festival: spend food/gold for morale and trust.
- Succession issue: choose heir training and faction support.
- Peasant petition: lower taxes, grant rights, send soldiers, or negotiate.

## Implementation Plan

1. Create a separate `PocketKingdom` sketch.
2. Implement realm overview, seasonal advance, worker assignment, and saving.
3. Add production/upkeep math and clear resource caps.
4. Add buildings and laws.
5. Add advisor text and class trust.
6. Add event choices and crisis chains.
7. Add victory/ending checks at the end of each year.
8. Balance a campaign around 30 years or 120 turns.

## Testing Checklist

- Compile with Arduino CLI.
- Confirm every resource label fits on screen.
- Advance 120 seasons without UI lockup or overflow.
- Test every worker job and building purchase.
- Save/load with active laws, workers, buildings, and advisors.
- Force famine, war, unrest, and magic crisis events.
- Reach at least three victory paths and collapse.
- Confirm number and cursor controls work on all menus.

## Prompt For Next Codex Session

```text
In /Users/cypher/Documents/GitHub/poke-trail, implement a separate Arduino CLI
Cardputer prototype for Pocket Kingdom Manager based on
docs/gameplans/04-pocket-kingdom-manager.md. Use compact text screens,
M5Cardputer input, SD saves, fixed C++ tables, seasonal turns, worker
assignment, resources, buildings, laws, advisors, events, and endings.
```

