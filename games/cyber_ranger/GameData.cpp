#include "GameData.h"

namespace CGC = CardputerGameCore;

// Resource indices (keep names in sync with RESOURCES order).
enum { R_FUNDS, R_STAM, R_BATT, R_HEAT, R_BIO, R_SIG, R_TRUST };

// Story flags. Screens set these cleanly (no shift), events advance the arc.
enum : uint32_t {
  F_CASE_OPEN     = 1u << 0,  // a case is on the board
  F_BIO           = 1u << 1,  // gathered biological evidence
  F_SIG           = 1u << 2,  // gathered signal evidence
  F_CORP_PROOF    = 1u << 3,  // found proof of hidden corporate sensors
  F_ENDGAME       = 1u << 4,  // the signal has answered
  F_ETHICS_BREACH = 1u << 5,  // took sponsor money to bury findings
};

static const CGC::ResourceDef RESOURCES[] = {
    // label   start min  max  prim safe  warn bad  inverted
    {"Funds",   40,  0,   999,  0,   0,   12,   4,  false},
    {"Stam",    80,  0,   100, -1,   6,   30,  12,  false},  // depletion ends the run
    {"Batt",    85,  0,   100,  0,   8,   25,  10,  false},
    {"Heat",     5,  0,   100,  1,  -4,   55,  80,  true},   // suspicion: high is bad
    {"Bio",      0,  0,    99,  0,   0,   -1,  -1,  false},
    {"Sig",      0,  0,    99,  0,   0,   -1,  -1,  false},
    {"Trust",   35,  0,   100,  0,   1,   20,   8,  false},
};

static const CGC::NamedValue ROWS_STATION[] = {
    {"Pine Case", "A logger reports lights over Pine Hollow."},
    {"Lake Case", "Anglers hear a voice under Mirror Lake."},
    {"Ridge Case", "Tagged elk vanish along Ridgeline 3."},
    {"Bog Case", "Methane readings spike in the Black Bog."},
    {"Lodge Case", "The old lodge radio keys itself at night."},
    {"Road Case", "Dashcams glitch on the fire road at dusk."},
    {"Cave Case", "Spelunkers find warm cable in cold caves."},
    {"Review Notes", "Re-read the open case file and plan."},
};

static const CGC::NamedValue ROWS_PARK[] = {
    {"Pine Hollow", "Sweep the pines for prints and scat."},
    {"Mirror Lake", "Walk the shoreline for tracks and hair."},
    {"Ridgeline 3", "Climb to the elk trail and tag points."},
    {"Black Bog", "Wade the margins; watch your footing."},
    {"Fire Road", "Patrol the service road for carcasses."},
    {"Cave Mouth", "Check the cave entrance for nesting signs."},
    {"Meadow Rim", "Glass the meadow at the treeline."},
};

static const CGC::NamedValue ROWS_SIGNAL[] = {
    {"Weather Echo", "Tune the band for the storm-echo pattern."},
    {"Tag Glitch", "Chase the corrupted wildlife-tag ping."},
    {"Sensor Ghost", "Triangulate a pulse with no listed source."},
    {"Bio Trace", "Isolate the faint bioelectric carrier."},
    {"Relay Hum", "Find the hum bleeding off the ridge relay."},
    {"Buried Line", "Sniff a signal coming from underground."},
};

static const CGC::NamedValue ROWS_DECK[] = {
    {"Scan", "Sweep nearby bands and log what answers."},
    {"Map", "Render the local node map from captures."},
    {"Decode", "Run the cipher pass on a captured packet."},
    {"Tag Read", "Pull the ID off a wildlife tag at range."},
    {"Log Pull", "Dump a hidden sensor's recent log."},
    {"Scrub", "Spoof your trace and shed some heat."},
};

static const CGC::NamedValue ROWS_GUIDE[] = {
    {"Pinewake", "Study the antlered pinewake sightings."},
    {"Lakewire", "Cross-reference the lakewire hydrophone."},
    {"Ridgecat", "Note the ridgecat's thermal habits."},
    {"Bogleaf", "Catalog the bogleaf's false-light lure."},
    {"Cave Echo", "Map the cave-echo's call-and-answer."},
    {"Meadow Crown", "Sketch the meadow crown's molt cycle."},
};

static const CGC::NamedValue ROWS_CASE[] = {
    {"File Report", "Assemble bio + signal proof into a case."},
    {"Brief Service", "Walk the park service through findings."},
    {"Publish", "Hand the dossier to a science partner."},
    {"Confront", "Take the proof straight to the sponsor."},
    {"Archive", "Lock the evidence in the cold archive."},
    {"Broadcast", "Answer the signal on its own band."},
};

