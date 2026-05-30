#include "GameData.h"

namespace CGC = CardputerGameCore;

// Resource indices (keep in sync with RESOURCES order).
enum {
  R_FUNDS,   // Funding — currency for gear and repairs
  R_STAM,    // Stamina — patrol limit; hitting 0 ends the run
  R_SUPPLY,  // Supplies — consumables, bait, sample jars
  R_BATT,    // Camera battery — needed for photo/video evidence
  R_PROOF,   // Evidence counter — cumulative proof gathered
  R_FEAR,    // Fear pressure — high is bad (inverted)
  R_ETHICS,  // Reputation/Ethics — campers, scientists, park service
};

// Story-arc flags. Set cleanly by screens, advanced by events.
enum : uint32_t {
  F_REPORT_OPEN   = 1u << 0,  // an active patrol report is set
  F_PRINTS_FOUND  = 1u << 1,  // physical print evidence collected
  F_PHOTO_TAKEN   = 1u << 2,  // photographic evidence captured
  F_AUDIO_LOGGED  = 1u << 3,  // audio evidence recorded
  F_GUIDE_ENTRY   = 1u << 4,  // first field-guide entry completed
  F_ETHICS_BREACH = 1u << 5,  // accepted exploitative sponsor deal
  F_MYSTERY_DEEP  = 1u << 6,  // deep preserve mystery uncovered
  F_PRESERVE_OPEN = 1u << 7,  // deep preserve zone unlocked
};

// ── RESOURCES ──────────────────────────────────────────────────────────────
static const CGC::ResourceDef RESOURCES[] = {
    // label    start  min  max  prim safe warn  bad  inverted
    {"Funds",   45,    0,   999,  0,   0,  14,   5,  false},
    {"Stam",    80,    0,   100, -1,   5,  28,  12,  false},  // loss trigger
    {"Supply",  30,    0,    99,  0,   0,  10,   4,  false},
    {"Batt",    90,    0,   100,  0,   6,  25,  10,  false},
    {"Proof",    0,    0,    99,  0,   0,  -1,  -1,  false},  // evidence counter
    {"Fear",     4,    0,   100,  1,  -3,  50,  75,  true},   // high is bad
    {"Ethics",  40,    0,   100,  0,   1,  18,   8,  false},
};

// ── SCREEN ROWS ────────────────────────────────────────────────────────────

// STATION — read and plan active patrol reports
static const CGC::NamedValue ROWS_STATION[] = {
    {"Camper Missing",   "Search report for lost hiker trail."},
    {"Fence Damage",     "Perimeter breach near the bog edge."},
    {"Odd Lights",       "Local reports: lights over ridge lake."},
    {"Strange Tracks",   "Deep prints near the old campground."},
    {"Injured Animal",   "Wildlife ranger needs assist, road 4."},
    {"Illegal Logging",  "Bootleg cut spotted on east slope."},
    {"Research Grant",   "University team asks for field data."},
    {"Review Notes",     "Reread open reports, adjust plan."},
};

// PATROL — travel through park zones, gather raw evidence
static const CGC::NamedValue ROWS_PATROL[] = {
    {"Pine Forest",   "Sweep the pines for prints and scat."},
    {"Mirror Lake",   "Walk shoreline for hair and watermarks."},
    {"Ridgeline 3",   "Climb elk trail; watch for wing sign."},
    {"Cave Mouth",    "Check entrance for nesting and musk."},
    {"Old Campground","Scan the cleared ground for track sets."},
    {"Fire Road",     "Walk the road edge; note carcasses."},
    {"Black Bog",     "Wade the margins; sample mud and reed."},
    {"Summit Trail",  "Glass the summit meadow at treeline."},
};

// GEAR — equip cameras, kits, and tools before patrol
static const CGC::NamedValue ROWS_GEAR[] = {
    {"Trail Camera",  "Deploy a camera on the known game trail."},
    {"Audio Recorder","Set a recorder near the ridge water gap."},
    {"Sample Kit",    "Pack jars, swabs, and cast plaster."},
    {"Thermal Imager","Loan the thermal unit from the station."},
    {"Extra Battery", "Buy two spare cold-rated battery cells."},
    {"Medkit",        "Restock the field aid kit from supply."},
    {"Bait Station",  "Set a scent lure at a likely crossing."},
};

// RESCUE — respond to camper and wildlife distress calls
static const CGC::NamedValue ROWS_RESCUE[] = {
    {"Find Lost Hiker",  "Guide the hiker back to trailhead."},
    {"Treat Injured Elk","Splint the elk and radio the vet."},
    {"Calm Panicked Camp","Settle the campsite; explain signs."},
    {"Pull Fence Stray", "Free animal caught in fence damage."},
    {"Aid Research Team","Help university team reach safe zone."},
    {"Evacuate Birders", "Move birder group from ridge flash."},
};

