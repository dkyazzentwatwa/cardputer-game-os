#include "GameData.h"

namespace CGC = CardputerGameCore;

// Resource indices (keep in sync with RESOURCES order).
enum {
  R_MONEY,    // 0
  R_FEED,     // 1 — sustain; negative primaryDelta → starvation ends the run
  R_MEDS,     // 2
  R_REP,      // 3
  R_BOND,     // 4
  R_FATIGUE,  // 5 — inverted: high is bad
  R_WEEKS,    // 6 — counter (weeks survived)
  R_HEALTH,   // 7
};

// Story flags for gated progression.
enum : uint32_t {
  F_CLINIC_UP    = 1u << 0,  // clinic or stable built
  F_YARD_UP      = 1u << 1,  // training yard built
  F_NURSERY_UP   = 1u << 2,  // nursery built → enables breeding
  F_PEN_UP       = 1u << 3,  // tournament pen built → enables tourneys
  F_EXPEDITION   = 1u << 4,  // first successful expedition complete
  F_RIVAL_MET    = 1u << 5,  // met the rival rancher
  F_VARIANT_BORN = 1u << 6,  // a rare variant has been hatched
  F_CHAMPION     = 1u << 7,  // won the frontier championship
};

// ── Resources ────────────────────────────────────────────────────────────────
static const CGC::ResourceDef RESOURCES[] = {
    // label    start  min  max  prim safe  warn  bad  inv
    {"Money",    80,    0,  999,  0,   0,   25,   8,  false},
    {"Feed",     50,    0,  120, -2,   6,   20,   8,  false},  // starvation = loss
    {"Meds",      6,    0,   30,  0,   0,    3,   1,  false},
    {"Rep",       5,    0,   99,  0,   0,   -1,  -1,  false},
    {"Bond",     20,    0,  100,  0,   1,   10,   4,  false},
    {"Fatigue",   0,    0,  100,  1,  -4,   55,  78,  true},   // high fatigue = amber/red
    {"Weeks",     0,    0,   99,  0,   0,   -1,  -1,  false},  // counter
    {"Health",   80,    0,  100,  0,   2,   30,  12,  false},
};

// ── Action rows ──────────────────────────────────────────────────────────────

// RANCH: weekly review and rest
static const CGC::NamedValue ROWS_RANCH[] = {
    {"Morning Rounds",   "Walk the pens; check on every creature."},
    {"Repair Fences",    "Patch broken rails before next storm."},
    {"Stock Inventory",  "Count feed and meds; plan the week."},
    {"Rest the Herd",    "Let everyone sleep in; fatigue drops."},
    {"Weather Watch",    "Study the sky; adjust work schedule."},
    {"Ranch Journal",    "Write up the week and note progress."},
    {"Groom Spriglet",   "Brush and bond; low-effort care."},
    {"Night Rounds",     "Late check; catch illness early."},
};

// CARE: feeding, healing, bonding (costs Feed + Meds)
static const CGC::NamedValue ROWS_CARE[] = {
    {"Favorite Meal",    "Cook a species-preferred dish; big bond."},
    {"Herb Poultice",    "Apply a meds poultice to sore muscles."},
    {"Groom & Brush",    "Full grooming session builds deep trust."},
    {"Hand-feed Snacks", "Personal feeding raises bond daily."},
    {"Mineral Bath",     "Soak tired creatures; cuts fatigue hard."},
    {"Vet Check",        "Use meds for a full health inspection."},
    {"Play Session",     "Supervised play: bond up, fatigue down."},
    {"Quarantine Check", "Isolate a sick creature before spread."},
};

// TRAIN: raises stats but adds Fatigue (costs Feed)
static const CGC::NamedValue ROWS_TRAIN[] = {
    {"Power Drills",     "Weighted pulls build raw power."},
    {"Grit Runs",        "Long-distance runs build endurance."},
    {"Speed Sprints",    "Short bursts sharpen reaction time."},
    {"Focus Puzzles",    "Concentration games improve tactics."},
    {"Bond Sparring",    "Paired practice raises bond and stats."},
    {"Agility Course",   "Obstacle run: speed and grit gains."},
    {"Light Recovery",   "Gentle stretches; small gains, less cost."},
    {"Combo Drill",      "Mixed session; broad gains, high cost."},
};

