#include "GameData.h"

namespace CGC = CardputerGameCore;

// Resource indices (must match RESOURCES order below).
enum {
  R_FOOD, R_GOLD, R_POP, R_SOLD,   // first four — FOOD has negative primaryDelta
  R_MOR,  R_FAITH, R_STAB, R_THREAT
};

// Story flags — screens set these; events gate and advance the arc.
enum : uint32_t {
  F_GRANARY    = 1u <<  0,  // granary built; unlocks advanced BUILD rows
  F_WALLS      = 1u <<  1,  // walls raised; reduces border raid severity
  F_BARRACKS   = 1u <<  2,  // barracks built; army screen fully unlocked
  F_EDICT      = 1u <<  3,  // first law decreed; marks legislative activity
  F_ADVISORS   = 1u <<  4,  // advisors formally consulted; unlocks COUNCIL
  F_THREAT_HI  = 1u <<  5,  // threat exceeds 60; crisis screen unlocked
  F_FAMINE     = 1u <<  6,  // famine declared; food production emergency
  F_DRAGON     = 1u <<  7,  // dragon rumor confirmed; gating late events
  F_SUCCESSION = 1u <<  8,  // succession secured; stability bonus
  F_MAGIC_SEAL = 1u <<  9,  // magical leak sealed; faith/stability bonus
};

// ── Resources ────────────────────────────────────────────────────────────────
// label  start min  max  prim safe  warn  bad   inverted
static const CGC::ResourceDef RESOURCES[] = {
  {"Food",   80,  0,  999,  -1,   4,   25,   10,  false}, // depletion ends run
  {"Gold",   55,  0,  999,   0,   0,   14,    5,  false},
  {"Pop",    40,  0,  200,   0,   1,   10,    4,  false},
  {"Sold",    8,  0,  100,   0,   0,    3,    1,  false},
  {"Morale", 65,  0,  100,   0,   2,   25,   10,  false},
  {"Faith",  30,  0,  100,   0,   0,   -1,   -1,  false},
  {"Stab",   60,  0,  100,   0,   1,   20,    8,  false},
  {"Threat", 10,  0,  100,   0,  -2,   55,   78,  true},  // high is bad
};

// ── Screen row tables ─────────────────────────────────────────────────────────

// REALM — review the season; advance the ledger
static const CGC::NamedValue ROWS_REALM[] = {
  {"Spring Audit",    "Survey fields and count planting labor."},
  {"Summer Survey",   "Measure grain growth and water level."},
  {"Autumn Tally",    "Record harvest yield and tax receipts."},
  {"Winter Review",   "Count reserves and assess winter toll."},
  {"Border Report",   "Hear patrol dispatches from the frontier."},
  {"Market Ledger",   "Balance trade accounts for the season."},
  {"Census Count",    "Record births, deaths, and migrations."},
  {"Royal Decree",    "Publish the season's formal proclamation."},
};

// WORKERS — assign population labor to farms, mines, walls, etc.
static const CGC::NamedValue ROWS_WORKERS[] = {
  {"Farm Crews",      "Send workers to till fields and granaries."},
  {"Mine Gangs",      "Route labor to the silver and iron mines."},
  {"Wall Crews",      "Set workers repairing and raising walls."},
  {"Temple Wards",    "Dedicate servants to temple upkeep."},
  {"Market Porters",  "Staff the market stalls and trade roads."},
  {"Study Fellows",   "Send scholars to the library and tower."},
  {"Garrison Drill",  "Rotate civilians through soldier training."},
  {"Road Gangs",      "Deploy crews to mend roads and bridges."},
};

// BUILD — spend Gold to raise and upgrade structures
static const CGC::NamedValue ROWS_BUILD[] = {
  {"Granary",         "Expand grain storage; eases famine risk."},
  {"Farm Terrace",    "Level terraces to boost crop yields."},
  {"Barracks",        "House and train a standing garrison."},
  {"Stone Wall",      "Raise the perimeter wall another course."},
  {"Market Hall",     "Open a covered market for inland trade."},
  {"Shrine",          "Dedicate a shrine to the realm's patron."},
  {"Library Wing",    "Add stacks and a reading hall for scholars."},
  {"Wizard Tower",    "Commission a tower for the court mage."},
  {"King's Road",     "Pave the royal road to the border keep."},
  {"Infirmary",       "Fund surgeons and a plague ward."},
};