// GUIDE — study field-guide entries and cross-reference evidence
static const CGC::NamedValue ROWS_GUIDE[] = {
    {"Mossback",      "Note habitat, print shape, bark rub."},
    {"Lake Wisp",     "Log call frequency and wake pattern."},
    {"Ridge Antler",  "Cross-ref molt cycle and elevation."},
    {"Cave Mute",     "Sketch echolocation bounce pattern."},
    {"Ash Cat",       "Record track depth and claw spread."},
    {"Bog Lamp",      "Note bioluminescent trigger weather."},
    {"Summit Moth",   "Map wing-dust zone and wind drift."},
    {"Pine Crown",    "Log branch-bend height and tree age."},
};

// DOCUMENT — resolve gathered evidence into a guide entry (gated)
static const CGC::NamedValue ROWS_DOCUMENT[] = {
    {"File Print Data",  "Assemble cast and photo into entry."},
    {"Log Audio Clip",   "Tag the recording with GPS context."},
    {"Submit Samples",   "Send field samples to the lab queue."},
    {"Write Sighting",   "Transcribe the full sighting notes."},
    {"Compile Report",   "Package evidence into the station."},
    {"Brief Scientist",  "Walk researcher through field data."},
};

// PRESERVE — deep-preserve mystery zone (gated on F_PRESERVE_OPEN)
static const CGC::NamedValue ROWS_PRESERVE[] = {
    {"Deep Hollow",      "Enter the old-growth hollow; listen."},
    {"Underground Spring","Follow the sound to the spring room."},
    {"Migration Wall",   "Document the carved migration map."},
    {"Nest Complex",     "Observe the multi-species nest site."},
    {"Summit Lair",      "Track the summit signature to its den."},
};

// ── SCREENS ────────────────────────────────────────────────────────────────
static const CGC::DeepScreenDef SCREENS[] = {
    // STATION: plan reports, recover stamina, gain Ethics — free action
    {.title = "STATION", .verb = "Plan", .rows = ROWS_STATION, .rowCount = 8,
     .resourceDeltas  = {0, 3, 0, 2, 0, -1, 1, 0},
     .progressDelta   = 1, .trackerIndex = 0,
     .flagMask        = F_REPORT_OPEN,
     .costResource    = -1, .costAmount = 0, .requireFlags = 0,
     .trackerDelta    = 1, .objective   = "Plan report; recover Stam"},

    // PATROL: costs Stamina; yields Proof + prints flag
    {.title = "PATROL", .verb = "Patrol", .rows = ROWS_PATROL, .rowCount = 8,
     .resourceDeltas  = {0, -5, -1, -1, 2, 1, 0, 0},
     .progressDelta   = 3, .trackerIndex = 1,
     .flagMask        = F_PRINTS_FOUND,
     .costResource    = R_STAM, .costAmount = 10, .requireFlags = 0,
     .trackerDelta    = 1, .objective   = "Patrol zone (-10 Stam)"},

    // GEAR: costs Funds; charges battery, replenishes supply
    {.title = "GEAR", .verb = "Equip", .rows = ROWS_GEAR, .rowCount = 7,
     .resourceDeltas  = {0, 0, 3, 5, 0, -1, 0, 0},
     .progressDelta   = 1, .trackerIndex = 2,
     .flagMask        = 0,
     .costResource    = R_FUNDS, .costAmount = 12, .requireFlags = 0,
     .trackerDelta    = 1, .objective   = "Equip gear (-12 Funds)"},

    // RESCUE: costs Stamina; boosts Ethics and Funds (donations), cuts Fear
    {.title = "RESCUE", .verb = "Help", .rows = ROWS_RESCUE, .rowCount = 6,
     .resourceDeltas  = {4, -4, -1, 0, 0, -2, 3, 0},
     .progressDelta   = 2, .trackerIndex = 3,
     .flagMask        = F_PHOTO_TAKEN,
     .costResource    = R_STAM, .costAmount = 8, .requireFlags = 0,
     .trackerDelta    = 1, .objective   = "Help people (-8 Stam, +Ethics)"},

    // GUIDE: free study; needs no flags; adds Guide tracker, boosts Ethics
    {.title = "GUIDE", .verb = "Study", .rows = ROWS_GUIDE, .rowCount = 8,
     .resourceDeltas  = {0, -1, 0, 0, 0, -1, 2, 0},
     .progressDelta   = 2, .trackerIndex = 4,
     .flagMask        = F_AUDIO_LOGGED,
     .costResource    = -1, .costAmount = 0, .requireFlags = 0,
     .trackerDelta    = 1, .objective   = "Study and log field guide"},

    // DOCUMENT: costs Supply; gated on prints + photo; resolves proof into guide entry
    {.title = "DOCUMENT", .verb = "Resolve", .rows = ROWS_DOCUMENT, .rowCount = 6,
     .resourceDeltas  = {6, -2, -2, 0, 3, -2, 2, 0},
     .progressDelta   = 6, .trackerIndex = 5,
     .flagMask        = F_GUIDE_ENTRY,
     .costResource    = R_SUPPLY, .costAmount = 4,
     .requireFlags    = F_PRINTS_FOUND | F_PHOTO_TAKEN,
     .trackerDelta    = 1, .objective   = "File entry (needs Prints+Photo)"},

    // PRESERVE: deep mystery zone — gated on guide entry + mystery flag
    {.title = "PRESERVE", .verb = "Explore", .rows = ROWS_PRESERVE, .rowCount = 5,
     .resourceDeltas  = {0, -6, -2, -2, 4, 2, 1, 0},
     .progressDelta   = 8, .trackerIndex = 6,
     .flagMask        = F_MYSTERY_DEEP,
     .costResource    = R_STAM, .costAmount = 14,
     .requireFlags    = F_GUIDE_ENTRY | F_PRESERVE_OPEN,
     .trackerDelta    = 1, .objective   = "Explore preserve (-14 Stam)"},
};

