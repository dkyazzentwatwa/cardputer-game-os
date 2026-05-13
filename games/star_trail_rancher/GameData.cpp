#include "GameData.h"

static const CardputerGameCore::ResourceDef RESOURCES[] = {
    {"Cred", 70, 0, 999, 5, -3},
    {"Fuel", 50, 0, 120, -4, 7},
    {"O2", 60, 0, 120, -3, 6},
    {"Feed", 45, 0, 120, -4, 6},
    {"Med", 5, 0, 99, -1, 1},
    {"Mor", 70, 0, 100, -2, 4},
    {"Hull", 90, 0, 100, -2, 5},
};

static const CardputerGameCore::NamedValue STAR_TRAIL_RANCHER_ROWS_0[] = {
    {"Hatch 01", "Review hatch #1: sets route context"},
    {"Cold 02", "Review cold #2: sets route context"},
    {"Heat 03", "Review heat #3: sets route context"},
    {"Wet 04", "Review wet #4: sets route context"},
    {"Low-G 05", "Review low-g #5: sets route context"},
    {"Quar 06", "Review quar #6: sets route context"},
    {"Sanctuary 07", "Review sanctuary #7: sets route context"},
    {"Market 08", "Review market #8: sets route context"},
    {"Black 09", "Review black #9: sets route context"},
    {"Gate 10", "Review gate #10: sets route context"},
    {"Nursery 11", "Review nursery #11: sets route context"},
    {"Clinic 12", "Review clinic #12: sets route context"},
};

static const CardputerGameCore::NamedValue STAR_TRAIL_RANCHER_ROWS_1[] = {
    {"Transport 01", "Accept transport #1: loads creature cargo"},
    {"Rescue 02", "Accept rescue #2: loads creature cargo"},
    {"Research 03", "Accept research #3: loads creature cargo"},
    {"Sanctuary 04", "Accept sanctuary #4: loads creature cargo"},
    {"Market 05", "Accept market #5: loads creature cargo"},
    {"Loan 06", "Accept loan #6: loads creature cargo"},
    {"Transport 07", "Accept transport #7: loads creature cargo"},
    {"Rescue 08", "Accept rescue #8: loads creature cargo"},
    {"Research 09", "Accept research #9: loads creature cargo"},
    {"Sanctuary 10", "Accept sanctuary #10: loads creature cargo"},
    {"Market 11", "Accept market #11: loads creature cargo"},
    {"Loan 12", "Accept loan #12: loads creature cargo"},
    {"Transport 13", "Accept transport #13: loads creature cargo"},
    {"Rescue 14", "Accept rescue #14: loads creature cargo"},
    {"Research 15", "Accept research #15: loads creature cargo"},
    {"Sanctuary 16", "Accept sanctuary #16: loads creature cargo"},
    {"Market 17", "Accept market #17: loads creature cargo"},
    {"Loan 18", "Accept loan #18: loads creature cargo"},
};

static const CardputerGameCore::NamedValue STAR_TRAIL_RANCHER_ROWS_2[] = {
    {"Grazer 01", "Tend grazer #1: checks habitat fit"},
    {"Climber 02", "Tend climber #2: checks habitat fit"},
    {"Wetling 03", "Tend wetling #3: checks habitat fit"},
    {"Quillox 04", "Tend quillox #4: checks habitat fit"},
    {"Glowcalf 05", "Tend glowcalf #5: checks habitat fit"},
    {"Frostfin 06", "Tend frostfin #6: checks habitat fit"},
    {"Mire Pup 07", "Tend mire pup #7: checks habitat fit"},
    {"Starling 08", "Tend starling #8: checks habitat fit"},
    {"Grazer 09", "Tend grazer #9: checks habitat fit"},
    {"Climber 10", "Tend climber #10: checks habitat fit"},
    {"Wetling 11", "Tend wetling #11: checks habitat fit"},
    {"Quillox 12", "Tend quillox #12: checks habitat fit"},
};

