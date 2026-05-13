#include "GameData.h"

static const CardputerGameCore::ResourceDef RESOURCES[] = {
    {"Money", 60, 0, 999, 4, -3},
    {"Feed", 40, 0, 120, -3, 6},
    {"Meds", 5, 0, 99, -1, 1},
    {"Rep", 10, 0, 99, 2, 0},
    {"Bond", 20, 0, 100, 2, 3},
    {"Fatigue", 0, 0, 100, 5, -8},
    {"Tier", 1, 0, 9, 1, 0},
    {"Weather", 0, 0, 9, 1, 1},
};

static const CardputerGameCore::NamedValue MONSTER_RANCH_TRAIL_ROWS_0[] = {
    {"Spring 01", "Plan spring #1: updates ranch pressure"},
    {"Rain 02", "Plan rain #2: updates ranch pressure"},
    {"Heat 03", "Plan heat #3: updates ranch pressure"},
    {"Fair 04", "Plan fair #4: updates ranch pressure"},
    {"Storm 05", "Plan storm #5: updates ranch pressure"},
    {"Quiet 06", "Plan quiet #6: updates ranch pressure"},
    {"Spring 07", "Plan spring #7: updates ranch pressure"},
    {"Rain 08", "Plan rain #8: updates ranch pressure"},
    {"Heat 09", "Plan heat #9: updates ranch pressure"},
    {"Fair 10", "Plan fair #10: updates ranch pressure"},
    {"Storm 11", "Plan storm #11: updates ranch pressure"},
    {"Quiet 12", "Plan quiet #12: updates ranch pressure"},
};

static const CardputerGameCore::NamedValue MONSTER_RANCH_TRAIL_ROWS_1[] = {
    {"Spriglet 01", "Care spriglet #1: tracks condition and bond"},
    {"Cinderpup 02", "Care cinderpup #2: tracks condition and bond"},
    {"Pebblor 03", "Care pebblor #3: tracks condition and bond"},
    {"Mireling 04", "Care mireling #4: tracks condition and bond"},
    {"Fusefox 05", "Care fusefox #5: tracks condition and bond"},
    {"Quillox 06", "Care quillox #6: tracks condition and bond"},
    {"Snowcalf 07", "Care snowcalf #7: tracks condition and bond"},
    {"Moonmoth 08", "Care moonmoth #8: tracks condition and bond"},
    {"Chrome Pup 09", "Care chrome pup #9: tracks condition and bond"},
    {"Spriglet 10", "Care spriglet #10: tracks condition and bond"},
    {"Cinderpup 11", "Care cinderpup #11: tracks condition and bond"},
    {"Pebblor 12", "Care pebblor #12: tracks condition and bond"},
    {"Mireling 13", "Care mireling #13: tracks condition and bond"},
    {"Fusefox 14", "Care fusefox #14: tracks condition and bond"},
    {"Quillox 15", "Care quillox #15: tracks condition and bond"},
    {"Snowcalf 16", "Care snowcalf #16: tracks condition and bond"},
    {"Moonmoth 17", "Care moonmoth #17: tracks condition and bond"},
    {"Chrome Pup 18", "Care chrome pup #18: tracks condition and bond"},
};

static const CardputerGameCore::NamedValue MONSTER_RANCH_TRAIL_ROWS_2[] = {
    {"Power 01", "Train power #1: adds stat growth and fatigue"},
    {"Grit 02", "Train grit #2: adds stat growth and fatigue"},
    {"Speed 03", "Train speed #3: adds stat growth and fatigue"},
    {"Focus 04", "Train focus #4: adds stat growth and fatigue"},
    {"Bond 05", "Train bond #5: adds stat growth and fatigue"},
    {"Power 06", "Train power #6: adds stat growth and fatigue"},
    {"Grit 07", "Train grit #7: adds stat growth and fatigue"},
    {"Speed 08", "Train speed #8: adds stat growth and fatigue"},
    {"Focus 09", "Train focus #9: adds stat growth and fatigue"},
    {"Bond 10", "Train bond #10: adds stat growth and fatigue"},
};

static const CardputerGameCore::NamedValue MONSTER_RANCH_TRAIL_ROWS_3[] = {
    {"Pine 01", "Explore pine #1: risks weather for discoveries"},
    {"Warm 02", "Explore warm #2: risks weather for discoveries"},
    {"Mire 03", "Explore mire #3: risks weather for discoveries"},
    {"Glass 04", "Explore glass #4: risks weather for discoveries"},
    {"Fuse 05", "Explore fuse #5: risks weather for discoveries"},
    {"Snow 06", "Explore snow #6: risks weather for discoveries"},
    {"Moon 07", "Explore moon #7: risks weather for discoveries"},
    {"Chrome 08", "Explore chrome #8: risks weather for discoveries"},
};

