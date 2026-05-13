#include "GameData.h"

static const CardputerGameCore::ResourceDef RESOURCES[] = {
    {"Cr", 90, 0, 999, 4, -2},
    {"Fuel", 50, 0, 120, -4, 8},
    {"O2", 60, 0, 120, -3, 7},
    {"Hull", 88, 0, 100, -2, 6},
    {"Heat", 0, 0, 100, 3, -4},
    {"Cargo", 0, 0, 80, 2, -1},
    {"Mor", 70, 0, 100, -1, 4},
    {"Debt", 30, 0, 999, -3, 0},
};

static const CardputerGameCore::NamedValue STAR_TRADER_POCKET_FRONTIER_ROWS_0[] = {
    {"Rumor 01", "Review rumor #1: sets the next route pressure"},
    {"Debt 02", "Review debt #2: sets the next route pressure"},
    {"Crew 03", "Review crew #3: sets the next route pressure"},
    {"Artifact 04", "Review artifact #4: sets the next route pressure"},
    {"Patrol 05", "Review patrol #5: sets the next route pressure"},
    {"Fuel 06", "Review fuel #6: sets the next route pressure"},
    {"Dock 07", "Review dock #7: sets the next route pressure"},
    {"Distress 08", "Review distress #8: sets the next route pressure"},
};

static const CardputerGameCore::NamedValue STAR_TRADER_POCKET_FRONTIER_ROWS_1[] = {
    {"Spice 01", "Trade spice #1: changes cargo, credits, and local heat"},
    {"Water 02", "Trade water #2: changes cargo, credits, and local heat"},
    {"Medgel 03", "Trade medgel #3: changes cargo, credits, and local heat"},
    {"Relics 04", "Trade relics #4: changes cargo, credits, and local heat"},
    {"Scrap 05", "Trade scrap #5: changes cargo, credits, and local heat"},
    {"Kelp 06", "Trade kelp #6: changes cargo, credits, and local heat"},
    {"Cores 07", "Trade cores #7: changes cargo, credits, and local heat"},
    {"Textiles 08", "Trade textiles #8: changes cargo, credits, and local heat"},
};

static const CardputerGameCore::NamedValue STAR_TRADER_POCKET_FRONTIER_ROWS_2[] = {
    {"Moth 01", "Fly moth #1: spends fuel and rolls a route event"},
    {"Tin 02", "Fly tin #2: spends fuel and rolls a route event"},
    {"Glass 03", "Fly glass #3: spends fuel and rolls a route event"},
    {"Drift 04", "Fly drift #4: spends fuel and rolls a route event"},
    {"Rust 05", "Fly rust #5: spends fuel and rolls a route event"},
    {"Blue 06", "Fly blue #6: spends fuel and rolls a route event"},
    {"Kelp 07", "Fly kelp #7: spends fuel and rolls a route event"},
    {"Crown 08", "Fly crown #8: spends fuel and rolls a route event"},
    {"Null 09", "Fly null #9: spends fuel and rolls a route event"},
    {"Gate 10", "Fly gate #10: spends fuel and rolls a route event"},
    {"Halo 11", "Fly halo #11: spends fuel and rolls a route event"},
    {"Moth 12", "Fly moth #12: spends fuel and rolls a route event"},
    {"Tin 13", "Fly tin #13: spends fuel and rolls a route event"},
    {"Glass 14", "Fly glass #14: spends fuel and rolls a route event"},
    {"Drift 15", "Fly drift #15: spends fuel and rolls a route event"},
    {"Rust 16", "Fly rust #16: spends fuel and rolls a route event"},
    {"Blue 17", "Fly blue #17: spends fuel and rolls a route event"},
    {"Kelp 18", "Fly kelp #18: spends fuel and rolls a route event"},
    {"Crown 19", "Fly crown #19: spends fuel and rolls a route event"},
    {"Null 20", "Fly null #20: spends fuel and rolls a route event"},
    {"Gate 21", "Fly gate #21: spends fuel and rolls a route event"},
    {"Halo 22", "Fly halo #22: spends fuel and rolls a route event"},
};