// ── EVENTS ─────────────────────────────────────────────────────────────────
static const CGC::EventDef EVENTS[] = {
    // ── Common fillers ──
    {.name = "Quiet morning",
     .note = "The park breathes; you catch your breath.",
     .resourceDeltas = {0, 2, 0, 0, 0, -1, 0, 0},
     .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = -1, .repDelta = 0, .weight = 5,
     .requireFlags = 0, .forbidFlags = 0, .tone = CGC::TONE_NEUTRAL},

    {.name = "Crisp trail sign",
     .note = "Clear prints in soft mud — textbook cast.",
     .resourceDeltas = {0, 0, -1, 0, 2, -1, 1, 0},
     .setFlags = F_PRINTS_FOUND, .trackerIndex = 4, .trackerDelta = 1,
     .repIndex = 1, .repDelta = 1, .weight = 4,
     .requireFlags = 0, .forbidFlags = 0, .tone = CGC::TONE_GOOD},

    {.name = "Rain rolls in",
     .note = "Storm washes prints; you retreat early.",
     .resourceDeltas = {0, -3, -1, -2, 0, 2, 0, 0},
     .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = -1, .repDelta = 0, .weight = 3,
     .requireFlags = 0, .forbidFlags = 0, .tone = CGC::TONE_BAD},

    {.name = "Camper grateful",
     .note = "A hiker donates to the station fund.",
     .resourceDeltas = {5, 1, 0, 0, 0, -1, 2, 0},
     .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = 0, .repDelta = 2, .weight = 3,
     .requireFlags = 0, .forbidFlags = 0, .tone = CGC::TONE_GOOD},

    {.name = "Battery fault",
     .note = "Cold snap kills a cell without warning.",
     .resourceDeltas = {0, 0, 0, -10, 0, 2, 0, 0},
     .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = -1, .repDelta = 0, .weight = 2,
     .requireFlags = 0, .forbidFlags = 0, .tone = CGC::TONE_BAD},

    {.name = "Ranger burnout",
     .note = "You pushed too far; legs won't cooperate.",
     .resourceDeltas = {0, -5, 0, 0, 0, 3, -1, 0},
     .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = -1, .repDelta = 0, .weight = 2,
     .requireFlags = 0, .forbidFlags = 0, .tone = CGC::TONE_BAD},

    // ── Report-gated events ──
    {.name = "Fresh sighting",
     .note = "Credible sighting near the lake shallows.",
     .resourceDeltas = {0, 0, 0, -1, 2, 1, 1, 0},
     .setFlags = F_PHOTO_TAKEN, .trackerIndex = 4, .trackerDelta = 1,
     .repIndex = 1, .repDelta = 2, .weight = 3,
     .requireFlags = F_REPORT_OPEN, .forbidFlags = 0, .tone = CGC::TONE_GOOD},

    {.name = "Territorial display",
     .note = "Creature charges briefly — you back off.",
     .resourceDeltas = {0, -2, 0, 0, 1, 4, -1, 0},
     .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = -1, .repDelta = 0, .weight = 2,
     .requireFlags = F_REPORT_OPEN, .forbidFlags = 0, .tone = CGC::TONE_BAD},

    {.name = "Vocalization logged",
     .note = "Recorder catches a multi-note call at dusk.",
     .resourceDeltas = {0, 0, -1, -2, 2, -1, 1, 0},
     .setFlags = F_AUDIO_LOGGED, .trackerIndex = 4, .trackerDelta = 1,
     .repIndex = 1, .repDelta = 1, .weight = 3,
     .requireFlags = F_REPORT_OPEN, .forbidFlags = 0, .tone = CGC::TONE_GOOD},

    // ── Prints-gated ──
    {.name = "Second print set",
     .note = "A second track line — different individual.",
     .resourceDeltas = {0, 0, -1, 0, 2, 0, 1, 0},
     .setFlags = 0, .trackerIndex = 5, .trackerDelta = 1,
     .repIndex = 1, .repDelta = 1, .weight = 3,
     .requireFlags = F_PRINTS_FOUND, .forbidFlags = 0, .tone = CGC::TONE_GOOD},

    {.name = "Bait sprung",
     .note = "Lure worked; camera caught partial fur.",
     .resourceDeltas = {0, 0, -2, -3, 2, 1, 0, 0},
     .setFlags = F_PHOTO_TAKEN, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = -1, .repDelta = 0, .weight = 2,
     .requireFlags = F_PRINTS_FOUND, .forbidFlags = 0, .tone = CGC::TONE_GOOD},

    // ── Ethics decision: sponsor pressure ──
    {.name = "Sponsor offer",
     .note = "Cash for sensational proof — no ethics.",
     .resourceDeltas = {12, 0, 0, 0, 0, 2, -4, 0},
     .setFlags = F_ETHICS_BREACH, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = 3, .repDelta = 3, .weight = 1,
     .requireFlags = F_GUIDE_ENTRY, .forbidFlags = F_ETHICS_BREACH,
     .tone = CGC::TONE_BAD},

    // ── Illegal logging ──
    {.name = "Bootleg cutters",
     .note = "Loggers spook a nesting site; you file.",
     .resourceDeltas = {0, -2, 0, 0, 0, 3, -2, 0},
     .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = 2, .repDelta = -2, .weight = 2,
     .requireFlags = F_REPORT_OPEN, .forbidFlags = 0, .tone = CGC::TONE_BAD},

    // ── Breakthrough ──
    {.name = "Breakthrough",
     .note = "Evidence pieces click — clear picture now.",
     .resourceDeltas = {0, 0, 0, 0, 3, -2, 2, 0},
     .setFlags = 0, .trackerIndex = 5, .trackerDelta = 1,
     .repIndex = 1, .repDelta = 2, .weight = 2,
     .requireFlags = F_PRINTS_FOUND | F_PHOTO_TAKEN, .forbidFlags = 0,
     .tone = CGC::TONE_GOOD},

    // ── Guide-gated: preserve mystery ──
    {.name = "Deep preserve sign",
     .note = "Old-growth marks suggest a larger range.",
     .resourceDeltas = {0, 0, 0, 0, 2, 1, 1, 0},
     .setFlags = F_PRESERVE_OPEN, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = -1, .repDelta = 0, .weight = 2,
     .requireFlags = F_GUIDE_ENTRY, .forbidFlags = F_PRESERVE_OPEN,
     .tone = CGC::TONE_GOOD},

    // ── Preserve-gated: mystery resolved ──
    {.name = "Migration corridor",
     .note = "The preserve links two unknown zones.",
     .resourceDeltas = {0, 0, 0, 0, 4, -2, 2, 0},
     .setFlags = F_MYSTERY_DEEP, .trackerIndex = 6, .trackerDelta = 1,
     .repIndex = -1, .repDelta = 0, .weight = 3,
     .requireFlags = F_PRESERVE_OPEN, .forbidFlags = F_MYSTERY_DEEP,
     .tone = CGC::TONE_GOOD},

    // ── Mystery-deep good event ──
    {.name = "New species trace",
     .note = "Markings match nothing in the database.",
     .resourceDeltas = {0, 0, 0, -1, 4, -2, 3, 0},
     .setFlags = 0, .trackerIndex = 5, .trackerDelta = 1,
     .repIndex = 1, .repDelta = 3, .weight = 3,
     .requireFlags = F_MYSTERY_DEEP, .forbidFlags = F_ETHICS_BREACH,
     .tone = CGC::TONE_GOOD},
};