// EXPEDITION: risks supplies, gains loot (gated behind F_EXPEDITION clearance)
static const CGC::NamedValue ROWS_EXPED[] = {
    {"Pine Hollow",      "Mild forest zone; common finds."},
    {"Warm Flats",       "Open grassland; good for cinderpups."},
    {"Mire Trail",       "Wet zone; mirelings thrive here."},
    {"Glass Ridge",      "Rocky heights; quillox territory."},
    {"Fuse Crater",      "Volcanic zone; rare fuse-type finds."},
    {"Snow Peaks",       "Cold summit; snowcalf eggs hidden here."},
    {"Moon Hollow",      "Night zone; moonmoth variants lurk."},
    {"Chrome Wastes",    "Frontier danger; legendary loot waits."},
};

// FACILITIES: build/upgrade (costs Money; sets progression flags)
static const CGC::NamedValue ROWS_BUILD[] = {
    {"Stable Upgrade",   "More stalls; house a bigger herd."},
    {"Clinic Build",     "Basic vet station; meds stretch further."},
    {"Training Yard",    "Proper yard; training gains improve."},
    {"Garden Plot",      "Grow feed locally; lower feed cost."},
    {"Workshop",         "Craft tools; cuts expedition supply use."},
    {"Nursery",          "Egg incubator; enables breeding."},
    {"Lookout Tower",    "Spot weather early; reduce storm hits."},
    {"Weather Vane",     "Fine-tune weather reads; fewer surprises."},
};

// BREED: hatch eggs and discover variants (gated: F_NURSERY_UP)
static const CGC::NamedValue ROWS_BREED[] = {
    {"Incubate Egg",     "Warm a mystery egg in the nursery."},
    {"Pair Cinderpups",  "Bond two pups; rare fusefox chance."},
    {"Pair Mirelings",   "Swamp pair; mudcrawler variant chance."},
    {"Pair Snowcalves",  "Cold-season pair; frost variant chance."},
    {"Moonmoth Ritual",  "Moonlit pairing; shimmer variant chance."},
    {"Variant Study",    "Observe variant traits; rep bonus."},
    {"Egg Trade",        "Trade a common egg for a rare one."},
    {"Release Surplus",  "Release extras; bond and rep up."},
};

// TOURNAMENT: big progress + reward (gated: F_PEN_UP)
static const CGC::NamedValue ROWS_TOURNEY[] = {
    {"Novice Cup",       "Entry-level bracket; easy prize."},
    {"Harvest Festival", "Seasonal tourney; reputation prize."},
    {"Storm League",     "Weather-tested brackets; meds prize."},
    {"Rival Challenge",  "Rival rancher's formal rematch."},
    {"Frontier Open",    "Hard bracket; frontier rep at stake."},
    {"Champion's Cup",   "Top-tier; only the best ranches win."},
};

