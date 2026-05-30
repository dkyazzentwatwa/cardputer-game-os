#include "GameData.h"

namespace CGC = CardputerGameCore;

// Resource indices (keep in sync with RESOURCES order).
enum {
  R_FOOD, R_WATER, R_MEDS, R_AMMO,
  R_SCRAP, R_POWER, R_MORALE, R_DEFENSE
};

// Story flags. Screens set these; events/endings gate on them.
enum : uint32_t {
  F_TEAM_FORMED   = 1u << 0,  // a scavenger team has been assigned
  F_CLINIC_BUILT  = 1u << 1,  // infirmary upgraded, can treat radiation
  F_RADIO_BUILT   = 1u << 2,  // radio tower constructed, regions unlocked
  F_WALLS_BUILT   = 1u << 3,  // walls up, defense threshold raised
  F_FACTION_ALLY  = 1u << 4,  // allied with one faction (river medics)
  F_SIGNAL_FOUND  = 1u << 5,  // radio signal contact made
  F_PLAN_CONVOY   = 1u << 6,  // chose mobile convoy as endgame plan
  F_PLAN_BEACON   = 1u << 7,  // chose signal beacon as endgame plan
  F_PLAN_BUNKER   = 1u << 8,  // chose bunker isolation as endgame plan
  F_ENDGAME       = 1u << 9,  // survival plan activated (gated endgame)
};

// ---------------------------------------------------------------------------
// Resources
// ---------------------------------------------------------------------------
static const CGC::ResourceDef RESOURCES[] = {
  // label   start  min  max  prim  safe  warn  bad   inverted
  {"Food",    55,   0,  999,  -3,    5,    18,   8,  false},  // loss trigger
  {"Water",   50,   0,  999,   0,    4,    15,   6,  false},
  {"Meds",    12,   0,   99,   0,    0,     6,   2,  false},
  {"Ammo",    22,   0,   99,   0,    0,     8,   3,  false},
  {"Scrap",   30,   0,  999,   0,    0,    -1,  -1,  false},  // currency
  {"Power",   20,   0,  120,   0,    0,     6,   2,  false},
  {"Morale",  60,   0,  100,   0,    0,    25,  12,  false},
  {"Defense", 15,   0,  100,   0,    0,     8,   3,  false},
};

// ---------------------------------------------------------------------------
// SETTLEMENT screen — weekly review, ration planning, morale calls
// ---------------------------------------------------------------------------
static const CGC::NamedValue ROWS_SETTLEMENT[] = {
  {"Ration Review",    "Audit stores; balance food vs headcount."},
  {"Water Check",      "Inspect filters and reserve tanks."},
  {"Count Heads",      "Log survivors present; note absences."},
  {"Council Meeting",  "Hear concerns; set week priorities."},
  {"Post Work Orders", "Assign repair crews to camp damage."},
  {"Trade Ledger",     "Review last faction exchange terms."},
  {"Read the Clock",   "Check threat meters; note due dates."},
  {"Night Watch Log",  "Review patrol reports for anomalies."},
};

// ---------------------------------------------------------------------------
// ROSTER screen — recruit and assign survivors by role
// ---------------------------------------------------------------------------
static const CGC::NamedValue ROWS_ROSTER[] = {
  {"Sign Scout",      "Recruit a scout; costs food to onboard."},
  {"Sign Medic",      "Recruit a medic; needs meds supply."},
  {"Sign Guard",      "Recruit a guard; bolsters defense."},
  {"Sign Tech",       "Recruit a tech; helps build and power."},
  {"Assign Roles",    "Shuffle survivor role assignments."},
  {"Brief the Team",  "Walk team through the next run plan."},
  {"Rest Cycle",      "Rotate tired survivors; ease stress."},
  {"Motivate Squad",  "Speech and rations boost team morale."},
};