// ── ENDINGS ────────────────────────────────────────────────────────────────
// Ordered most-specific first; first match wins.
static const CGC::EndingDef ENDINGS[] = {
    // 1. Mystery Solved — only if ethics clean, deep mystery found, high proof
    {.name = "Mystery Solved",
     .note = "Your corridor data reshapes field biology.",
     .cond = {.resourceIndex = R_PROOF,  .resourceMin = 18, .resourceMax = 99,
              .trackerIndex  = 6,        .trackerMin  = 2,
              .repIndex      = -1,       .repMin      = 0,
              .requireFlags  = F_MYSTERY_DEEP,
              .forbidFlags   = F_ETHICS_BREACH,
              .requiresVictory = true}},

    // 2. Science Guide — guide entries complete, high ethics, no breach
    {.name = "Science Guide",
     .note = "The field guide becomes the reference work.",
     .cond = {.resourceIndex = R_ETHICS, .resourceMin = 50, .resourceMax = 100,
              .trackerIndex  = 4,        .trackerMin  = 5,
              .repIndex      = 1,        .repMin      = 3,
              .requireFlags  = F_GUIDE_ENTRY,
              .forbidFlags   = F_ETHICS_BREACH,
              .requiresVictory = true}},

    // 3. Park Guardian — strong rescues and ethics, solid proof
    {.name = "Park Guardian",
     .note = "The park service names you chief ranger.",
     .cond = {.resourceIndex = R_ETHICS, .resourceMin = 40, .resourceMax = 100,
              .trackerIndex  = 3,        .trackerMin  = 3,
              .repIndex      = 2,        .repMin      = 2,
              .requireFlags  = 0,
              .forbidFlags   = F_ETHICS_BREACH,
              .requiresVictory = true}},

    // 4. Ethics Breach — took sponsor money; locked from best endings
    {.name = "Ethics Breach",
     .note = "Sensational footage aired. The park suffers.",
     .cond = {.resourceIndex = -1, .resourceMin = 0, .resourceMax = 0,
              .trackerIndex  = -1, .trackerMin  = 0,
              .repIndex      = -1, .repMin      = 0,
              .requireFlags  = F_ETHICS_BREACH,
              .forbidFlags   = 0,
              .requiresVictory = true}},

    // 5. Town Legend — catch-all victory (ethics clean)
    {.name = "Town Legend",
     .note = "Stories persist; the park endures quietly.",
     .cond = {.resourceIndex = -1, .resourceMin = 0, .resourceMax = 0,
              .trackerIndex  = -1, .trackerMin  = 0,
              .repIndex      = -1, .repMin      = 0,
              .requireFlags  = 0,
              .forbidFlags   = 0,
              .requiresVictory = true}},

    // 6. Lost Patrol — catch-all loss (Stamina depleted)
    {.name = "Lost Patrol",
     .note = "You ran yourself empty in the deep preserve.",
     .cond = {.resourceIndex = -1, .resourceMin = 0, .resourceMax = 0,
              .trackerIndex  = -1, .trackerMin  = 0,
              .repIndex      = -1, .repMin      = 0,
              .requireFlags  = 0,
              .forbidFlags   = 0,
              .requiresVictory = false}},
};

