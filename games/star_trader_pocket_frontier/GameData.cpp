#include "GameData.h"

namespace CGC = CardputerGameCore;

// Resource indices (keep in sync with RESOURCES order).
enum {
  R_CR,    // Credits: currency
  R_FUEL,  // Fuel: survival; negative primaryDelta = loss trigger
  R_O2,    // Oxygen: survival buffer on long legs
  R_HULL,  // Hull: ship condition
  R_HEAT,  // Heat: patrol attention (inverted: high is bad)
  R_CARGO, // Cargo pressure
  R_MOR,   // Crew morale
  R_DEBT   // Debt (inverted: high is bad)
};

// Story flags. Screens set these when first visited; events advance the arc.
enum : uint32_t {
  F_BRIDGE_BRIEFED  = 1u << 0,  // captain reviewed ship status
  F_FIRST_TRADE     = 1u << 1,  // at least one trade completed
  F_ROUTE_FLOWN     = 1u << 2,  // at least one route leg flown
  F_CREW_HIRED      = 1u << 3,  // at least one crew member signed
  F_UPGRADED        = 1u << 4,  // at least one ship upgrade installed
  F_RELIC_FOUND     = 1u << 5,  // alien relic fragment recovered
  F_DEBT_RESOLVED   = 1u << 6,  // debt paid down to safe level
  F_FACTION_PACT    = 1u << 7,  // faction allegiance struck
  F_RELIC_AWAKENED  = 1u << 8,  // relic fully decoded and live
  F_PIRATE_DEAL     = 1u << 9,  // smuggler compact agreed to
  F_CORP_PAWN       = 1u << 10, // sold route data to megacorp
};

static const CGC::ResourceDef RESOURCES[] = {
    // label  start  min   max  prim safe  warn  bad   inverted
    {"Cr",    120,   0,   999,   0,   0,   40,   15,  false},  // credits: low warn
    {"Fuel",   60,   0,   120,  -1,   8,   25,   10,  false},  // depletion = loss
    {"O2",     70,   0,   120,   0,   6,   20,    8,  false},  // buffer; runs out on long legs
    {"Hull",   85,   0,   100,   0,   4,   30,   12,  false},  // ship condition
    {"Heat",    5,   0,   100,   1,  -4,   55,   80,  true},   // patrol attention: high is bad
    {"Cargo",   0,   0,    80,   0,   0,   -1,   -1,  false},  // cargo pressure (counter)
    {"Mor",    70,   0,   100,   0,   2,   30,   12,  false},  // crew morale
    {"Debt",   35,   0,   999,   0,   0,   60,   90,  true},   // debt: high is bad
};

// ---- BRIDGE rows ----
static const CGC::NamedValue ROWS_BRIDGE[] = {
    {"Ship Status",     "Check fuel, O2, hull — plan next leg."},
    {"Debt Ledger",     "Review loans; pressure noted in log."},
    {"Patrol Report",   "Read sector patrols; trim heat risk."},
    {"Distress Ping",   "Relay asks for aid; track it down."},
    {"Artifact Lead",   "Coordinate fragment parsed from log."},
    {"Dock Forecast",   "Next port fees and market outlook."},
    {"Crew Briefing",   "Morale notes; prep crew for next jump."},
    {"Fuel Estimate",   "Plot reserve vs. route cost; adjust."},
};

// ---- MARKET rows ----
static const CGC::NamedValue ROWS_MARKET[] = {
    {"Spice",     "High-value; watch heat on restricted."},
    {"Water",     "Cheap bulk good; safe but thin margin."},
    {"Medgel",    "Medical stock; sells well at Outposts."},
    {"Relics",    "Ancient tech; risky but lucrative."},
    {"Scrap",     "Heavy; cheap filler for lean legs."},
    {"Kelp-Paste","Station staple; steady low profit."},
    {"Data Cores","Faction freight; heat if uninspected."},
    {"Textiles",  "Legal, light; safe margin in Core."},
};

