#include "GameData.h"

namespace CGC = CardputerGameCore;

// Resource indices — keep in sync with RESOURCES order.
// R_STRESS and R_QRISK are inverted (high is bad).
enum {
  R_CRED,   // Credits
  R_FUEL,   // Fuel (survival — primaryDelta < 0)
  R_O2,     // Oxygen reserves
  R_FEED,   // Creature feed
  R_MED,    // Med gel
  R_HULL,   // Hull integrity
  R_STRESS, // Creature stress (inverted, high = bad)
  R_QRISK,  // Quarantine risk (inverted, high = bad)
};

// Story flags — set cleanly by screens, advanced by events.
enum : uint32_t {
  F_CONTRACT    = 1u << 0,  // a contract is accepted (set by JOBS)
  F_HAB_READY   = 1u << 1,  // habitats assigned and checked (set by HAB)
  F_PAPERS      = 1u << 2,  // inspection papers bought (set by MARKET)
  F_UNKNOWN_EGG = 1u << 3,  // unknown egg discovered mid-route (event)
  F_SANCTUARY   = 1u << 4,  // chose sanctuary delivery path
  F_BLACK_MKT   = 1u << 5,  // sold to black market buyer
  F_CORP_DEAL   = 1u << 6,  // signed corporate transporter contract
  F_DISCOVERY   = 1u << 7,  // new species catalogued and protected
};

// ── RESOURCES ────────────────────────────────────────────────────────────────
//  label   start min  max  prim safe  warn  bad  inverted
static const CGC::ResourceDef RESOURCES[] = {
  {"Cred",    60,  0, 999,  0,   0,   18,    6,  false},
  {"Fuel",    70,  0, 120, -1,   5,   25,   10,  false},  // loss trigger
  {"O2",      65,  0, 120,  0,   4,   22,    8,  false},
  {"Feed",    55,  0, 120,  0,   3,   18,    6,  false},
  {"Med",     30,  0,  99,  0,   1,   10,    3,  false},
  {"Hull",    85,  0, 100,  0,   2,   30,   12,  false},
  {"Stress",   8,  0, 100,  0,  -2,   55,   75,  true},   // inverted: high=bad
  {"QRisk",    4,  0, 100,  0,  -1,   45,   65,  true},   // inverted: high=bad
};

// ── SCREEN ROW TABLES ─────────────────────────────────────────────────────────

// BRIDGE — review route context and plan the next haul.
static const CGC::NamedValue ROWS_BRIDGE[] = {
  {"Manifest review",   "Audit cargo manifest and hab logs."},
  {"Fuel check",        "Verify tanks, top-off plan, route calc."},
  {"O2 inventory",      "Check O2 reserves against route length."},
  {"Hab status",        "Scan all modules for temp/feed alerts."},
  {"Hull integrity",    "Run hull stress analysis and patch list."},
  {"Plot short route",  "Mark a nearby-planet corridor on chart."},
  {"Plot long route",   "Mark a deep-space corridor on chart."},
  {"Threat brief",      "Review inspection zones and hazards."},
};

// JOBS — accept transport contracts, pick up creatures.
static const CGC::NamedValue ROWS_JOBS[] = {
  {"Grazer haul",       "Transport hardy grazers to Duneport."},
  {"Frostfin charter",  "Move frostfins to the ice-shelf colony."},
  {"Wetling rescue",    "Emergency pickup: wetlings in dry hab."},
  {"Research loan",     "Carry a quillox pair for lab study."},
  {"Sanctuary run",     "Deliver glowcalves to the free refuge."},
  {"Mire pup transit",  "Haul mire pups through the bog lanes."},
  {"Rare starling job", "Transport rare starlings; papers needed."},
  {"Breeding pair",     "Move bonded climbers to colony station."},
};