// ── TRACKER LABELS ─────────────────────────────────────────────────────────
// Index: 0=Reports 1=Patrols 2=Gear 3=Rescues 4=Guide 5=Reports 6=Preserve
static const char* const TRACKER_LABELS[] = {
    "Reports", "Patrols", "Gear", "Rescues", "Guide", "Filings", "Preserve", nullptr,
};

// ── GAME DEFINITION ────────────────────────────────────────────────────────
static const CGC::DeepGameDefinition DEF = {
    .title         = "Cryptid Ranger",
    .slug          = "cryptid-park-ranger",
    .subtitle      = "Ethical field research",
    .saveMagic     = "CPR2",
    .resources     = RESOURCES,
    .resourceCount = sizeof(RESOURCES) / sizeof(RESOURCES[0]),
    .screens       = SCREENS,
    .screenCount   = sizeof(SCREENS) / sizeof(SCREENS[0]),
    .events        = EVENTS,
    .eventCount    = sizeof(EVENTS) / sizeof(EVENTS[0]),
    .endings       = ENDINGS,
    .endingCount   = sizeof(ENDINGS) / sizeof(ENDINGS[0]),
    .targetProgress   = 150,
    .accentColor      = 0x07E0,
    .warningColor     = 0xFD20,
    .trackerLabels    = TRACKER_LABELS,
    .trackerLabelCount = 8,
    .objective     = "Document cryptids; protect the park.",
    .primaryTracker = 5,
};

const CGC::DeepGameDefinition& gameDefinition() {
    return DEF;
}
