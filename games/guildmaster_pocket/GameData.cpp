#include "GameData.h"

namespace CGC = CardputerGameCore;

// Resource indices — keep in sync with RESOURCES array order.
enum {
  R_GOLD,    // 0  primary currency; negative primaryDelta → bankruptcy ends run
  R_FAME,    // 1  guild reputation; gates tiers and unlock events
  R_STRESS,  // 2  roster morale; high is bad (inverted); wears heroes down
  R_WOUNDS,  // 3  injury burden; high is bad (inverted); roster attrition
  R_RANK,    // 4  guild rank tier; gates boss contracts
  R_TRUST,   // 5  client and town confidence; affects contract flow
  R_ROSTER,  // 6  count of healthy active heroes
  R_RENOWN,  // 7  long-term prestige score tracked for endings
};

// Story flags. Screens set these cleanly; events advance the arc.
enum : uint32_t {
  F_BOARD_CHECKED = 1u << 0,  // quest board reviewed; enables PARTY
  F_PARTY_READY   = 1u << 1,  // party assembled; enables QUEST launch
  F_QUEST_DONE    = 1u << 2,  // at least one quest completed
  F_INFIRMARY     = 1u << 3,  // infirmary wing built; enables INFIRMARY screen
  F_HALL_UPGRADED = 1u << 4,  // hall has one or more structural upgrades
  F_BOSS_TIER     = 1u << 5,  // rank and library intel reached boss level
  F_RIVAL_THREAT  = 1u << 6,  // rival guild is actively pressuring the hall
  F_BOSS_DONE     = 1u << 7,  // a boss contract has been successfully cleared
};

// ---------------------------------------------------------------------------
// Resources (7 thematic + 1 prestige counter, 8 total)
// ---------------------------------------------------------------------------
static const CGC::ResourceDef RESOURCES[] = {
  // label    start  min   max  prim safe  warn  bad  inv
  {"Gold",      65,    0,  999,  -1,   3,   18,   6,  false}, // loss trigger on 0
  {"Fame",      10,    0,  999,   0,   0,   -1,  -1,  false}, // tier unlock counter
  {"Stress",    12,    0,  100,   0,   0,   55,  80,  true},  // high is bad
  {"Wounds",     4,    0,   50,   0,   0,   28,  42,  true},  // high is bad
  {"Rank",       1,    0,    9,   0,   0,   -1,  -1,  false}, // quest tier gate
  {"Trust",     42,    0,  100,   0,   1,   20,   8,  false}, // client confidence
  {"Roster",     6,    1,   18,   0,   0,    2,   1,  false}, // healthy heroes
  {"Renown",     0,    0,   99,   0,   0,   -1,  -1,  false}, // prestige counter
};

// ---------------------------------------------------------------------------
// Screen row tables
// ---------------------------------------------------------------------------

// HALL — weekly guild management; free; builds Trust and recovers Gold trickle
static const CGC::NamedValue ROWS_HALL[] = {
  {"Morning Brief",    "Review contracts, ledger, and client mail."},
  {"Walk the Barracks","Check each bunk; morale and readiness."},
  {"Client Parlour",   "Hear town requests; cement goodwill."},
  {"Merchant Escort",  "Quick ward run; gold, easy stress cost."},
  {"Rest the Hall",    "Light duties; let stress bleed off slowly."},
  {"Bard Night",       "Music in the common room; stress drops."},
  {"Audit the Ledger", "Review gold vs. obligations carefully."},
  {"Post Notice",      "Advertise; attract new recruit interest."},
};

// BOARD — review quest contracts; sets F_BOARD_CHECKED; small Fame tick
static const CGC::NamedValue ROWS_BOARD[] = {
  {"Rat Cellar",     "Minor pest job; low risk, coin for supplies."},
  {"Moss Road",      "Escort north along the flooded passage."},
  {"Coin Vault",     "Investigate a robbed merchant strongbox."},
  {"Moon Mine",      "Clear haunted tunnels in the old moon mine."},
  {"Ash Keep",       "Recon a keep razed by the Ash Legion."},
  {"Frost Gate",     "Courier run through the frozen mountain pass."},
  {"Signal Ruin",    "Survey a ruin that jams all compasses."},
  {"Baron Road",     "Bodyguard detail for a skittish noble lord."},
};