static const CardputerGameCore::NamedValue STAR_TRAIL_RANCHER_ROWS_3[] = {
    {"Short 01", "Travel short #1: applies live cargo decay"},
    {"Long 02", "Travel long #2: applies live cargo decay"},
    {"Quar 03", "Travel quar #3: applies live cargo decay"},
    {"Storm 04", "Travel storm #4: applies live cargo decay"},
    {"Gate 05", "Travel gate #5: applies live cargo decay"},
    {"Moon 06", "Travel moon #6: applies live cargo decay"},
    {"Short 07", "Travel short #7: applies live cargo decay"},
    {"Long 08", "Travel long #8: applies live cargo decay"},
    {"Quar 09", "Travel quar #9: applies live cargo decay"},
    {"Storm 10", "Travel storm #10: applies live cargo decay"},
    {"Gate 11", "Travel gate #11: applies live cargo decay"},
    {"Moon 12", "Travel moon #12: applies live cargo decay"},
    {"Short 13", "Travel short #13: applies live cargo decay"},
    {"Long 14", "Travel long #14: applies live cargo decay"},
    {"Quar 15", "Travel quar #15: applies live cargo decay"},
    {"Storm 16", "Travel storm #16: applies live cargo decay"},
    {"Gate 17", "Travel gate #17: applies live cargo decay"},
    {"Moon 18", "Travel moon #18: applies live cargo decay"},
};

static const CardputerGameCore::NamedValue STAR_TRAIL_RANCHER_ROWS_4[] = {
    {"Feed 01", "Buy feed #1: prepares ethical transport"},
    {"Med Gel 02", "Buy med gel #2: prepares ethical transport"},
    {"Oxygen 03", "Buy oxygen #3: prepares ethical transport"},
    {"Papers 04", "Buy papers #4: prepares ethical transport"},
    {"Habitat 05", "Buy habitat #5: prepares ethical transport"},
    {"Filter 06", "Buy filter #6: prepares ethical transport"},
    {"Feed 07", "Buy feed #7: prepares ethical transport"},
    {"Med Gel 08", "Buy med gel #8: prepares ethical transport"},
    {"Oxygen 09", "Buy oxygen #9: prepares ethical transport"},
    {"Papers 10", "Buy papers #10: prepares ethical transport"},
    {"Habitat 11", "Buy habitat #11: prepares ethical transport"},
    {"Filter 12", "Buy filter #12: prepares ethical transport"},
};

static const CardputerGameCore::NamedValue STAR_TRAIL_RANCHER_ROWS_5[] = {
    {"Egg 01", "Record egg #1: sets discovery and ending flags"},
    {"Permit 02", "Record permit #2: sets discovery and ending flags"},
    {"Sanctuary 03", "Record sanctuary #3: sets discovery and ending flags"},
    {"Black Port 04", "Record black port #4: sets discovery and ending flags"},
    {"Rare Buyer 05", "Record rare buyer #5: sets discovery and ending flags"},
    {"Bond 06", "Record bond #6: sets discovery and ending flags"},
    {"Egg 07", "Record egg #7: sets discovery and ending flags"},
    {"Permit 08", "Record permit #8: sets discovery and ending flags"},
    {"Sanctuary 09", "Record sanctuary #9: sets discovery and ending flags"},
    {"Black Port 10", "Record black port #10: sets discovery and ending flags"},
    {"Rare Buyer 11", "Record rare buyer #11: sets discovery and ending flags"},
    {"Bond 12", "Record bond #12: sets discovery and ending flags"},
};

static const CardputerGameCore::DeepScreenDef SCREENS[] = {
    {"BRIDGE", "Review", STAR_TRAIL_RANCHER_ROWS_0, 12, {1, 0, 0, -1, 0, 1, 0, 0}, 3, 0, 1UL},
    {"JOBS", "Accept", STAR_TRAIL_RANCHER_ROWS_1, 18, {7, -1, -1, -2, 0, 1, -1, 0}, 5, 1, 2UL},
    {"HAB", "Tend", STAR_TRAIL_RANCHER_ROWS_2, 12, {-1, 0, -1, -3, -1, 3, 0, 0}, 3, 2, 4UL},
    {"ROUTE", "Travel", STAR_TRAIL_RANCHER_ROWS_3, 18, {6, -5, -4, -5, -1, -3, -2, 0}, 5, 3, 8UL},
    {"MARKET", "Buy", STAR_TRAIL_RANCHER_ROWS_4, 12, {-7, 2, 3, 3, 1, 1, 1, 0}, 4, 4, 16UL},
    {"LOG", "Record", STAR_TRAIL_RANCHER_ROWS_5, 12, {1, 0, 0, 0, 0, 2, 0, 0}, 3, 5, 32UL},
};