// HAB — assign creatures to modules; reduce stress and quarantine risk.
static const CGC::NamedValue ROWS_HAB[] = {
  {"Cold tank",         "Place frostfins in cryo-climate module."},
  {"Heat bay",          "Settle heat-world grazers in warm bay."},
  {"Wet pod",           "Submerge wetlings and mire pups in pod."},
  {"Low-grav stall",    "Float starlings in the low-g chamber."},
  {"Quarantine lock",   "Isolate high-QRisk creature in locker."},
  {"Nursery",           "Move juvenile or bonded pair to nursery."},
  {"Shielded den",      "Transfer quillox to armored den module."},
  {"Enrichment round",  "Run enrichment to calm all creatures."},
};

// ROUTE — fly a route leg; costs Fuel and stresses the ship.
static const CGC::NamedValue ROWS_ROUTE[] = {
  {"Short hop",         "Quick corridor; low risk, low Fuel cost."},
  {"Deep-space leg",    "Long crossing; high Fuel, rich reward."},
  {"Quarantine lane",   "Cleared path; needs papers or detour."},
  {"Storm corridor",    "Risk hull damage for a faster arrival."},
  {"Gate transit",      "Use jump gate; cheaper Fuel, flat fee."},
  {"Moon relay",        "Slow moon-hop; safer but burns O2."},
  {"Smuggler bypass",   "Skips inspections; raises QRisk."},
  {"Sanctuary lane",    "Scenic route; calms creatures in transit."},
};

// CARE — feed and treat creatures; costs Feed and Med.
static const CGC::NamedValue ROWS_CARE[] = {
  {"Full rations",      "Issue full feed for all cargo creatures."},
  {"Med gel treatment", "Apply gel to stressed or sick creatures."},
  {"Quarantine flush",  "Disinfect quarantine locker; cuts QRisk."},
  {"Stress relief",     "Run calming protocol; lower stress fast."},
  {"Injury triage",     "Treat wounds; costs Med but saves hull."},
  {"Nutrition boost",   "Premium feed; cost more, better results."},
  {"Hydration cycle",   "Water-world species get deep soak."},
  {"Health check",      "Full scan; can reveal hidden QRisk."},
};

// MARKET / INSPECT — buy supplies, pay customs, trade cargo.
static const CGC::NamedValue ROWS_MARKET[] = {
  {"Restock feed",      "Buy bulk creature feed at port rates."},
  {"Buy med gel",       "Purchase medical gel packs."},
  {"Top off O2",        "Refill oxygen reserve tanks."},
  {"Buy transit papers","Get inspection papers before next leg."},
  {"Hab module parts",  "Buy upgrade parts; lowers stress long-term."},
  {"Pay customs",       "Settle customs fees; clears QRisk alert."},
  {"Sell cargo goods",  "Offload non-creature cargo for credits."},
  {"Black market offer","High-risk sale; illegal but lucrative."},
};

// LOG / DISCOVERY — record finds, confirm sanctuary choices, name species.
static const CGC::NamedValue ROWS_LOG[] = {
  {"Log egg find",      "Record unknown egg found in hab module."},
  {"Name new species",  "Catalogue species and file with registry."},
  {"Sanctuary pledge",  "Commit delivery to free-range refuge."},
  {"Bond record",       "Note crew-creature bond in the log."},
  {"Rare buyer note",   "Log black-market contact for later."},
  {"Permit registry",   "File transport permits for rare cargo."},
  {"Extinction tip",    "Document rumoured species loss pattern."},
  {"Research dossier",  "Compile scan data for science partner."},
};

