#include "GameData.h"

namespace CGC = CardputerGameCore;

// ── Resource indices ─────────────────────────────────────────────────────────
enum {
  R_CRED,   // 0 currency
  R_HEAT,   // 1 campaign pressure (inverted: high is bad)
  R_TRACE,  // 2 per-job burn     (inverted: high is bad)
  R_DECK,   // 3 deck HP          (primaryDelta<0 → loss on 0)
  R_BATT,   // 4 action buffer
  R_FILES,  // 5 conspiracy counter
  R_REP,    // 6 street reputation
  R_GHOST,  // 7 ghost-signal pressure
};

// ── Story flags ───────────────────────────────────────────────────────────────
enum : uint32_t {
  F_JOB_TAKEN     = 1u << 0,  // a job is active on the board
  F_FACTION_STR   = 1u << 1,  // committed to Streetline
  F_FACTION_NEO   = 1u << 2,  // committed to Neon Crown
  F_FACTION_GHO   = 1u << 3,  // committed to Ghost Relay
  F_FIRST_FILE    = 1u << 4,  // first ghost echo recovered
  F_BLACKBOX      = 1u << 5,  // Blackbox Drive installed
  F_RELAY_CONTACT = 1u << 6,  // Ghost Relay handler made contact
  F_CONSPIRACY    = 1u << 7,  // relay pact / conspiracy chain unlocked
  F_BURNED        = 1u << 8,  // faction burned the player
  F_COLD_CPU      = 1u << 9,  // Cold CPU upgrade installed
};

// ── Resources ─────────────────────────────────────────────────────────────────
//   label   start  min   max  prim safe  warn  bad   inv
static const CGC::ResourceDef RESOURCES[] = {
  {"Cred",   30,    0,   999,   0,   0,   10,    3,  false},  // currency
  {"Heat",    4,    0,   100,   1,  -3,   55,   80,  true },  // pressure high=bad
  {"Trace",   0,    0,   100,   0,   0,   50,   75,  true },  // per-job burn high=bad
  {"Deck",   90,    0,   100,  -1,   5,   30,   12,  false},  // survival resource
  {"Batt",   80,    0,   100,   0,   6,   25,   10,  false},  // action buffer
  {"Files",   0,    0,    99,   0,   0,   -1,   -1,  false},  // conspiracy counter
  {"Rep",    20,    0,    99,   0,   1,   10,    4,  false},  // street standing
  {"Ghost",   0,    0,    99,   0,   0,   -1,   -1,  false},  // signal pressure
};

// ── Screen row tables ─────────────────────────────────────────────────────────

// SAFEHOUSE — lay low, review status, plan next move
static const CGC::NamedValue ROWS_SAFEHOUSE[] = {
  {"Lay Low",        "Rest and reduce campaign heat."},
  {"Patch Deck",     "Swap cells; shore up deck plating."},
  {"Check Scanner",  "Sweep local EM for active watchers."},
  {"Fence Contact",  "Sell minor loot; collect owed cred."},
  {"Clinic Visit",   "Get patched; costs cred, saves deck."},
  {"Review Files",   "Re-read recovered intel carefully."},
  {"Charge Stack",   "Run trickle-charge on battery cells."},
  {"Plan Route",     "Map tomorrow's node approach."},
};

// BOARD — pick a faction job (sets F_JOB_TAKEN + faction flag)
static const CGC::NamedValue ROWS_BOARD[] = {
  {"Rat Trap Receipt",  "SL: scan a shop till node. Easy."},
  {"Laundromat Ledger", "SL: decrypt a cash-wash ledger."},
  {"Clinic Queue Patch","SL: adjust a clinic wait-list."},
  {"Servo Pawn Audit",  "SL: inject a pawn-shop database."},
  {"Crown Demo Cache",  "NC: high-pay corp asset grab."},
  {"Exec Calendar",     "NC: spoof a boardroom schedule."},
  {"Private Weather",   "NC: tap a closed met-feed tower."},
  {"Vault Without Walls","NC: hard lock, big payout."},
  {"Relay 17 Echo",     "GR: first ghost-signal trace."},
  {"Dead Mall Repeater","GR: old repeater holds a secret."},
  {"Underpass Choir",   "GR: signal choir in the pipes."},
  {"The Old Signal",    "GR: final ghost node. Story gate."},
};

