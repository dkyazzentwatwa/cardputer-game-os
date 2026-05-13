# Star Trail Rancher

## Pitch

Star Trail Rancher combines space trading, Oregon Trail travel, and alien
creature care. The player transports live alien creatures between frontier
planets while keeping them calm, fed, quarantined, and healthy. The ship is both
freighter and mobile ranch. It fits the Cardputer because the drama is in
compact manifests, creature condition, route risk, habitat modules, and trade
decisions.

## Target Experience

- Total playtime target: 5 to 10+ hours per captain-rancher career.
- Session length: 5 to 20 minutes per route.
- Tone: frontier sci-fi, creature-care, risky commerce.
- Player fantasy: "I run a weird little alien livestock ship."
- Campaign shape: haul creatures and goods, upgrade habitats, discover species,
  build reputation, and choose humane or exploitative frontier paths.
- Hardware constraints: Arduino CLI, M5Cardputer keyboard, 240x135 text UI,
  SD saves, fixed planet/species/job tables, offline simulation.

## Core Loop

1. Review ship: credits, fuel, oxygen, hull, habitats, feed, med gel, quarantine
   risk, creature stress.
2. Accept transport contracts or buy creatures/goods.
3. Assign creatures to habitat modules.
4. Choose route and prepare supplies.
5. Resolve travel days with creature events, ship hazards, inspections, and
   market shifts.
6. Deliver safely, sell goods, earn reputation, or keep rare species.
7. Upgrade ship habitats and unlock longer routes.

## Controls And UI

- `1`-`5`: choose contracts, route, habitat action, or event response.
- `w/s` and `;/.`: scroll manifests, creatures, planets, and upgrades.
- `Enter`: confirm highlighted choice.
- `Backspace/q`: return to bridge.
- Main screens: `BRIDGE`, `JOBS`, `HAB`, `CREW`, `ROUTE`, `MARKET`, `LOG`.
- Creature condition rows should show `HP`, `Stress`, `Feed`, `Temp`, `Q`.

## Major Systems

- Planet routes: 18 to 24 planets with market demand and habitat laws.
- Creature cargo: up to 8 creatures, each with species, value, diet, climate,
  stress, health, quarantine, and temperament.
- Habitat modules: cold tank, heat bay, wet pod, low-grav stall, quarantine
  locker, nursery, shielded den.
- Contracts: transport, rescue, research, black market sale, breeding loan,
  sanctuary delivery.
- Ship resources: fuel, oxygen, hull, feed, med gel, credits, crew morale.
- Care events: illness, escape, nesting, aggression, bonding, illegal mutation,
  stress spiral.
- Ethics/reputation: scientists, smugglers, sanctuaries, corporate buyers.
- Discovery: unknown species can be scanned, named, protected, sold, or studied.

## Progression

Early game carries hardy species on short routes. Midgame introduces specialized
habitats, quarantine rules, and higher-value contracts. Late game asks whether
to build a sanctuary route, become a corporate transporter, smuggle rare species,
or reveal a frontier extinction plot. Difficulty rises through species needs,
inspection rules, route hazards, and habitat limitations.

## Data Model

Recommended compiled tables:

- `PlanetDef`: id, name, faction, routeMask, demandTags, lawStrictness.
- `SpeciesDef`: id, name, diet, climate, baseValue, stressRate, rarity,
  temperament, habitatTag.
- `HabitatDef`: id, name, climateTag, capacity, upgradeCost, effectId.
- `ContractDef`: id, origin, destination, speciesTag, reward, deadline,
  reputationEffects.
- `RouteDef`: from, to, distance, hazard, fuelCost, inspectionChance.
- `EventDef`: id, tagMask, weight, textId, effects.

Recommended save fields:

- seed, day, planetId, credits, fuel, oxygen, hull, feed, medGel, morale.
- creatures[8]: speciesId, health, stress, quarantine, bond, habitatSlot,
  contractId.
- habitatLevels[7], cargoGoods[6], reputation[4], discoveryMask, flags[3],
  ending.

## Content Tables

V1 content target:

- 22 planets.
- 36 routes.
- 30 alien species.
- 7 habitat modules.
- 40 contracts.
- 6 trade goods.
- 56 ship/creature events.
- 6 endings.

## Events And Encounters

- Cold-climate creature overheats if habitat is underleveled.
- Inspection finds unregistered species unless papers are bought.
- Creature bonds with crew and reduces stress on later routes.
- Quarantine alarm forces med gel, isolation, or route diversion.
- Sanctuary offers lower pay but better reputation.
- Black market buyer pays double for a rare species.
- Unknown egg hatches mid-route and changes the manifest.

## Implementation Plan

1. Create a separate `StarTrailRancher` sketch.
2. Implement bridge, contracts, habitat manifest, route choice, and saving.
3. Add planets/routes and ship resources.
4. Add creature cargo condition and habitat compatibility.
5. Add route travel with creature care events.
6. Add reputation and contract outcomes.
7. Add discovery and rare species flags.
8. Balance around 30 to 50 completed contracts.

## Testing Checklist

- Compile with Arduino CLI.
- Verify manifest and habitat screens fit on 240x135.
- Accept, complete, fail, and abandon creature contracts.
- Save/load with active contracts and creature states.
- Test habitat mismatch, stress, illness, quarantine, and inspection.
- Travel every route and confirm no unreachable contract destination.
- Confirm ethical and black market paths both work.
- Reach sanctuary, corporate, smuggler, discovery, and failure endings.

## Prompt For Next Codex Session

```text
In /Users/cypher/Documents/GitHub/poke-trail, build a separate Arduino CLI
Cardputer prototype for Star Trail Rancher from
docs/gameplans/12-star-trail-rancher.md. Use compact M5Cardputer text screens,
SD saves, fixed tables, planets/routes, creature cargo, habitat modules,
transport contracts, ship resources, route events, and reputation. Keep it
offline and menu-driven.
```

