#include "GameData.h"

namespace CGC = CardputerGameCore;

// Resource indices (keep names in sync with RESOURCES order).
enum { R_GOLD, R_STAM, R_PACK, R_STRESS, R_WOUND, R_SUSP, R_STAMP };

// Story flags. Screens set these; events advance the arc.
enum : uint32_t {
  F_JOB_ACTIVE   = 1u << 0,  // accepted a contract from BOARD
  F_SUPPLIED     = 1u << 1,  // bought supplies before a route leg
  F_EN_ROUTE     = 1u << 2,  // currently travelling a route leg
  F_PKG_WEIRD    = 1u << 3,  // package is unusual (set by PACKAGE screen)
  F_PKG_OPENED   = 1u << 4,  // courier opened a sealed parcel
  F_CURSED       = 1u << 5,  // a cursed package or act flagged the run
  F_RIVAL_MET    = 1u << 6,  // encountered a rival courier
  F_PERK_BOUGHT  = 1u << 7,  // purchased at least one perk
  F_LATE_GAME    = 1u << 8,  // earned 8+ stamps — guild trial unlocked
};

// ---------------------------------------------------------------------------
// Resources
// ---------------------------------------------------------------------------
static const CGC::ResourceDef RESOURCES[] = {
    // label   start  min   max  prim  safe  warn  bad   inverted
    {"Gold",    45,    0,   999,   0,    0,   15,    5,  false},
    {"Stam",    80,    0,   100,  -1,    5,   28,   12,  false},  // depletion = loss
    {"Pack",   100,    0,   100,   0,    0,   40,   20,  false},  // integrity, low=bad
    {"Stress",   4,    0,   100,   0,    0,   -1,   -1,  false},  // no auto-warn; endings read it
    {"Wound",    0,    0,    12,   0,    0,   -1,   -1,  false},  // no auto-warn; endings read it
    {"Susp",     3,    0,   100,   0,    0,   50,   75,  true},   // high is bad
    {"Stamp",    0,    0,    99,   0,    0,   -1,   -1,  false},  // career counter, no warn
    {"",         0,    0,     0,   0,    0,   -1,   -1,  false},  // unused slot 7
};

// ---------------------------------------------------------------------------
// Screen rows
// ---------------------------------------------------------------------------

// BOARD — pick a contract from the job board
static const CGC::NamedValue ROWS_BOARD[] = {
    {"Letter to Vex",     "Salt trader wants next-day post."},
    {"Herbs to Thorngate","Apothecary: keep dry, no peeking."},
    {"Parcel to Ashfen",  "Wax-sealed. Sender very emphatic."},
    {"Crate to Stoneholt","Heavy. Rattles. Urgent."},
    {"Satchel to Mirwell","Guild docs — time-sensitive."},
    {"Box to the Abbey",  "Marked FRAGILE in three languages."},
    {"Vial to Deepwick",  "Cold glass. Tongs recommended."},
    {"Scroll to Caldrun", "Scholar's parcel. No bending."},
};

// SUPPLIES — stock up before hitting the road
static const CGC::NamedValue ROWS_SUPPLIES[] = {
    {"Rations",     "Trail food eases the road ahead."},
    {"Bandages",    "Cloth and salve for rough routes."},
    {"Torch Oil",   "Light for dark tunnels and caves."},
    {"Rope",        "Bypasses bridge tolls and cliffs."},
    {"Charm Token", "Ward off curses; calm weird boxes."},
    {"Wax Seal",    "Re-seal an opened parcel cleanly."},
    {"Route Map",   "Cuts stamina cost on next leg."},
    {"Courier Ink", "Official ink for suspect signatures."},
};

// ROUTE — travel a leg; different risk/cost profiles
static const CGC::NamedValue ROWS_ROUTE[] = {
    {"King's Road",   "Safe, wide, toll-guarded road."},
    {"Mill Track",    "Common dirt path; moderate risk."},
    {"Marsh Shortcut","Bog crossing; fast but treacherous."},
    {"Ridge Pass",    "Mountain trail; cold and exposed."},
    {"Old Tunnel",    "Crumbling but direct; no weather."},
    {"River Ferry",   "Costs coin but spares the legs."},
    {"Night Run",     "Travel dark; avoid guards and tolls."},
};