// LAWS — issue decrees; sets flags, trade-offs between factions
static const CGC::NamedValue ROWS_LAWS[] = {
  {"Bread Dole",      "Grant commoners a daily bread ration."},
  {"Tax Reform",      "Restructure tithes; nobles resist."},
  {"Militia Charter", "Arm the commons for local defense."},
  {"Temple Tithe",    "Compel a levy on all temple income."},
  {"Trade Rights",    "Open the southern road to merchant guilds."},
  {"School Edict",    "Fund a village school in each district."},
  {"Night Curfew",    "Impose curfew to curb street unrest."},
  {"Water Charter",   "Regulate mill rights on the river."},
  {"Inheritance Law", "Reform noble succession by royal writ."},
  {"Relief Fund",     "Open the treasury to disaster relief."},
  {"Magic License",   "Require mages to register with the court."},
  {"People Charter",  "Codify commoner rights before the throne."},
};

// ARMY — raise troops, fortify, and respond to border pressure
static const CGC::NamedValue ROWS_ARMY[] = {
  {"Levy Draft",      "Conscript seasonal soldiers from farms."},
  {"Garrison Patrol", "Send patrols along the border roads."},
  {"Cavalry Scout",   "Dispatch light horse to watch the passes."},
  {"Fortify Keep",    "Spend gold shoring up the border keep."},
  {"Diplomacy Rider", "Send a rider with tribute to calm raiders."},
  {"War Council",     "Hold formal war council; plan next move."},
  {"Rally Banner",    "Call banners; volunteers swell the ranks."},
  {"Veteran Hire",    "Hire a company of experienced veterans."},
};

// ADVISORS — heed counsel; reputation effects, factional trade-offs
static const CGC::NamedValue ROWS_ADVISORS[] = {
  {"Steward's Ledger","Heed the steward's cost-cutting plan."},
  {"Marshal's Plan",  "Approve the marshal's troop dispositions."},
  {"High Priest",     "Receive the priest's sermon and mandate."},
  {"Court Mage",      "Listen to the mage's arcane assessment."},
  {"Noble Faction",   "Hear the nobles' petition for privilege."},
  {"Farmer's Voice",  "Let the farm elder speak to the court."},
  {"Heir's Report",   "Review the heir's progress and counsel."},
  {"Merchant Envoy",  "Negotiate terms with the trade envoy."},
};

// CRISIS — late-game screen; locked until Threat is high (F_THREAT_HI)
static const CGC::NamedValue ROWS_CRISIS[] = {
  {"Open Granary",    "Release reserves to break the famine."},
  {"Buy Foreign Grain","Spend gold to import emergency food."},
  {"Seal Magic Leak", "Send mages to close the unstable rift."},
  {"Ransom Raiders",  "Pay tribute to halt the border raids."},
  {"Call Inquisitor", "Send an inquisitor into the plague ward."},
  {"Dragon Parley",   "Ride to the mountain with a peace gift."},
  {"Exile Schemer",   "Banish the court schemer openly."},
  {"Rally the Realm", "Issue a unity proclamation; spend morale."},
};