// PARTY — assemble by class lane; sets F_PARTY_READY; requires Board reviewed
static const CGC::NamedValue ROWS_PARTY[] = {
  {"Fighter Lane",    "Lead fighter; front-line combat coverage."},
  {"Scout Lane",      "Point scout; traps and ambush detection."},
  {"Medic Lane",      "Field medic; treat wounds on the march."},
  {"Mage Lane",       "Mage support; ruins and arcane threats."},
  {"Guard Lane",      "Heavy guard; escort and ward protection."},
  {"Bard Lane",       "Bard cover; stress control and parley."},
  {"Engineer Lane",   "Engineer slot; doors, devices, and traps."},
  {"Mystic Lane",     "Mystic read; weird threats and lost lore."},
};

// QUEST — launch active contract; big resource swing; gated on Board + Party
static const CGC::NamedValue ROWS_QUEST[] = {
  {"Launch Contract", "Send the party; burns supplies for the road."},
  {"Strike Fast",     "Rush the site; less travel, higher combat risk."},
  {"Scout Approach",  "Route-scout first; lowers trap danger."},
  {"Negotiate Entry", "Talk before steel; saves sword-stress cost."},
  {"Night Run",       "Dark travel; rivals notice nothing."},
  {"Full Kit Out",    "Max supply load; gold cost, fewer wounds."},
};

// INFIRMARY — treat wounds; costs Gold; locked until infirmary is built
static const CGC::NamedValue ROWS_INFIRMARY[] = {
  {"Bind Wounds",      "Basic field care; clears light cuts fast."},
  {"Set Bones",        "Proper splinting for fractures and sprains."},
  {"Rest Ward",        "Two-day bed rest; deep stress recovery."},
  {"Herb Poultice",    "Old forest remedies; cheap and reliable."},
  {"Alchemist Draught","Expensive brew; rapid full-wound clearance."},
  {"Counsel Session",  "Stress therapy; mind before sword again."},
};

// UPGRADE — build hall wings; costs Gold; grants Rank and Fame ticks
static const CGC::NamedValue ROWS_UPGRADE[] = {
  {"Stone Infirmary",  "Ward wing; unlocks the infirmary screen."},
  {"Expand Barracks",  "More bunks; raise healthy-hero cap."},
  {"Stock the Library","Study hall; builds boss-contract intel."},
  {"Build Blacksmith", "Forge; better gear, fewer wound costs."},
  {"Chart Room",       "Surveys and maps; unlock new quest routes."},
  {"Trophy Wall",      "Display victories; fame and trust bonus."},
  {"Mess Hall Annex",  "Better rations; weekly stress recovery."},
};

// BOSS — high-stakes elite contracts; gated on Quest history + Hall upgrade
static const CGC::NamedValue ROWS_BOSS[] = {
  {"Void Drake",     "Hunt the city-ruin drake. Full party req."},
  {"Ash Lord",       "Confront the Ash Legion field commander."},
  {"Curse Broker",   "Capture the hex-merchant alive."},
  {"Shadow Cartel",  "Raid the shadow guild's sealed vault."},
  {"Relic Warden",   "Recover the sealed relic before rivals do."},
};