// ---- ROUTE rows ----
static const CGC::NamedValue ROWS_ROUTE[] = {
    {"Moth Crossing",  "Short hop; low hazard, low patrol."},
    {"Tin Belt",       "Asteroid fringe; saves fuel, risky."},
    {"Glass Corridor", "Clear lane; fast but heavily scanned."},
    {"Drift Run",      "Fuel-free coast leg; long travel time."},
    {"Rust Expanse",   "Salvage-rich; hull stress zone."},
    {"Blue Approach",  "Faction space; heat if no permit."},
    {"Null Passage",   "Dead zone; no help if engines fail."},
    {"Gate Transit",   "Jump-gate; fast, costs port tax."},
};

// ---- CARGO rows ----
static const CGC::NamedValue ROWS_CARGO[] = {
    {"Purge Scrap",     "Dump low-value mass; free capacity."},
    {"Repack Hold",     "Redistribute load; lower heat risk."},
    {"Inspect Goods",   "Check for contraband before customs."},
    {"Seal Smuggler Bay","Lock restricted items behind panel."},
    {"Log Manifest",    "File accurate manifest; heat drops."},
    {"Jettison Mass",   "Emergency dump; gain O2 headroom."},
    {"Sort Priority",   "Front-load relic cargo for delivery."},
    {"Cache Fuel Cell", "Stash a reserve cell in cargo bay."},
};

// ---- CREW rows ----
static const CGC::NamedValue ROWS_CREW[] = {
    {"Kav — Pilot",      "Fuel-efficient burns; reduces fuel use."},
    {"Rin — Broker",     "Market intel; better trade margins."},
    {"Doc Peel — Medic", "Keeps morale up; patches hull cuts."},
    {"Torq — Mechanic",  "Hull repair in flight; patch leaks."},
    {"Zess — Scout",     "Spots patrol lanes; lowers heat gain."},
    {"Poma — Cook",      "Crew fed well; strong morale buffer."},
    {"Shid — Shield Tech","Deflects pirate shots; hull shield."},
    {"Vera — Archivist", "Decodes relic data; gate to ending."},
};

// ---- SHIP UPGRADES rows ----
static const CGC::NamedValue ROWS_SHIP[] = {
    {"Cargo Bay+",    "Expands hold; carry more per leg."},
    {"Fuel Tank+",    "Extra tank; longer range, less risk."},
    {"O2 Recycler",   "Cuts O2 burn; safer on long legs."},
    {"Shield Plates", "Reduces hull damage from events."},
    {"Deep Scanner",  "Spots salvage and relic fragments."},
    {"Smuggler Hold", "Hides restricted cargo from customs."},
    {"Engine Tune",   "Faster burns; Fuel saves per route."},
    {"Med Bay",       "Crew heals mid-flight; morale boost."},
};

// ---- LOG / RELIC rows (gated) ----
static const CGC::NamedValue ROWS_LOG[] = {
    {"Relic Fragment A", "Decode first coordinate shard."},
    {"Relic Fragment B", "Cross-reference second ping data."},
    {"Gate Archive",     "Pull jump-gate logs for clue."},
    {"Faction Dossier",  "Faction favors toward allegiance."},
    {"Courier Thread",   "Delivery contract; credits+route."},
    {"Debt Notice",      "Formal demand; negotiate rate."},
    {"Patrol Pattern",   "Map patrols; plan low-heat leg."},
    {"Salvage Report",   "Known salvage zones on route map."},
};