// ── Screens ──────────────────────────────────────────────────────────────────
static const CGC::DeepScreenDef SCREENS[] = {
    // RANCH: weekly overview and rest. Sets no gate flag; always open.
    {.title = "RANCH", .verb = "Review", .rows = ROWS_RANCH, .rowCount = 8,
     .resourceDeltas = {0, -1, 0, 0, 1, -4, 1, 2}, .progressDelta = 1, .trackerIndex = 0,
     .flagMask = 0, .costResource = -1, .costAmount = 0, .requireFlags = 0,
     .trackerDelta = 1, .objective = "Plan the week; rest reduces fatigue"},

    // CARE: heals and bonds. Costs Feed + small Meds.
    {.title = "CARE", .verb = "Tend", .rows = ROWS_CARE, .rowCount = 8,
     .resourceDeltas = {0, -3, -1, 0, 4, -5, 0, 3}, .progressDelta = 1, .trackerIndex = 1,
     .flagMask = F_CLINIC_UP, .costResource = R_FEED, .costAmount = 4, .requireFlags = 0,
     .trackerDelta = 1, .objective = "Care for herd (-4 Feed, -1 Meds)"},

    // TRAIN: raises stats but spikes Fatigue. Costs Feed.
    {.title = "TRAIN", .verb = "Train", .rows = ROWS_TRAIN, .rowCount = 8,
     .resourceDeltas = {0, -4, 0, 1, 2, 8, 0, 0}, .progressDelta = 3, .trackerIndex = 2,
     .flagMask = F_YARD_UP, .costResource = R_FEED, .costAmount = 5, .requireFlags = 0,
     .trackerDelta = 1, .objective = "Train hard (-5 Feed, +8 Fatigue!)"},

    // EXPEDITION: costs Money + Feed; sets F_EXPEDITION on first use.
    {.title = "EXPED", .verb = "Explore", .rows = ROWS_EXPED, .rowCount = 8,
     .resourceDeltas = {-6, -5, 0, 2, 1, 5, 0, -2}, .progressDelta = 4, .trackerIndex = 3,
     .flagMask = F_EXPEDITION, .costResource = R_MONEY, .costAmount = 10, .requireFlags = 0,
     .trackerDelta = 1, .objective = "Explore zones (-10 Money, -5 Feed)"},

    // FACILITIES: build and upgrade. Costs Money; sets clinic/yard/nursery/pen flags.
    {.title = "BUILD", .verb = "Build", .rows = ROWS_BUILD, .rowCount = 8,
     .resourceDeltas = {-18, 0, 0, 1, 0, 0, 0, 1}, .progressDelta = 3, .trackerIndex = 4,
     .flagMask = F_NURSERY_UP | F_PEN_UP, .costResource = R_MONEY, .costAmount = 18,
     .requireFlags = 0, .trackerDelta = 1, .objective = "Build facilities (-18 Money)"},

    // BREED: egg hatching, variants. Gated behind F_NURSERY_UP.
    {.title = "BREED", .verb = "Hatch", .rows = ROWS_BREED, .rowCount = 8,
     .resourceDeltas = {-5, -3, 0, 2, 3, 0, 0, 0}, .progressDelta = 3, .trackerIndex = 5,
     .flagMask = F_VARIANT_BORN, .costResource = R_MONEY, .costAmount = 5,
     .requireFlags = F_NURSERY_UP, .trackerDelta = 1,
     .objective = "Breed variants (need Nursery)"},

    // TOURNAMENT: big progress reward. Gated behind F_PEN_UP. Costs Meds.
    {.title = "TOURNEY", .verb = "Enter", .rows = ROWS_TOURNEY, .rowCount = 6,
     .resourceDeltas = {15, -2, -2, 5, 0, 6, 0, -3}, .progressDelta = 8, .trackerIndex = 6,
     .flagMask = F_CHAMPION, .costResource = R_MEDS, .costAmount = 2,
     .requireFlags = F_PEN_UP, .trackerDelta = 1,
     .objective = "Enter a cup (need Pen, -2 Meds)"},
};