static const CardputerGameCore::NamedValue MONSTER_RANCH_TRAIL_ROWS_4[] = {
    {"Stable 01", "Build stable #1: improves ranch capability"},
    {"Clinic 02", "Build clinic #2: improves ranch capability"},
    {"Yard 03", "Build yard #3: improves ranch capability"},
    {"Garden 04", "Build garden #4: improves ranch capability"},
    {"Workshop 05", "Build workshop #5: improves ranch capability"},
    {"Nursery 06", "Build nursery #6: improves ranch capability"},
    {"Lookout 07", "Build lookout #7: improves ranch capability"},
    {"Vane 08", "Build vane #8: improves ranch capability"},
    {"Pen 09", "Build pen #9: improves ranch capability"},
    {"Stable 10", "Build stable #10: improves ranch capability"},
    {"Clinic 11", "Build clinic #11: improves ranch capability"},
    {"Yard 12", "Build yard #12: improves ranch capability"},
    {"Garden 13", "Build garden #13: improves ranch capability"},
    {"Workshop 14", "Build workshop #14: improves ranch capability"},
    {"Nursery 15", "Build nursery #15: improves ranch capability"},
    {"Lookout 16", "Build lookout #16: improves ranch capability"},
    {"Vane 17", "Build vane #17: improves ranch capability"},
    {"Pen 18", "Build pen #18: improves ranch capability"},
    {"Stable 19", "Build stable #19: improves ranch capability"},
    {"Clinic 20", "Build clinic #20: improves ranch capability"},
    {"Yard 21", "Build yard #21: improves ranch capability"},
    {"Garden 22", "Build garden #22: improves ranch capability"},
    {"Workshop 23", "Build workshop #23: improves ranch capability"},
    {"Nursery 24", "Build nursery #24: improves ranch capability"},
    {"Lookout 25", "Build lookout #25: improves ranch capability"},
    {"Vane 26", "Build vane #26: improves ranch capability"},
    {"Pen 27", "Build pen #27: improves ranch capability"},
};

static const CardputerGameCore::NamedValue MONSTER_RANCH_TRAIL_ROWS_5[] = {
    {"Novice 01", "Compete novice #1: converts stats to fame"},
    {"Harvest 02", "Compete harvest #2: converts stats to fame"},
    {"Storm 03", "Compete storm #3: converts stats to fame"},
    {"Rival 04", "Compete rival #4: converts stats to fame"},
    {"Frontier 05", "Compete frontier #5: converts stats to fame"},
    {"Champion 06", "Compete champion #6: converts stats to fame"},
};

static const CardputerGameCore::NamedValue MONSTER_RANCH_TRAIL_ROWS_6[] = {
    {"Refusal 01", "Handle refusal #1: creates ranch story flags"},
    {"Storm 02", "Handle storm #2: creates ranch story flags"},
    {"Rival 03", "Handle rival #3: creates ranch story flags"},
    {"Cure 04", "Handle cure #4: creates ranch story flags"},
    {"Injury 05", "Handle injury #5: creates ranch story flags"},
    {"Bond 06", "Handle bond #6: creates ranch story flags"},
    {"Egg 07", "Handle egg #7: creates ranch story flags"},
    {"Filter 08", "Handle filter #8: creates ranch story flags"},
    {"Refusal 09", "Handle refusal #9: creates ranch story flags"},
    {"Storm 10", "Handle storm #10: creates ranch story flags"},
    {"Rival 11", "Handle rival #11: creates ranch story flags"},
    {"Cure 12", "Handle cure #12: creates ranch story flags"},
    {"Injury 13", "Handle injury #13: creates ranch story flags"},
    {"Bond 14", "Handle bond #14: creates ranch story flags"},
    {"Egg 15", "Handle egg #15: creates ranch story flags"},
    {"Filter 16", "Handle filter #16: creates ranch story flags"},
    {"Refusal 17", "Handle refusal #17: creates ranch story flags"},
    {"Storm 18", "Handle storm #18: creates ranch story flags"},
    {"Rival 19", "Handle rival #19: creates ranch story flags"},
    {"Cure 20", "Handle cure #20: creates ranch story flags"},
    {"Injury 21", "Handle injury #21: creates ranch story flags"},
    {"Bond 22", "Handle bond #22: creates ranch story flags"},
    {"Egg 23", "Handle egg #23: creates ranch story flags"},
    {"Filter 24", "Handle filter #24: creates ranch story flags"},
};