static const CGC::DeepScreenDef SCREENS[] = {
    // BRIDGE — free; always open; sets F_BRIDGE_BRIEFED; small O2 and Mor gain
    {.title = "BRIDGE", .verb = "Review", .rows = ROWS_BRIDGE, .rowCount = 8,
     .resourceDeltas = {0, 0, 1, 0, -1, 0, 2, 0},
     .progressDelta = 2, .trackerIndex = 0,
     .flagMask = F_BRIDGE_BRIEFED,
     .costResource = -1, .costAmount = 0, .requireFlags = 0,
     .trackerDelta = 1,
     .objective = "Review ship status and plan ahead"},

    // MARKET — costs 10 Cr per action; sets F_FIRST_TRADE; earn Cr, fill Cargo
    {.title = "MARKET", .verb = "Trade", .rows = ROWS_MARKET, .rowCount = 8,
     .resourceDeltas = {12, 0, 0, 0, 2, 3, 0, -2},
     .progressDelta = 3, .trackerIndex = 1,
     .flagMask = F_FIRST_TRADE,
     .costResource = R_CR, .costAmount = 10, .requireFlags = 0,
     .trackerDelta = 1,
     .objective = "Buy low, sell high (-10 Cr stake)"},

    // ROUTES — costs Fuel; sets F_ROUTE_FLOWN; main progress engine
    {.title = "ROUTES", .verb = "Jump", .rows = ROWS_ROUTE, .rowCount = 8,
     .resourceDeltas = {0, -6, -3, -1, 3, 0, -2, 0},
     .progressDelta = 6, .trackerIndex = 2,
     .flagMask = F_ROUTE_FLOWN,
     .costResource = R_FUEL, .costAmount = 6, .requireFlags = 0,
     .trackerDelta = 1,
     .objective = "Fly a leg (-6 Fuel, +Heat, event)"},

    // CARGO — free; requires at least one trade; lowers Heat and Cargo pressure
    {.title = "CARGO", .verb = "Manage", .rows = ROWS_CARGO, .rowCount = 8,
     .resourceDeltas = {0, 0, 0, 0, -2, -3, 1, 0},
     .progressDelta = 1, .trackerIndex = 3,
     .flagMask = 0,
     .costResource = -1, .costAmount = 0, .requireFlags = F_FIRST_TRADE,
     .trackerDelta = 1,
     .objective = "Manage hold (needs a trade first)"},

    // CREW — costs 15 Cr; requires route flown; sets F_CREW_HIRED; Mor + O2
    {.title = "CREW", .verb = "Hire", .rows = ROWS_CREW, .rowCount = 8,
     .resourceDeltas = {0, 0, 2, 2, 0, 0, 4, -3},
     .progressDelta = 3, .trackerIndex = 4,
     .flagMask = F_CREW_HIRED,
     .costResource = R_CR, .costAmount = 15, .requireFlags = F_ROUTE_FLOWN,
     .trackerDelta = 1,
     .objective = "Sign crew (-15 Cr, needs Route)"},

    // SHIP — costs 25 Cr; requires crew hired; sets F_UPGRADED; Hull + Fuel
    {.title = "SHIP", .verb = "Upgrade", .rows = ROWS_SHIP, .rowCount = 8,
     .resourceDeltas = {0, 4, 3, 4, -2, 2, 0, 0},
     .progressDelta = 4, .trackerIndex = 5,
     .flagMask = F_UPGRADED,
     .costResource = R_CR, .costAmount = 25, .requireFlags = F_CREW_HIRED,
     .trackerDelta = 1,
     .objective = "Install upgrade (-25 Cr, needs Crew)"},

    // LOG — free; requires relic found via events; sets F_RELIC_AWAKENED path
    {.title = "LOG", .verb = "Trace", .rows = ROWS_LOG, .rowCount = 8,
     .resourceDeltas = {2, 0, 0, 0, -1, 0, 1, -2},
     .progressDelta = 5, .trackerIndex = 6,
     .flagMask = F_RELIC_AWAKENED,
     .costResource = -1, .costAmount = 0, .requireFlags = F_RELIC_FOUND,
     .trackerDelta = 1,
     .objective = "Trace relic data (needs Relic)"},
};