// ── Events ───────────────────────────────────────────────────────────────────
static const CGC::EventDef EVENTS[] = {
    // Common neutral fillers ─────────────────────────────────────────────────
    {.name = "Quiet week", .note = "Ranch hums along; herd content.",
     .resourceDeltas = {0, 0, 0, 0, 1, -2, 0, 1}, .setFlags = 0, .trackerIndex = -1,
     .trackerDelta = 0, .repIndex = -1, .repDelta = 0, .weight = 5, .requireFlags = 0,
     .forbidFlags = 0, .tone = CGC::TONE_NEUTRAL},

    {.name = "Good harvest", .note = "Garden yields extra; feed stocks up.",
     .resourceDeltas = {0, 8, 0, 0, 0, 0, 0, 0}, .setFlags = 0, .trackerIndex = -1,
     .trackerDelta = 0, .repIndex = -1, .repDelta = 0, .weight = 3, .requireFlags = 0,
     .forbidFlags = 0, .tone = CGC::TONE_GOOD},

    {.name = "Mild weather", .note = "Clear skies keep fatigue low.",
     .resourceDeltas = {0, 0, 0, 0, 0, -3, 0, 1}, .setFlags = 0, .trackerIndex = -1,
     .trackerDelta = 0, .repIndex = -1, .repDelta = 0, .weight = 3, .requireFlags = 0,
     .forbidFlags = 0, .tone = CGC::TONE_GOOD},

    {.name = "Stiff joints", .note = "Herd sore after hard week; rest needed.",
     .resourceDeltas = {0, 0, -1, 0, 0, 5, 0, -3}, .setFlags = 0, .trackerIndex = -1,
     .trackerDelta = 0, .repIndex = -1, .repDelta = 0, .weight = 3, .requireFlags = 0,
     .forbidFlags = 0, .tone = CGC::TONE_BAD},

    {.name = "Feed spoiled", .note = "Damp feed barrel — toss and reorder.",
     .resourceDeltas = {-8, -10, 0, 0, 0, 0, 0, 0}, .setFlags = 0, .trackerIndex = -1,
     .trackerDelta = 0, .repIndex = -1, .repDelta = 0, .weight = 2, .requireFlags = 0,
     .forbidFlags = 0, .tone = CGC::TONE_BAD},

    // Creature illness ────────────────────────────────────────────────────────
    {.name = "Creature ill", .note = "Spriglet coughs; treat fast or it spreads.",
     .resourceDeltas = {0, 0, -2, 0, -2, 2, 0, -6}, .setFlags = 0, .trackerIndex = -1,
     .trackerDelta = 0, .repIndex = -1, .repDelta = 0, .weight = 2, .requireFlags = 0,
     .forbidFlags = 0, .tone = CGC::TONE_BAD},

    {.name = "Clinic saves herd", .note = "Clinic catches outbreak before it spreads.",
     .resourceDeltas = {0, 0, -1, 0, 1, 0, 0, 2}, .setFlags = 0, .trackerIndex = -1,
     .trackerDelta = 0, .repIndex = 0, .repDelta = 1, .weight = 2,
     .requireFlags = F_CLINIC_UP, .forbidFlags = 0, .tone = CGC::TONE_GOOD},

    // Weather events ──────────────────────────────────────────────────────────
    {.name = "Frontier storm", .note = "Heavy storm; facilities take minor damage.",
     .resourceDeltas = {-5, 0, 0, 0, 0, 3, 0, -4}, .setFlags = 0, .trackerIndex = -1,
     .trackerDelta = 0, .repIndex = -1, .repDelta = 0, .weight = 2, .requireFlags = 0,
     .forbidFlags = 0, .tone = CGC::TONE_BAD},

    {.name = "Vane warns in time", .note = "Lookout tip lets you shelter early.",
     .resourceDeltas = {0, 0, 0, 0, 0, -1, 0, 1}, .setFlags = 0, .trackerIndex = -1,
     .trackerDelta = 0, .repIndex = -1, .repDelta = 0, .weight = 2,
     .requireFlags = F_CLINIC_UP, .forbidFlags = 0, .tone = CGC::TONE_GOOD},

    // Expedition finds ────────────────────────────────────────────────────────
    {.name = "Rare egg found", .note = "Expedition returns with a mystery egg.",
     .resourceDeltas = {0, 0, 0, 2, 2, 0, 0, 0}, .setFlags = 0, .trackerIndex = 5,
     .trackerDelta = 1, .repIndex = 1, .repDelta = 2, .weight = 2,
     .requireFlags = F_EXPEDITION, .forbidFlags = 0, .tone = CGC::TONE_GOOD},

    {.name = "Supply cache", .note = "Hidden cache fills meds and money.",
     .resourceDeltas = {12, 0, 3, 0, 0, 0, 0, 0}, .setFlags = 0, .trackerIndex = -1,
     .trackerDelta = 0, .repIndex = -1, .repDelta = 0, .weight = 1,
     .requireFlags = F_EXPEDITION, .forbidFlags = 0, .tone = CGC::TONE_GOOD},

    {.name = "Zone injury", .note = "Creature hurt in rough terrain; health dips.",
     .resourceDeltas = {0, 0, -2, 0, 0, 4, 0, -8}, .setFlags = 0, .trackerIndex = -1,
     .trackerDelta = 0, .repIndex = -1, .repDelta = 0, .weight = 2,
     .requireFlags = F_EXPEDITION, .forbidFlags = 0, .tone = CGC::TONE_BAD},

    // Rival rancher ───────────────────────────────────────────────────────────
    {.name = "Rival Daska stops by", .note = "Rival sizes up your herd; rep on the line.",
     .resourceDeltas = {0, 0, 0, 1, 0, 0, 0, 0}, .setFlags = F_RIVAL_MET, .trackerIndex = -1,
     .trackerDelta = 0, .repIndex = 2, .repDelta = -1, .weight = 2, .requireFlags = 0,
     .forbidFlags = F_RIVAL_MET, .tone = CGC::TONE_NEUTRAL},

    {.name = "Rival trade offer", .note = "Daska wants a rare egg for med supplies.",
     .resourceDeltas = {0, 0, 4, 1, 0, 0, 0, 0}, .setFlags = 0, .trackerIndex = -1,
     .trackerDelta = 0, .repIndex = 2, .repDelta = 1, .weight = 2,
     .requireFlags = F_RIVAL_MET, .forbidFlags = 0, .tone = CGC::TONE_GOOD},

    // Breakthrough / variant ──────────────────────────────────────────────────
    {.name = "Variant hatches!", .note = "A shimmering variant emerges from the nest.",
     .resourceDeltas = {0, 0, 0, 4, 4, 0, 0, 0}, .setFlags = F_VARIANT_BORN,
     .trackerIndex = 5, .trackerDelta = 1, .repIndex = 1, .repDelta = 3, .weight = 1,
     .requireFlags = F_NURSERY_UP, .forbidFlags = F_VARIANT_BORN, .tone = CGC::TONE_GOOD},

    {.name = "Training break!", .note = "Cinderpup finally masters the agility course.",
     .resourceDeltas = {0, 0, 0, 2, 2, -2, 0, 2}, .setFlags = 0, .trackerIndex = 2,
     .trackerDelta = 1, .repIndex = 0, .repDelta = 2, .weight = 2,
     .requireFlags = F_YARD_UP, .forbidFlags = 0, .tone = CGC::TONE_GOOD},

    // Fatigue injury ──────────────────────────────────────────────────────────
    {.name = "Overtraining injury", .note = "Pushed too hard; creature pulls a tendon.",
     .resourceDeltas = {0, 0, -3, 0, -2, 5, 0, -10}, .setFlags = 0, .trackerIndex = -1,
     .trackerDelta = 0, .repIndex = -1, .repDelta = 0, .weight = 2, .requireFlags = 0,
     .forbidFlags = 0, .tone = CGC::TONE_BAD},

    // Tournament drama ────────────────────────────────────────────────────────
    {.name = "Traveling judge", .note = "Judge offers a pop-up tourney slot today.",
     .resourceDeltas = {8, 0, 0, 3, 0, 0, 0, 0}, .setFlags = 0, .trackerIndex = 6,
     .trackerDelta = 1, .repIndex = 0, .repDelta = 2, .weight = 1,
     .requireFlags = F_PEN_UP, .forbidFlags = 0, .tone = CGC::TONE_GOOD},
};