// NODE — run simulated commands (gated: needs active job)
static const CGC::NamedValue ROWS_NODE[] = {
  {"scan",        "Reveal one hidden stat. Low trace."},
  {"decrypt",     "Reduce lock or pull file if low."},
  {"spoof",       "Drop trace; bypass shield if chipped."},
  {"inject",      "Extract data. Raises trace + alarm."},
  {"trace",       "Read how close they are to a burn."},
  {"disconnect",  "Bail out. Partial pay; no heat spike."},
};

// TOOLS — fake commands that cost Battery (gated: needs job taken)
static const CGC::NamedValue ROWS_TOOLS[] = {
  {"Cold Flush",    "Dump trace log; kill active trace."},
  {"Signal Scrub",  "Spoof MAC chain; shed some heat."},
  {"Shield Probe",  "Soft scan node perimeter. +Data."},
  {"Blackbox Pull", "Attempt file extraction from node."},
  {"Alarm Spoof",   "Feed false OK to alarm watchdog."},
  {"Hard Eject",    "Cut all links; safe but costs batt."},
};

// MARKET — deck upgrades (costs Cred)
static const CGC::NamedValue ROWS_MARKET[] = {
  {"Cold CPU",      "Cuts trace gain each node op."},
  {"Spoof Chip",    "Improves spoof; unlocks shield bypass."},
  {"Battery Stack", "Adds buffer; less disconnect damage."},
  {"Blackbox Drive","Stores files; unlocks conspiracy arc."},
  {"Deck Armor",    "Hardens plating vs trace-burn dmg."},
  {"Signal Filter", "Dampens ghost bleed; lowers Ghost."},
};

// FILES — review recovered intel (gated: needs first file)
static const CGC::NamedValue ROWS_FILES[] = {
  {"Ghost Echo",    "Repeater 17 signal source — partial."},
  {"Crown Debt",    "Neon Crown owed debts, names inside."},
  {"Street Map",    "Old node map from a burned runner."},
  {"Relay Pact",    "Terms from the original relay deal."},
  {"Clinic Bill",   "Forged clinic invoices. Someone's dirty."},
  {"Weather Lie",   "Met-feed was spoofed. By who?"},
  {"Mall Tape",     "Dead-mall cam feed. Ghost signal clear."},
  {"Servo Key",     "Pawn-node master key. Reusable."},
  {"Old Signal",    "Raw ghost waveform. Pre-infrastructure."},
  {"Final Chain",   "All pieces. The source is clear now."},
};

// CONSPIRACY — final ghost-chain screen (gated: Files>=4 + Blackbox)
static const CGC::NamedValue ROWS_CONSPIRACY[] = {
  {"Trace Origin",    "Follow the waveform to its root node."},
  {"Broadcast Reply", "Transmit back on the ghost band."},
  {"Relay Handshake", "Complete the pact with Ghost Relay."},
  {"Publish Dossier", "Leak the full conspiracy to the net."},
  {"Burn Evidence",   "Destroy the chain. Stay safe."},
  {"Sell to Crown",   "Hand intel to Neon Crown for cred."},
};

