# Star Trader: Pocket Frontier

## Pitch

Star Trader: Pocket Frontier is a space trading and survival RPG built for the
Cardputer as a tiny captain's console. The player pilots a small freighter
through dangerous systems, buys and sells goods, manages fuel and oxygen, hires
crew, evades pirates, and uncovers old alien infrastructure. It fits the
Cardputer because most of the fun is decision-making, route planning, logs,
prices, and compact ship status screens instead of graphics.

## Target Experience

- Total playtime target: 5 to 10+ hours per serious campaign.
- Session length: 5 to 20 minutes, one route leg or market visit at a time.
- Tone: frontier sci-fi, scrappy, dangerous, lightly funny.
- Player fantasy: "My pocket device is my ship console."
- Core promise: the same map can produce different fortunes based on prices,
  crew, repairs, factions, and risk appetite.
- Hardware constraints: Arduino CLI, M5Cardputer keyboard, 240x135 text UI,
  SD saves, no internet, no real networking, fixed tables plus seeded random.

## Core Loop

1. Check current planet status, ship condition, fuel, oxygen, cargo, and local
   prices.
2. Buy or sell goods based on price hints, faction law, cargo space, and route
   risk.
3. Choose a destination from nearby systems.
4. Spend fuel and oxygen to travel.
5. Resolve route events: pirate scan, distress call, meteor damage, salvage,
   customs stop, crew conflict, or artifact clue.
6. Arrive, update market prices, repair, refuel, hire, trade, and accept jobs.
7. Push toward one of several endgame routes: wealthy trader, faction hero,
   artifact discovery, smuggler lord, or stranded failure.

## Controls And UI

- `1`-`5`: choose visible menu actions.
- `w/s` and `;/.`: move through market rows, route choices, and crew list.
- `Enter`: confirm highlighted action.
- `Backspace` or `q`: return to the previous screen.
- Main screens: `BRIDGE`, `MARKET`, `ROUTES`, `CARGO`, `CREW`, `SHIP`, `LOG`.
- Keep each screen to 5 visible lines plus header/footer.
- Use short labels: `Fuel`, `O2`, `Hull`, `Cr`, `Rep`, `Heat`.

## Major Systems

- Sector map: 24 to 32 planets arranged as nodes with 2 to 4 routes each.
  Each route stores distance, hazard class, patrol level, and faction control.
- Economy: 8 goods with planet-specific base prices and seeded daily drift.
  Goods include food blocks, ore, medkits, water, data cores, relics, machine
  parts, and restricted biotech.
- Ship survival: fuel, oxygen, hull, cargo slots, credits, heat, and crew
  morale. Running out of fuel or oxygen triggers emergency events rather than
  instant failure.
- Crew: up to 4 crew members with role and trait. Roles can reduce fuel use,
  improve trading, repair hull, lower heat, or improve event odds.
- Factions: 4 faction reputations. High reputation opens discounts and safe
  routes; low reputation raises customs risk and blocks ports.
- Upgrades: cargo bay, oxygen recycler, fuel tank, shield plates, scanner,
  smuggler hold, engine, and med bay.
- Quests: delivery contracts, bounty rumors, artifact fragments, faction favors,
  and debt pressure.
- Endings: honest merchant, frontier savior, relic awakener, pirate bargain,
  corporate pawn, lost ship.

## Progression

Early game teaches prices and survival with short safe routes. Midgame unlocks
larger cargo, faction routes, and longer loops where running out of one resource
creates cascading problems. Late game forces commitments: pay off debt, back a
faction, follow artifact coordinates, or exploit black market routes. Difficulty
ramps through route distance, fuel cost, customs pressure, pirate strength, and
repair prices.

## Data Model

Recommended compiled tables:

- `PlanetDef`: id, name, faction, tags, basePrice[8], services, routeStart,
  routeCount.
- `RouteDef`: from, to, distance, hazard, patrol, fuelCost, clueFlags.
- `GoodDef`: id, name, legalClass, baseMass, volatility.
- `CrewDef`: id, name, role, trait, wage, effectId.
- `UpgradeDef`: id, name, cost, effectId, maxRank.
- `EventDef`: id, routeTag, minDay, weight, textId, effectId.

Recommended save fields:

- seed, day, currentPlanet, credits, debt, cargo[8], fuel, oxygen, hull, heat.
- factionRep[4], crewIds[4], crewMorale[4], upgradeRanks[8].
- questFlags[2], artifactFragments, ending, gameOver.

## Content Tables

V1 content target:

- 28 planets.
- 46 routes.
- 8 trade goods.
- 4 factions.
- 20 crew entries.
- 8 ship upgrades with 2 ranks each.
- 36 route events.
- 12 jobs.
- 6 endings.

## Events And Encounters

- Pirate demand: pay credits, dump cargo, fight with hull risk, or run.
- Customs scan: legal cargo passes, restricted cargo adds heat or fines.
- Salvage beacon: spend oxygen to investigate for parts or artifact clue.
- Engine cough: repair with parts or lose extra fuel.
- Crew dispute: morale drop unless captain pays bonus or rests.
- Black market offer: high profit with heat gain.
- Alien ruin ping: unlocks coordinate toward the artifact ending.

## Implementation Plan

1. Start a separate `StarTrader` Arduino sketch using the Poke-Trail display,
   input, and save style.
2. Implement bridge, market, routes, and message screens first.
3. Add fixed planet, route, and good tables with seeded price drift.
4. Implement buy/sell, travel resource costs, arrival, and autosave.
5. Add route events and faction reputation.
6. Add crew and upgrades.
7. Add quest flags and endings.
8. Balance a full run so victory usually takes 80 to 140 route legs.

## Testing Checklist

- Compile with Arduino CLI for `m5stack:esp32:m5stack_cardputer`.
- Confirm all screens fit on 240x135 with no wrapped menu labels.
- Test `1`-`5`, `w/s`, `;/.`, `Enter`, `Backspace`, and `q`.
- Buy and sell every good without negative cargo or credits.
- Travel until low fuel, low oxygen, high heat, and low hull events trigger.
- Save, reboot, and load after trade, travel, crew hire, and upgrade purchase.
- Simulate 150 travel legs and confirm no resource overflow or stuck route.
- Reach at least two endings.

## Prompt For Next Codex Session

```text
In /Users/cypher/Documents/GitHub/poke-trail, build the first playable Arduino
CLI Cardputer prototype for docs/gameplans/01-star-trader-pocket-frontier.md as
a separate sketch folder. Reuse the Poke-Trail style for M5Cardputer display,
keyboard input, SD save/load, compact fixed tables, and 240x135 text screens.
Implement bridge, market, routes, cargo, travel, seeded prices, basic events,
autosave, and a manual test checklist. Do not use real networking or graphics.
```