static const CGC::NamedValue ROWS_GEAR[] = {
    {"Scanner", "Tune the scanner to cut battery drain."},
    {"Mapper", "Upgrade the mapper's cache and optics."},
    {"Decoder", "Load a faster decoder rulebook."},
    {"Tag Reader", "Boost the tag reader's antenna gain."},
    {"Shield", "Patch the deck's trace shield."},
    {"Cells", "Buy fresh cold-rated battery cells."},
};

static const CGC::DeepScreenDef SCREENS[] = {
    {.title = "STATION", .verb = "Plan", .rows = ROWS_STATION, .rowCount = 8,
     .resourceDeltas = {0, 2, 2, -1, 0, 0, 1, 0}, .progressDelta = 1, .trackerIndex = 0,
     .flagMask = F_CASE_OPEN, .costResource = -1, .costAmount = 0, .requireFlags = 0,
     .trackerDelta = 1, .objective = "Open and plan a case"},

    {.title = "PARK", .verb = "Patrol", .rows = ROWS_PARK, .rowCount = 7,
     .resourceDeltas = {0, -4, -1, 0, 2, 0, 1, 0}, .progressDelta = 2, .trackerIndex = 1,
     .flagMask = F_BIO, .costResource = R_STAM, .costAmount = 8, .requireFlags = 0,
     .trackerDelta = 1, .objective = "Patrol for bio evidence (-8 Stam)"},

    {.title = "SIGNAL", .verb = "Inspect", .rows = ROWS_SIGNAL, .rowCount = 6,
     .resourceDeltas = {0, -1, -3, 3, 0, 2, 0, 0}, .progressDelta = 2, .trackerIndex = 2,
     .flagMask = F_SIG, .costResource = R_BATT, .costAmount = 6, .requireFlags = 0,
     .trackerDelta = 1, .objective = "Inspect anomalies (-6 Batt, +Heat)"},

    {.title = "DECK", .verb = "Run", .rows = ROWS_DECK, .rowCount = 6,
     .resourceDeltas = {0, -1, -5, -2, 1, 1, 0, 0}, .progressDelta = 2, .trackerIndex = 3,
     .flagMask = F_CORP_PROOF, .costResource = R_BATT, .costAmount = 8, .requireFlags = F_SIG,
     .trackerDelta = 1, .objective = "Run deck tools (needs Sig, -8 Batt)"},

    {.title = "GUIDE", .verb = "Study", .rows = ROWS_GUIDE, .rowCount = 6,
     .resourceDeltas = {0, -1, 1, -1, 0, 0, 2, 0}, .progressDelta = 1, .trackerIndex = 4,
     .flagMask = 0, .costResource = -1, .costAmount = 0, .requireFlags = 0,
     .trackerDelta = 1, .objective = "Build the field guide"},

    {.title = "CASE", .verb = "Resolve", .rows = ROWS_CASE, .rowCount = 6,
     .resourceDeltas = {6, -2, -2, 1, -1, -1, 4, 0}, .progressDelta = 6, .trackerIndex = 5,
     .flagMask = F_ENDGAME, .costResource = -1, .costAmount = 0, .requireFlags = F_BIO | F_SIG,
     .trackerDelta = 1, .objective = "Resolve case (needs Bio + Sig)"},

    {.title = "GEAR", .verb = "Upgrade", .rows = ROWS_GEAR, .rowCount = 6,
     .resourceDeltas = {0, 0, 4, -3, 0, 0, 1, 0}, .progressDelta = 1, .trackerIndex = 6,
     .flagMask = 0, .costResource = R_FUNDS, .costAmount = 14, .requireFlags = 0,
     .trackerDelta = 1, .objective = "Upgrade gear (-14 Funds)"},
};