// ---------------------------------------------------------------------------
// Screens (7 total)
// ---------------------------------------------------------------------------
static const CGC::DeepScreenDef SCREENS[] = {

  // HALL: free weekly review; no flags required; small gold trickle + trust
  {.title       = "HALL",
   .verb        = "Manage",
   .rows        = ROWS_HALL,
   .rowCount    = 8,
   .resourceDeltas = {3, 0, -2, 0, 0, 2, 0, 0},
   .progressDelta  = 1,
   .trackerIndex   = 0,
   .flagMask       = 0,
   .costResource   = -1, .costAmount = 0, .requireFlags = 0,
   .trackerDelta   = 1,
   .objective      = "Run the hall; keep Trust and Gold up"},

  // BOARD: review contracts; sets F_BOARD_CHECKED; free; small Fame bump
  {.title       = "BOARD",
   .verb        = "Review",
   .rows        = ROWS_BOARD,
   .rowCount    = 8,
   .resourceDeltas = {0, 1, 0, 0, 0, 1, 0, 0},
   .progressDelta  = 1,
   .trackerIndex   = 1,
   .flagMask       = F_BOARD_CHECKED,
   .costResource   = -1, .costAmount = 0, .requireFlags = 0,
   .trackerDelta   = 1,
   .objective      = "Pick a contract from the board"},

  // PARTY: assemble class lanes; sets F_PARTY_READY; needs Board reviewed
  {.title       = "PARTY",
   .verb        = "Assemble",
   .rows        = ROWS_PARTY,
   .rowCount    = 8,
   .resourceDeltas = {0, 0, 1, 0, 0, 0, 0, 0},
   .progressDelta  = 1,
   .trackerIndex   = 2,
   .flagMask       = F_PARTY_READY,
   .costResource   = -1, .costAmount = 0, .requireFlags = F_BOARD_CHECKED,
   .trackerDelta   = 1,
   .objective      = "Cover class lanes (needs Board)"},

  // QUEST: launch contract; big swing; costs 10 Gold; needs Board + Party
  {.title       = "QUEST",
   .verb        = "Launch",
   .rows        = ROWS_QUEST,
   .rowCount    = 6,
   .resourceDeltas = {8, 4, 3, 3, 0, 1, 0, 1},
   .progressDelta  = 8,
   .trackerIndex   = 3,
   .flagMask       = F_QUEST_DONE,
   .costResource   = R_GOLD, .costAmount = 10,
   .requireFlags   = F_BOARD_CHECKED | F_PARTY_READY,
   .trackerDelta   = 1,
   .objective      = "Launch quest (Board+Party, -10 Gold)"},

  // INFIRMARY: treat wounds + stress; 8 Gold; locked until wing is built
  {.title       = "INFIRMARY",
   .verb        = "Treat",
   .rows        = ROWS_INFIRMARY,
   .rowCount    = 6,
   .resourceDeltas = {0, 0, -4, -5, 0, 1, 0, 0},
   .progressDelta  = 1,
   .trackerIndex   = 4,
   .flagMask       = F_INFIRMARY,
   .costResource   = R_GOLD, .costAmount = 8,
   .requireFlags   = F_INFIRMARY,
   .trackerDelta   = 1,
   .objective      = "Heal the roster (Infirmary built, -8 Gold)"},

  // UPGRADE: build hall wings; 18 Gold; grants Rank + Renown
  {.title       = "UPGRADE",
   .verb        = "Build",
   .rows        = ROWS_UPGRADE,
   .rowCount    = 7,
   .resourceDeltas = {0, 1, 0, 0, 1, 2, 1, 1},
   .progressDelta  = 2,
   .trackerIndex   = 5,
   .flagMask       = F_HALL_UPGRADED,
   .costResource   = R_GOLD, .costAmount = 18,
   .requireFlags   = 0,
   .trackerDelta   = 1,
   .objective      = "Upgrade the hall (-18 Gold)"},

  // BOSS: elite contracts; 15 Gold; needs Quest history + Hall upgrade
  {.title       = "BOSS",
   .verb        = "Contract",
   .rows        = ROWS_BOSS,
   .rowCount    = 5,
   .resourceDeltas = {22, 8, 4, 3, 1, 2, 0, 3},
   .progressDelta  = 20,
   .trackerIndex   = 6,
   .flagMask       = F_BOSS_DONE,
   .costResource   = R_GOLD, .costAmount = 15,
   .requireFlags   = F_QUEST_DONE | F_HALL_UPGRADED,
   .trackerDelta   = 1,
   .objective      = "Boss contract (Quest+Upgrade, -15 Gold)"},
};