// ── Endings ───────────────────────────────────────────────────────────────────
// Most-specific first; first match wins.
static const CGC::EndingDef ENDINGS[] = {
    // League winner: won Champion's Cup + high rep
    {.name = "League Winner",
     .note = "Trophies line every wall. The frontier knows your name.",
     .cond = {.resourceIndex = R_REP, .resourceMin = 25, .resourceMax = 99,
              .trackerIndex = 6, .trackerMin = 3,
              .repIndex = -1, .repMin = 0,
              .requireFlags = F_CHAMPION, .forbidFlags = 0, .requiresVictory = true}},

    // Variant archive: nursery + 3+ variants hatched
    {.name = "Variant Archive",
     .note = "Your nursery ledger rivals any research lab.",
     .cond = {.resourceIndex = -1, .resourceMin = 0, .resourceMax = 0,
              .trackerIndex = 5, .trackerMin = 3,
              .repIndex = -1, .repMin = 0,
              .requireFlags = F_NURSERY_UP | F_VARIANT_BORN, .forbidFlags = 0,
              .requiresVictory = true}},

    // Rival friend: met rival, did tourney, high bond
    {.name = "Rival Friend",
     .note = "Daska calls it even; you run joint expeditions.",
     .cond = {.resourceIndex = R_BOND, .resourceMin = 55, .resourceMax = 100,
              .trackerIndex = 6, .trackerMin = 2,
              .repIndex = -1, .repMin = 0,
              .requireFlags = F_RIVAL_MET, .forbidFlags = 0, .requiresVictory = true}},

    // Research star: many expeditions + high rep
    {.name = "Research Star",
     .note = "Every zone catalogued; universities ask for data.",
     .cond = {.resourceIndex = R_REP, .resourceMin = 18, .resourceMax = 99,
              .trackerIndex = 3, .trackerMin = 5,
              .repIndex = -1, .repMin = 0,
              .requireFlags = F_EXPEDITION, .forbidFlags = 0, .requiresVictory = true}},

    // Cozy ranch: catch-all victory
    {.name = "Cozy Ranch",
     .note = "Quiet, happy, and well-fed. A good life in the frontier.",
     .cond = {.resourceIndex = -1, .resourceMin = 0, .resourceMax = 0,
              .trackerIndex = -1, .trackerMin = 0,
              .repIndex = -1, .repMin = 0,
              .requireFlags = 0, .forbidFlags = 0, .requiresVictory = true}},

    // Empty barn: loss
    {.name = "Empty Barn",
     .note = "The herd collapsed. The barn stands empty and quiet.",
     .cond = {.resourceIndex = -1, .resourceMin = 0, .resourceMax = 0,
              .trackerIndex = -1, .trackerMin = 0,
              .repIndex = -1, .repMin = 0,
              .requireFlags = 0, .forbidFlags = 0, .requiresVictory = false}},
};