// ── Screens ───────────────────────────────────────────────────────────────────
static const CGC::DeepScreenDef SCREENS[] = {
  // SAFEHOUSE: free rest. Reduces heat & trace, recovers batt. Sets no flag.
  {.title = "SAFEHOUSE",
   .verb = "Rest",
   .rows = ROWS_SAFEHOUSE,
   .rowCount = 8,
   .resourceDeltas = {2, -3, -6, 1, 5, 0, 1, 0},
   .progressDelta = 1,
   .trackerIndex = 0,
   .flagMask = 0,
   .costResource = -1,
   .costAmount = 0,
   .requireFlags = 0,
   .trackerDelta = 1,
   .objective = "Rest, plan, and cut heat"},

  // BOARD: take a faction job. Sets F_JOB_TAKEN. Costs nothing; earns cred on
  // completion. Deck + trace pressure represent the difficulty of committing.
  {.title = "BOARD",
   .verb = "Take",
   .rows = ROWS_BOARD,
   .rowCount = 12,
   .resourceDeltas = {0, 1, 3, -1, -2, 0, 0, 0},
   .progressDelta = 2,
   .trackerIndex = 1,
   .flagMask = F_JOB_TAKEN,
   .costResource = -1,
   .costAmount = 0,
   .requireFlags = 0,
   .trackerDelta = 1,
   .objective = "Pick a faction job"},

  // NODE: run simulated commands. Costs Battery. Gated on active job.
  // Big cred payout; pushes trace/heat up; Deck wear risk.
  {.title = "NODE",
   .verb = "Run",
   .rows = ROWS_NODE,
   .rowCount = 6,
   .resourceDeltas = {8, 3, 5, -2, -8, 0, 2, 1},
   .progressDelta = 5,
   .trackerIndex = 2,
   .flagMask = F_FACTION_STR,
   .costResource = R_BATT,
   .costAmount = 10,
   .requireFlags = F_JOB_TAKEN,
   .trackerDelta = 1,
   .objective = "Run node cmds (-10 Batt, +Heat)"},

  // TOOLS: fake utility commands. Costs Battery. Lower trace/heat.
  {.title = "TOOLS",
   .verb = "Use",
   .rows = ROWS_TOOLS,
   .rowCount = 6,
   .resourceDeltas = {0, -2, -8, 0, -5, 0, 0, 0},
   .progressDelta = 1,
   .trackerIndex = 3,
   .flagMask = 0,
   .costResource = R_BATT,
   .costAmount = 6,
   .requireFlags = F_JOB_TAKEN,
   .trackerDelta = 1,
   .objective = "Use tools to cut trace (-6 Batt)"},

  // MARKET: buy deck upgrades. Costs Cred. Sets F_BLACKBOX via event chain.
  {.title = "MARKET",
   .verb = "Buy",
   .rows = ROWS_MARKET,
   .rowCount = 6,
   .resourceDeltas = {0, 0, 0, 3, 4, 0, 0, -1},
   .progressDelta = 2,
   .trackerIndex = 4,
   .flagMask = F_COLD_CPU,
   .costResource = R_CRED,
   .costAmount = 18,
   .requireFlags = 0,
   .trackerDelta = 1,
   .objective = "Buy deck modules (-18 Cred)"},

  // FILES: review recovered intel. Gated on first file found.
  {.title = "FILES",
   .verb = "Read",
   .rows = ROWS_FILES,
   .rowCount = 10,
   .resourceDeltas = {0, -1, -2, 0, 0, 2, 1, 2},
   .progressDelta = 3,
   .trackerIndex = 5,
   .flagMask = F_RELAY_CONTACT,
   .costResource = -1,
   .costAmount = 0,
   .requireFlags = F_FIRST_FILE,
   .trackerDelta = 1,
   .objective = "Review intel (need first file)"},

  // CONSPIRACY: final ghost-chain (gated: relay contact + blackbox installed)
  {.title = "CONSPIRACY",
   .verb = "Pursue",
   .rows = ROWS_CONSPIRACY,
   .rowCount = 6,
   .resourceDeltas = {4, 4, 6, -3, -6, 1, 0, 4},
   .progressDelta = 8,
   .trackerIndex = 6,
   .flagMask = F_CONSPIRACY,
   .costResource = R_BATT,
   .costAmount = 12,
   .requireFlags = F_RELAY_CONTACT | F_BLACKBOX,
   .trackerDelta = 1,
   .objective = "Pursue the ghost chain finale"},
};