// ---------------------------------------------------------------------------
// Events (16 total — common fillers, mid-arc beats, endgame escalation)
// ---------------------------------------------------------------------------
static const CGC::EventDef EVENTS[] = {

  // --- Common filler (always eligible, low drama) ---

  {.name = "Quiet week",
   .note = "Light jobs trickle in; hall stays calm.",
   .resourceDeltas = {3, 0, -1, 0, 0, 1, 0, 0},
   .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
   .repIndex = -1, .repDelta = 0, .weight = 4,
   .requireFlags = 0, .forbidFlags = 0, .tone = CGC::TONE_NEUTRAL},

  {.name = "Town donation",
   .note = "A grateful merchant tips the front desk.",
   .resourceDeltas = {4, 0, 0, 0, 0, 2, 0, 0},
   .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
   .repIndex = 0, .repDelta = 2, .weight = 3,
   .requireFlags = 0, .forbidFlags = 0, .tone = CGC::TONE_GOOD},

  {.name = "Mess-hall argument",
   .note = "Two heroes squabble over the last ration.",
   .resourceDeltas = {0, 0, 4, 0, 0, -1, 0, 0},
   .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
   .repIndex = -1, .repDelta = 0, .weight = 3,
   .requireFlags = 0, .forbidFlags = 0, .tone = CGC::TONE_BAD},

  {.name = "Bandit ambush",
   .note = "Road bandits catch the party mid-march.",
   .resourceDeltas = {-5, 0, 3, 4, 0, -1, 0, 0},
   .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
   .repIndex = -1, .repDelta = 0, .weight = 2,
   .requireFlags = F_PARTY_READY, .forbidFlags = 0, .tone = CGC::TONE_BAD},

  {.name = "Scout reads the path",
   .note = "Point scout spots a pit trap in time.",
   .resourceDeltas = {0, 0, 1, -3, 0, 1, 0, 0},
   .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
   .repIndex = -1, .repDelta = 0, .weight = 2,
   .requireFlags = F_PARTY_READY, .forbidFlags = 0, .tone = CGC::TONE_GOOD},

  {.name = "Supply windfall",
   .note = "Scavenged loot sells well at the market.",
   .resourceDeltas = {6, 0, 0, 0, 0, 0, 0, 0},
   .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
   .repIndex = -1, .repDelta = 0, .weight = 2,
   .requireFlags = 0, .forbidFlags = 0, .tone = CGC::TONE_GOOD},

  // --- Mid-arc events (require at least one quest) ---

  {.name = "Grateful client",
   .note = "A satisfied patron sends a bonus purse.",
   .resourceDeltas = {5, 2, 0, 0, 0, 2, 0, 0},
   .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
   .repIndex = 0, .repDelta = 2, .weight = 3,
   .requireFlags = F_QUEST_DONE, .forbidFlags = 0, .tone = CGC::TONE_GOOD},

  {.name = "Hero takes a bad hit",
   .note = "A fighter limps home with a broken rib.",
   .resourceDeltas = {0, 0, 4, 5, 0, 0, -1, 0},
   .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
   .repIndex = -1, .repDelta = 0, .weight = 3,
   .requireFlags = F_QUEST_DONE, .forbidFlags = 0, .tone = CGC::TONE_BAD},

  {.name = "Rival undercut",
   .note = "Red Ledger Guild steals a board contract.",
   .resourceDeltas = {-3, -1, 2, 0, 0, -2, 0, 0},
   .setFlags = F_RIVAL_THREAT, .trackerIndex = -1, .trackerDelta = 0,
   .repIndex = 2, .repDelta = -2, .weight = 2,
   .requireFlags = F_QUEST_DONE, .forbidFlags = F_RIVAL_THREAT, .tone = CGC::TONE_BAD},

  {.name = "Veteran steps up",
   .note = "A loyal hero takes point without being asked.",
   .resourceDeltas = {0, 2, -3, -2, 0, 1, 0, 1},
   .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
   .repIndex = 1, .repDelta = 2, .weight = 2,
   .requireFlags = F_QUEST_DONE, .forbidFlags = 0, .tone = CGC::TONE_GOOD},

  {.name = "Stress break",
   .note = "Overworked scout snaps and refuses duty.",
   .resourceDeltas = {0, 0, 5, 0, 0, -2, -1, 0},
   .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
   .repIndex = -1, .repDelta = 0, .weight = 2,
   .requireFlags = F_QUEST_DONE, .forbidFlags = F_INFIRMARY, .tone = CGC::TONE_BAD},

  // --- Hall-upgrade events ---

  {.name = "Forge dividend",
   .note = "Blacksmith turns salvage into coin and kit.",
   .resourceDeltas = {6, 1, 0, -1, 0, 1, 0, 1},
   .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
   .repIndex = 0, .repDelta = 1, .weight = 2,
   .requireFlags = F_HALL_UPGRADED, .forbidFlags = 0, .tone = CGC::TONE_GOOD},

  {.name = "Library intelligence",
   .note = "A chart reveals the boss-contract approach.",
   .resourceDeltas = {0, 2, -2, 0, 1, 1, 0, 1},
   .setFlags = F_BOSS_TIER, .trackerIndex = -1, .trackerDelta = 0,
   .repIndex = -1, .repDelta = 0, .weight = 2,
   .requireFlags = F_HALL_UPGRADED, .forbidFlags = F_BOSS_TIER, .tone = CGC::TONE_GOOD},

  // --- Rival-pressure events (require active rival threat) ---

  {.name = "Rival public duel",
   .note = "Moon Seat Guild demands a street challenge.",
   .resourceDeltas = {0, 0, 4, 2, 0, -2, 0, 0},
   .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
   .repIndex = 2, .repDelta = -1, .weight = 2,
   .requireFlags = F_RIVAL_THREAT, .forbidFlags = F_BOSS_DONE, .tone = CGC::TONE_BAD},

  // --- Boss-tier events ---

  {.name = "Lord's commission",
   .note = "A city lord summons your best party now.",
   .resourceDeltas = {0, 3, 2, 0, 0, 2, 0, 2},
   .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
   .repIndex = 0, .repDelta = 2, .weight = 3,
   .requireFlags = F_BOSS_TIER, .forbidFlags = F_BOSS_DONE, .tone = CGC::TONE_GOOD},

  // --- Endgame / post-boss ---

  {.name = "Rival surrenders",
   .note = "Ivory Hall asks to merge under your banner.",
   .resourceDeltas = {8, 4, -3, -2, 1, 3, 1, 2},
   .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
   .repIndex = 2, .repDelta = 4, .weight = 2,
   .requireFlags = F_BOSS_DONE, .forbidFlags = 0, .tone = CGC::TONE_GOOD},
};

