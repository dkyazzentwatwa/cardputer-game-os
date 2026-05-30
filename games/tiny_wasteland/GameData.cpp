#include "GameData.h"

namespace CGC = CardputerGameCore;

// Resource indices — must match RESOURCES order below.
enum { R_FOOD, R_WAT, R_MED, R_FUEL, R_SCRAP, R_MOR, R_RAD, R_VEH };

// Story / progression flags.
enum : uint32_t {
  F_ROAD_OPEN   = 1u << 0,  // party has moved at least once
  F_SCAV_DONE   = 1u << 1,  // party has scavenged at least once
  F_FACTION_MET = 1u << 2,  // made contact with any faction
  F_SIGNAL_RECV = 1u << 3,  // safe-zone radio signal received
  F_CULT_DEAL   = 1u << 4,  // accepted machine-cult offer
  F_MEDIC_PACT  = 1u << 5,  // river-medic alliance formed
  F_CAMP_READY  = 1u << 6,  // enough scrap/morale to found a camp
  F_SIGNAL_TRAP = 1u << 7,  // signal confirmed as raider bait
};

// ── RESOURCES ────────────────────────────────────────────────────────────────
// label  start  min  max  pDelta  safeDelta  warn  bad  inverted
static const CGC::ResourceDef RESOURCES[] = {
    {"Fd",    60,   0,  999,   0,    4,   18,   6, false},  // Food (scarcity, not loss)
    {"Wat",   55,   0,  999,  -1,    3,   16,   5, false},  // Water — depletion ends run
    {"Med",    8,   0,   99,   0,    1,    4,   1, false},
    {"Fuel",  42,   0,   99,   0,    2,   12,   4, false},
    {"Scrap",  8,   0,  999,   0,    0,   -1,  -1, false},  // currency; no warn
    {"Mor",   72,   0,  100,   0,    2,   28,  10, false},
    {"Rad",    4,   0,  100,   0,    0,   40,  65, true},   // high is bad; inverted
    {"Veh",   80,   0,  100,   0,    3,   25,  10, false},
};

// ── SCREEN ROW TABLES ─────────────────────────────────────────────────────────

static const CGC::NamedValue ROWS_ROAD[] = {
    {"Dust Plains",   "Flat run; dust clogs filters."},
    {"Dry Riverbed",  "Cracked basin; sun hammers Wat."},
    {"Clinic Road",   "Old med corridor; slow going."},
    {"Orchard Loop",  "Overgrown track; scav tempting."},
    {"Flats Run",     "Open highway; good speed, raiders."},
    {"Bone Valley",   "Rad-hot pass; shields help."},
    {"Static Stretch","EMF zone; Veh sensors glitch."},
    {"Bunker Road",   "Cult-marked stretch; watch speed."},
};

static const CGC::NamedValue ROWS_CAMP[] = {
    {"Ration Stocks",  "Stretch food; cut portions."},
    {"Treat Wounds",   "Use meds; patch the injured."},
    {"Night Guard",    "Trade sleep for safety."},
    {"Cook Big Pot",   "Use food to lift morale."},
    {"Talk It Out",    "Hold council; hear complaints."},
    {"Map the Route",  "Plot next leg; reduce surprises."},
    {"Filter Check",   "Clear filters; cuts Rad gain."},
    {"Signal Listen",  "Tune radio; watch for the signal."},
};

static const CGC::NamedValue ROWS_SCAV[] = {
    {"Pharmacy Ruin",   "Meds likely; infection risk."},
    {"Fuel Depot",      "Tank up; vapor fumes and sparks."},
    {"School Cellar",   "Food cache; slow and enclosed."},
    {"Hardware Store",  "Scrap gold; structural hazard."},
    {"Farmstead Barn",  "Food + scrap; loose dogs."},
    {"Clinic Annex",    "Meds + risk of biohazard."},
    {"Garage Bay",      "Veh parts; may meet squatters."},
    {"Radio Tower",     "Signal clues; exposed, Rad risk."},
};

static const CGC::NamedValue ROWS_REPAIR[] = {
    {"Patch Body Panels", "Scrap keeps rusting holes shut."},
    {"Fix Fuel Line",     "Seal leaks; stops Fuel bleed."},
    {"Swap Tires",        "Pull spares; traction restored."},
    {"Flush Radiator",    "Clear gunk; engine runs cooler."},
    {"Rebuild Brake",     "Safety first on bad roads."},
    {"Weld Frame Crack",  "Structural fix; uses most Scrap."},
};