// ── SCREENS ───────────────────────────────────────────────────────────────────
// resourceDeltas order: Cred, Fuel, O2, Feed, Med, Hull, Stress, QRisk
static const CGC::DeepScreenDef SCREENS[] = {
  // BRIDGE — free planning; sets F_CONTRACT flag (clears old debt on review).
  {.title = "BRIDGE", .verb = "Plan", .rows = ROWS_BRIDGE, .rowCount = 8,
   .resourceDeltas = {0, 1, 1, 0, 0, 1, -1, -1}, .progressDelta = 1, .trackerIndex = 0,
   .flagMask = 0, .costResource = -1, .costAmount = 0, .requireFlags = 0,
   .trackerDelta = 1, .objective = "Plan the route and check cargo"},

  // JOBS — accept a contract; costs nothing but requires no active contract.
  {.title = "JOBS", .verb = "Accept", .rows = ROWS_JOBS, .rowCount = 8,
   .resourceDeltas = {8, 0, 0, -2, 0, 0, 2, 1}, .progressDelta = 2, .trackerIndex = 1,
   .flagMask = F_CONTRACT, .costResource = -1, .costAmount = 0, .requireFlags = 0,
   .trackerDelta = 1, .objective = "Accept a creature transport contract"},

  // HAB — assign creatures; costs Feed and reduces Stress/QRisk. Needs contract.
  {.title = "HAB", .verb = "Assign", .rows = ROWS_HAB, .rowCount = 8,
   .resourceDeltas = {0, 0, 0, -3, 0, 0, -4, -3}, .progressDelta = 2, .trackerIndex = 2,
   .flagMask = F_HAB_READY, .costResource = R_FEED, .costAmount = 5, .requireFlags = F_CONTRACT,
   .trackerDelta = 1, .objective = "Assign creatures to hab modules (-5 Feed)"},

  // ROUTE — fly a leg; costs Fuel. Needs hab assignment.
  {.title = "ROUTE", .verb = "Fly", .rows = ROWS_ROUTE, .rowCount = 8,
   .resourceDeltas = {12, -8, -3, -4, 0, -2, 3, 2}, .progressDelta = 5, .trackerIndex = 3,
   .flagMask = 0, .costResource = R_FUEL, .costAmount = 10, .requireFlags = F_HAB_READY,
   .trackerDelta = 1, .objective = "Fly a route leg (-10 Fuel)"},

  // CARE — feed and treat; costs Med. Open any time.
  {.title = "CARE", .verb = "Treat", .rows = ROWS_CARE, .rowCount = 8,
   .resourceDeltas = {0, 0, 0, -4, -3, 0, -5, -4}, .progressDelta = 2, .trackerIndex = 4,
   .flagMask = 0, .costResource = R_MED, .costAmount = 4, .requireFlags = 0,
   .trackerDelta = 1, .objective = "Feed and treat creatures (-4 Med)"},

  // MARKET — buy supplies, pay customs, trade goods. Costs Credits.
  {.title = "MARKET", .verb = "Buy", .rows = ROWS_MARKET, .rowCount = 8,
   .resourceDeltas = {-10, 3, 3, 4, 3, 2, -1, -2}, .progressDelta = 1, .trackerIndex = 5,
   .flagMask = F_PAPERS, .costResource = R_CRED, .costAmount = 12, .requireFlags = 0,
   .trackerDelta = 1, .objective = "Restock supplies at port (-12 Cred)"},

  // LOG — record discoveries, pledge sanctuary. Needs F_UNKNOWN_EGG to unlock discovery arc.
  {.title = "LOG", .verb = "Record", .rows = ROWS_LOG, .rowCount = 8,
   .resourceDeltas = {2, 0, 0, 0, 0, 0, -1, -1}, .progressDelta = 3, .trackerIndex = 6,
   .flagMask = F_DISCOVERY, .costResource = -1, .costAmount = 0, .requireFlags = F_UNKNOWN_EGG,
   .trackerDelta = 1, .objective = "Catalogue species (needs egg find)"},
};