// ---------------------------------------------------------------------------
// Endings (6 — most-specific first; catch-all victory last, then loss)
// ---------------------------------------------------------------------------
static const CGC::EndingDef ENDINGS[] = {

  // 1. Legendary guild: boss done, high fame, hall upgraded — beloved name
  {.name = "Beloved Guild",
   .note = "Songs fill every tavern. Your hall stands for a generation.",
   .cond = {.resourceIndex = R_FAME, .resourceMin = 40, .resourceMax = 999,
            .trackerIndex = -1, .trackerMin = 0, .repIndex = -1, .repMin = 0,
            .requireFlags = F_BOSS_DONE | F_HALL_UPGRADED,
            .forbidFlags  = 0, .requiresVictory = true}},

  // 2. Iron Mercenaries: boss done but wounds and stress are high
  {.name = "Iron Mercenaries",
   .note = "Fame, coin, and a roster of walking scars. Results matter.",
   .cond = {.resourceIndex = R_WOUNDS, .resourceMin = 25, .resourceMax = 50,
            .trackerIndex = -1, .trackerMin = 0, .repIndex = -1, .repMin = 0,
            .requireFlags = F_BOSS_DONE,
            .forbidFlags  = 0, .requiresVictory = true}},

  // 3. Scholar Guild: hall upgraded, high renown, no boss needed
  {.name = "Scholar Guild",
   .note = "Your maps and records outlast every blade. Rare praise.",
   .cond = {.resourceIndex = R_RENOWN, .resourceMin = 20, .resourceMax = 99,
            .trackerIndex = -1, .trackerMin = 0, .repIndex = -1, .repMin = 0,
            .requireFlags = F_HALL_UPGRADED | F_QUEST_DONE,
            .forbidFlags  = F_BOSS_DONE, .requiresVictory = true}},

  // 4. Town Protector: high trust, quests done, no boss — quiet local legend
  {.name = "Town Protector",
   .note = "The town sleeps soundly. A modest hall the people trust.",
   .cond = {.resourceIndex = R_TRUST, .resourceMin = 65, .resourceMax = 100,
            .trackerIndex = -1, .trackerMin = 0, .repIndex = -1, .repMin = 0,
            .requireFlags = F_QUEST_DONE,
            .forbidFlags  = F_BOSS_DONE, .requiresVictory = true}},

  // 5. Catch-all victory — workmanlike but standing
  {.name = "Steady Hall",
   .note = "Not legendary, but the doors stay open. A working guild.",
   .cond = {.resourceIndex = -1, .resourceMin = 0, .resourceMax = 0,
            .trackerIndex = -1, .trackerMin = 0, .repIndex = -1, .repMin = 0,
            .requireFlags = 0, .forbidFlags = 0, .requiresVictory = true}},

  // 6. Bankrupt loss — gold at zero, heroes scatter
  {.name = "Empty Coffers",
   .note = "The gold is gone. Heroes scatter; the board goes dark.",
   .cond = {.resourceIndex = -1, .resourceMin = 0, .resourceMax = 0,
            .trackerIndex = -1, .trackerMin = 0, .repIndex = -1, .repMin = 0,
            .requireFlags = 0, .forbidFlags = 0, .requiresVictory = false}},
};