static const CGC::NamedValue ROWS_TRADE[] = {
    {"League Post",   "Trade scrap for fuel and food."},
    {"Raider Baron",  "Risky deal; ammo for supplies."},
    {"River Medics",  "Meds for morale and rep."},
    {"Free Scavs",    "Scrap barter; fair rates."},
    {"Settlement",    "Food + water; costs reputation."},
    {"Depot Merchant","Fuel for scrap and veh parts."},
};

static const CGC::NamedValue ROWS_PARTY[] = {
    {"Check Scout",    "Scout reports route conditions."},
    {"Check Medic",    "Medic assesses wound severity."},
    {"Check Driver",   "Driver rates vehicle stress."},
    {"Check Cook",     "Cook inventories food supply."},
    {"Recruit Offer",  "Stranger requests to join."},
    {"Morale Talk",    "Leader speech; boosts spirits."},
    {"Rest Rotation",  "Rest the most exhausted member."},
    {"Dismiss Guard",  "Release a low-morale guard."},
};

static const CGC::NamedValue ROWS_SAFEZONE[] = {
    {"Enter Gate",     "Show credentials; enter safely."},
    {"Negotiate Stay", "Bargain long-term settlement."},
    {"Share Intel",    "Give route map for better terms."},
    {"Donate Scrap",   "Contribute to the community."},
    {"Settle Quietly", "Keep a low profile; just rest."},
    {"Signal Answer",  "Broadcast your position back."},
};

// ── SCREENS ───────────────────────────────────────────────────────────────────
// Field order: title, verb, rows, rowCount, resourceDeltas[8],
//              progressDelta, trackerIndex, flagMask,
//              costResource, costAmount, requireFlags,
//              trackerDelta, objective

static const CGC::DeepScreenDef SCREENS[] = {
    // ROAD — costs Fuel; sets F_ROAD_OPEN; advances Regions tracker
    {.title = "ROAD", .verb = "Drive", .rows = ROWS_ROAD, .rowCount = 8,
     .resourceDeltas = {-3, -4, 0, -3, 0, -1, 2, -2},
     .progressDelta = 6, .trackerIndex = 0,
     .flagMask = F_ROAD_OPEN,
     .costResource = R_FUEL, .costAmount = 6, .requireFlags = 0,
     .trackerDelta = 1, .objective = "Move west; burns Fuel + Water"},

    // CAMP — free; recovers party; advances Days tracker
    {.title = "CAMP", .verb = "Rest", .rows = ROWS_CAMP, .rowCount = 8,
     .resourceDeltas = {2, 3, 0, 0, 0, 4, -1, 2},
     .progressDelta = 1, .trackerIndex = 5,
     .flagMask = 0,
     .costResource = -1, .costAmount = 0, .requireFlags = 0,
     .trackerDelta = 1, .objective = "Rest and recover the party"},

    // SCAV — costs Mor (stress); sets F_SCAV_DONE; advances Scavenges tracker
    {.title = "SCAV", .verb = "Search", .rows = ROWS_SCAV, .rowCount = 8,
     .resourceDeltas = {3, 1, 2, 2, 4, -2, 3, -1},
     .progressDelta = 3, .trackerIndex = 1,
     .flagMask = F_SCAV_DONE,
     .costResource = R_MOR, .costAmount = 6, .requireFlags = 0,
     .trackerDelta = 1, .objective = "Risk the ruins for supplies"},

    // REPAIR — costs Scrap; advances Repairs tracker
    {.title = "REPAIR", .verb = "Fix", .rows = ROWS_REPAIR, .rowCount = 6,
     .resourceDeltas = {0, 0, 0, 0, -3, 2, 0, 6},
     .progressDelta = 2, .trackerIndex = 3,
     .flagMask = 0,
     .costResource = R_SCRAP, .costAmount = 5, .requireFlags = 0,
     .trackerDelta = 1, .objective = "Fix the vehicle (-5 Scrap)"},

    // TRADE — needs faction contact; costs Scrap; advances Trades tracker
    {.title = "TRADE", .verb = "Barter", .rows = ROWS_TRADE, .rowCount = 6,
     .resourceDeltas = {4, 3, 1, 4, -4, 1, 0, 0},
     .progressDelta = 2, .trackerIndex = 4,
     .flagMask = F_FACTION_MET,
     .costResource = R_SCRAP, .costAmount = 4, .requireFlags = F_ROAD_OPEN,
     .trackerDelta = 1, .objective = "Barter with factions (-4 Scrap)"},

    // PARTY — free; tracks survivors; advances Survivors tracker
    {.title = "PARTY", .verb = "Tend", .rows = ROWS_PARTY, .rowCount = 8,
     .resourceDeltas = {-1, 0, -1, 0, 0, 3, -1, 0},
     .progressDelta = 1, .trackerIndex = 2,
     .flagMask = 0,
     .costResource = -1, .costAmount = 0, .requireFlags = 0,
     .trackerDelta = 1, .objective = "Manage the survivors"},

    // SAFE ZONE — gated by signal receipt; endgame screen
    {.title = "SAFEZONE", .verb = "Enter", .rows = ROWS_SAFEZONE, .rowCount = 6,
     .resourceDeltas = {6, 6, 3, 0, 0, 6, -3, 0},
     .progressDelta = 10, .trackerIndex = 0,
     .flagMask = F_CAMP_READY,
     .costResource = -1, .costAmount = 0, .requireFlags = F_SIGNAL_RECV,
     .trackerDelta = 0, .objective = "Reach the safe zone (needs Signal)"},
};