// ── EVENTS ────────────────────────────────────────────────────────────────────
// resourceDeltas order: Cred, Fuel, O2, Feed, Med, Hull, Stress, QRisk
static const CGC::EventDef EVENTS[] = {
  // --- Common neutral/filler ---
  {.name = "Calm transit", .note = "Creatures settle; smooth crossing.",
   .resourceDeltas = {0, 0, 0, 0, 0, 0, -2, 0}, .setFlags = 0, .trackerIndex = -1,
   .trackerDelta = 0, .repIndex = -1, .repDelta = 0, .weight = 5,
   .requireFlags = 0, .forbidFlags = 0, .tone = CGC::TONE_NEUTRAL},

  {.name = "Feed drop", .note = "A feeder jams; creatures go hungry.",
   .resourceDeltas = {0, 0, 0, -3, 0, 0, 4, 0}, .setFlags = 0, .trackerIndex = -1,
   .trackerDelta = 0, .repIndex = -1, .repDelta = 0, .weight = 3,
   .requireFlags = 0, .forbidFlags = 0, .tone = CGC::TONE_BAD},

  {.name = "Trade wind", .note = "Favorable drift cuts fuel by a day.",
   .resourceDeltas = {0, 4, 0, 0, 0, 0, 0, 0}, .setFlags = 0, .trackerIndex = -1,
   .trackerDelta = 0, .repIndex = -1, .repDelta = 0, .weight = 3,
   .requireFlags = 0, .forbidFlags = 0, .tone = CGC::TONE_GOOD},

  {.name = "Micro-debris hit", .note = "Pebble-strike chips the hull plating.",
   .resourceDeltas = {0, 0, 0, 0, 0, -4, 2, 0}, .setFlags = 0, .trackerIndex = -1,
   .trackerDelta = 0, .repIndex = -1, .repDelta = 0, .weight = 2,
   .requireFlags = 0, .forbidFlags = 0, .tone = CGC::TONE_BAD},

  // --- Creature care events ---
  {.name = "Creature panic", .note = "Three creatures thrash hab walls.",
   .resourceDeltas = {0, 0, 0, -2, -2, -3, 8, 2}, .setFlags = 0, .trackerIndex = -1,
   .trackerDelta = 0, .repIndex = -1, .repDelta = 0, .weight = 2,
   .requireFlags = F_CONTRACT, .forbidFlags = F_HAB_READY, .tone = CGC::TONE_BAD},

  {.name = "Escape attempt", .note = "A climber slips out of its stall.",
   .resourceDeltas = {0, 0, -2, 0, -2, -2, 6, 3}, .setFlags = 0, .trackerIndex = -1,
   .trackerDelta = 0, .repIndex = 0, .repDelta = -1, .weight = 2,
   .requireFlags = F_CONTRACT, .forbidFlags = 0, .tone = CGC::TONE_BAD},

  {.name = "Sickness outbreak", .note = "Two mire pups show fever signs.",
   .resourceDeltas = {0, 0, 0, 0, -4, 0, 5, 6}, .setFlags = 0, .trackerIndex = -1,
   .trackerDelta = 0, .repIndex = -1, .repDelta = 0, .weight = 2,
   .requireFlags = F_CONTRACT, .forbidFlags = 0, .tone = CGC::TONE_BAD},

  {.name = "Creature bond", .note = "A glowcalf calms the whole cargo hold.",
   .resourceDeltas = {0, 0, 0, 0, 0, 0, -5, -2}, .setFlags = 0, .trackerIndex = 2,
   .trackerDelta = 1, .repIndex = 1, .repDelta = 1, .weight = 2,
   .requireFlags = F_HAB_READY, .forbidFlags = 0, .tone = CGC::TONE_GOOD},

  // --- Inspection and customs events ---
  {.name = "Port inspection", .note = "Customs boards; papers clear you fast.",
   .resourceDeltas = {-4, 0, 0, 0, 0, 0, 0, -4}, .setFlags = 0, .trackerIndex = 5,
   .trackerDelta = 1, .repIndex = 2, .repDelta = 1, .weight = 3,
   .requireFlags = F_PAPERS, .forbidFlags = 0, .tone = CGC::TONE_NEUTRAL},

  {.name = "Unregistered cargo", .note = "No papers; you pay a steep fine.",
   .resourceDeltas = {-12, 0, 0, 0, 0, 0, 3, 5}, .setFlags = 0, .trackerIndex = -1,
   .trackerDelta = 0, .repIndex = 2, .repDelta = -2, .weight = 2,
   .requireFlags = 0, .forbidFlags = F_PAPERS, .tone = CGC::TONE_BAD},

  // --- Discovery / species events ---
  {.name = "Unknown egg", .note = "An unregistered egg found in a feeder.",
   .resourceDeltas = {0, 0, 0, -1, 0, 0, 0, 3}, .setFlags = F_UNKNOWN_EGG, .trackerIndex = 6,
   .trackerDelta = 1, .repIndex = -1, .repDelta = 0, .weight = 2,
   .requireFlags = 0, .forbidFlags = F_UNKNOWN_EGG, .tone = CGC::TONE_NEUTRAL},

  {.name = "Species signal", .note = "Scanner pings a creature not in the DB.",
   .resourceDeltas = {0, 0, 0, 0, 0, 0, 0, 2}, .setFlags = F_UNKNOWN_EGG, .trackerIndex = 6,
   .trackerDelta = 1, .repIndex = 1, .repDelta = 1, .weight = 1,
   .requireFlags = 0, .forbidFlags = F_UNKNOWN_EGG, .tone = CGC::TONE_GOOD},

  // --- Ethical fork events ---
  {.name = "Black market offer", .note = "Buyer offers double for the rare cargo.",
   .resourceDeltas = {16, 0, 0, 0, 0, 0, 0, 4}, .setFlags = F_BLACK_MKT, .trackerIndex = -1,
   .trackerDelta = 0, .repIndex = 0, .repDelta = -2, .weight = 1,
   .requireFlags = F_CONTRACT, .forbidFlags = F_SANCTUARY, .tone = CGC::TONE_BAD},

  {.name = "Sanctuary contact", .note = "Free-range refuge offers a delivery bond.",
   .resourceDeltas = {4, 0, 0, 0, 0, 0, -3, -2}, .setFlags = F_SANCTUARY, .trackerIndex = -1,
   .trackerDelta = 0, .repIndex = 1, .repDelta = 2, .weight = 2,
   .requireFlags = F_CONTRACT, .forbidFlags = F_BLACK_MKT, .tone = CGC::TONE_GOOD},

  {.name = "Corp recruiter", .note = "Corporate agent wants exclusivity.",
   .resourceDeltas = {10, 5, 0, 3, 0, 0, 0, 0}, .setFlags = F_CORP_DEAL, .trackerIndex = -1,
   .trackerDelta = 0, .repIndex = 3, .repDelta = 2, .weight = 1,
   .requireFlags = 0, .forbidFlags = F_SANCTUARY, .tone = CGC::TONE_NEUTRAL},

  // --- Ship hazard events ---
  {.name = "O2 scrubber fault", .note = "Scrubber clogs; reserves drop fast.",
   .resourceDeltas = {0, 0, -6, 0, 0, 0, 4, 0}, .setFlags = 0, .trackerIndex = -1,
   .trackerDelta = 0, .repIndex = -1, .repDelta = 0, .weight = 2,
   .requireFlags = 0, .forbidFlags = 0, .tone = CGC::TONE_BAD},

  {.name = "Quarantine alarm", .note = "QRisk spikes; isolate now or face reroute.",
   .resourceDeltas = {0, 0, 0, 0, -3, 0, 3, 8}, .setFlags = 0, .trackerIndex = -1,
   .trackerDelta = 0, .repIndex = -1, .repDelta = 0, .weight = 2,
   .requireFlags = 0, .forbidFlags = F_PAPERS, .tone = CGC::TONE_BAD},

  // --- Gated discovery endgame ---
  {.name = "Extinction alert", .note = "A species vanish pattern emerges in data.",
   .resourceDeltas = {0, 0, 0, 0, 0, 0, 0, 0}, .setFlags = F_DISCOVERY, .trackerIndex = 6,
   .trackerDelta = 2, .repIndex = 1, .repDelta = 3, .weight = 3,
   .requireFlags = F_UNKNOWN_EGG, .forbidFlags = F_BLACK_MKT, .tone = CGC::TONE_GOOD},
};