// ---------------------------------------------------------------------------
// EXPEDITION screen — send team into ruins; costs Ammo, gated on F_TEAM_FORMED
// ---------------------------------------------------------------------------
static const CGC::NamedValue ROWS_EXPEDITION[] = {
  {"Clinic Ruins",    "Strip old clinic for meds and gear."},
  {"Depot Sweep",     "Hit the warehouse district for scrap."},
  {"Radio Tower",     "Climb to restore a signal relay."},
  {"Tunnel Network",  "Probe the underground passage routes."},
  {"Farm Salvage",    "Recover seeds and irrigation parts."},
  {"Faction Camp",    "Make contact with the river medics."},
  {"Rescue Signal",   "Follow a distress beacon into danger."},
  {"Authority Cache", "Breach a pre-collapse storage vault."},
};

// ---------------------------------------------------------------------------
// INFIRMARY screen — treat wounds and radiation; costs Meds
// ---------------------------------------------------------------------------
static const CGC::NamedValue ROWS_INFIRMARY[] = {
  {"Dress Wounds",     "Bandage lacerations from last run."},
  {"Treat Infection",  "IV antibiotics for a feverish scout."},
  {"Flush Radiation",  "Chelation course for rad-exposed crew."},
  {"Rest Protocol",    "Enforce full rest for critical cases."},
  {"Triage Report",    "Assess roster injuries; set priority."},
  {"Brew Antiseptic",  "Stretch meds supply with field brew."},
};

// ---------------------------------------------------------------------------
// FACILITIES screen — build and upgrade; costs Scrap + Power
// ---------------------------------------------------------------------------
static const CGC::NamedValue ROWS_FACILITIES[] = {
  {"Garden Bed",     "Grow calories; reduce food pressure."},
  {"Water Purifier", "Filter water; boost safe daily yield."},
  {"Clinic Bay",     "Upgrade infirmary; enables rad flush."},
  {"Workshop",       "Fabricate parts; stretch scrap supply."},
  {"Barracks",       "Bunk expansion; boosts morale floor."},
  {"Watchtower",     "Spotters improve threat early warning."},
  {"Radio Mast",     "Long-range contact; unlocks regions."},
  {"Walls Upgrade",  "Reinforce perimeter; raises defense."},
};

// ---------------------------------------------------------------------------
// DEFENSE screen — answer threat clocks; costs Ammo
// ---------------------------------------------------------------------------
static const CGC::NamedValue ROWS_DEFENSE[] = {
  {"Repel Raiders",   "Drive off scouts probing the walls."},
  {"Sickness Drill",  "Isolate sick; slow outbreak spread."},
  {"Storm Prep",      "Board windows; secure loose stores."},
  {"Hunger Measure",  "Emergency ration cut to stretch food."},
  {"Faction Demand",  "Negotiate or pay the baron toll."},
  {"Perimeter Check", "Walk the wall; patch weak points."},
  {"Ammo Recount",    "Inventory and redistribute ammo."},
};

// ---------------------------------------------------------------------------
// ENDGAME screen — choose survival plan; gated on F_RADIO_BUILT + F_SIGNAL_FOUND
// ---------------------------------------------------------------------------
static const CGC::NamedValue ROWS_ENDGAME[] = {
  {"Fortify the Town", "Commit to walls, militia, and roots."},
  {"Launch Convoy",    "Pack the best and go mobile."},
  {"River Alliance",   "Merge with the river medic faction."},
  {"Beacon Broadcast", "Transmit coordinates to all bands."},
  {"Seal the Bunker",  "Lock down; go dark and survive alone."},
  {"Hold the Line",    "Keep the plan open; wait for more."},
};