// ── EVENTS ────────────────────────────────────────────────────────────────────
// Field order: name, note, resourceDeltas[8], setFlags,
//              trackerIndex, trackerDelta,
//              repIndex, repDelta,
//              weight, requireFlags, forbidFlags, tone

static const CGC::EventDef EVENTS[] = {
    // === Common filler ===
    {.name = "Quiet mile",
     .note = "Nothing moves. The road is yours.",
     .resourceDeltas = {0, 1, 0, 0, 0, 1, 0, 0},
     .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = -1, .repDelta = 0,
     .weight = 5, .requireFlags = 0, .forbidFlags = 0,
     .tone = CGC::TONE_NEUTRAL},

    {.name = "Salvage cache",
     .note = "Crate by the road. Clean scrap inside.",
     .resourceDeltas = {0, 0, 0, 0, 4, 1, 0, 0},
     .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = -1, .repDelta = 0,
     .weight = 3, .requireFlags = 0, .forbidFlags = 0,
     .tone = CGC::TONE_GOOD},

    {.name = "Water puddle",
     .note = "Runoff pool — murky but safe enough.",
     .resourceDeltas = {0, 4, 0, 0, 0, 0, 0, 0},
     .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = -1, .repDelta = 0,
     .weight = 3, .requireFlags = 0, .forbidFlags = 0,
     .tone = CGC::TONE_GOOD},

    // === Hazard / bad ===
    {.name = "Dust storm",
     .note = "Choking wall of grit; lose time and Wat.",
     .resourceDeltas = {0, -4, 0, -2, 0, -2, 1, -1},
     .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = -1, .repDelta = 0,
     .weight = 3, .requireFlags = 0, .forbidFlags = 0,
     .tone = CGC::TONE_BAD},

    {.name = "Raider ambush",
     .note = "Armed crew blocks the road. Pay or fight.",
     .resourceDeltas = {-4, -2, 0, -3, -3, -3, 0, -2},
     .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = 0, .repDelta = -1,
     .weight = 2, .requireFlags = F_ROAD_OPEN, .forbidFlags = 0,
     .tone = CGC::TONE_BAD},

    {.name = "Rad pocket",
     .note = "Hot zone; no warning until it is too late.",
     .resourceDeltas = {0, -2, 0, 0, 0, -1, 4, -1},
     .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = -1, .repDelta = 0,
     .weight = 2, .requireFlags = 0, .forbidFlags = 0,
     .tone = CGC::TONE_BAD},

    {.name = "Sickness outbreak",
     .note = "Two survivors feverish; meds or morale drops.",
     .resourceDeltas = {0, -2, -3, 0, 0, -3, 0, 0},
     .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = -1, .repDelta = 0,
     .weight = 2, .requireFlags = F_ROAD_OPEN, .forbidFlags = 0,
     .tone = CGC::TONE_BAD},

    {.name = "Axle crack",
     .note = "Grinding metal. Repair now or lose Veh fast.",
     .resourceDeltas = {0, 0, 0, 0, -2, -2, 0, -5},
     .setFlags = 0, .trackerIndex = 3, .trackerDelta = 1,
     .repIndex = -1, .repDelta = 0,
     .weight = 2, .requireFlags = F_ROAD_OPEN, .forbidFlags = 0,
     .tone = CGC::TONE_BAD},

    {.name = "Abandoned cargo",
     .note = "Someone dumped supplies. A gift or a trap?",
     .resourceDeltas = {3, 2, 1, 0, 2, 0, 0, 0},
     .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = -1, .repDelta = 0,
     .weight = 2, .requireFlags = F_SCAV_DONE, .forbidFlags = 0,
     .tone = CGC::TONE_GOOD},

    // === Moral choices / faction gates ===
    {.name = "Stranger at the road",
     .note = "Wounded traveler. Take them in or drive on?",
     .resourceDeltas = {-2, -1, -2, 0, 0, 2, 0, 0},
     .setFlags = F_FACTION_MET, .trackerIndex = 2, .trackerDelta = 1,
     .repIndex = 1, .repDelta = 2,
     .weight = 2, .requireFlags = F_ROAD_OPEN, .forbidFlags = 0,
     .tone = CGC::TONE_NEUTRAL},

    {.name = "Machine cult toll",
     .note = "Robed guards demand scrap tribute to pass.",
     .resourceDeltas = {0, 0, 0, 0, -4, -2, 0, 0},
     .setFlags = F_FACTION_MET, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = 2, .repDelta = 1,
     .weight = 2, .requireFlags = F_ROAD_OPEN, .forbidFlags = F_CULT_DEAL,
     .tone = CGC::TONE_BAD},

    {.name = "Cult maintenance deal",
     .note = "Cult offers Veh repair for a promised service.",
     .resourceDeltas = {0, 0, 0, 0, 0, 0, 0, 5},
     .setFlags = F_CULT_DEAL, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = 2, .repDelta = 3,
     .weight = 1, .requireFlags = F_FACTION_MET, .forbidFlags = F_CULT_DEAL,
     .tone = CGC::TONE_NEUTRAL},

    {.name = "River medic camp",
     .note = "White flags on the bridge. They heal for free.",
     .resourceDeltas = {0, 3, 4, 0, 0, 3, -2, 0},
     .setFlags = F_MEDIC_PACT, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = 1, .repDelta = 3,
     .weight = 1, .requireFlags = F_ROAD_OPEN, .forbidFlags = F_MEDIC_PACT,
     .tone = CGC::TONE_GOOD},

    {.name = "Safe-zone signal",
     .note = "Faint coords on the radio. Could be real.",
     .resourceDeltas = {0, 0, 0, 0, 0, 3, 0, 0},
     .setFlags = F_SIGNAL_RECV, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = -1, .repDelta = 0,
     .weight = 1, .requireFlags = F_SCAV_DONE, .forbidFlags = F_SIGNAL_RECV,
     .tone = CGC::TONE_GOOD},

    {.name = "Signal confirmed trap",
     .note = "Raiders on that freq. You note the pattern.",
     .resourceDeltas = {0, 0, 0, 0, 0, -3, 0, 0},
     .setFlags = F_SIGNAL_TRAP, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = 0, .repDelta = -2,
     .weight = 1, .requireFlags = F_SIGNAL_RECV, .forbidFlags = F_SIGNAL_TRAP,
     .tone = CGC::TONE_BAD},

    // === Scav-gated discoveries ===
    {.name = "Pharmacy windfall",
     .note = "Back room untouched. Med box still sealed.",
     .resourceDeltas = {0, 0, 5, 0, 0, 1, 0, 0},
     .setFlags = 0, .trackerIndex = 1, .trackerDelta = 1,
     .repIndex = -1, .repDelta = 0,
     .weight = 2, .requireFlags = F_SCAV_DONE, .forbidFlags = 0,
     .tone = CGC::TONE_GOOD},

    {.name = "Geiger shortcut offer",
     .note = "Guide says hot ground saves two days. Your call.",
     .resourceDeltas = {0, 0, 0, 2, 0, -1, 3, -1},
     .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = -1, .repDelta = 0,
     .weight = 1, .requireFlags = F_SCAV_DONE, .forbidFlags = 0,
     .tone = CGC::TONE_NEUTRAL},

    // === Endgame / camp-ready ===
    {.name = "Good campsite found",
     .note = "Walls, water, sight lines. This could work.",
     .resourceDeltas = {0, 2, 0, 0, 2, 4, -1, 0},
     .setFlags = F_CAMP_READY, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = -1, .repDelta = 0,
     .weight = 1, .requireFlags = F_MEDIC_PACT, .forbidFlags = F_CAMP_READY,
     .tone = CGC::TONE_GOOD},
};