static const CGC::EventDef EVENTS[] = {
    // --- Common filler: always eligible ---
    {.name = "Clear Skies",
     .note = "Uneventful leg; crew rests a little.",
     .resourceDeltas = {0, 0, 2, 0, 0, 0, 2, 0},
     .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = -1, .repDelta = 0, .weight = 4,
     .requireFlags = 0, .forbidFlags = 0, .tone = CGC::TONE_NEUTRAL},

    {.name = "Tailwind Burn",
     .note = "Engine timing perfect; saves fuel.",
     .resourceDeltas = {0, 3, 0, 0, 0, 0, 0, 0},
     .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = -1, .repDelta = 0, .weight = 3,
     .requireFlags = 0, .forbidFlags = 0, .tone = CGC::TONE_GOOD},

    {.name = "Micro-Debris",
     .note = "Hull scratches; minor O2 leak patched.",
     .resourceDeltas = {0, 0, -2, -2, 0, 0, -1, 0},
     .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = -1, .repDelta = 0, .weight = 3,
     .requireFlags = 0, .forbidFlags = 0, .tone = CGC::TONE_BAD},

    {.name = "Crew Squabble",
     .note = "Argument in the hold; morale dips.",
     .resourceDeltas = {0, 0, 0, 0, 0, 0, -3, 0},
     .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = -1, .repDelta = 0, .weight = 2,
     .requireFlags = 0, .forbidFlags = 0, .tone = CGC::TONE_BAD},

    // --- Route-active events ---
    {.name = "Pirate Demand",
     .note = "Pay up or run; hull takes grazing hit.",
     .resourceDeltas = {-8, 0, 0, -3, 4, 0, -2, 0},
     .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = 2, .repDelta = -1, .weight = 3,
     .requireFlags = F_ROUTE_FLOWN, .forbidFlags = 0, .tone = CGC::TONE_BAD},

    {.name = "Customs Scan",
     .note = "Papers pass; clean manifest holds.",
     .resourceDeltas = {0, 0, 0, 0, -2, 0, 1, 0},
     .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = 1, .repDelta = 1, .weight = 3,
     .requireFlags = F_ROUTE_FLOWN, .forbidFlags = 0, .tone = CGC::TONE_NEUTRAL},

    {.name = "Contraband Flag",
     .note = "Inspector flags cargo; fine levied.",
     .resourceDeltas = {-12, 0, 0, 0, 6, -2, -1, 4},
     .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = 1, .repDelta = -2, .weight = 2,
     .requireFlags = F_ROUTE_FLOWN, .forbidFlags = F_UPGRADED, .tone = CGC::TONE_BAD},

    {.name = "Salvage Beacon",
     .note = "Derelict yields parts and O2 canisters.",
     .resourceDeltas = {5, 0, 4, 2, 0, 0, 1, 0},
     .setFlags = 0, .trackerIndex = 7, .trackerDelta = 1,
     .repIndex = -1, .repDelta = 0, .weight = 2,
     .requireFlags = F_ROUTE_FLOWN, .forbidFlags = 0, .tone = CGC::TONE_GOOD},

    {.name = "Meteor Volley",
     .note = "Cluster strike; hull breached, patched.",
     .resourceDeltas = {0, 0, -3, -5, 1, 0, -3, 0},
     .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = -1, .repDelta = 0, .weight = 2,
     .requireFlags = F_ROUTE_FLOWN, .forbidFlags = 0, .tone = CGC::TONE_BAD},

    {.name = "Market Swing",
     .note = "Arrival price surge; cargo sells high.",
     .resourceDeltas = {10, 0, 0, 0, 0, -2, 0, -3},
     .setFlags = 0, .trackerIndex = 1, .trackerDelta = 1,
     .repIndex = 0, .repDelta = 1, .weight = 2,
     .requireFlags = F_FIRST_TRADE, .forbidFlags = 0, .tone = CGC::TONE_GOOD},

    {.name = "Distress Call",
     .note = "You respond; spare O2 and Cr shared.",
     .resourceDeltas = {-5, 0, -3, 0, -2, 0, 3, 0},
     .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = 0, .repDelta = 2, .weight = 2,
     .requireFlags = F_BRIDGE_BRIEFED, .forbidFlags = 0, .tone = CGC::TONE_NEUTRAL},

    {.name = "Fuel Cache",
     .note = "Abandoned pod has sealed fuel cells.",
     .resourceDeltas = {0, 5, 0, 0, 0, 0, 1, 0},
     .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = -1, .repDelta = 0, .weight = 2,
     .requireFlags = F_ROUTE_FLOWN, .forbidFlags = 0, .tone = CGC::TONE_GOOD},

    // --- Gated story beats ---
    {.name = "Alien Ruin Ping",
     .note = "Old infrastructure; relic fragment logged.",
     .resourceDeltas = {0, 0, 0, 0, 1, 0, 0, 0},
     .setFlags = F_RELIC_FOUND, .trackerIndex = 6, .trackerDelta = 1,
     .repIndex = -1, .repDelta = 0, .weight = 2,
     .requireFlags = F_UPGRADED, .forbidFlags = F_RELIC_FOUND, .tone = CGC::TONE_GOOD},

    {.name = "Faction Envoy",
     .note = "Representative offers trade exclusivity.",
     .resourceDeltas = {8, 0, 0, 0, -3, 0, 2, -4},
     .setFlags = F_FACTION_PACT, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = 3, .repDelta = 3, .weight = 2,
     .requireFlags = F_CREW_HIRED | F_FIRST_TRADE, .forbidFlags = F_FACTION_PACT, .tone = CGC::TONE_GOOD},

    {.name = "Black Market Offer",
     .note = "Huge profit with serious heat penalty.",
     .resourceDeltas = {20, 0, 0, 0, 8, 2, 0, 0},
     .setFlags = F_PIRATE_DEAL, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = 2, .repDelta = 2, .weight = 1,
     .requireFlags = F_ROUTE_FLOWN, .forbidFlags = F_FACTION_PACT, .tone = CGC::TONE_BAD},

    {.name = "Corp Data Offer",
     .note = "Megacorp wants your route logs; easy Cr.",
     .resourceDeltas = {15, 0, 0, 0, 0, 0, -2, 0},
     .setFlags = F_CORP_PAWN, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = 3, .repDelta = -2, .weight = 1,
     .requireFlags = F_ROUTE_FLOWN, .forbidFlags = F_FACTION_PACT, .tone = CGC::TONE_BAD},

    {.name = "Debt Collector",
     .note = "Agent boards ship; forced payment made.",
     .resourceDeltas = {-10, 0, 0, -1, 3, 0, -2, -5},
     .setFlags = F_DEBT_RESOLVED, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = -1, .repDelta = 0, .weight = 2,
     .requireFlags = F_ROUTE_FLOWN, .forbidFlags = F_DEBT_RESOLVED, .tone = CGC::TONE_BAD},

    {.name = "Relic Awakens",
     .note = "Fragment broadcasts; coordinates confirmed.",
     .resourceDeltas = {0, 0, 0, 0, 2, 0, 4, -3},
     .setFlags = F_RELIC_AWAKENED, .trackerIndex = 6, .trackerDelta = 1,
     .repIndex = -1, .repDelta = 0, .weight = 3,
     .requireFlags = F_RELIC_FOUND | F_UPGRADED, .forbidFlags = F_RELIC_AWAKENED, .tone = CGC::TONE_GOOD},
};