// ── ENDINGS ───────────────────────────────────────────────────────────────────
// Most-specific first; catch-all victory last, then catch-all loss.
static const CGC::EndingDef ENDINGS[] = {
  // 1. Sanctuary Fleet — ethical path, no black market dealings.
  {.name = "Sanctuary Fleet",
   .note = "Your refuge fleet becomes the frontier standard.",
   .cond = {.resourceIndex = R_STRESS, .resourceMin = 0, .resourceMax = 40,
            .trackerIndex = 3, .trackerMin = 4,
            .repIndex = 1, .repMin = 4,
            .requireFlags = F_SANCTUARY | F_DISCOVERY,
            .forbidFlags = F_BLACK_MKT,
            .requiresVictory = true}},

  // 2. Discovery Ark — catalogued species without selling them.
  {.name = "Discovery Ark",
   .note = "A new species registry bears your captain name.",
   .cond = {.resourceIndex = -1, .resourceMin = 0, .resourceMax = 0,
            .trackerIndex = 6, .trackerMin = 3,
            .repIndex = 1, .repMin = 3,
            .requireFlags = F_DISCOVERY,
            .forbidFlags = F_BLACK_MKT,
            .requiresVictory = true}},

  // 3. Corporate Transporter — signed corp deal, no sanctuary.
  {.name = "Corp Transporter",
   .note = "The frontier franchise bears the company brand.",
   .cond = {.resourceIndex = R_CRED, .resourceMin = 80, .resourceMax = 999,
            .trackerIndex = -1, .trackerMin = 0,
            .repIndex = 3, .repMin = 3,
            .requireFlags = F_CORP_DEAL,
            .forbidFlags = F_SANCTUARY,
            .requiresVictory = true}},

  // 4. Smuggler Hold — took black market deals, ran the bypass.
  {.name = "Smuggler Hold",
   .note = "Rich and wanted; the hold hides the rare cargo.",
   .cond = {.resourceIndex = R_CRED, .resourceMin = 70, .resourceMax = 999,
            .trackerIndex = -1, .trackerMin = 0,
            .repIndex = 0, .repMin = 0,
            .requireFlags = F_BLACK_MKT,
            .forbidFlags = F_SANCTUARY,
            .requiresVictory = true}},

  // 5. Catch-all victory — completed the haul, no special distinction.
  {.name = "Frontier Captain",
   .note = "Cargo delivered; the route log earns you a rep.",
   .cond = {.resourceIndex = -1, .resourceMin = 0, .resourceMax = 0,
            .trackerIndex = -1, .trackerMin = 0,
            .repIndex = -1, .repMin = 0,
            .requireFlags = 0, .forbidFlags = 0,
            .requiresVictory = true}},

  // 6. Stranded Ship — loss ending.
  {.name = "Stranded Ship",
   .note = "Fuel gone; the creatures and crew drift in silence.",
   .cond = {.resourceIndex = -1, .resourceMin = 0, .resourceMax = 0,
            .trackerIndex = -1, .trackerMin = 0,
            .repIndex = -1, .repMin = 0,
            .requireFlags = 0, .forbidFlags = 0,
            .requiresVictory = false}},
};

