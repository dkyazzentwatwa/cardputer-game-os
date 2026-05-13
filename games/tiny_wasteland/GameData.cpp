#include "GameData.h"

static const CardputerGameCore::ResourceDef RESOURCES[] = {
    {"Fd", 70, 0, 999, -5, 8},
    {"Wat", 65, 0, 999, -6, 8},
    {"Med", 4, 0, 99, -1, 2},
    {"Fuel", 45, 0, 99, -4, 4},
    {"Scrap", 10, 0, 999, 3, -1},
    {"Mor", 75, 0, 100, -2, 6},
    {"Rad", 0, 0, 100, 3, -2},
    {"Veh", 85, 0, 100, -4, 5},
};

static const CardputerGameCore::NamedValue TINY_WASTELAND_ROWS_0[] = {
    {"Dust 01", "Cross dust #1: applies region hazard pressure"},
    {"Dry 02", "Cross dry #2: applies region hazard pressure"},
    {"Clinic 03", "Cross clinic #3: applies region hazard pressure"},
    {"Orchard 04", "Cross orchard #4: applies region hazard pressure"},
    {"Flats 05", "Cross flats #5: applies region hazard pressure"},
    {"Bones 06", "Cross bones #6: applies region hazard pressure"},
    {"Static 07", "Cross static #7: applies region hazard pressure"},
    {"Bunker 08", "Cross bunker #8: applies region hazard pressure"},
    {"Green 09", "Cross green #9: applies region hazard pressure"},
    {"Safe 10", "Cross safe #10: applies region hazard pressure"},
};

static const CardputerGameCore::NamedValue TINY_WASTELAND_ROWS_1[] = {
    {"Ration 01", "Settle ration #1: recovers at the cost of time"},
    {"Treat 02", "Settle treat #2: recovers at the cost of time"},
    {"Repair 03", "Settle repair #3: recovers at the cost of time"},
    {"Guard 04", "Settle guard #4: recovers at the cost of time"},
    {"Cook 05", "Settle cook #5: recovers at the cost of time"},
    {"Talk 06", "Settle talk #6: recovers at the cost of time"},
    {"Map 07", "Settle map #7: recovers at the cost of time"},
    {"Quiet 08", "Settle quiet #8: recovers at the cost of time"},
    {"Signal 09", "Settle signal #9: recovers at the cost of time"},
    {"Watch 10", "Settle watch #10: recovers at the cost of time"},
};

static const CardputerGameCore::NamedValue TINY_WASTELAND_ROWS_2[] = {
    {"Pharmacy 01", "Scavenge pharmacy #1: trades risk for loot"},
    {"Fuel Stop 02", "Scavenge fuel stop #2: trades risk for loot"},
    {"School 03", "Scavenge school #3: trades risk for loot"},
    {"Depot 04", "Scavenge depot #4: trades risk for loot"},
    {"Clinic 05", "Scavenge clinic #5: trades risk for loot"},
    {"Farm 06", "Scavenge farm #6: trades risk for loot"},
    {"Garage 07", "Scavenge garage #7: trades risk for loot"},
    {"Tower 08", "Scavenge tower #8: trades risk for loot"},
};

static const CardputerGameCore::NamedValue TINY_WASTELAND_ROWS_3[] = {
    {"Scout 01", "Tend scout #1: changes party condition"},
    {"Medic 02", "Tend medic #2: changes party condition"},
    {"Driver 03", "Tend driver #3: changes party condition"},
    {"Cook 04", "Tend cook #4: changes party condition"},
    {"Stranger 05", "Tend stranger #5: changes party condition"},
    {"Guard 06", "Tend guard #6: changes party condition"},
    {"Teacher 07", "Tend teacher #7: changes party condition"},
    {"Mechanic 08", "Tend mechanic #8: changes party condition"},
    {"Scout 09", "Tend scout #9: changes party condition"},
    {"Medic 10", "Tend medic #10: changes party condition"},
    {"Driver 11", "Tend driver #11: changes party condition"},
    {"Cook 12", "Tend cook #12: changes party condition"},
    {"Stranger 13", "Tend stranger #13: changes party condition"},
    {"Guard 14", "Tend guard #14: changes party condition"},
    {"Teacher 15", "Tend teacher #15: changes party condition"},
    {"Mechanic 16", "Tend mechanic #16: changes party condition"},
};

