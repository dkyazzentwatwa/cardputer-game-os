# Dungeon Courier

## Pitch

Dungeon Courier is a delivery RPG where the player carries urgent letters,
medicine, cursed relics, strange packages, and town gossip through dangerous
routes. The game is mission-based, funny, and tense: choose jobs, plan a route,
protect the parcel, survive hazards, and decide whether to open packages that
should probably stay sealed. It fits the Cardputer because each delivery can be
expressed through menus, logs, compact route maps, and short event text.

## Target Experience

- Total playtime target: 5 to 10+ hours across one courier career.
- Session length: 3 to 15 minutes per delivery.
- Tone: weird fantasy, cozy danger, contract comedy.
- Player fantasy: "I am the tiny keyboard-powered courier nobody should
  underestimate."
- Campaign shape: many short contracts that build reputation, unlock towns,
  and create long-term consequences.
- Hardware constraints: Arduino CLI, M5Cardputer keyboard, 240x135 text UI,
  offline simulated systems, SD saves, fixed contract and event tables.

## Core Loop

1. Visit the job board in the current town.
2. Compare delivery contracts by reward, deadline, package type, route hazard,
   reputation effect, and weirdness.
3. Pack supplies: food, torches, bandages, charms, rope, courier stamps.
4. Choose safe road, normal road, or shortcut.
5. Resolve travel segments with stamina, danger, weather, and package events.
6. Deliver, fail, open, lose, trade, or refuse the package.
7. Collect rewards, update reputation, unlock perks, and accept harder jobs.

## Controls And UI

- `1`-`5`: choose visible actions or contract options.
- `w/s` and `;/.`: move through job board, inventory, perk list, and route list.
- `Enter`: accept highlighted job or confirm event choice.
- `Backspace/q`: leave current menu.
- Main screens: `TOWN`, `BOARD`, `ROUTE`, `PACK`, `EVENT`, `STATUS`, `PERKS`.
- Delivery events should show one short description and up to 4 choices.

## Major Systems

- Contract generator: combines origin town, destination town, package type,
  deadline, reward, risk, secrecy, and sender faction.
- Package personalities: some packages hum, argue, leak, lie, tempt the player,
  or alter route events.
- Route planning: each town link offers a safe path, common path, or shortcut
  with different stamina cost, deadline pressure, and hazard table.
- Courier condition: stamina, hunger, wounds, stress, suspicion, and package
  integrity.
- Reputation: 5 town/faction reputations affect job quality, guards, prices,
  and whether failed deliveries are forgiven.
- Perks: route memory, steady hands, silver tongue, night walker, medic, lucky
  stamp, iron stomach, package whisperer.
- Rival couriers: recurring competitors can steal contracts, spread rumors, or
  rescue the player for a price.
- Consequences: failed jobs reduce reputation, cursed jobs create flags, opened
  packages can unlock story chains or permanent penalties.

## Progression

The campaign begins with local errands between 3 nearby towns. The player earns
stamps and perks, then unlocks mountain passes, old tunnels, marsh roads, and
capital contracts. Midgame adds time pressure, rivals, and package oddities.
Late game presents faction-defining deliveries and a final courier license
trial. Endings depend on reputation, completed chain jobs, opened cursed parcels,
and whether the player becomes trusted, infamous, haunted, or legendary.

## Data Model

Recommended compiled tables:

- `TownDef`: id, name, region, services, faction, dangerBias.
- `RouteDef`: from, to, safeCost, normalCost, shortCost, hazardMask.
- `PackageDef`: id, name, tag, integrityMax, weirdness, eventMask.
- `ContractTemplate`: senderFaction, packageId, rewardBase, deadlineBase,
  repReward, chainId.
- `HazardEventDef`: id, hazardMask, weight, textId, choices, effects.
- `PerkDef`: id, name, cost, effectId.

Recommended save fields:

- seed, day, currentTown, careerRank, stamps, money.
- stamina, wounds, stress, suspicion, food, torches, bandages, charms.
- activeContract fields: destination, packageId, integrity, deadline, reward,
  senderFaction, routeProgress, opened.
- reputation[5], perkMask, rivalState[3], chainFlags[2], ending.

## Content Tables

V1 content target:

- 9 towns.
- 15 route links, each with 3 route modes.
- 16 package types.
- 36 contract templates.
- 48 hazard or package events.
- 10 perks.
- 3 rival couriers.
- 5 reputation groups.
- 6 endings.

## Events And Encounters

- Package whispers a shortcut clue, but following it raises suspicion.
- Bridge toll asks for money, charm, or a reputation check.
- Rain damages package integrity unless waterproof wrap was packed.
- Rival courier offers to swap parcels.
- Bandit stop can be talked through, outrun, bribed, or fought.
- Package begs to be opened and may reveal bonus pay or a curse.
- Wrong address forces a choice: spend time searching or fake the signature.

## Implementation Plan

1. Create a separate `DungeonCourier` sketch with the same Cardputer display and
   input conventions as Poke-Trail.
2. Implement town screen, job board, contract accept, and compact status screen.
3. Add contract generation from fixed templates and seeded town/destination
   choices.
4. Implement route travel, stamina/deadline/integrity updates, and delivery
   success/failure.
5. Add events with 2 to 4 choices and clear resource consequences.
6. Add reputation and perks.
7. Add rival states and package chain flags.
8. Balance 40 to 70 deliveries as a full career.

## Testing Checklist

- Compile with Arduino CLI for Cardputer.
- Verify all contract rows fit on the tiny screen.
- Accept, abandon, complete, fail, and open packages.
- Test all route modes and deadline math.
- Confirm no stat goes below zero or above intended caps.
- Save/load during town, route, event, and active contract states.
- Run 50 generated contracts and confirm destinations are reachable.
- Confirm at least one good, one bad, and one strange ending can trigger.

## Prompt For Next Codex Session

```text
In /Users/cypher/Documents/GitHub/poke-trail, implement the first playable
Dungeon Courier Cardputer prototype from docs/gameplans/02-dungeon-courier.md
as a separate Arduino CLI sketch. Use M5Cardputer keyboard/display, 240x135
menu screens, SD saves, fixed tables, contract generation, route travel,
package integrity, stamina, reputation, and delivery outcomes. Keep all content
offline and fictional.
```