// ── TRACKER LABELS ────────────────────────────────────────────────────────────
static const char* const TRACKER_LABELS[] = {
  "Routes",     // 0
  "Contracts",  // 1
  "Creatures",  // 2
  "Legs",       // 3
  "Care",       // 4
  "Inspections",// 5
  "Species",    // 6
  nullptr,      // 7
};

// ── GAME DEFINITION ───────────────────────────────────────────────────────────
static const CGC::DeepGameDefinition DEF = {
  .title = "Star Trail Ranch",
  .slug = "star-trail-rancher",
  .subtitle = "Creature transport sim",
  .saveMagic = "STR2",
  .resources = RESOURCES,
  .resourceCount = sizeof(RESOURCES) / sizeof(RESOURCES[0]),
  .screens = SCREENS,
  .screenCount = sizeof(SCREENS) / sizeof(SCREENS[0]),
  .events = EVENTS,
  .eventCount = sizeof(EVENTS) / sizeof(EVENTS[0]),
  .endings = ENDINGS,
  .endingCount = sizeof(ENDINGS) / sizeof(ENDINGS[0]),
  .targetProgress = 150,
  .accentColor = 0xF81F,
  .warningColor = 0xFD20,
  .trackerLabels = TRACKER_LABELS,
  .trackerLabelCount = 8,
  .objective = "Haul creatures; keep them alive en route.",
  .primaryTracker = 1,
};

const CGC::DeepGameDefinition& gameDefinition() {
  return DEF;
}