// DELIVER — close the contract at the destination
static const CGC::NamedValue ROWS_DELIVER[] = {
    {"Present Parcel",   "Hand it over and collect pay."},
    {"Log the Delivery", "Record in the guild ledger."},
    {"Note Condition",   "Flag parcel state to recipient."},
    {"Collect Tip",      "Good condition earns extra coin."},
    {"Sign & Stamp",     "Earn a guild stamp for the run."},
    {"Explain the Delay","Negotiate if delivery ran late."},
};

// PACKAGE — inspect or handle a weird parcel mid-route
static const CGC::NamedValue ROWS_PACKAGE[] = {
    {"Shake & Listen",  "Rattle test; non-invasive."},
    {"Sniff the Seam",  "Check for leaks or strange smell."},
    {"Peek Inside",     "Break the seal and look in."},
    {"Rebind the Cord", "Tighten loose wrappings."},
    {"Talk to It",      "Some parcels respond to kindness."},
    {"Ignore It",       "Keep walking; it'll settle down."},
};

// PERKS — spend stamps on courier edges
static const CGC::NamedValue ROWS_PERKS[] = {
    {"Route Memory",    "Shortcut costs -2 Stam forever."},
    {"Steady Hands",    "Pack integrity loss -2 per event."},
    {"Silver Tongue",   "Tolls and bandits easier to bluff."},
    {"Medic",           "Bandage rest heals +4 Stam."},
    {"Lucky Stamp",     "Earn bonus stamp on each delivery."},
    {"Night Walker",    "Night Run costs -3 Stam."},
    {"Iron Stomach",    "Rations restore +3 extra Stam."},
    {"Package Whisperer","Weird boxes raise Stress less."},
};

// REST — late-game guild-gated recovery and perk reinforcement
static const CGC::NamedValue ROWS_REST[] = {
    {"Guild Infirmary",  "Full rest and wound treatment."},
    {"Scribe the Route", "Document a path for the guild."},
    {"Mentor a Courier", "Share knowledge; gain renown."},
    {"Review Ledger",    "Audit stamps and career record."},
    {"Plan Trial Route", "Study the guild license trial."},
    {"Accept Chain Job", "Take a linked prestige contract."},
};

// ---------------------------------------------------------------------------
// Screens
// ---------------------------------------------------------------------------
static const CGC::DeepScreenDef SCREENS[] = {
    // BOARD: review contracts, sets F_JOB_ACTIVE
    {.title = "BOARD", .verb = "Accept", .rows = ROWS_BOARD, .rowCount = 8,
     .resourceDeltas = {0, 1, 0, -1, 0, 0, 0, 0},
     .progressDelta = 1, .trackerIndex = 0,
     .flagMask = F_JOB_ACTIVE,
     .costResource = -1, .costAmount = 0, .requireFlags = 0,
     .trackerDelta = 1,
     .objective = "Accept a delivery contract"},

    // SUPPLIES: buy food/bandages/gear before route leg
    {.title = "SUPPLIES", .verb = "Buy", .rows = ROWS_SUPPLIES, .rowCount = 8,
     .resourceDeltas = {0, 4, 5, -1, 0, 0, 0, 0},
     .progressDelta = 1, .trackerIndex = 1,
     .flagMask = F_SUPPLIED,
     .costResource = R_GOLD, .costAmount = 10, .requireFlags = F_JOB_ACTIVE,
     .trackerDelta = 1,
     .objective = "Stock up for the road (-10 Gold)"},

    // ROUTE: travel a leg; costs Stam; triggers events
    {.title = "ROUTE", .verb = "Travel", .rows = ROWS_ROUTE, .rowCount = 7,
     .resourceDeltas = {0, -8, -3, 2, 0, 1, 0, 0},
     .progressDelta = 4, .trackerIndex = 2,
     .flagMask = F_EN_ROUTE,
     .costResource = -1, .costAmount = 0, .requireFlags = F_JOB_ACTIVE,
     .trackerDelta = 1,
     .objective = "Travel a route leg (-8 Stam)"},

    // DELIVER: close the contract; gated on having a job active
    {.title = "DELIVER", .verb = "Deliver", .rows = ROWS_DELIVER, .rowCount = 6,
     .resourceDeltas = {18, 3, 0, -4, 0, -2, 1, 0},
     .progressDelta = 8, .trackerIndex = 3,
     .flagMask = 0,
     .costResource = -1, .costAmount = 0, .requireFlags = F_JOB_ACTIVE | F_EN_ROUTE,
     .trackerDelta = 1,
     .objective = "Complete the contract (+Gold, +Stamp)"},

    // PACKAGE: handle/peek at weird parcel; risky, sets flags
    {.title = "PACKAGE", .verb = "Handle", .rows = ROWS_PACKAGE, .rowCount = 6,
     .resourceDeltas = {0, 0, -4, 3, 0, 2, 0, 0},
     .progressDelta = 2, .trackerIndex = 4,
     .flagMask = F_PKG_WEIRD,
     .costResource = -1, .costAmount = 0, .requireFlags = F_JOB_ACTIVE,
     .trackerDelta = 1,
     .objective = "Investigate the parcel (risky)"},

    // PERKS: spend stamps to buy courier edges
    {.title = "PERKS", .verb = "Learn", .rows = ROWS_PERKS, .rowCount = 8,
     .resourceDeltas = {0, 2, 0, -2, -1, 0, -2, 0},
     .progressDelta = 2, .trackerIndex = 5,
     .flagMask = F_PERK_BOUGHT,
     .costResource = R_STAMP, .costAmount = 3, .requireFlags = 0,
     .trackerDelta = 1,
     .objective = "Buy a courier edge (-3 Stamps)"},

    // REST: late-game guild rest stop; gated on F_LATE_GAME
    {.title = "REST", .verb = "Rest", .rows = ROWS_REST, .rowCount = 6,
     .resourceDeltas = {-5, 15, 8, -5, -2, -3, 0, 0},
     .progressDelta = 3, .trackerIndex = 6,
     .flagMask = 0,
     .costResource = -1, .costAmount = 0, .requireFlags = F_LATE_GAME,
     .trackerDelta = 1,
     .objective = "Guild rest (needs 8 Stamps earned)"},
};