// ---------------------------------------------------------------------------
// Screens
// ---------------------------------------------------------------------------
static const CGC::DeepScreenDef SCREENS[] = {
  // SETTLEMENT — free; advances Days tracker; sets no flags; always open
  {.title = "SETTLEMENT", .verb = "Review", .rows = ROWS_SETTLEMENT, .rowCount = 8,
   .resourceDeltas = {2, 1, 0, 0, 0, 0, 2, 0}, .progressDelta = 2,
   .trackerIndex = 0, .flagMask = 0,
   .costResource = -1, .costAmount = 0, .requireFlags = 0,
   .trackerDelta = 1, .objective = "Keep food/water stable; note threats"},

  // ROSTER — costs Food (feed recruits); advances Survivors tracker; sets F_TEAM_FORMED
  {.title = "ROSTER", .verb = "Recruit", .rows = ROWS_ROSTER, .rowCount = 8,
   .resourceDeltas = {-3, 0, 0, 0, 0, 0, 3, 1}, .progressDelta = 3,
   .trackerIndex = 2, .flagMask = F_TEAM_FORMED,
   .costResource = R_FOOD, .costAmount = 6, .requireFlags = 0,
   .trackerDelta = 1, .objective = "Recruit survivors; build a team (-6 Food)"},

  // EXPEDITION — costs Ammo; advances Expeditions tracker; gated on team
  {.title = "EXPEDITION", .verb = "Send", .rows = ROWS_EXPEDITION, .rowCount = 8,
   .resourceDeltas = {3, 2, 2, -4, 4, 1, -2, 0}, .progressDelta = 6,
   .trackerIndex = 1, .flagMask = 0,
   .costResource = R_AMMO, .costAmount = 5, .requireFlags = F_TEAM_FORMED,
   .trackerDelta = 1, .objective = "Send team to loot ruins (needs Team, -5 Ammo)"},

  // INFIRMARY — costs Meds; advances Survivors tracker; gated on clinic built
  {.title = "INFIRMARY", .verb = "Treat", .rows = ROWS_INFIRMARY, .rowCount = 6,
   .resourceDeltas = {0, 0, -4, 0, 0, 0, 2, 0}, .progressDelta = 2,
   .trackerIndex = 2, .flagMask = F_CLINIC_BUILT,
   .costResource = R_MEDS, .costAmount = 4, .requireFlags = 0,
   .trackerDelta = 0, .objective = "Treat wounds and radiation (-4 Meds)"},

  // FACILITIES — costs Scrap; advances Facilities tracker; sets F_RADIO_BUILT or F_WALLS_BUILT via events
  {.title = "FACILITIES", .verb = "Build", .rows = ROWS_FACILITIES, .rowCount = 8,
   .resourceDeltas = {0, 0, 0, 0, -8, -3, 1, 2}, .progressDelta = 4,
   .trackerIndex = 3, .flagMask = 0,
   .costResource = R_SCRAP, .costAmount = 10, .requireFlags = 0,
   .trackerDelta = 1, .objective = "Build or upgrade facilities (-10 Scrap)"},

  // DEFENSE — costs Ammo; advances Threats tracker
  {.title = "DEFENSE", .verb = "Counter", .rows = ROWS_DEFENSE, .rowCount = 7,
   .resourceDeltas = {0, 0, 0, -3, 0, 0, 2, 3}, .progressDelta = 3,
   .trackerIndex = 4, .flagMask = 0,
   .costResource = R_AMMO, .costAmount = 4, .requireFlags = 0,
   .trackerDelta = 1, .objective = "Answer threats; hold the perimeter (-4 Ammo)"},

  // ENDGAME — free; gated on radio + signal; advances Rescues tracker
  {.title = "SURVIVAL PLAN", .verb = "Commit", .rows = ROWS_ENDGAME, .rowCount = 6,
   .resourceDeltas = {0, 0, 0, 0, 0, 0, 4, 4}, .progressDelta = 8,
   .trackerIndex = 5, .flagMask = F_ENDGAME,
   .costResource = -1, .costAmount = 0, .requireFlags = F_RADIO_BUILT | F_SIGNAL_FOUND,
   .trackerDelta = 1, .objective = "Choose settlement fate (needs Radio + Signal)"},
};