static const CardputerGameCore::NamedValue TINY_WASTELAND_ROWS_4[] = {
    {"League 01", "Trade league #1: converts supplies and reputation"},
    {"Baron 02", "Trade baron #2: converts supplies and reputation"},
    {"Medic 03", "Trade medic #3: converts supplies and reputation"},
    {"Scav 04", "Trade scav #4: converts supplies and reputation"},
    {"Camp 05", "Trade camp #5: converts supplies and reputation"},
    {"Depot 06", "Trade depot #6: converts supplies and reputation"},
    {"League 07", "Trade league #7: converts supplies and reputation"},
    {"Baron 08", "Trade baron #8: converts supplies and reputation"},
    {"Medic 09", "Trade medic #9: converts supplies and reputation"},
    {"Scav 10", "Trade scav #10: converts supplies and reputation"},
    {"Camp 11", "Trade camp #11: converts supplies and reputation"},
    {"Depot 12", "Trade depot #12: converts supplies and reputation"},
};

static const CardputerGameCore::NamedValue TINY_WASTELAND_ROWS_5[] = {
    {"Storm 01", "Decide storm #1: sets moral and survival flags"},
    {"Sickness 02", "Decide sickness #2: sets moral and survival flags"},
    {"Tribute 03", "Decide tribute #3: sets moral and survival flags"},
    {"Shortcut 04", "Decide shortcut #4: sets moral and survival flags"},
    {"Rescue 05", "Decide rescue #5: sets moral and survival flags"},
    {"Fog 06", "Decide fog #6: sets moral and survival flags"},
    {"Signal 07", "Decide signal #7: sets moral and survival flags"},
    {"Raid 08", "Decide raid #8: sets moral and survival flags"},
    {"Storm 09", "Decide storm #9: sets moral and survival flags"},
    {"Sickness 10", "Decide sickness #10: sets moral and survival flags"},
    {"Tribute 11", "Decide tribute #11: sets moral and survival flags"},
    {"Shortcut 12", "Decide shortcut #12: sets moral and survival flags"},
    {"Rescue 13", "Decide rescue #13: sets moral and survival flags"},
    {"Fog 14", "Decide fog #14: sets moral and survival flags"},
    {"Signal 15", "Decide signal #15: sets moral and survival flags"},
    {"Raid 16", "Decide raid #16: sets moral and survival flags"},
    {"Storm 17", "Decide storm #17: sets moral and survival flags"},
    {"Sickness 18", "Decide sickness #18: sets moral and survival flags"},
    {"Tribute 19", "Decide tribute #19: sets moral and survival flags"},
    {"Shortcut 20", "Decide shortcut #20: sets moral and survival flags"},
    {"Rescue 21", "Decide rescue #21: sets moral and survival flags"},
    {"Fog 22", "Decide fog #22: sets moral and survival flags"},
    {"Signal 23", "Decide signal #23: sets moral and survival flags"},
    {"Raid 24", "Decide raid #24: sets moral and survival flags"},
    {"Storm 25", "Decide storm #25: sets moral and survival flags"},
    {"Sickness 26", "Decide sickness #26: sets moral and survival flags"},
    {"Tribute 27", "Decide tribute #27: sets moral and survival flags"},
    {"Shortcut 28", "Decide shortcut #28: sets moral and survival flags"},
    {"Rescue 29", "Decide rescue #29: sets moral and survival flags"},
    {"Fog 30", "Decide fog #30: sets moral and survival flags"},
};

static const CardputerGameCore::DeepScreenDef SCREENS[] = {
    {"ROAD", "Drive", TINY_WASTELAND_ROWS_0, 10, {-4, -5, -1, -4, 1, -2, 2, -3}, 5, 0, 1UL},
    {"CAMP", "Rest", TINY_WASTELAND_ROWS_1, 10, {3, 3, 1, 1, -2, 5, -1, 4}, 3, 1, 2UL},
    {"SCAV", "Search", TINY_WASTELAND_ROWS_2, 8, {2, 1, 2, 2, 5, -1, 3, -2}, 5, 2, 4UL},
    {"PARTY", "Check", TINY_WASTELAND_ROWS_3, 16, {-1, -1, 1, 0, -1, 3, -1, 1}, 3, 3, 8UL},
    {"TRADE", "Barter", TINY_WASTELAND_ROWS_4, 12, {4, 4, 2, 3, -4, 0, -1, 0}, 4, 4, 16UL},
    {"EVENT", "Choose", TINY_WASTELAND_ROWS_5, 30, {-3, -3, -1, -2, 1, -3, 2, -2}, 4, 5, 32UL},
};

