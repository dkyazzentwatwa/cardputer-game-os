#include "GameData.h"

static const CardputerGameCore::ResourceDef RESOURCES[] = {
    {"Cred", 25, 0, 999, 8, -5},
    {"Heat", 4, 0, 100, 5, -8},
    {"Deck", 90, 0, 100, -4, 7},
    {"Batt", 80, 0, 100, -7, 12},
    {"Street", 10, 0, 99, 2, 0},
    {"Files", 0, 0, 99, 2, 0},
    {"Ghost", 0, 0, 99, 1, 0},
};

static const CardputerGameCore::NamedValue SIGNAL_RAT_CYBERDECK_RPG_ROWS_0[] = {
    {"Streetline 01", "Rest streetline #1: sets active lead"},
    {"Neon Crown 02", "Rest neon crown #2: sets active lead"},
    {"Ghost Relay 03", "Rest ghost relay #3: sets active lead"},
    {"Debt 04", "Rest debt #4: sets active lead"},
    {"Blackbox 05", "Rest blackbox #5: sets active lead"},
    {"Patch 06", "Rest patch #6: sets active lead"},
    {"Fence 07", "Rest fence #7: sets active lead"},
    {"Clinic 08", "Rest clinic #8: sets active lead"},
};

static const CardputerGameCore::NamedValue SIGNAL_RAT_CYBERDECK_RPG_ROWS_1[] = {
    {"Rat Trap 01", "Take rat trap #1: loads a fictional contract"},
    {"Ledger Node 02", "Take ledger node #2: loads a fictional contract"},
    {"Crown Cache 03", "Take crown cache #3: loads a fictional contract"},
    {"Relay 17 04", "Take relay 17 #4: loads a fictional contract"},
    {"Clinic Queue 05", "Take clinic queue #5: loads a fictional contract"},
    {"Dead Mall 06", "Take dead mall #6: loads a fictional contract"},
    {"Servo Pawn 07", "Take servo pawn #7: loads a fictional contract"},
    {"Weather Feed 08", "Take weather feed #8: loads a fictional contract"},
    {"Rat Trap 09", "Take rat trap #9: loads a fictional contract"},
    {"Ledger Node 10", "Take ledger node #10: loads a fictional contract"},
    {"Crown Cache 11", "Take crown cache #11: loads a fictional contract"},
    {"Relay 17 12", "Take relay 17 #12: loads a fictional contract"},
    {"Clinic Queue 13", "Take clinic queue #13: loads a fictional contract"},
    {"Dead Mall 14", "Take dead mall #14: loads a fictional contract"},
    {"Servo Pawn 15", "Take servo pawn #15: loads a fictional contract"},
};

static const CardputerGameCore::NamedValue SIGNAL_RAT_CYBERDECK_RPG_ROWS_2[] = {
    {"Scan 01", "Run scan #1: changes simulated node pressure"},
    {"Decrypt 02", "Run decrypt #2: changes simulated node pressure"},
    {"Spoof 03", "Run spoof #3: changes simulated node pressure"},
    {"Inject 04", "Run inject #4: changes simulated node pressure"},
    {"Trace 05", "Run trace #5: changes simulated node pressure"},
    {"Disconnect 06", "Run disconnect #6: changes simulated node pressure"},
    {"Blackbox 07", "Run blackbox #7: changes simulated node pressure"},
    {"Scrub 08", "Run scrub #8: changes simulated node pressure"},
};

static const CardputerGameCore::NamedValue SIGNAL_RAT_CYBERDECK_RPG_ROWS_3[] = {
    {"Ghost Echo 01", "Recover ghost echo #1: sets final chain flags"},
    {"Crown Debt 02", "Recover crown debt #2: sets final chain flags"},
    {"Street Map 03", "Recover street map #3: sets final chain flags"},
    {"Relay Pact 04", "Recover relay pact #4: sets final chain flags"},
    {"Clinic Bill 05", "Recover clinic bill #5: sets final chain flags"},
    {"Weather Lie 06", "Recover weather lie #6: sets final chain flags"},
    {"Mall Tape 07", "Recover mall tape #7: sets final chain flags"},
    {"Servo Key 08", "Recover servo key #8: sets final chain flags"},
    {"Old Signal 09", "Recover old signal #9: sets final chain flags"},
    {"Final Chain 10", "Recover final chain #10: sets final chain flags"},
};