// ---------------------------------------------------------------------------
// Events
// ---------------------------------------------------------------------------
static const CGC::EventDef EVENTS[] = {

  // --- Common neutral/good fillers ---
  {.name = "Quiet Week",
   .note = "No major crises; stores hold steady.",
   .resourceDeltas = {1, 1, 0, 0, 0, 0, 1, 0},
   .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
   .repIndex = -1, .repDelta = 0, .weight = 4,
   .requireFlags = 0, .forbidFlags = 0, .tone = CGC::TONE_NEUTRAL},

  {.name = "Salvage Windfall",
   .note = "Team finds a cache; scrap and meds gained.",
   .resourceDeltas = {0, 0, 2, 0, 4, 0, 2, 0},
   .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
   .repIndex = -1, .repDelta = 0, .weight = 3,
   .requireFlags = F_TEAM_FORMED, .forbidFlags = 0, .tone = CGC::TONE_GOOD},

  {.name = "Clean Water Run",
   .note = "Purifier output spikes; water stocks rise.",
   .resourceDeltas = {0, 4, 0, 0, 0, 0, 1, 0},
   .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
   .repIndex = -1, .repDelta = 0, .weight = 3,
   .requireFlags = 0, .forbidFlags = 0, .tone = CGC::TONE_GOOD},

  {.name = "Morale Rally",
   .note = "Camp bonfire; survivors sing and swap stories.",
   .resourceDeltas = {-1, 0, 0, 0, 0, 0, 5, 0},
   .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
   .repIndex = -1, .repDelta = 0, .weight = 2,
   .requireFlags = 0, .forbidFlags = 0, .tone = CGC::TONE_GOOD},

  // --- Pressure / bad events ---
  {.name = "Raider Probe",
   .note = "Scouts test your perimeter; ammo spent.",
   .resourceDeltas = {0, 0, 0, -4, 0, 0, -2, -2},
   .setFlags = 0, .trackerIndex = 4, .trackerDelta = 1,
   .repIndex = -1, .repDelta = 0, .weight = 3,
   .requireFlags = 0, .forbidFlags = F_WALLS_BUILT, .tone = CGC::TONE_BAD},

  {.name = "Sickness Outbreak",
   .note = "Fever sweeps the barracks; meds drain fast.",
   .resourceDeltas = {0, 0, -5, 0, 0, 0, -3, 0},
   .setFlags = 0, .trackerIndex = 4, .trackerDelta = 1,
   .repIndex = -1, .repDelta = 0, .weight = 2,
   .requireFlags = 0, .forbidFlags = 0, .tone = CGC::TONE_BAD},

  {.name = "Supply Rot",
   .note = "Unsealed stores spoil; food stocks drop.",
   .resourceDeltas = {-5, 0, 0, 0, 0, 0, -1, 0},
   .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
   .repIndex = -1, .repDelta = 0, .weight = 2,
   .requireFlags = 0, .forbidFlags = 0, .tone = CGC::TONE_BAD},

  {.name = "Power Fault",
   .note = "A generator coil blows; power dips sharply.",
   .resourceDeltas = {0, 0, 0, 0, 0, -4, -1, 0},
   .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
   .repIndex = -1, .repDelta = 0, .weight = 2,
   .requireFlags = 0, .forbidFlags = 0, .tone = CGC::TONE_BAD},

  {.name = "Expedition Wounds",
   .note = "Team returns hurt; need meds and rest.",
   .resourceDeltas = {0, 0, -3, 0, 0, 0, -2, 0},
   .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
   .repIndex = -1, .repDelta = 0, .weight = 3,
   .requireFlags = F_TEAM_FORMED, .forbidFlags = 0, .tone = CGC::TONE_BAD},

  {.name = "Radiation Exposure",
   .note = "Scout logs rads in old sector; meds consumed.",
   .resourceDeltas = {0, 0, -4, 0, 0, 0, -1, 0},
   .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
   .repIndex = -1, .repDelta = 0, .weight = 2,
   .requireFlags = F_TEAM_FORMED, .forbidFlags = 0, .tone = CGC::TONE_BAD},

  {.name = "Storm Damage",
   .note = "Gale tears roofing; scrap spent on repairs.",
   .resourceDeltas = {0, -2, 0, 0, -3, -2, -2, -1},
   .setFlags = 0, .trackerIndex = 4, .trackerDelta = 1,
   .repIndex = -1, .repDelta = 0, .weight = 2,
   .requireFlags = 0, .forbidFlags = 0, .tone = CGC::TONE_BAD},

  // --- Faction / story events ---
  {.name = "River Medics Arrive",
   .note = "Medics offer meds for food; morale lifts.",
   .resourceDeltas = {-4, 0, 5, 0, 0, 0, 3, 0},
   .setFlags = F_FACTION_ALLY, .trackerIndex = -1, .trackerDelta = 0,
   .repIndex = 0, .repDelta = 3, .weight = 2,
   .requireFlags = 0, .forbidFlags = F_FACTION_ALLY, .tone = CGC::TONE_GOOD},

  {.name = "Scrap Baron Toll",
   .note = "Baron crew demands scrap or blocks routes.",
   .resourceDeltas = {0, 0, 0, 0, -5, 0, -2, 0},
   .setFlags = 0, .trackerIndex = 4, .trackerDelta = 1,
   .repIndex = 1, .repDelta = -2, .weight = 2,
   .requireFlags = 0, .forbidFlags = 0, .tone = CGC::TONE_BAD},

  {.name = "Signal Cult Offer",
   .note = "Cultists share a broadcast frequency key.",
   .resourceDeltas = {0, 0, 0, 0, 0, 2, 2, 0},
   .setFlags = F_SIGNAL_FOUND, .trackerIndex = 5, .trackerDelta = 1,
   .repIndex = 2, .repDelta = 2, .weight = 2,
   .requireFlags = F_RADIO_BUILT, .forbidFlags = F_SIGNAL_FOUND, .tone = CGC::TONE_GOOD},

  {.name = "Rescue Survivor",
   .note = "Distress call leads to a wounded stranger.",
   .resourceDeltas = {-2, -1, -2, 0, 0, 0, 4, 0},
   .setFlags = 0, .trackerIndex = 5, .trackerDelta = 1,
   .repIndex = -1, .repDelta = 0, .weight = 2,
   .requireFlags = F_TEAM_FORMED, .forbidFlags = 0, .tone = CGC::TONE_NEUTRAL},

  {.name = "Walls Hold Attack",
   .note = "Raiders hit the wall; defense absorbs blow.",
   .resourceDeltas = {0, 0, 0, -2, 0, 0, 2, -1},
   .setFlags = 0, .trackerIndex = 4, .trackerDelta = 1,
   .repIndex = -1, .repDelta = 0, .weight = 3,
   .requireFlags = F_WALLS_BUILT, .forbidFlags = 0, .tone = CGC::TONE_NEUTRAL},

  {.name = "Betrayal",
   .note = "A guard defects; ammo and intel vanish.",
   .resourceDeltas = {0, 0, 0, -3, 0, 0, -4, -2},
   .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
   .repIndex = -1, .repDelta = 0, .weight = 1,
   .requireFlags = F_TEAM_FORMED, .forbidFlags = F_FACTION_ALLY, .tone = CGC::TONE_BAD},

  {.name = "Radio Contact",
   .note = "A voice answers the mast; new regions open.",
   .resourceDeltas = {0, 0, 0, 0, 0, 0, 3, 0},
   .setFlags = F_RADIO_BUILT | F_SIGNAL_FOUND, .trackerIndex = 5, .trackerDelta = 1,
   .repIndex = 2, .repDelta = 2, .weight = 3,
   .requireFlags = F_RADIO_BUILT, .forbidFlags = F_SIGNAL_FOUND, .tone = CGC::TONE_GOOD},
};