static const CGC::EventDef EVENTS[] = {
    {.name = "Quiet trail", .note = "The preserve is calm; you catch your breath.",
     .resourceDeltas = {0, 2, 0, 0, 0, 0, 0, 0}, .setFlags = 0, .trackerIndex = -1,
     .trackerDelta = 0, .repIndex = -1, .repDelta = 0, .weight = 4, .requireFlags = 0,
     .forbidFlags = 0, .tone = CGC::TONE_NEUTRAL},

    {.name = "Fresh tracks", .note = "Clean cryptid prints set in the mud.",
     .resourceDeltas = {0, 0, 0, 0, 2, 0, 1, 0}, .setFlags = 0, .trackerIndex = 4,
     .trackerDelta = 1, .repIndex = 1, .repDelta = 1, .weight = 3, .requireFlags = F_CASE_OPEN,
     .forbidFlags = 0, .tone = CGC::TONE_GOOD},

    {.name = "Clean signal", .note = "A crisp trace cuts through the noise.",
     .resourceDeltas = {0, 0, 1, -2, 0, 2, 0, 0}, .setFlags = 0, .trackerIndex = -1,
     .trackerDelta = 0, .repIndex = -1, .repDelta = 0, .weight = 3, .requireFlags = F_CASE_OPEN,
     .forbidFlags = 0, .tone = CGC::TONE_GOOD},

    {.name = "Ranger burnout", .note = "You push too hard and pay for it.",
     .resourceDeltas = {0, -6, 0, 4, 0, 0, -1, 0}, .setFlags = 0, .trackerIndex = -1,
     .trackerDelta = 0, .repIndex = -1, .repDelta = 0, .weight = 2, .requireFlags = 0,
     .forbidFlags = 0, .tone = CGC::TONE_BAD},

    {.name = "Sponsor drone", .note = "A corporate drone logs your position.",
     .resourceDeltas = {0, 0, -2, 6, 0, 0, -1, 0}, .setFlags = 0, .trackerIndex = -1,
     .trackerDelta = 0, .repIndex = 3, .repDelta = -1, .weight = 2, .requireFlags = F_SIG,
     .forbidFlags = 0, .tone = CGC::TONE_BAD},

    {.name = "Hidden sensor", .note = "You find an unlogged corporate sensor.",
     .resourceDeltas = {0, 0, 0, 3, 0, 1, 0, 0}, .setFlags = F_CORP_PROOF, .trackerIndex = -1,
     .trackerDelta = 0, .repIndex = 1, .repDelta = 2, .weight = 2, .requireFlags = F_SIG,
     .forbidFlags = F_CORP_PROOF, .tone = CGC::TONE_GOOD},

    {.name = "Battery fault", .note = "A cell fails in the cold.",
     .resourceDeltas = {0, 0, -8, 0, 0, 0, 0, 0}, .setFlags = 0, .trackerIndex = -1,
     .trackerDelta = 0, .repIndex = -1, .repDelta = 0, .weight = 1, .requireFlags = 0,
     .forbidFlags = 0, .tone = CGC::TONE_BAD},

    {.name = "Grateful campers", .note = "Campers tip you and spread the word.",
     .resourceDeltas = {6, 0, 0, 0, 0, 0, 2, 0}, .setFlags = 0, .trackerIndex = -1,
     .trackerDelta = 0, .repIndex = 0, .repDelta = 3, .weight = 2, .requireFlags = 0,
     .forbidFlags = 0, .tone = CGC::TONE_GOOD},

    {.name = "Storm rolls in", .note = "Rain drives you back toward shelter.",
     .resourceDeltas = {0, -3, -2, 0, 0, 0, 0, 0}, .setFlags = 0, .trackerIndex = -1,
     .trackerDelta = 0, .repIndex = -1, .repDelta = 0, .weight = 2, .requireFlags = 0,
     .forbidFlags = 0, .tone = CGC::TONE_BAD},

    {.name = "Sponsor offer", .note = "Easy money to bury what you found.",
     .resourceDeltas = {10, 0, 0, -2, 0, 0, -3, 0}, .setFlags = F_ETHICS_BREACH,
     .trackerIndex = -1, .trackerDelta = 0, .repIndex = 3, .repDelta = 3, .weight = 1,
     .requireFlags = F_CORP_PROOF, .forbidFlags = 0, .tone = CGC::TONE_BAD},

    {.name = "Breakthrough", .note = "The pieces finally line up.",
     .resourceDeltas = {0, 0, 0, 0, 1, 1, 1, 0}, .setFlags = 0, .trackerIndex = 5,
     .trackerDelta = 1, .repIndex = -1, .repDelta = 0, .weight = 2, .requireFlags = F_BIO | F_SIG,
     .forbidFlags = 0, .tone = CGC::TONE_GOOD},

    {.name = "The signal answers", .note = "Something on the band knows your name.",
     .resourceDeltas = {0, 0, 0, 0, 0, 3, 2, 0}, .setFlags = F_ENDGAME, .trackerIndex = -1,
     .trackerDelta = 0, .repIndex = -1, .repDelta = 0, .weight = 4, .requireFlags = F_CORP_PROOF,
     .forbidFlags = 0, .tone = CGC::TONE_GOOD},

    {.name = "Gear cache", .note = "You salvage a charged cell from a cache.",
     .resourceDeltas = {0, 0, 6, 0, 0, 0, 0, 0}, .setFlags = 0, .trackerIndex = -1,
     .trackerDelta = 0, .repIndex = -1, .repDelta = 0, .weight = 1, .requireFlags = 0,
     .forbidFlags = 0, .tone = CGC::TONE_GOOD},

    {.name = "False echo", .note = "Just a weather echo after all.",
     .resourceDeltas = {0, -1, -1, 1, 0, 0, 0, 0}, .setFlags = 0, .trackerIndex = -1,
     .trackerDelta = 0, .repIndex = -1, .repDelta = 0, .weight = 2, .requireFlags = 0,
     .forbidFlags = 0, .tone = CGC::TONE_NEUTRAL},
};