// ── Screens ───────────────────────────────────────────────────────────────────
static const CGC::DeepScreenDef SCREENS[] = {
  // REALM — free; advances season count; sets broad flag on first use
  {.title = "REALM", .verb = "Review", .rows = ROWS_REALM, .rowCount = 8,
   .resourceDeltas = {2, 1, 0, 0, 1, 0, 1, -1},
   .progressDelta = 2, .trackerIndex = 0,
   .flagMask = 0, .costResource = -1, .costAmount = 0, .requireFlags = 0,
   .trackerDelta = 1, .objective = "Review the season; hold the realm"},

  // WORKERS — free; big food & gold swing; morale pressure
  {.title = "WORKERS", .verb = "Assign", .rows = ROWS_WORKERS, .rowCount = 8,
   .resourceDeltas = {4, 2, 0, 0, -1, 0, 1, -1},
   .progressDelta = 3, .trackerIndex = 1,
   .flagMask = 0, .costResource = -1, .costAmount = 0, .requireFlags = 0,
   .trackerDelta = 1, .objective = "Assign labor; keep food flowing"},

  // BUILD — costs Gold; requires at least one REALM review first
  {.title = "BUILD", .verb = "Raise", .rows = ROWS_BUILD, .rowCount = 10,
   .resourceDeltas = {0, -2, 0, 0, 1, 1, 2, -2},
   .progressDelta = 4, .trackerIndex = 2,
   .flagMask = F_GRANARY, .costResource = R_GOLD, .costAmount = 12,
   .requireFlags = 0, .trackerDelta = 1,
   .objective = "Raise structures (-12 Gold)"},

  // LAWS — costs Gold; sets F_EDICT; no prior gate (laws available early)
  {.title = "LAWS", .verb = "Decree", .rows = ROWS_LAWS, .rowCount = 12,
   .resourceDeltas = {0, -3, 0, 0, 2, 1, 1, -1},
   .progressDelta = 3, .trackerIndex = 3,
   .flagMask = F_EDICT, .costResource = R_GOLD, .costAmount = 8,
   .requireFlags = 0, .trackerDelta = 1,
   .objective = "Issue a decree (-8 Gold)"},

  // ARMY — costs Gold + People (Sold is the product); needs Barracks
  {.title = "ARMY", .verb = "Muster", .rows = ROWS_ARMY, .rowCount = 8,
   .resourceDeltas = {-1, -2, -1, 3, -1, 0, 1, -3},
   .progressDelta = 3, .trackerIndex = 4,
   .flagMask = F_BARRACKS, .costResource = R_GOLD, .costAmount = 10,
   .requireFlags = F_GRANARY, .trackerDelta = 1,
   .objective = "Muster soldiers (-10 Gold, needs Granary)"},

  // ADVISORS — free; reputation trade-offs; sets F_ADVISORS
  {.title = "ADVISORS", .verb = "Heed", .rows = ROWS_ADVISORS, .rowCount = 8,
   .resourceDeltas = {0, 1, 0, 0, 1, 1, 1, -1},
   .progressDelta = 2, .trackerIndex = 5,
   .flagMask = F_ADVISORS, .costResource = -1, .costAmount = 0,
   .requireFlags = 0, .trackerDelta = 1,
   .objective = "Heed advisors; balance factions"},

  // CRISIS — locked until Threat spikes (F_THREAT_HI); costly; high reward
  {.title = "CRISIS", .verb = "Answer", .rows = ROWS_CRISIS, .rowCount = 8,
   .resourceDeltas = {3, -4, 0, -1, 2, 1, 3, -5},
   .progressDelta = 6, .trackerIndex = 6,
   .flagMask = F_MAGIC_SEAL, .costResource = R_GOLD, .costAmount = 15,
   .requireFlags = F_THREAT_HI, .trackerDelta = 1,
   .objective = "Answer the crisis (-15 Gold, needs F_THREAT_HI)"},
};