// ---------------------------------------------------------------------------
// Tracker labels (index matches trackerIndex in screens/events)
// ---------------------------------------------------------------------------
static const char* const TRACKER_LABELS[] = {
  "Hall",       // 0 — weekly management turns
  "Contracts",  // 1 — board reviews
  "Parties",    // 2 — party assemblies
  "Quests",     // 3 — quests launched  ← primaryTracker shown on hub
  "Healed",     // 4 — infirmary treatments
  "Upgrades",   // 5 — hall builds
  "Bosses",     // 6 — boss contracts cleared
  nullptr,
};

// ---------------------------------------------------------------------------
// Game definition
// ---------------------------------------------------------------------------
static const CGC::DeepGameDefinition DEF = {
  .title             = "Guildmaster",
  .slug              = "guildmaster-pocket",
  .subtitle          = "Tiny quest desk",
  .saveMagic         = "GMP2",
  .resources         = RESOURCES,
  .resourceCount     = sizeof(RESOURCES) / sizeof(RESOURCES[0]),
  .screens           = SCREENS,
  .screenCount       = sizeof(SCREENS) / sizeof(SCREENS[0]),
  .events            = EVENTS,
  .eventCount        = sizeof(EVENTS) / sizeof(EVENTS[0]),
  .endings           = ENDINGS,
  .endingCount       = sizeof(ENDINGS) / sizeof(ENDINGS[0]),
  .targetProgress    = 175,
  .accentColor       = 0xA145,
  .warningColor      = 0xFD20,
  .trackerLabels     = TRACKER_LABELS,
  .trackerLabelCount = 8,
  .objective         = "Build fame; clear the boss contract.",
  .primaryTracker    = 3,
};

const CGC::DeepGameDefinition& gameDefinition() {
  return DEF;
}