static const CGC::EndingDef ENDINGS[] = {
    {.name = "Corp Leak", .note = "Too much heat. The sponsor owns your story now.",
     .cond = {.resourceIndex = R_HEAT, .resourceMin = 70, .resourceMax = 100,
              .trackerIndex = -1, .trackerMin = 0, .repIndex = -1, .repMin = 0,
              .requireFlags = 0, .forbidFlags = 0, .requiresVictory = true}},

    {.name = "Signal Pact", .note = "You answered the band, and it answered back.",
     .cond = {.resourceIndex = R_SIG, .resourceMin = 6, .resourceMax = 99,
              .trackerIndex = -1, .trackerMin = 0, .repIndex = -1, .repMin = 0,
              .requireFlags = F_CORP_PROOF | F_ENDGAME, .forbidFlags = F_ETHICS_BREACH,
              .requiresVictory = true}},

    {.name = "Park Truth", .note = "The corporate dossier reaches the public, clean.",
     .cond = {.resourceIndex = R_TRUST, .resourceMin = 50, .resourceMax = 100,
              .trackerIndex = -1, .trackerMin = 0, .repIndex = -1, .repMin = 0,
              .requireFlags = F_CORP_PROOF, .forbidFlags = F_ETHICS_BREACH,
              .requiresVictory = true}},

    {.name = "Science Archive", .note = "Your field guide becomes the reference record.",
     .cond = {.resourceIndex = R_BIO, .resourceMin = 6, .resourceMax = 99,
              .trackerIndex = -1, .trackerMin = 0, .repIndex = -1, .repMin = 0,
              .requireFlags = 0, .forbidFlags = 0, .requiresVictory = true}},

    {.name = "Town Legend", .note = "The work is done, quietly, and the park endures.",
     .cond = {.resourceIndex = -1, .resourceMin = 0, .resourceMax = 0,
              .trackerIndex = -1, .trackerMin = 0, .repIndex = -1, .repMin = 0,
              .requireFlags = 0, .forbidFlags = 0, .requiresVictory = true}},

    {.name = "Lost Patrol", .note = "You ran yourself empty in the deep preserve.",
     .cond = {.resourceIndex = -1, .resourceMin = 0, .resourceMax = 0,
              .trackerIndex = -1, .trackerMin = 0, .repIndex = -1, .repMin = 0,
              .requireFlags = 0, .forbidFlags = 0, .requiresVictory = false}},
};

static const char* const TRACKER_LABELS[] = {
    "Cases", "Patrols", "Signals", "Decodes", "Guide", "Closed", "Gear", nullptr,
};

static const CGC::DeepGameDefinition DEF = {
    .title = "Cyber Ranger",
    .slug = "cyber-ranger",
    .subtitle = "Fictional cyber-wilderness",
    .saveMagic = "CYR2",
    .resources = RESOURCES,
    .resourceCount = sizeof(RESOURCES) / sizeof(RESOURCES[0]),
    .screens = SCREENS,
    .screenCount = sizeof(SCREENS) / sizeof(SCREENS[0]),
    .events = EVENTS,
    .eventCount = sizeof(EVENTS) / sizeof(EVENTS[0]),
    .endings = ENDINGS,
    .endingCount = sizeof(ENDINGS) / sizeof(ENDINGS[0]),
    .targetProgress = 150,
    .accentColor = 0x07BF,
    .warningColor = 0xFD20,
    .trackerLabels = TRACKER_LABELS,
    .trackerLabelCount = 8,
    .objective = "Find real evidence; resolve the case.",
    .primaryTracker = 5,
};

const CGC::DeepGameDefinition& gameDefinition() {
  return DEF;
}