// ── Events ────────────────────────────────────────────────────────────────────
static const CGC::EventDef EVENTS[] = {
  // --- Common neutral fillers ---
  {.name = "Quiet season",
   .note = "Nothing stirs; the realm catches its breath.",
   .resourceDeltas = {1, 0, 0, 0, 1, 0, 0, 0},
   .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
   .repIndex = -1, .repDelta = 0, .weight = 5,
   .requireFlags = 0, .forbidFlags = 0, .tone = CGC::TONE_NEUTRAL},

  {.name = "Market day",
   .note = "Traders arrive; coin flows through the market.",
   .resourceDeltas = {0, 3, 0, 0, 1, 0, 0, 0},
   .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
   .repIndex = 0, .repDelta = 1, .weight = 4,
   .requireFlags = 0, .forbidFlags = 0, .tone = CGC::TONE_GOOD},

  {.name = "Good harvest",
   .note = "Fields yield double; granaries swell.",
   .resourceDeltas = {6, 2, 0, 0, 2, 0, 1, -1},
   .setFlags = 0, .trackerIndex = 0, .trackerDelta = 1,
   .repIndex = 0, .repDelta = 2, .weight = 3,
   .requireFlags = 0, .forbidFlags = F_FAMINE, .tone = CGC::TONE_GOOD},

  // --- Negative pressure events ---
  {.name = "Poor yield",
   .note = "Blight strikes the eastern fields; food falls.",
   .resourceDeltas = {-4, 0, 0, 0, -2, 0, -1, 2},
   .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
   .repIndex = -1, .repDelta = 0, .weight = 3,
   .requireFlags = 0, .forbidFlags = 0, .tone = CGC::TONE_BAD},

  {.name = "Border raid",
   .note = "Raiders strike the frontier; soldiers fall.",
   .resourceDeltas = {-2, -2, -1, -2, -2, 0, -2, 4},
   .setFlags = 0, .trackerIndex = 4, .trackerDelta = 1,
   .repIndex = 1, .repDelta = -1, .weight = 3,
   .requireFlags = 0, .forbidFlags = F_WALLS, .tone = CGC::TONE_BAD},

  {.name = "Border held",
   .note = "Walls turn the raiders; threat eases.",
   .resourceDeltas = {0, 0, 0, 0, 2, 0, 1, -3},
   .setFlags = 0, .trackerIndex = 4, .trackerDelta = 1,
   .repIndex = 1, .repDelta = 2, .weight = 3,
   .requireFlags = F_WALLS, .forbidFlags = 0, .tone = CGC::TONE_GOOD},

  {.name = "Plague ward",
   .note = "Fever spreads through the lower districts.",
   .resourceDeltas = {-1, -1, -2, -1, -3, -1, -2, 3},
   .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
   .repIndex = 0, .repDelta = -1, .weight = 2,
   .requireFlags = 0, .forbidFlags = 0, .tone = CGC::TONE_BAD},

  {.name = "Infirmary check",
   .note = "Surgeons contain the fever quickly.",
   .resourceDeltas = {0, -1, 0, 0, 1, 1, 1, -1},
   .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
   .repIndex = 0, .repDelta = 1, .weight = 2,
   .requireFlags = F_GRANARY, .forbidFlags = 0, .tone = CGC::TONE_GOOD},

  // --- Story beats: famine arc ---
  {.name = "Famine warning",
   .note = "Steward warns: reserves will not last winter.",
   .resourceDeltas = {-3, 0, 0, 0, -2, 0, -1, 2},
   .setFlags = F_FAMINE, .trackerIndex = -1, .trackerDelta = 0,
   .repIndex = 0, .repDelta = -1, .weight = 2,
   .requireFlags = 0, .forbidFlags = F_FAMINE | F_GRANARY, .tone = CGC::TONE_BAD},

  {.name = "Famine breaks",
   .note = "Opened granary saves the hungry season.",
   .resourceDeltas = {4, -3, 0, 0, 3, 1, 2, -2},
   .setFlags = 0, .trackerIndex = 6, .trackerDelta = 1,
   .repIndex = 0, .repDelta = 2, .weight = 3,
   .requireFlags = F_FAMINE | F_GRANARY, .forbidFlags = 0, .tone = CGC::TONE_GOOD},

  // --- Dragon rumor arc ---
  {.name = "Dragon sighting",
   .note = "Shepherds flee a smoking ridge; rumor spreads.",
   .resourceDeltas = {0, 0, 0, 0, -3, -1, -2, 4},
   .setFlags = F_DRAGON, .trackerIndex = -1, .trackerDelta = 0,
   .repIndex = 2, .repDelta = -1, .weight = 1,
   .requireFlags = 0, .forbidFlags = F_DRAGON, .tone = CGC::TONE_BAD},

  {.name = "Dragon parley",
   .note = "A peace gift soothes the mountain wyrm.",
   .resourceDeltas = {0, -4, 0, 0, 2, 2, 3, -4},
   .setFlags = F_MAGIC_SEAL, .trackerIndex = 6, .trackerDelta = 1,
   .repIndex = 2, .repDelta = 3, .weight = 2,
   .requireFlags = F_DRAGON | F_ADVISORS, .forbidFlags = 0, .tone = CGC::TONE_GOOD},

  // --- Succession arc ---
  {.name = "Succession dispute",
   .note = "Two nobles claim the heir's right; tension rises.",
   .resourceDeltas = {0, -1, 0, 0, -2, -1, -3, 3},
   .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
   .repIndex = 3, .repDelta = -1, .weight = 2,
   .requireFlags = F_EDICT, .forbidFlags = F_SUCCESSION, .tone = CGC::TONE_BAD},

  {.name = "Heir confirmed",
   .note = "Court backs the heir; nobles fall in line.",
   .resourceDeltas = {0, 0, 0, 0, 3, 1, 4, -2},
   .setFlags = F_SUCCESSION, .trackerIndex = 5, .trackerDelta = 1,
   .repIndex = 3, .repDelta = 2, .weight = 2,
   .requireFlags = F_ADVISORS, .forbidFlags = F_SUCCESSION, .tone = CGC::TONE_GOOD},

  // --- Magical leak arc ---
  {.name = "Arcane rift",
   .note = "The tower bleeds wild magic; villagers flee.",
   .resourceDeltas = {0, 0, -1, 0, -2, -2, -2, 3},
   .setFlags = F_THREAT_HI, .trackerIndex = -1, .trackerDelta = 0,
   .repIndex = 2, .repDelta = -2, .weight = 1,
   .requireFlags = F_EDICT, .forbidFlags = F_MAGIC_SEAL, .tone = CGC::TONE_BAD},

  {.name = "Rift sealed",
   .note = "Mages contain the leak; faith restored.",
   .resourceDeltas = {0, -2, 0, 0, 1, 4, 3, -3},
   .setFlags = F_MAGIC_SEAL, .trackerIndex = 6, .trackerDelta = 1,
   .repIndex = 2, .repDelta = 3, .weight = 2,
   .requireFlags = F_THREAT_HI, .forbidFlags = F_MAGIC_SEAL, .tone = CGC::TONE_GOOD},

  // --- Advisor scheming event ---
  {.name = "Advisor scandal",
   .note = "A minister is caught selling royal secrets.",
   .resourceDeltas = {0, -2, 0, 0, -3, -1, -2, 2},
   .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
   .repIndex = 3, .repDelta = -2, .weight = 1,
   .requireFlags = F_ADVISORS, .forbidFlags = 0, .tone = CGC::TONE_BAD},

  // --- Festival ---
  {.name = "Harvest festival",
   .note = "Feasting and song; morale soars for a season.",
   .resourceDeltas = {-2, -1, 0, 0, 5, 2, 1, -1},
   .setFlags = 0, .trackerIndex = 0, .trackerDelta = 1,
   .repIndex = 0, .repDelta = 2, .weight = 2,
   .requireFlags = 0, .forbidFlags = F_FAMINE, .tone = CGC::TONE_GOOD},
};