static const CardputerGameCore::NamedValue SIGNAL_RAT_CYBERDECK_RPG_ROWS_4[] = {
    {"Cold CPU 01", "Upgrade cold cpu #1: ranks up deck modules"},
    {"Spoof Chip 02", "Upgrade spoof chip #2: ranks up deck modules"},
    {"Battery Stack 03", "Upgrade battery stack #3: ranks up deck modules"},
    {"Blackbox Drive 04", "Upgrade blackbox drive #4: ranks up deck modules"},
    {"Cold CPU 05", "Upgrade cold cpu #5: ranks up deck modules"},
    {"Spoof Chip 06", "Upgrade spoof chip #6: ranks up deck modules"},
    {"Battery Stack 07", "Upgrade battery stack #7: ranks up deck modules"},
    {"Blackbox Drive 08", "Upgrade blackbox drive #8: ranks up deck modules"},
};

static const CardputerGameCore::DeepScreenDef SCREENS[] = {
    {"SAFEHOUSE", "Rest", SIGNAL_RAT_CYBERDECK_RPG_ROWS_0, 8, {1, -5, 5, 6, 1, 0, 1, 0}, 3, 0, 1UL},
    {"JOB BOARD", "Take", SIGNAL_RAT_CYBERDECK_RPG_ROWS_1, 15, {8, 4, -3, -5, 2, 1, 1, 0}, 5, 1, 2UL},
    {"NODE", "Run", SIGNAL_RAT_CYBERDECK_RPG_ROWS_2, 8, {4, 5, -4, -6, 1, 2, 2, 0}, 5, 2, 4UL},
    {"FILES", "Recover", SIGNAL_RAT_CYBERDECK_RPG_ROWS_3, 10, {2, 1, 0, -1, 1, 4, 2, 0}, 4, 3, 8UL},
    {"DECK", "Upgrade", SIGNAL_RAT_CYBERDECK_RPG_ROWS_4, 8, {-8, -2, 5, 6, 1, 1, 1, 0}, 4, 4, 16UL},
};

static const CardputerGameCore::NamedValue EVENTS[] = {
    {"Pressure 01", "A bespoke Signal Rat event changes saved state and story flags."},
    {"Choice 02", "A bespoke Signal Rat event changes saved state and story flags."},
    {"Discovery 03", "A bespoke Signal Rat event changes saved state and story flags."},
    {"Trouble 04", "A bespoke Signal Rat event changes saved state and story flags."},
    {"Favor 05", "A bespoke Signal Rat event changes saved state and story flags."},
    {"Warning 06", "A bespoke Signal Rat event changes saved state and story flags."},
    {"Breakthrough 07", "A bespoke Signal Rat event changes saved state and story flags."},
    {"Setback 08", "A bespoke Signal Rat event changes saved state and story flags."},
    {"Pressure 09", "A bespoke Signal Rat event changes saved state and story flags."},
    {"Choice 10", "A bespoke Signal Rat event changes saved state and story flags."},
    {"Discovery 11", "A bespoke Signal Rat event changes saved state and story flags."},
    {"Trouble 12", "A bespoke Signal Rat event changes saved state and story flags."},
    {"Favor 13", "A bespoke Signal Rat event changes saved state and story flags."},
    {"Warning 14", "A bespoke Signal Rat event changes saved state and story flags."},
    {"Breakthrough 15", "A bespoke Signal Rat event changes saved state and story flags."},
    {"Setback 16", "A bespoke Signal Rat event changes saved state and story flags."},
    {"Pressure 17", "A bespoke Signal Rat event changes saved state and story flags."},
    {"Choice 18", "A bespoke Signal Rat event changes saved state and story flags."},
};

static const CardputerGameCore::NamedValue ENDINGS[] = {
    {"Clean Ghost", "Ending path 1: resources, flags, and reputation decide this outcome."},
    {"Rich Crown", "Ending path 2: resources, flags, and reputation decide this outcome."},
    {"Street Legend", "Ending path 3: resources, flags, and reputation decide this outcome."},
    {"Relay Pact", "Ending path 4: resources, flags, and reputation decide this outcome."},
    {"Debt Lock", "Ending path 5: resources, flags, and reputation decide this outcome."},
    {"Deck Burnout", "Ending path 6: resources, flags, and reputation decide this outcome."},
};

static const CardputerGameCore::DeepGameDefinition DEF = {
    "Signal Rat",
    "signal-rat-cyberdeck-rpg",
    "Flagship fictional cyberdeck",
    "SRAT2",
    RESOURCES,
    sizeof(RESOURCES) / sizeof(RESOURCES[0]),
    SCREENS,
    sizeof(SCREENS) / sizeof(SCREENS[0]),
    EVENTS,
    sizeof(EVENTS) / sizeof(EVENTS[0]),
    ENDINGS,
    sizeof(ENDINGS) / sizeof(ENDINGS[0]),
    165,
    0x07FF,
    0xFD20
};

const CardputerGameCore::DeepGameDefinition& gameDefinition() {
  return DEF;
}