// ── Events ────────────────────────────────────────────────────────────────────
// resourceDeltas order: Cred, Heat, Trace, Deck, Batt, Files, Rep, Ghost
static const CGC::EventDef EVENTS[] = {

  // ── Common neutral / good fillers (always eligible) ──────────────────────

  {.name = "Clean cashout",
   .note = "Node logs wiped. Full pay, zero blowback.",
   .resourceDeltas = {6, -2, -4, 0, 0, 0, 1, 0},
   .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
   .repIndex = 0, .repDelta = 2,
   .weight = 4, .requireFlags = 0, .forbidFlags = 0,
   .tone = CGC::TONE_GOOD},

  {.name = "Battery flicker",
   .note = "A cell shorts. Batt hits the floor fast.",
   .resourceDeltas = {0, 0, 0, 0, -10, 0, 0, 0},
   .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
   .repIndex = -1, .repDelta = 0,
   .weight = 2, .requireFlags = 0, .forbidFlags = 0,
   .tone = CGC::TONE_BAD},

  {.name = "Quiet sector",
   .note = "No watchdogs. You breathe a little easier.",
   .resourceDeltas = {0, -2, -3, 1, 2, 0, 0, 0},
   .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
   .repIndex = -1, .repDelta = 0,
   .weight = 3, .requireFlags = 0, .forbidFlags = 0,
   .tone = CGC::TONE_NEUTRAL},

  {.name = "Trace spike",
   .note = "Alarm ping. Trace climbs hard — move fast.",
   .resourceDeltas = {0, 2, 8, -1, 0, 0, 0, 0},
   .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
   .repIndex = -1, .repDelta = 0,
   .weight = 3, .requireFlags = 0, .forbidFlags = 0,
   .tone = CGC::TONE_BAD},

  {.name = "Partial pull",
   .note = "File locked behind second layer. Half pay.",
   .resourceDeltas = {3, 1, 3, 0, -3, 0, -1, 0},
   .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
   .repIndex = -1, .repDelta = 0,
   .weight = 2, .requireFlags = 0, .forbidFlags = 0,
   .tone = CGC::TONE_NEUTRAL},

  {.name = "Deck overheat",
   .note = "Sustained ops burn a board trace. Deck -3.",
   .resourceDeltas = {0, 0, 0, -3, -4, 0, 0, 0},
   .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
   .repIndex = -1, .repDelta = 0,
   .weight = 2, .requireFlags = 0, .forbidFlags = 0,
   .tone = CGC::TONE_BAD},

  // ── Streetline faction events ─────────────────────────────────────────────

  {.name = "SL: handler bonus",
   .note = "Street fixer tips you for clean work. +Cred.",
   .resourceDeltas = {8, -1, 0, 0, 0, 0, 2, 0},
   .setFlags = F_FACTION_STR, .trackerIndex = 1, .trackerDelta = 1,
   .repIndex = 0, .repDelta = 3,
   .weight = 3, .requireFlags = F_JOB_TAKEN, .forbidFlags = F_BURNED,
   .tone = CGC::TONE_GOOD},

  {.name = "SL: client pulls job",
   .note = "Street client ghosts. No pay, trace lingers.",
   .resourceDeltas = {-4, 2, 4, 0, 0, 0, -2, 0},
   .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
   .repIndex = 0, .repDelta = -2,
   .weight = 2, .requireFlags = F_FACTION_STR, .forbidFlags = 0,
   .tone = CGC::TONE_BAD},

  // ── Neon Crown faction events ─────────────────────────────────────────────

  {.name = "NC: corp surplus",
   .note = "Crown broker slips extra cred for fast pull.",
   .resourceDeltas = {12, 0, 2, 0, 0, 0, 0, 0},
   .setFlags = F_FACTION_NEO, .trackerIndex = 1, .trackerDelta = 1,
   .repIndex = 1, .repDelta = 2,
   .weight = 3, .requireFlags = F_JOB_TAKEN, .forbidFlags = F_BURNED,
   .tone = CGC::TONE_GOOD},

  {.name = "NC: heat escalation",
   .note = "Crown job traced to your deck sig. Heat +5.",
   .resourceDeltas = {0, 5, 4, -2, 0, 0, -1, 0},
   .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
   .repIndex = 1, .repDelta = -1,
   .weight = 2, .requireFlags = F_FACTION_NEO, .forbidFlags = 0,
   .tone = CGC::TONE_BAD},

  {.name = "NC: burn notice",
   .note = "Crown sold your node log. You've been made.",
   .resourceDeltas = {-6, 8, 10, -3, 0, 0, -3, 0},
   .setFlags = F_BURNED, .trackerIndex = -1, .trackerDelta = 0,
   .repIndex = 1, .repDelta = -3,
   .weight = 1, .requireFlags = F_FACTION_NEO, .forbidFlags = F_BURNED,
   .tone = CGC::TONE_BAD},

  // ── Ghost Relay / conspiracy events ──────────────────────────────────────

  {.name = "Ghost echo",
   .note = "Phantom carrier on the band. File drops.",
   .resourceDeltas = {0, 1, 2, 0, -2, 1, 0, 3},
   .setFlags = F_FIRST_FILE, .trackerIndex = 5, .trackerDelta = 1,
   .repIndex = 2, .repDelta = 2,
   .weight = 3, .requireFlags = F_JOB_TAKEN, .forbidFlags = 0,
   .tone = CGC::TONE_GOOD},

  {.name = "Relay handler",
   .note = "Ghost Relay makes contact. Trust the static.",
   .resourceDeltas = {0, 0, 0, 0, 0, 1, 0, 2},
   .setFlags = F_RELAY_CONTACT, .trackerIndex = 5, .trackerDelta = 1,
   .repIndex = 2, .repDelta = 3,
   .weight = 2, .requireFlags = F_FIRST_FILE, .forbidFlags = F_RELAY_CONTACT,
   .tone = CGC::TONE_GOOD},

  {.name = "Signal bleed",
   .note = "Ghost waveform corrupts your logs. Deck -2.",
   .resourceDeltas = {0, 2, 3, -2, -3, 0, 0, 4},
   .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
   .repIndex = 2, .repDelta = -1,
   .weight = 2, .requireFlags = F_RELAY_CONTACT, .forbidFlags = 0,
   .tone = CGC::TONE_BAD},

  {.name = "Blackbox drop",
   .note = "Dead runner's drive shows up at your door.",
   .resourceDeltas = {0, 0, 0, 0, 0, 2, 1, 2},
   .setFlags = F_BLACKBOX, .trackerIndex = 5, .trackerDelta = 2,
   .repIndex = 2, .repDelta = 2,
   .weight = 2, .requireFlags = F_RELAY_CONTACT, .forbidFlags = F_BLACKBOX,
   .tone = CGC::TONE_GOOD},

  {.name = "Old signal answer",
   .note = "The ghost band answers. Arc locks in.",
   .resourceDeltas = {0, 0, 0, 0, 0, 2, 0, 5},
   .setFlags = F_CONSPIRACY, .trackerIndex = 5, .trackerDelta = 2,
   .repIndex = 2, .repDelta = 4,
   .weight = 4, .requireFlags = F_RELAY_CONTACT | F_BLACKBOX,
   .forbidFlags = F_CONSPIRACY,
   .tone = CGC::TONE_GOOD},

  {.name = "Civic sweep",
   .note = "Enforcement sweeps the district. Heat up.",
   .resourceDeltas = {0, 6, 5, 0, 0, 0, -2, 0},
   .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
   .repIndex = -1, .repDelta = 0,
   .weight = 1, .requireFlags = 0, .forbidFlags = 0,
   .tone = CGC::TONE_BAD},
};