static const CardputerGameCore::DeepScreenDef SCREENS[] = {
    {"RANCH", "Plan", MONSTER_RANCH_TRAIL_ROWS_0, 12, {2, -2, 0, 1, 2, 1, 0, 1}, 3, 0, 1UL},
    {"CREATURE", "Care", MONSTER_RANCH_TRAIL_ROWS_1, 18, {-1, -2, -1, 1, 4, -3, 0, 0}, 3, 1, 2UL},
    {"TRAIN", "Train", MONSTER_RANCH_TRAIL_ROWS_2, 10, {0, -2, 0, 1, 2, 6, 0, 0}, 5, 2, 4UL},
    {"EXPED", "Explore", MONSTER_RANCH_TRAIL_ROWS_3, 8, {4, -4, -1, 2, 2, 4, 1, 1}, 5, 3, 8UL},
    {"BUILD", "Upgrade", MONSTER_RANCH_TRAIL_ROWS_4, 27, {-8, 2, 1, 1, 2, -2, 1, 0}, 5, 4, 16UL},
    {"TOURNEY", "Enter", MONSTER_RANCH_TRAIL_ROWS_5, 6, {12, -3, -1, 4, 2, 7, 1, 0}, 7, 5, 32UL},
    {"EVENT", "Choose", MONSTER_RANCH_TRAIL_ROWS_6, 24, {-2, -2, -1, 2, 3, 3, 0, 1}, 4, 6, 64UL},
};

static const CardputerGameCore::NamedValue EVENTS[] = {
    {"Pressure 01", "A bespoke Monster Ranch event changes saved state and story flags."},
    {"Choice 02", "A bespoke Monster Ranch event changes saved state and story flags."},
    {"Discovery 03", "A bespoke Monster Ranch event changes saved state and story flags."},
    {"Trouble 04", "A bespoke Monster Ranch event changes saved state and story flags."},
    {"Favor 05", "A bespoke Monster Ranch event changes saved state and story flags."},
    {"Warning 06", "A bespoke Monster Ranch event changes saved state and story flags."},
    {"Breakthrough 07", "A bespoke Monster Ranch event changes saved state and story flags."},
    {"Setback 08", "A bespoke Monster Ranch event changes saved state and story flags."},
    {"Pressure 09", "A bespoke Monster Ranch event changes saved state and story flags."},
    {"Choice 10", "A bespoke Monster Ranch event changes saved state and story flags."},
    {"Discovery 11", "A bespoke Monster Ranch event changes saved state and story flags."},
    {"Trouble 12", "A bespoke Monster Ranch event changes saved state and story flags."},
    {"Favor 13", "A bespoke Monster Ranch event changes saved state and story flags."},
    {"Warning 14", "A bespoke Monster Ranch event changes saved state and story flags."},
    {"Breakthrough 15", "A bespoke Monster Ranch event changes saved state and story flags."},
    {"Setback 16", "A bespoke Monster Ranch event changes saved state and story flags."},
    {"Pressure 17", "A bespoke Monster Ranch event changes saved state and story flags."},
    {"Choice 18", "A bespoke Monster Ranch event changes saved state and story flags."},
    {"Discovery 19", "A bespoke Monster Ranch event changes saved state and story flags."},
    {"Trouble 20", "A bespoke Monster Ranch event changes saved state and story flags."},
    {"Favor 21", "A bespoke Monster Ranch event changes saved state and story flags."},
    {"Warning 22", "A bespoke Monster Ranch event changes saved state and story flags."},
    {"Breakthrough 23", "A bespoke Monster Ranch event changes saved state and story flags."},
    {"Setback 24", "A bespoke Monster Ranch event changes saved state and story flags."},
};

static const CardputerGameCore::NamedValue ENDINGS[] = {
    {"League Winner", "Ending path 1: resources, flags, and reputation decide this outcome."},
    {"Cozy Ranch", "Ending path 2: resources, flags, and reputation decide this outcome."},
    {"Variant Archive", "Ending path 3: resources, flags, and reputation decide this outcome."},
    {"Rival Friend", "Ending path 4: resources, flags, and reputation decide this outcome."},
    {"Research Star", "Ending path 5: resources, flags, and reputation decide this outcome."},
    {"Empty Barn", "Ending path 6: resources, flags, and reputation decide this outcome."},
};

static const CardputerGameCore::DeepGameDefinition DEF = {
    "Monster Ranch",
    "monster-ranch-trail",
    "Weekly creature ranch",
    "MRAN2",
    RESOURCES,
    sizeof(RESOURCES) / sizeof(RESOURCES[0]),
    SCREENS,
    sizeof(SCREENS) / sizeof(SCREENS[0]),
    EVENTS,
    sizeof(EVENTS) / sizeof(EVENTS[0]),
    ENDINGS,
    sizeof(ENDINGS) / sizeof(ENDINGS[0]),
    170,
    0x07E0,
    0xFD20
};

const CardputerGameCore::DeepGameDefinition& gameDefinition() {
  return DEF;
}