static const CGC::EndingDef ENDINGS[] = {
    // Most-specific first

    {.name = "Relic Awakener",
     .note = "Ancient coordinates answered. You led them there.",
     .cond = {.resourceIndex = R_CR, .resourceMin = 30, .resourceMax = 999,
              .trackerIndex = 6, .trackerMin = 3,
              .repIndex = -1, .repMin = 0,
              .requireFlags = F_RELIC_AWAKENED,
              .forbidFlags = F_CORP_PAWN,
              .requiresVictory = true}},

    {.name = "Frontier Savior",
     .note = "You backed the faction. The frontier is safer.",
     .cond = {.resourceIndex = R_MOR, .resourceMin = 50, .resourceMax = 100,
              .trackerIndex = 4, .trackerMin = 3,
              .repIndex = 3, .repMin = 3,
              .requireFlags = F_FACTION_PACT | F_CREW_HIRED,
              .forbidFlags = F_PIRATE_DEAL,
              .requiresVictory = true}},

    {.name = "Pirate Bargain",
     .note = "You dealt with the dark side. Good living, bad rep.",
     .cond = {.resourceIndex = R_CR, .resourceMin = 60, .resourceMax = 999,
              .trackerIndex = -1, .trackerMin = 0,
              .repIndex = 2, .repMin = 2,
              .requireFlags = F_PIRATE_DEAL,
              .forbidFlags = 0,
              .requiresVictory = true}},

    {.name = "Corporate Pawn",
     .note = "Your route data is a product now. They own you.",
     .cond = {.resourceIndex = -1, .resourceMin = 0, .resourceMax = 0,
              .trackerIndex = -1, .trackerMin = 0,
              .repIndex = -1, .repMin = 0,
              .requireFlags = F_CORP_PAWN,
              .forbidFlags = 0,
              .requiresVictory = true}},

    // Catch-all victory: honest merchant
    {.name = "Merchant Star",
     .note = "Clean ledger, loyal crew, open frontier. Well done.",
     .cond = {.resourceIndex = -1, .resourceMin = 0, .resourceMax = 0,
              .trackerIndex = -1, .trackerMin = 0,
              .repIndex = -1, .repMin = 0,
              .requireFlags = 0,
              .forbidFlags = 0,
              .requiresVictory = true}},

    // Catch-all loss: lost ship
    {.name = "Lost Ship",
     .note = "Fuel gone, hull cracked. No signal came back.",
     .cond = {.resourceIndex = -1, .resourceMin = 0, .resourceMax = 0,
              .trackerIndex = -1, .trackerMin = 0,
              .repIndex = -1, .repMin = 0,
              .requireFlags = 0,
              .forbidFlags = 0,
              .requiresVictory = false}},
};