// ── Endings ───────────────────────────────────────────────────────────────────
// Most specific first. resourceIndex, trackerIndex, repIndex — use -1 to skip.
// resourceMin/Max: inclusive range check. trackerMin: minimum count.
static const CGC::EndingDef ENDINGS[] = {

  // 1. Relay Pact — best ghost ending: conspiracy done + blackbox + files >= 4
  {.name = "Relay Pact",
   .note = "You broadcast back. The old signal acknowledges you. Arc complete.",
   .cond = {.resourceIndex = R_FILES, .resourceMin = 4, .resourceMax = 99,
            .trackerIndex = 6, .trackerMin = 3,
            .repIndex = -1, .repMin = 0,
            .requireFlags = F_CONSPIRACY | F_BLACKBOX,
            .forbidFlags = F_BURNED,
            .requiresVictory = true}},

  // 2. Clean Ghost — finished with Ghost Relay, low heat, no burn
  {.name = "Clean Ghost",
   .note = "Node logs clean. Ghost band silent. You walk free and invisible.",
   .cond = {.resourceIndex = R_HEAT, .resourceMin = 0, .resourceMax = 40,
            .trackerIndex = -1, .trackerMin = 0,
            .repIndex = -1, .repMin = 0,
            .requireFlags = F_RELAY_CONTACT,
            .forbidFlags = F_BURNED,
            .requiresVictory = true}},

  // 3. Rich Corporate — Neon Crown committed, high cred
  {.name = "Rich Corporate",
   .note = "Crown paid well. The burn notices come later. You cash out first.",
   .cond = {.resourceIndex = R_CRED, .resourceMin = 80, .resourceMax = 999,
            .trackerIndex = -1, .trackerMin = 0,
            .repIndex = -1, .repMin = 0,
            .requireFlags = F_FACTION_NEO,
            .forbidFlags = 0,
            .requiresVictory = true}},

  // 4. Street Legend — Streetline rep + high street jobs completed
  {.name = "Street Legend",
   .note = "Every fixer on the strip knows your handle. Low pay, high respect.",
   .cond = {.resourceIndex = R_REP, .resourceMin = 50, .resourceMax = 99,
            .trackerIndex = 1, .trackerMin = 6,
            .repIndex = -1, .repMin = 0,
            .requireFlags = F_FACTION_STR,
            .forbidFlags = 0,
            .requiresVictory = true}},

  // 5. Catch-all victory
  {.name = "Rat Survives",
   .note = "Broke, a little scorched, still breathing. Not every run is clean.",
   .cond = {.resourceIndex = -1, .resourceMin = 0, .resourceMax = 0,
            .trackerIndex = -1, .trackerMin = 0,
            .repIndex = -1, .repMin = 0,
            .requireFlags = 0, .forbidFlags = 0,
            .requiresVictory = true}},

  // 6. Debt Lock — loss, broke with burned flag
  {.name = "Debt Lock",
   .note = "Cred gone. Deck in hock. No fence will front you. Game over.",
   .cond = {.resourceIndex = R_CRED, .resourceMin = 0, .resourceMax = 4,
            .trackerIndex = -1, .trackerMin = 0,
            .repIndex = -1, .repMin = 0,
            .requireFlags = F_BURNED,
            .forbidFlags = 0,
            .requiresVictory = false}},

  // 7. Deck Burnout — loss, deck hit zero
  {.name = "Deck Burnout",
   .note = "Trace burned through the board. Deck is slag. You've got nothing.",
   .cond = {.resourceIndex = -1, .resourceMin = 0, .resourceMax = 0,
            .trackerIndex = -1, .trackerMin = 0,
            .repIndex = -1, .repMin = 0,
            .requireFlags = 0, .forbidFlags = 0,
            .requiresVictory = false}},
};