// ---------------------------------------------------------------------------
// Endings  (most-specific first)
// ---------------------------------------------------------------------------
static const CGC::EndingDef ENDINGS[] = {

  // Bunker — chose plan bunker, has walls, has low morale (sealed off alone)
  {.name = "Bunker Rule",
   .note = "Sealed in, rationed tight. Silence, but alive.",
   .cond = {.resourceIndex = R_DEFENSE, .resourceMin = 40, .resourceMax = 100,
            .trackerIndex = -1, .trackerMin = 0, .repIndex = -1, .repMin = 0,
            .requireFlags = F_ENDGAME | F_PLAN_BUNKER | F_WALLS_BUILT,
            .forbidFlags = 0, .requiresVictory = true}},

  // Signal Beacon — plan beacon + faction ally + signal found
  {.name = "Signal Beacon",
   .note = "Your signal reaches far. Others are coming.",
   .cond = {.resourceIndex = -1, .resourceMin = 0, .resourceMax = 0,
            .trackerIndex = 5, .trackerMin = 3, .repIndex = 2, .repMin = 2,
            .requireFlags = F_ENDGAME | F_PLAN_BEACON | F_SIGNAL_FOUND,
            .forbidFlags = 0, .requiresVictory = true}},

  // River Alliance — ally + signal + high morale
  {.name = "River Alliance",
   .note = "Merged with the medics. Two camps, one future.",
   .cond = {.resourceIndex = R_MORALE, .resourceMin = 50, .resourceMax = 100,
            .trackerIndex = -1, .trackerMin = 0, .repIndex = 0, .repMin = 3,
            .requireFlags = F_ENDGAME | F_FACTION_ALLY | F_SIGNAL_FOUND,
            .forbidFlags = 0, .requiresVictory = true}},

  // Mobile Convoy — plan convoy + walls not required + has ammo
  {.name = "Mobile Convoy",
   .note = "Wheels rolling. The waste is wide; you'll find a way.",
   .cond = {.resourceIndex = R_AMMO, .resourceMin = 10, .resourceMax = 99,
            .trackerIndex = 1, .trackerMin = 5, .repIndex = -1, .repMin = 0,
            .requireFlags = F_ENDGAME | F_PLAN_CONVOY,
            .forbidFlags = 0, .requiresVictory = true}},

  // Fortified Town — walls + defense + no specific plan (default fortress)
  {.name = "Fortress Town",
   .note = "Walls hold. The settlement endures. That's enough.",
   .cond = {.resourceIndex = R_DEFENSE, .resourceMin = 35, .resourceMax = 100,
            .trackerIndex = 3, .trackerMin = 3, .repIndex = -1, .repMin = 0,
            .requireFlags = F_ENDGAME | F_WALLS_BUILT,
            .forbidFlags = 0, .requiresVictory = true}},

  // Catch-all victory
  {.name = "Survivors",
   .note = "Not glory — just alive. The base stands.",
   .cond = {.resourceIndex = -1, .resourceMin = 0, .resourceMax = 0,
            .trackerIndex = -1, .trackerMin = 0, .repIndex = -1, .repMin = 0,
            .requireFlags = 0, .forbidFlags = 0, .requiresVictory = true}},

  // Catch-all loss — starvation or collapse
  {.name = "Camp Collapse",
   .note = "Food ran out. The last fire went cold.",
   .cond = {.resourceIndex = -1, .resourceMin = 0, .resourceMax = 0,
            .trackerIndex = -1, .trackerMin = 0, .repIndex = -1, .repMin = 0,
            .requireFlags = 0, .forbidFlags = 0, .requiresVictory = false}},
};