static const CardputerGameCore::NamedValue EVENTS[] = {
    {"Pressure 01", "A bespoke Tiny Wasteland event changes saved state and story flags."},
    {"Choice 02", "A bespoke Tiny Wasteland event changes saved state and story flags."},
    {"Discovery 03", "A bespoke Tiny Wasteland event changes saved state and story flags."},
    {"Trouble 04", "A bespoke Tiny Wasteland event changes saved state and story flags."},
    {"Favor 05", "A bespoke Tiny Wasteland event changes saved state and story flags."},
    {"Warning 06", "A bespoke Tiny Wasteland event changes saved state and story flags."},
    {"Breakthrough 07", "A bespoke Tiny Wasteland event changes saved state and story flags."},
    {"Setback 08", "A bespoke Tiny Wasteland event changes saved state and story flags."},
    {"Pressure 09", "A bespoke Tiny Wasteland event changes saved state and story flags."},
    {"Choice 10", "A bespoke Tiny Wasteland event changes saved state and story flags."},
    {"Discovery 11", "A bespoke Tiny Wasteland event changes saved state and story flags."},
    {"Trouble 12", "A bespoke Tiny Wasteland event changes saved state and story flags."},
    {"Favor 13", "A bespoke Tiny Wasteland event changes saved state and story flags."},
    {"Warning 14", "A bespoke Tiny Wasteland event changes saved state and story flags."},
    {"Breakthrough 15", "A bespoke Tiny Wasteland event changes saved state and story flags."},
    {"Setback 16", "A bespoke Tiny Wasteland event changes saved state and story flags."},
    {"Pressure 17", "A bespoke Tiny Wasteland event changes saved state and story flags."},
    {"Choice 18", "A bespoke Tiny Wasteland event changes saved state and story flags."},
    {"Discovery 19", "A bespoke Tiny Wasteland event changes saved state and story flags."},
    {"Trouble 20", "A bespoke Tiny Wasteland event changes saved state and story flags."},
    {"Favor 21", "A bespoke Tiny Wasteland event changes saved state and story flags."},
    {"Warning 22", "A bespoke Tiny Wasteland event changes saved state and story flags."},
    {"Breakthrough 23", "A bespoke Tiny Wasteland event changes saved state and story flags."},
    {"Setback 24", "A bespoke Tiny Wasteland event changes saved state and story flags."},
    {"Pressure 25", "A bespoke Tiny Wasteland event changes saved state and story flags."},
    {"Choice 26", "A bespoke Tiny Wasteland event changes saved state and story flags."},
    {"Discovery 27", "A bespoke Tiny Wasteland event changes saved state and story flags."},
    {"Trouble 28", "A bespoke Tiny Wasteland event changes saved state and story flags."},
    {"Favor 29", "A bespoke Tiny Wasteland event changes saved state and story flags."},
    {"Warning 30", "A bespoke Tiny Wasteland event changes saved state and story flags."},
};

static const CardputerGameCore::NamedValue ENDINGS[] = {
    {"Safe-Zone", "Ending path 1: resources, flags, and reputation decide this outcome."},
    {"Faction Road", "Ending path 2: resources, flags, and reputation decide this outcome."},
    {"Lone Survivor", "Ending path 3: resources, flags, and reputation decide this outcome."},
    {"Convoy Dream", "Ending path 4: resources, flags, and reputation decide this outcome."},
    {"Camp Founded", "Ending path 5: resources, flags, and reputation decide this outcome."},
    {"Signal Trap", "Ending path 6: resources, flags, and reputation decide this outcome."},
    {"Collapse", "Ending path 7: resources, flags, and reputation decide this outcome."},
};

static const CardputerGameCore::DeepGameDefinition DEF = {
    "Tiny Wasteland",
    "tiny-wasteland",
    "Survival road drama",
    "TWST2",
    RESOURCES,
    sizeof(RESOURCES) / sizeof(RESOURCES[0]),
    SCREENS,
    sizeof(SCREENS) / sizeof(SCREENS[0]),
    EVENTS,
    sizeof(EVENTS) / sizeof(EVENTS[0]),
    ENDINGS,
    sizeof(ENDINGS) / sizeof(ENDINGS[0]),
    175,
    0xFD20,
    0xFD20
};

const CardputerGameCore::DeepGameDefinition& gameDefinition() {
  return DEF;
}