// ---------------------------------------------------------------------------
// Events
// ---------------------------------------------------------------------------
static const CGC::EventDef EVENTS[] = {
    // --- Common neutral filler ---
    {.name = "Quiet road",
     .note = "No trouble. You make good time.",
     .resourceDeltas = {0, 2, 0, -1, 0, 0, 0, 0},
     .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = -1, .repDelta = 0, .weight = 5,
     .requireFlags = 0, .forbidFlags = 0, .tone = CGC::TONE_NEUTRAL},

    {.name = "Gentle rest",
     .note = "A shaded mile lets you recover.",
     .resourceDeltas = {0, 3, 0, -1, 0, 0, 0, 0},
     .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = -1, .repDelta = 0, .weight = 4,
     .requireFlags = 0, .forbidFlags = 0, .tone = CGC::TONE_GOOD},

    // --- Route hazards ---
    {.name = "Bandit stop",
     .note = "Demand payment or surrender the parcel.",
     .resourceDeltas = {-8, -2, 0, 3, 1, 2, 0, 0},
     .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = 0, .repDelta = -1, .weight = 3,
     .requireFlags = F_EN_ROUTE, .forbidFlags = 0, .tone = CGC::TONE_BAD},

    {.name = "Toll gate",
     .note = "Guards expect coin to let you pass.",
     .resourceDeltas = {-6, 0, 0, 1, 0, 1, 0, 0},
     .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = -1, .repDelta = 0, .weight = 3,
     .requireFlags = F_EN_ROUTE, .forbidFlags = 0, .tone = CGC::TONE_BAD},

    {.name = "Storm breaks",
     .note = "Rain batters the parcel and your boots.",
     .resourceDeltas = {0, -4, -6, 2, 0, 0, 0, 0},
     .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = -1, .repDelta = 0, .weight = 3,
     .requireFlags = F_EN_ROUTE, .forbidFlags = 0, .tone = CGC::TONE_BAD},

    {.name = "Mudslide",
     .note = "The path is blocked. Brutal detour.",
     .resourceDeltas = {0, -6, -2, 3, 1, 0, 0, 0},
     .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = -1, .repDelta = 0, .weight = 2,
     .requireFlags = F_EN_ROUTE, .forbidFlags = 0, .tone = CGC::TONE_BAD},

    {.name = "Lucky stamp find",
     .note = "An old guild stamp lies in the road.",
     .resourceDeltas = {0, 0, 0, 0, 0, 0, 1, 0},
     .setFlags = 0, .trackerIndex = 5, .trackerDelta = 1,
     .repIndex = 1, .repDelta = 1, .weight = 2,
     .requireFlags = 0, .forbidFlags = 0, .tone = CGC::TONE_GOOD},

    {.name = "Friendly merchant",
     .note = "Trade a story for coin and rations.",
     .resourceDeltas = {5, 2, 0, -1, 0, 0, 0, 0},
     .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = 0, .repDelta = 1, .weight = 3,
     .requireFlags = 0, .forbidFlags = 0, .tone = CGC::TONE_GOOD},

    // --- Weird / cursed package events ---
    {.name = "Package hums",
     .note = "A low tone from inside. Unsettling.",
     .resourceDeltas = {0, 0, 0, 3, 0, 1, 0, 0},
     .setFlags = F_PKG_WEIRD, .trackerIndex = 4, .trackerDelta = 1,
     .repIndex = -1, .repDelta = 0, .weight = 3,
     .requireFlags = F_JOB_ACTIVE, .forbidFlags = F_PKG_WEIRD, .tone = CGC::TONE_BAD},

    {.name = "Parcel argues",
     .note = "It disagrees with your route choice.",
     .resourceDeltas = {0, -1, 0, 4, 0, 0, 0, 0},
     .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = -1, .repDelta = 0, .weight = 2,
     .requireFlags = F_PKG_WEIRD, .forbidFlags = 0, .tone = CGC::TONE_BAD},

    {.name = "Parcel leaks",
     .note = "Something oozes. Integrity drops fast.",
     .resourceDeltas = {0, 0, -8, 2, 0, 2, 0, 0},
     .setFlags = F_PKG_WEIRD, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = -1, .repDelta = 0, .weight = 2,
     .requireFlags = F_JOB_ACTIVE, .forbidFlags = 0, .tone = CGC::TONE_BAD},

    {.name = "Parcel tempts you",
     .note = "Gold glints through a crack in the seal.",
     .resourceDeltas = {0, 0, 0, 4, 0, 3, 0, 0},
     .setFlags = F_PKG_WEIRD, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = -1, .repDelta = 0, .weight = 2,
     .requireFlags = F_JOB_ACTIVE, .forbidFlags = F_PKG_OPENED, .tone = CGC::TONE_BAD},

    {.name = "Opened and cursed",
     .note = "Inside: a mirror that shows nothing good.",
     .resourceDeltas = {8, 0, -10, 5, 0, 5, 0, 0},
     .setFlags = F_PKG_OPENED | F_CURSED, .trackerIndex = 4, .trackerDelta = 1,
     .repIndex = 2, .repDelta = -2, .weight = 2,
     .requireFlags = F_PKG_WEIRD, .forbidFlags = F_PKG_OPENED, .tone = CGC::TONE_BAD},

    {.name = "Opened: bonus pay",
     .note = "Client hid a note: double rate if intact.",
     .resourceDeltas = {12, 0, 0, 1, 0, 2, 1, 0},
     .setFlags = F_PKG_OPENED, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = 1, .repDelta = 1, .weight = 1,
     .requireFlags = F_PKG_WEIRD, .forbidFlags = F_PKG_OPENED | F_CURSED, .tone = CGC::TONE_GOOD},

    // --- Rival couriers ---
    {.name = "Rival undercuts",
     .note = "Courier Bleck snatches your next client.",
     .resourceDeltas = {-4, 0, 0, 3, 0, 0, 0, 0},
     .setFlags = F_RIVAL_MET, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = 3, .repDelta = -1, .weight = 2,
     .requireFlags = 0, .forbidFlags = F_RIVAL_MET, .tone = CGC::TONE_BAD},

    {.name = "Rival trades favors",
     .note = "Rival Maren swaps a map for silence.",
     .resourceDeltas = {0, 3, 0, 0, 0, -1, 1, 0},
     .setFlags = 0, .trackerIndex = 5, .trackerDelta = 1,
     .repIndex = 1, .repDelta = 1, .weight = 2,
     .requireFlags = F_RIVAL_MET, .forbidFlags = 0, .tone = CGC::TONE_GOOD},

    // --- Late-game arc ---
    {.name = "Guild trial notice",
     .note = "Eight stamps: you qualify for the trial.",
     .resourceDeltas = {0, 0, 0, -2, 0, 0, 0, 0},
     .setFlags = F_LATE_GAME, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = 1, .repDelta = 2, .weight = 3,
     .requireFlags = 0, .forbidFlags = F_LATE_GAME, .tone = CGC::TONE_GOOD},
};