// ── Tracker labels ────────────────────────────────────────────────────────────
// order: Rests, Jobs, Nodes, Tools, Upgrades, Files, Conspiracy, (spare)
static const char* const TRACKER_LABELS[] = {
  "Rests", "Jobs", "Nodes", "Tools", "Upgrades", "Files", "Conspiracy", nullptr,
};

// ── Game definition ───────────────────────────────────────────────────────────
static const CGC::DeepGameDefinition DEF = {
  .title         = "Signal Rat",
  .slug          = "signal-rat-cyberdeck-rpg",
  .subtitle      = "Flagship fictional cyberdeck",
  .saveMagic     = "SRAT2",
  .resources     = RESOURCES,
  .resourceCount = sizeof(RESOURCES) / sizeof(RESOURCES[0]),
  .screens       = SCREENS,
  .screenCount   = sizeof(SCREENS) / sizeof(SCREENS[0]),
  .events        = EVENTS,
  .eventCount    = sizeof(EVENTS) / sizeof(EVENTS[0]),
  .endings       = ENDINGS,
  .endingCount   = sizeof(ENDINGS) / sizeof(ENDINGS[0]),
  .targetProgress    = 150,
  .accentColor   = 0x07FF,
  .warningColor  = 0xFD20,
  .trackerLabels      = TRACKER_LABELS,
  .trackerLabelCount  = 8,
  .objective     = "Hack nodes; recover files; end the signal.",
  .primaryTracker = 2,
};

const CGC::DeepGameDefinition& gameDefinition() {
  return DEF;
}