static const CardputerGameCore::NamedValue STAR_TRADER_POCKET_FRONTIER_ROWS_3[] = {
    {"Spice 01", "Sort spice #1: prevents over-capacity and flags contraband"},
    {"Water 02", "Sort water #2: prevents over-capacity and flags contraband"},
    {"Medgel 03", "Sort medgel #3: prevents over-capacity and flags contraband"},
    {"Relics 04", "Sort relics #4: prevents over-capacity and flags contraband"},
    {"Scrap 05", "Sort scrap #5: prevents over-capacity and flags contraband"},
    {"Kelp 06", "Sort kelp #6: prevents over-capacity and flags contraband"},
    {"Cores 07", "Sort cores #7: prevents over-capacity and flags contraband"},
    {"Textiles 08", "Sort textiles #8: prevents over-capacity and flags contraband"},
};

static const CardputerGameCore::NamedValue STAR_TRADER_POCKET_FRONTIER_ROWS_4[] = {
    {"Pilot 01", "Hire pilot #1: adds a compact crew modifier"},
    {"Broker 02", "Hire broker #2: adds a compact crew modifier"},
    {"Medic 03", "Hire medic #3: adds a compact crew modifier"},
    {"Mechanic 04", "Hire mechanic #4: adds a compact crew modifier"},
    {"Scout 05", "Hire scout #5: adds a compact crew modifier"},
    {"Cook 06", "Hire cook #6: adds a compact crew modifier"},
    {"Shield 07", "Hire shield #7: adds a compact crew modifier"},
    {"Archivist 08", "Hire archivist #8: adds a compact crew modifier"},
    {"Navigator 09", "Hire navigator #9: adds a compact crew modifier"},
    {"Fixer 10", "Hire fixer #10: adds a compact crew modifier"},
    {"Courier 11", "Hire courier #11: adds a compact crew modifier"},
    {"Guard 12", "Hire guard #12: adds a compact crew modifier"},
};

static const CardputerGameCore::NamedValue STAR_TRADER_POCKET_FRONTIER_ROWS_5[] = {
    {"Cargo Bay 01", "Install cargo bay #1: improves long route survival"},
    {"Fuel Tank 02", "Install fuel tank #2: improves long route survival"},
    {"O2 Recycler 03", "Install o2 recycler #3: improves long route survival"},
    {"Shield 04", "Install shield #4: improves long route survival"},
    {"Scanner 05", "Install scanner #5: improves long route survival"},
    {"Hold 06", "Install hold #6: improves long route survival"},
    {"Engine 07", "Install engine #7: improves long route survival"},
    {"Med Bay 08", "Install med bay #8: improves long route survival"},
};

static const CardputerGameCore::NamedValue STAR_TRADER_POCKET_FRONTIER_ROWS_6[] = {
    {"Artifact 01", "Study artifact #1: advances ending pressure"},
    {"Courier 02", "Study courier #2: advances ending pressure"},
    {"Faction 03", "Study faction #3: advances ending pressure"},
    {"Debt 04", "Study debt #4: advances ending pressure"},
    {"Crew 05", "Study crew #5: advances ending pressure"},
    {"Relic 06", "Study relic #6: advances ending pressure"},
    {"Patrol 07", "Study patrol #7: advances ending pressure"},
    {"Salvage 08", "Study salvage #8: advances ending pressure"},
    {"Gate 09", "Study gate #9: advances ending pressure"},
    {"Rumor 10", "Study rumor #10: advances ending pressure"},
};