// ---------------------------------------------------------------------------
// Endings (most-specific first)
// ---------------------------------------------------------------------------
static const CGC::EndingDef ENDINGS[] = {
    // 1. Cursed legend: completed run AND opened a cursed parcel
    {.name = "Cursed Legend",
     .note = "Every delivery haunts you. But you delivered.",
     .cond = {.resourceIndex = R_STAMP, .resourceMin = 8, .resourceMax = 99,
              .trackerIndex = -1, .trackerMin = 0,
              .repIndex = -1, .repMin = 0,
              .requireFlags = F_PKG_OPENED | F_CURSED, .forbidFlags = 0,
              .requiresVictory = true}},

    // 2. Rival ruin: victory, rival met, low suspicion — you out-ran them
    {.name = "Rival Ruin",
     .note = "Bleck and Maren never caught up. You did.",
     .cond = {.resourceIndex = R_SUSP, .resourceMin = 0, .resourceMax = 30,
              .trackerIndex = 3, .trackerMin = 6,
              .repIndex = -1, .repMin = 0,
              .requireFlags = F_RIVAL_MET, .forbidFlags = F_CURSED,
              .requiresVictory = true}},

    // 3. Infamous: victory but high suspicion — known but feared
    {.name = "Infamous Courier",
     .note = "Notorious, alive, and still delivering.",
     .cond = {.resourceIndex = R_SUSP, .resourceMin = 60, .resourceMax = 100,
              .trackerIndex = -1, .trackerMin = 0,
              .repIndex = -1, .repMin = 0,
              .requireFlags = 0, .forbidFlags = 0,
              .requiresVictory = true}},

    // 4. Haunted: victory with opened package but not cursed
    {.name = "Haunted Run",
     .note = "You know what was inside. You always will.",
     .cond = {.resourceIndex = -1, .resourceMin = 0, .resourceMax = 0,
              .trackerIndex = -1, .trackerMin = 0,
              .repIndex = -1, .repMin = 0,
              .requireFlags = F_PKG_OPENED, .forbidFlags = F_CURSED,
              .requiresVictory = true}},

    // 5. Trusted career: clean victory with perks and stamps
    {.name = "Trusted Courier",
     .note = "The guild hangs your crest at every gate.",
     .cond = {.resourceIndex = R_STAMP, .resourceMin = 6, .resourceMax = 99,
              .trackerIndex = -1, .trackerMin = 0,
              .repIndex = -1, .repMin = 0,
              .requireFlags = F_PERK_BOUGHT, .forbidFlags = F_CURSED,
              .requiresVictory = true}},

    // 6. Catch-all victory
    {.name = "Career Complete",
     .note = "Stamps in hand, roads behind you.",
     .cond = {.resourceIndex = -1, .resourceMin = 0, .resourceMax = 0,
              .trackerIndex = -1, .trackerMin = 0,
              .repIndex = -1, .repMin = 0,
              .requireFlags = 0, .forbidFlags = 0,
              .requiresVictory = true}},

    // 7. Catch-all loss
    {.name = "Lost in the Road",
     .note = "Stamina spent, parcel lost, career over.",
     .cond = {.resourceIndex = -1, .resourceMin = 0, .resourceMax = 0,
              .trackerIndex = -1, .trackerMin = 0,
              .repIndex = -1, .repMin = 0,
              .requireFlags = 0, .forbidFlags = 0,
              .requiresVictory = false}},
};