// ── Tracker labels ────────────────────────────────────────────────────────────
static const char* const TRACKER_LABELS[] = {
    "Weeks",       // 0 — primaryTracker
    "Care",        // 1
    "Training",    // 2
    "Expeditions", // 3
    "Facilities",  // 4
    "Variants",    // 5
    "Tourneys",    // 6
    nullptr,       // 7
};

// ── Game definition ───────────────────────────────────────────────────────────
static const CGC::DeepGameDefinition DEF = {
    .title       = "Monster Ranch",
    .slug        = "monster-ranch-trail",
    .subtitle    = "Weekly creature ranch",
    .saveMagic   = "MRAN2",
    .resources   = RESOURCES,
    .resourceCount = sizeof(RESOURCES) / sizeof(RESOURCES[0]),
    .screens     = SCREENS,
    .screenCount = sizeof(SCREENS) / sizeof(SCREENS[0]),
    .events      = EVENTS,
    .eventCount  = sizeof(EVENTS) / sizeof(EVENTS[0]),
    .endings     = ENDINGS,
    .endingCount = sizeof(ENDINGS) / sizeof(ENDINGS[0]),
    .targetProgress = 170,
    .accentColor    = 0x07E0,
    .warningColor   = 0xFD20,
    .trackerLabels     = TRACKER_LABELS,
    .trackerLabelCount = 8,
    .objective   = "Keep herd fed; win the frontier league.",
    .primaryTracker = 0,
};

const CGC::DeepGameDefinition& gameDefinition() {
  return DEF;
}