static const CardputerGameCore::DeepScreenDef SCREENS[] = {
    {"BRIDGE", "Command", STAR_TRADER_POCKET_FRONTIER_ROWS_0, 8, {1, -1, -1, 0, 1, 0, 1, -1}, 3, 0, 1UL},
    {"MARKET", "Trade", STAR_TRADER_POCKET_FRONTIER_ROWS_1, 8, {5, 0, 0, 0, 1, 2, 0, -1}, 4, 1, 2UL},
    {"ROUTES", "Jump", STAR_TRADER_POCKET_FRONTIER_ROWS_2, 22, {3, -5, -4, -2, 2, 0, -1, -1}, 6, 2, 4UL},
    {"CARGO", "Manage", STAR_TRADER_POCKET_FRONTIER_ROWS_3, 8, {2, 0, 0, 0, -1, -2, 1, 0}, 2, 3, 8UL},
    {"CREW", "Hire", STAR_TRADER_POCKET_FRONTIER_ROWS_4, 12, {-8, 0, 1, 1, -1, 0, 2, 0}, 4, 4, 16UL},
    {"SHIP", "Upgrade", STAR_TRADER_POCKET_FRONTIER_ROWS_5, 8, {-12, 4, 4, 4, -1, 4, 1, -1}, 5, 5, 32UL},
    {"LOG", "Trace", STAR_TRADER_POCKET_FRONTIER_ROWS_6, 10, {1, 0, 0, 0, -1, 0, 1, -2}, 3, 6, 64UL},
};

static const CardputerGameCore::NamedValue EVENTS[] = {
    {"Pressure 01", "A bespoke Star Trader event changes saved state and story flags."},
    {"Choice 02", "A bespoke Star Trader event changes saved state and story flags."},
    {"Discovery 03", "A bespoke Star Trader event changes saved state and story flags."},
    {"Trouble 04", "A bespoke Star Trader event changes saved state and story flags."},
    {"Favor 05", "A bespoke Star Trader event changes saved state and story flags."},
    {"Warning 06", "A bespoke Star Trader event changes saved state and story flags."},
    {"Breakthrough 07", "A bespoke Star Trader event changes saved state and story flags."},
    {"Setback 08", "A bespoke Star Trader event changes saved state and story flags."},
    {"Pressure 09", "A bespoke Star Trader event changes saved state and story flags."},
    {"Choice 10", "A bespoke Star Trader event changes saved state and story flags."},
    {"Discovery 11", "A bespoke Star Trader event changes saved state and story flags."},
    {"Trouble 12", "A bespoke Star Trader event changes saved state and story flags."},
    {"Favor 13", "A bespoke Star Trader event changes saved state and story flags."},
    {"Warning 14", "A bespoke Star Trader event changes saved state and story flags."},
    {"Breakthrough 15", "A bespoke Star Trader event changes saved state and story flags."},
    {"Setback 16", "A bespoke Star Trader event changes saved state and story flags."},
    {"Pressure 17", "A bespoke Star Trader event changes saved state and story flags."},
    {"Choice 18", "A bespoke Star Trader event changes saved state and story flags."},
};

static const CardputerGameCore::NamedValue ENDINGS[] = {
    {"Legend Run", "Ending path 1: resources, flags, and reputation decide this outcome."},
    {"Hard Truth", "Ending path 2: resources, flags, and reputation decide this outcome."},
    {"Quiet Win", "Ending path 3: resources, flags, and reputation decide this outcome."},
    {"Faction Path", "Ending path 4: resources, flags, and reputation decide this outcome."},
    {"Costly Deal", "Ending path 5: resources, flags, and reputation decide this outcome."},
    {"Run Failed", "Ending path 6: resources, flags, and reputation decide this outcome."},
};

static const CardputerGameCore::DeepGameDefinition DEF = {
    "Star Trader",
    "star-trader-pocket-frontier",
    "Ship-console trading sim",
    "STPF2",
    RESOURCES,
    sizeof(RESOURCES) / sizeof(RESOURCES[0]),
    SCREENS,
    sizeof(SCREENS) / sizeof(SCREENS[0]),
    EVENTS,
    sizeof(EVENTS) / sizeof(EVENTS[0]),
    ENDINGS,
    sizeof(ENDINGS) / sizeof(ENDINGS[0]),
    180,
    0x07FF,
    0xFD20
};

const CardputerGameCore::DeepGameDefinition& gameDefinition() {
  return DEF;
}