static const CardputerGameCore::NamedValue EVENTS[] = {
    {"Pressure 01", "A bespoke Star Trail Ranch event changes saved state and story flags."},
    {"Choice 02", "A bespoke Star Trail Ranch event changes saved state and story flags."},
    {"Discovery 03", "A bespoke Star Trail Ranch event changes saved state and story flags."},
    {"Trouble 04", "A bespoke Star Trail Ranch event changes saved state and story flags."},
    {"Favor 05", "A bespoke Star Trail Ranch event changes saved state and story flags."},
    {"Warning 06", "A bespoke Star Trail Ranch event changes saved state and story flags."},
    {"Breakthrough 07", "A bespoke Star Trail Ranch event changes saved state and story flags."},
    {"Setback 08", "A bespoke Star Trail Ranch event changes saved state and story flags."},
    {"Pressure 09", "A bespoke Star Trail Ranch event changes saved state and story flags."},
    {"Choice 10", "A bespoke Star Trail Ranch event changes saved state and story flags."},
    {"Discovery 11", "A bespoke Star Trail Ranch event changes saved state and story flags."},
    {"Trouble 12", "A bespoke Star Trail Ranch event changes saved state and story flags."},
    {"Favor 13", "A bespoke Star Trail Ranch event changes saved state and story flags."},
    {"Warning 14", "A bespoke Star Trail Ranch event changes saved state and story flags."},
    {"Breakthrough 15", "A bespoke Star Trail Ranch event changes saved state and story flags."},
    {"Setback 16", "A bespoke Star Trail Ranch event changes saved state and story flags."},
    {"Pressure 17", "A bespoke Star Trail Ranch event changes saved state and story flags."},
    {"Choice 18", "A bespoke Star Trail Ranch event changes saved state and story flags."},
    {"Discovery 19", "A bespoke Star Trail Ranch event changes saved state and story flags."},
    {"Trouble 20", "A bespoke Star Trail Ranch event changes saved state and story flags."},
    {"Favor 21", "A bespoke Star Trail Ranch event changes saved state and story flags."},
    {"Warning 22", "A bespoke Star Trail Ranch event changes saved state and story flags."},
    {"Breakthrough 23", "A bespoke Star Trail Ranch event changes saved state and story flags."},
    {"Setback 24", "A bespoke Star Trail Ranch event changes saved state and story flags."},
};

static const CardputerGameCore::NamedValue ENDINGS[] = {
    {"Sanctuary Fleet", "Ending path 1: resources, flags, and reputation decide this outcome."},
    {"Corp Ranch", "Ending path 2: resources, flags, and reputation decide this outcome."},
    {"Smuggler Hold", "Ending path 3: resources, flags, and reputation decide this outcome."},
    {"Discovery Ark", "Ending path 4: resources, flags, and reputation decide this outcome."},
    {"Creature Riot", "Ending path 5: resources, flags, and reputation decide this outcome."},
    {"Stranded Ship", "Ending path 6: resources, flags, and reputation decide this outcome."},
};

static const CardputerGameCore::DeepGameDefinition DEF = {
    "Star Trail Ranch",
    "star-trail-rancher",
    "Creature transport sim",
    "STR2",
    RESOURCES,
    sizeof(RESOURCES) / sizeof(RESOURCES[0]),
    SCREENS,
    sizeof(SCREENS) / sizeof(SCREENS[0]),
    EVENTS,
    sizeof(EVENTS) / sizeof(EVENTS[0]),
    ENDINGS,
    sizeof(ENDINGS) / sizeof(ENDINGS[0]),
    170,
    0xF81F,
    0xFD20
};

const CardputerGameCore::DeepGameDefinition& gameDefinition() {
  return DEF;
}