// ── Endings ───────────────────────────────────────────────────────────────────
// Most-specific first; catch-all victory and loss at the end.
static const CGC::EndingDef ENDINGS[] = {
  // 1. Wizard Pact — magic sealed, faith high, Threat beaten
  {.name = "Wizard Pact",
   .note = "The mages pledge eternal service; the realm glows.",
   .cond = {.resourceIndex = R_FAITH, .resourceMin = 60, .resourceMax = 100,
            .trackerIndex = 6, .trackerMin = 3,
            .repIndex = 2, .repMin = 3,
            .requireFlags = F_MAGIC_SEAL, .forbidFlags = 0,
            .requiresVictory = true}},

  // 2. Holy Mandate — Faith very high, succession secured
  {.name = "Holy Mandate",
   .note = "The clergy crown the heir; a holy dynasty begins.",
   .cond = {.resourceIndex = R_FAITH, .resourceMin = 55, .resourceMax = 100,
            .trackerIndex = -1, .trackerMin = 0,
            .repIndex = -1, .repMin = 0,
            .requireFlags = F_SUCCESSION | F_EDICT, .forbidFlags = 0,
            .requiresVictory = true}},

  // 3. Iron Crown — Soldiers high, walls built, Threat crushed
  {.name = "Iron Crown",
   .note = "The realm's banners fly at every frontier.",
   .cond = {.resourceIndex = R_SOLD, .resourceMin = 30, .resourceMax = 100,
            .trackerIndex = 4, .trackerMin = 4,
            .repIndex = 1, .repMin = 3,
            .requireFlags = F_BARRACKS | F_WALLS, .forbidFlags = 0,
            .requiresVictory = true}},

  // 4. People's Charter — laws passed, morale high, advisor trusted
  {.name = "People's Charter",
   .note = "Common folk cheer the charter; a new age of rights.",
   .cond = {.resourceIndex = R_MOR, .resourceMin = 70, .resourceMax = 100,
            .trackerIndex = 3, .trackerMin = 4,
            .repIndex = 0, .repMin = 3,
            .requireFlags = F_EDICT | F_ADVISORS, .forbidFlags = 0,
            .requiresVictory = true}},

  // 5. Golden Age — broad prosperity (food, gold, stability all healthy)
  {.name = "Golden Age",
   .note = "Abundance and peace: the dynasty enters legend.",
   .cond = {.resourceIndex = R_STAB, .resourceMin = 70, .resourceMax = 100,
            .trackerIndex = 2, .trackerMin = 3,
            .repIndex = -1, .repMin = 0,
            .requireFlags = F_GRANARY, .forbidFlags = 0,
            .requiresVictory = true}},

  // 6. Catch-all victory
  {.name = "Steady Reign",
   .note = "The kingdom endures — sturdy, if unremarkable.",
   .cond = {.resourceIndex = -1, .resourceMin = 0, .resourceMax = 0,
            .trackerIndex = -1, .trackerMin = 0,
            .repIndex = -1, .repMin = 0,
            .requireFlags = 0, .forbidFlags = 0,
            .requiresVictory = true}},

  // 7. Exile — loss ending
  {.name = "Exile",
   .note = "The throne falls. You flee with the court jewels.",
   .cond = {.resourceIndex = -1, .resourceMin = 0, .resourceMax = 0,
            .trackerIndex = -1, .trackerMin = 0,
            .repIndex = -1, .repMin = 0,
            .requireFlags = 0, .forbidFlags = 0,
            .requiresVictory = false}},
};