static const char* const TRACKER_LABELS[] = {
    "Reviews",   // 0 — BRIDGE visits
    "Trades",    // 1 — MARKET actions
    "Routes",    // 2 — legs flown
    "Cargo Ops", // 3 — CARGO actions
    "Crew",      // 4 — crew signed
    "Upgrades",  // 5 — upgrades installed
    "Relics",    // 6 — relic fragments
    "Salvage",   // 7 — salvage events
};

static const CGC::DeepGameDefinition DEF = {
    .title       = "Star Trader",
    .slug        = "star-trader-pocket-frontier",
    .subtitle    = "Ship-console trading sim",
    .saveMagic   = "STPF2",
    .resources   = RESOURCES,
    .resourceCount = sizeof(RESOURCES) / sizeof(RESOURCES[0]),
    .screens     = SCREENS,
    .screenCount = sizeof(SCREENS) / sizeof(SCREENS[0]),
    .events      = EVENTS,
    .eventCount  = sizeof(EVENTS) / sizeof(EVENTS[0]),
    .endings     = ENDINGS,
    .endingCount = sizeof(ENDINGS) / sizeof(ENDINGS[0]),
    .targetProgress = 180,
    .accentColor    = 0x07FF,
    .warningColor   = 0xFD20,
    .trackerLabels  = TRACKER_LABELS,
    .trackerLabelCount = 8,
    .objective   = "Trade, fly, and survive the frontier.",
    .primaryTracker = 2,  // Routes flown as headline
};

const CGC::DeepGameDefinition& gameDefinition() {
  return DEF;
}