// ---------------------------------------------------------------------------
// Tracker labels
// ---------------------------------------------------------------------------
static const char* const TRACKER_LABELS[] = {
  "Days", "Expeditions", "Survivors", "Facilities", "Threats", "Rescues",
  nullptr, nullptr,
};

// ---------------------------------------------------------------------------
// Game definition
// ---------------------------------------------------------------------------
static const CGC::DeepGameDefinition DEF = {
  .title          = "Waste Guild",
  .slug           = "wasteland-guildmaster",
  .subtitle       = "Settlement command sim",
  .saveMagic      = "WGM2",
  .resources      = RESOURCES,
  .resourceCount  = sizeof(RESOURCES) / sizeof(RESOURCES[0]),
  .screens        = SCREENS,
  .screenCount    = sizeof(SCREENS) / sizeof(SCREENS[0]),
  .events         = EVENTS,
  .eventCount     = sizeof(EVENTS) / sizeof(EVENTS[0]),
  .endings        = ENDINGS,
  .endingCount    = sizeof(ENDINGS) / sizeof(ENDINGS[0]),
  .targetProgress = 175,
  .accentColor    = 0xFD20,
  .warningColor   = 0xFD20,
  .trackerLabels  = TRACKER_LABELS,
  .trackerLabelCount = 8,
  .objective      = "Build the settlement; launch a survival plan.",
  .primaryTracker = 1,  // Expeditions on the hub headline
};

const CGC::DeepGameDefinition& gameDefinition() {
  return DEF;
}