// ── ENDINGS ───────────────────────────────────────────────────────────────────
// Most-specific first. Last two are catch-all victory and loss.

static const CGC::EndingDef ENDINGS[] = {
    // Signal trap: walked into the ambush
    {.name = "Signal Trap",
     .note = "The coords were bait. Raiders took everything.",
     .cond = {.resourceIndex = -1, .resourceMin = 0, .resourceMax = 0,
              .trackerIndex = -1, .trackerMin = 0,
              .repIndex = -1, .repMin = 0,
              .requireFlags = F_SIGNAL_TRAP, .forbidFlags = 0,
              .requiresVictory = false}},

    // Cult deal gone wrong: Veh high but morale crushed
    {.name = "Cult Debt",
     .note = "The cult owns your vehicle now. You serve them.",
     .cond = {.resourceIndex = R_MOR, .resourceMin = 0, .resourceMax = 20,
              .trackerIndex = -1, .trackerMin = 0,
              .repIndex = -1, .repMin = 0,
              .requireFlags = F_CULT_DEAL, .forbidFlags = 0,
              .requiresVictory = true}},

    // Lone survivor: got there but morale and party devastated
    {.name = "Lone Survivor",
     .note = "You made it. No one else did. Was it worth it?",
     .cond = {.resourceIndex = R_MOR, .resourceMin = 0, .resourceMax = 18,
              .trackerIndex = 2, .trackerMin = 0,
              .repIndex = -1, .repMin = 0,
              .requireFlags = 0, .forbidFlags = 0,
              .requiresVictory = true}},

    // Founded camp: medic pact + camp ready, no safe zone
    {.name = "Camp Founded",
     .note = "You built the safe zone. Others will follow.",
     .cond = {.resourceIndex = -1, .resourceMin = 0, .resourceMax = 0,
              .trackerIndex = -1, .trackerMin = 0,
              .repIndex = -1, .repMin = 0,
              .requireFlags = F_CAMP_READY | F_MEDIC_PACT, .forbidFlags = F_SIGNAL_RECV,
              .requiresVictory = true}},

    // Faction settlement: cult deal + no signal trap
    {.name = "Faction Road",
     .note = "Cult territory. Strange peace, stranger laws.",
     .cond = {.resourceIndex = -1, .resourceMin = 0, .resourceMax = 0,
              .trackerIndex = -1, .trackerMin = 0,
              .repIndex = -1, .repMin = 0,
              .requireFlags = F_CULT_DEAL, .forbidFlags = F_SIGNAL_TRAP,
              .requiresVictory = true}},

    // Convoy dream: high morale, many survivors, signal received
    {.name = "Convoy Dream",
     .note = "Party intact, morale high. A real community.",
     .cond = {.resourceIndex = R_MOR, .resourceMin = 55, .resourceMax = 100,
              .trackerIndex = 2, .trackerMin = 4,
              .repIndex = -1, .repMin = 0,
              .requireFlags = F_SIGNAL_RECV, .forbidFlags = F_SIGNAL_TRAP,
              .requiresVictory = true}},

    // Catch-all victory
    {.name = "Safe Zone",
     .note = "You reached the gates. The road is behind you.",
     .cond = {.resourceIndex = -1, .resourceMin = 0, .resourceMax = 0,
              .trackerIndex = -1, .trackerMin = 0,
              .repIndex = -1, .repMin = 0,
              .requireFlags = 0, .forbidFlags = 0,
              .requiresVictory = true}},

    // Catch-all loss
    {.name = "Collapse",
     .note = "The wasteland took everything. The road wins.",
     .cond = {.resourceIndex = -1, .resourceMin = 0, .resourceMax = 0,
              .trackerIndex = -1, .trackerMin = 0,
              .repIndex = -1, .repMin = 0,
              .requireFlags = 0, .forbidFlags = 0,
              .requiresVictory = false}},
};