// ── Tracker labels ────────────────────────────────────────────────────────────
static const char* const TRACKER_LABELS[] = {
  "Seasons", "Workers", "Buildings", "Laws",
  "Battles", "Advisors", "Crises",   nullptr,
};

// ── Game definition ───────────────────────────────────────────────────────────
static const CGC::DeepGameDefinition DEF = {
  .title         = "Pocket Kingdom",
  .slug          = "pocket-kingdom-manager",
  .subtitle      = "Seasonal kingdom ledger",
  .saveMagic     = "PKM2",
  .resources     = RESOURCES,
  .resourceCount = sizeof(RESOURCES) / sizeof(RESOURCES[0]),
  .screens       = SCREENS,
  .screenCount   = sizeof(SCREENS) / sizeof(SCREENS[0]),
  .events        = EVENTS,
  .eventCount    = sizeof(EVENTS) / sizeof(EVENTS[0]),
  .endings       = ENDINGS,
  .endingCount   = sizeof(ENDINGS) / sizeof(ENDINGS[0]),
  .targetProgress    = 160,
  .accentColor       = 0xFFE0,  // preserved from original
  .warningColor      = 0xFD20,  // preserved from original
  .trackerLabels     = TRACKER_LABELS,
  .trackerLabelCount = 8,
  .objective         = "Stabilize the realm before it collapses.",
  .primaryTracker    = 0,       // Seasons is the headline
};

const CGC::DeepGameDefinition& gameDefinition() {
  return DEF;
}