// ---------------------------------------------------------------------------
// Tracker labels
// ---------------------------------------------------------------------------
static const char* const TRACKER_LABELS[] = {
    "Contracts", "Supplies", "Routes", "Deliveries",
    "Packages",  "Stamps",   "Rests",  nullptr,
};

// ---------------------------------------------------------------------------
// Game definition
// ---------------------------------------------------------------------------
static const CGC::DeepGameDefinition DEF = {
    .title          = "Dungeon Courier",
    .slug           = "dungeon-courier",
    .subtitle       = "Contract delivery RPG",
    .saveMagic      = "DCR2",
    .resources      = RESOURCES,
    .resourceCount  = sizeof(RESOURCES) / sizeof(RESOURCES[0]),
    .screens        = SCREENS,
    .screenCount    = sizeof(SCREENS) / sizeof(SCREENS[0]),
    .events         = EVENTS,
    .eventCount     = sizeof(EVENTS) / sizeof(EVENTS[0]),
    .endings        = ENDINGS,
    .endingCount    = sizeof(ENDINGS) / sizeof(ENDINGS[0]),
    .targetProgress = 160,
    .accentColor    = 0xB81F,
    .warningColor   = 0xFD20,
    .trackerLabels      = TRACKER_LABELS,
    .trackerLabelCount  = 8,
    .objective          = "Deliver contracts; build a courier career.",
    .primaryTracker     = 3,
};

const CGC::DeepGameDefinition& gameDefinition() {
  return DEF;
}