// ── TRACKER LABELS ────────────────────────────────────────────────────────────
static const char* const TRACKER_LABELS[] = {
    "Regions", "Scavenges", "Survivors", "Repairs", "Trades", "Days", nullptr, nullptr,
};

// ── GAME DEFINITION ───────────────────────────────────────────────────────────
static const CGC::DeepGameDefinition DEF = {
    .title        = "Tiny Wasteland",
    .slug         = "tiny-wasteland",
    .subtitle     = "Survival road drama",
    .saveMagic    = "TWST2",
    .resources    = RESOURCES,
    .resourceCount = sizeof(RESOURCES) / sizeof(RESOURCES[0]),
    .screens      = SCREENS,
    .screenCount  = sizeof(SCREENS) / sizeof(SCREENS[0]),
    .events       = EVENTS,
    .eventCount   = sizeof(EVENTS) / sizeof(EVENTS[0]),
    .endings      = ENDINGS,
    .endingCount  = sizeof(ENDINGS) / sizeof(ENDINGS[0]),
    .targetProgress = 175,
    .accentColor  = 0xFD20,
    .warningColor = 0xFD20,
    .trackerLabels     = TRACKER_LABELS,
    .trackerLabelCount = 8,
    .objective    = "Drive west; reach a viable ending.",
    .primaryTracker = 0,
};

const CGC::DeepGameDefinition& gameDefinition() {
    return DEF;
}
