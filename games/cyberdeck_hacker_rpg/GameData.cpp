#include "GameData.h"

namespace CGC = CardputerGameCore;

// Resource indices — keep in sync with RESOURCES order.
enum {
  R_CRED,   // 0: Credits (currency)
  R_DECK,   // 1: Deck HP (loss trigger)
  R_HEAT,   // 2: Campaign Heat (inverted, high-is-bad)
  R_TRACE,  // 3: Per-job Trace (inverted, high-is-bad)
  R_BATT,   // 4: Battery (action buffer)
  R_FILES,  // 5: Recovered Files counter
  R_REP,    // 6: Street Rep
  R_GHOST,  // 7: Faction conspiracy pressure
};

// Story flags — set cleanly by screens, advanced by events.
enum : uint32_t {
  F_JOB_ACTIVE   = 1u << 0,  // a contract is loaded from the board
  F_FILES_READ   = 1u << 1,  // player has opened the FILES screen once
  F_CONSPIRACY   = 1u << 2,  // recovered 4+ files — conspiracy arc unlocked
  F_CORP_DEAL    = 1u << 3,  // accepted a corporate handler payout
  F_SIGNAL_CULT  = 1u << 4,  // aligned with the signal cult faction
  F_NETWATCH     = 1u << 5,  // civic netwatch knows the player's handle
  F_GHOST_PROOF  = 1u << 6,  // recovered the Crown Ledger ghost-file
  F_BURNED       = 1u << 7,  // trace blew through the ceiling; identity compromised
};

// Reputation channels: 0=Street, 1=Corp, 2=SignalCult, 3=Netwatch
enum { REP_STREET, REP_CORP, REP_SIGNAL, REP_CIVIC };

// ---------------------------------------------------------------------------
// Resources
// ---------------------------------------------------------------------------
static const CGC::ResourceDef RESOURCES[] = {
    //  label   start  min   max  prim  safe  warn  bad   inverted
    {"Cred",     40,    0,  999,   0,    0,   15,   5,   false},  // currency
    {"Deck",     90,    0,  100,  -1,    4,   35,  15,   false},  // loss if 0
    {"Heat",      5,    0,  100,   0,   -3,   55,  80,   true},   // campaign; high=bad
    {"Trace",     0,    0,  100,   0,   -4,   60,  85,   true},   // per-job; high=bad
    {"Batt",     80,    0,  100,   0,    5,   20,   8,   false},  // action buffer
    {"Files",     0,    0,   99,   0,    0,   -1,  -1,   false},  // counter
    {"Rep",      10,    0,   99,   0,    1,   -1,  -1,   false},  // street rep
    {"Ghost",     0,    0,   99,   0,    0,   -1,  -1,   false},  // conspiracy pressure
};

// ---------------------------------------------------------------------------
// Screen row tables
// ---------------------------------------------------------------------------

// SAFEHOUSE — rest, patch, read leads, manage heat
static const CGC::NamedValue ROWS_SAFEHOUSE[] = {
    {"Patch Deck",   "Solder busted traces; restore Deck HP."},
    {"Cool Down",    "Vent heat sinks; shed campaign Heat."},
    {"Read Leads",   "Cross-ref rumor channels for next job."},
    {"Fence Report", "Check the fence on open payout flags."},
    {"Scrub Handle", "Rotate alias and ghost last trace ping."},
    {"Rest Cycle",   "Kill the deck, sleep, recover Battery."},
    {"Broker Call",  "Voice-spoof a broker; get intel drop."},
    {"Safe Prep",    "Layout tools; plan the next node run."},
};

// BOARD — take a contract; sets F_JOB_ACTIVE
static const CGC::NamedValue ROWS_BOARD[] = {
    {"Glass Bank Sim",    "Strip a sim vault; low heat, street pay."},
    {"Transit Ghostbox",  "Ride ghost rails; pull schedule logs."},
    {"Clinic Mirror",     "Clone a clinic mirror; patient ledger."},
    {"Weather Feed Hack", "Inject false storm; chaos for a client."},
    {"Servo Pawn Drop",   "Brick a pawn shop's servo-lock array."},
    {"Underpass Router",  "Root an underpass mesh; street bounty."},
    {"Root Door Corp",    "Corp side-door; high pay, high trace."},
    {"Crown Ledger Run",  "Faction job: ghost-file target node."},
};

// NODE — run fake commands; gated on F_JOB_ACTIVE; costs Battery
static const CGC::NamedValue ROWS_NODE[] = {
    {"scan",        "Map node structure; find open ports."},
    {"spoof",       "Fake an admin handshake; drop trace."},
    {"decrypt",     "Crack data lock; extract payload."},
    {"inject",      "Push exploit; fast but noisy."},
    {"scrub",       "Burn trace log; slow but clean."},
    {"trace-map",   "Chart active tripwires; read heat."},
    {"ghost-fork",  "Fork process; hide in node shadow."},
    {"disconnect",  "Cleanly pull jack; end the run."},
};

// TOOLS — upgrade deck programs; costs Credits
static const CGC::NamedValue ROWS_TOOLS[] = {
    {"Scanner v2",     "Better recon; cuts trace on scan."},
    {"Decryptor v2",   "Faster crack; less Deck wear."},
    {"Spoof Chip v2",  "Deeper alias; bigger trace drop."},
    {"Injector v2",    "Quieter push; halves alarm gain."},
    {"Trace Sink",     "Passive drain; bleeds trace each turn."},
    {"Blackbox",       "Panic buffer; absorbs one alarm hit."},
    {"Battery Mod",    "Bigger cell; extends node time."},
    {"Shield Patch",   "Hardens Deck; reduces HP bleed."},
};

// MARKET — buy black-market items; costs Credits
static const CGC::NamedValue ROWS_MARKET[] = {
    {"Cold CPU",       "Overclocked proc; boosts Ghost score."},
    {"Battery Stack",  "Big spare cells; refills Battery."},
    {"Clean Shell",    "New chassis; heals Deck HP."},
    {"Spoof Key",      "One-shot deep alias; crushes Heat."},
    {"Trace Sink Bag", "Bulk scrub pack; burns Trace fast."},
    {"Patch Kit",      "Field repair; restore Deck mid-run."},
    {"Street Info",    "Broker intel package; boosts Rep."},
    {"Ghost Codec",    "Rare file tool; unlocks Ghost arc."},
};

// FILES — review recovered data; gated on F_FILES_READ (set by first NODE run)
static const CGC::NamedValue ROWS_FILES[] = {
    {"Ledger Fragment",  "Bank transfer trails — follow credits."},
    {"Clinic Dump",      "Patient ID map; a name repeats."},
    {"Weather Splice",   "Storm data faked upstream — who?"},
    {"Transit Ghost",    "Schedule gaps hide shipment routes."},
    {"Ghost Signal",     "Carrier wave from Signal Cult node."},
    {"Civic Intercept",  "Netwatch flagged your handle once."},
    {"Street Whisper",   "Runner burned — same corp handler."},
    {"Crown Log",        "Top-tier ghost file; unlocks arc."},
};

// CONSPIRACY — gated on F_CONSPIRACY; the endgame confrontation screen
static const CGC::NamedValue ROWS_CONSPIRACY[] = {
    {"Leak to Press",    "Send files to a fiction-net outlet."},
    {"Ghost Publish",    "Drop encrypted archive on dark mesh."},
    {"Sell to Corp",     "Trade the proof for a corp buyout."},
    {"Signal Broadcast", "Transmit via the cult's rogue relay."},
    {"Civic Handoff",    "Pass files to a netwatch defector."},
    {"Vanish",           "Wipe handle; walk away clean."},
};

// ---------------------------------------------------------------------------
// Screens
// ---------------------------------------------------------------------------
static const CGC::DeepScreenDef SCREENS[] = {
    // SAFEHOUSE: rest & prep — free, restores Deck/Batt/reduces Heat
    {.title      = "SAFEHOUSE",
     .verb       = "Rest",
     .rows       = ROWS_SAFEHOUSE,
     .rowCount   = 8,
     .resourceDeltas = {0, 3, -2, -3, 4, 0, 0, 0},
     .progressDelta  = 1,
     .trackerIndex   = 0,
     .flagMask       = 0,
     .costResource   = -1,
     .costAmount     = 0,
     .requireFlags   = 0,
     .trackerDelta   = 1,
     .objective      = "Rest; patch deck; cool heat"},

    // BOARD: pick a contract — free, sets job flag
    {.title      = "BOARD",
     .verb       = "Take",
     .rows       = ROWS_BOARD,
     .rowCount   = 8,
     .resourceDeltas = {4, 0, 2, 0, -1, 0, 1, 0},
     .progressDelta  = 2,
     .trackerIndex   = 1,
     .flagMask       = F_JOB_ACTIVE,
     .costResource   = -1,
     .costAmount     = 0,
     .requireFlags   = 0,
     .trackerDelta   = 1,
     .objective      = "Take a contract from the board"},

    // NODE: run the job — costs Battery, gated on job flag
    {.title      = "NODE",
     .verb       = "Run",
     .rows       = ROWS_NODE,
     .rowCount   = 8,
     .resourceDeltas = {0, -2, 3, 5, -6, 1, 0, 1},
     .progressDelta  = 5,
     .trackerIndex   = 2,
     .flagMask       = F_FILES_READ,
     .costResource   = R_BATT,
     .costAmount     = 8,
     .requireFlags   = F_JOB_ACTIVE,
     .trackerDelta   = 1,
     .objective      = "Run node cmds (-8 Batt, needs job)"},

    // TOOLS: upgrade programs — costs Credits
    {.title      = "TOOLS",
     .verb       = "Upgrade",
     .rows       = ROWS_TOOLS,
     .rowCount   = 8,
     .resourceDeltas = {0, 1, -1, -2, 1, 0, 0, 0},
     .progressDelta  = 1,
     .trackerIndex   = 3,
     .flagMask       = 0,
     .costResource   = R_CRED,
     .costAmount     = 15,
     .requireFlags   = 0,
     .trackerDelta   = 1,
     .objective      = "Upgrade programs (-15 Cred)"},

    // MARKET: black-market buys — costs Credits
    {.title      = "MARKET",
     .verb       = "Buy",
     .rows       = ROWS_MARKET,
     .rowCount   = 8,
     .resourceDeltas = {0, 2, -1, -1, 3, 0, 1, 1},
     .progressDelta  = 1,
     .trackerIndex   = 4,
     .flagMask       = 0,
     .costResource   = R_CRED,
     .costAmount     = 20,
     .requireFlags   = 0,
     .trackerDelta   = 1,
     .objective      = "Buy market items (-20 Cred)"},

    // FILES: read recovered data — gated on F_FILES_READ
    {.title      = "FILES",
     .verb       = "Read",
     .rows       = ROWS_FILES,
     .rowCount   = 8,
     .resourceDeltas = {0, 0, 0, -1, -1, 2, 0, 2},
     .progressDelta  = 2,
     .trackerIndex   = 5,
     .flagMask       = F_CONSPIRACY,
     .costResource   = -1,
     .costAmount     = 0,
     .requireFlags   = F_FILES_READ,
     .trackerDelta   = 1,
     .objective      = "Read files (needs node run first)"},

    // CONSPIRACY: endgame confrontation — gated on F_CONSPIRACY
    {.title      = "CONSPIRACY",
     .verb       = "Act",
     .rows       = ROWS_CONSPIRACY,
     .rowCount   = 6,
     .resourceDeltas = {8, -3, 4, 3, -2, 0, 2, 3},
     .progressDelta  = 8,
     .trackerIndex   = 6,
     .flagMask       = F_GHOST_PROOF,
     .costResource   = -1,
     .costAmount     = 0,
     .requireFlags   = F_CONSPIRACY,
     .trackerDelta   = 1,
     .objective      = "Confront the faction arc (gated)"},
};

// ---------------------------------------------------------------------------
// Events
// ---------------------------------------------------------------------------
static const CGC::EventDef EVENTS[] = {
    // --- Common filler (always eligible) ---
    {.name = "Clean jack",
     .note = "Deck runs cold; no flags raised.",
     .resourceDeltas = {0, 0, -1, -2, 1, 0, 0, 0},
     .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = -1, .repDelta = 0, .weight = 5,
     .requireFlags = 0, .forbidFlags = 0, .tone = CGC::TONE_GOOD},

    {.name = "Static noise",
     .note = "Node lag; Batt bleeds, no progress.",
     .resourceDeltas = {0, 0, 0, 1, -2, 0, 0, 0},
     .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = -1, .repDelta = 0, .weight = 4,
     .requireFlags = 0, .forbidFlags = 0, .tone = CGC::TONE_NEUTRAL},

    {.name = "Trace spike",
     .note = "Alert daemon wakes; trace surges.",
     .resourceDeltas = {0, -1, 2, 6, -1, 0, 0, 0},
     .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = -1, .repDelta = 0, .weight = 3,
     .requireFlags = 0, .forbidFlags = 0, .tone = CGC::TONE_BAD},

    {.name = "Extra payload",
     .note = "Hidden data cache; bonus credits.",
     .resourceDeltas = {6, 0, 0, 0, 0, 0, 1, 0},
     .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = REP_STREET, .repDelta = 1, .weight = 3,
     .requireFlags = F_JOB_ACTIVE, .forbidFlags = 0, .tone = CGC::TONE_GOOD},

    {.name = "Deck overheat",
     .note = "Thermal spike burns a board layer.",
     .resourceDeltas = {0, -4, 0, 2, -3, 0, 0, 0},
     .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = -1, .repDelta = 0, .weight = 2,
     .requireFlags = 0, .forbidFlags = 0, .tone = CGC::TONE_BAD},

    // --- Street faction events (require job active) ---
    {.name = "Street tip",
     .note = "Runner drops a free intel packet.",
     .resourceDeltas = {3, 0, -1, -2, 0, 0, 2, 0},
     .setFlags = 0, .trackerIndex = 5, .trackerDelta = 0,
     .repIndex = REP_STREET, .repDelta = 2, .weight = 3,
     .requireFlags = F_JOB_ACTIVE, .forbidFlags = F_BURNED, .tone = CGC::TONE_GOOD},

    {.name = "Rival handle",
     .note = "Rival leaks your alias unless paid.",
     .resourceDeltas = {-5, 0, 3, 2, 0, 0, -1, 0},
     .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = REP_STREET, .repDelta = -1, .weight = 2,
     .requireFlags = F_JOB_ACTIVE, .forbidFlags = 0, .tone = CGC::TONE_BAD},

    // --- Corporate handler events ---
    {.name = "Corp bonus",
     .note = "Handler pays extra; your Heat ticks up.",
     .resourceDeltas = {8, 0, 4, 0, 0, 0, 0, 0},
     .setFlags = F_CORP_DEAL, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = REP_CORP, .repDelta = 3, .weight = 2,
     .requireFlags = F_JOB_ACTIVE, .forbidFlags = F_SIGNAL_CULT, .tone = CGC::TONE_BAD},

    {.name = "Corp honeytrap",
     .note = "Fake admin login stings the deck.",
     .resourceDeltas = {0, -3, 3, 4, -2, 0, 0, 0},
     .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = REP_CORP, .repDelta = -1, .weight = 2,
     .requireFlags = F_JOB_ACTIVE, .forbidFlags = 0, .tone = CGC::TONE_BAD},

    // --- Signal Cult events ---
    {.name = "Cult signal",
     .note = "Ghost carrier pulse; strange file drop.",
     .resourceDeltas = {0, 0, 0, 0, -1, 1, 0, 3},
     .setFlags = F_SIGNAL_CULT, .trackerIndex = 5, .trackerDelta = 1,
     .repIndex = REP_SIGNAL, .repDelta = 2, .weight = 2,
     .requireFlags = F_FILES_READ, .forbidFlags = F_CORP_DEAL, .tone = CGC::TONE_GOOD},

    {.name = "Corrupted log",
     .note = "Cult node pushes a scrambled archive.",
     .resourceDeltas = {0, 0, 0, 1, -1, 1, 0, 2},
     .setFlags = 0, .trackerIndex = 5, .trackerDelta = 1,
     .repIndex = REP_SIGNAL, .repDelta = 1, .weight = 2,
     .requireFlags = F_SIGNAL_CULT, .forbidFlags = 0, .tone = CGC::TONE_NEUTRAL},

    // --- Netwatch / civic events ---
    {.name = "Civic sweep",
     .note = "Netwatch pings your subnet; Heat rises.",
     .resourceDeltas = {0, 0, 4, 3, 0, 0, 0, 0},
     .setFlags = F_NETWATCH, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = REP_CIVIC, .repDelta = -1, .weight = 2,
     .requireFlags = 0, .forbidFlags = 0, .tone = CGC::TONE_BAD},

    {.name = "Civic defector",
     .note = "Rogue netwatch agent sells you a key.",
     .resourceDeltas = {4, 1, -2, -3, 0, 0, 1, 0},
     .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = REP_CIVIC, .repDelta = 2, .weight = 1,
     .requireFlags = F_NETWATCH, .forbidFlags = F_BURNED, .tone = CGC::TONE_GOOD},

    // --- Conspiracy / endgame events (require FILES_READ) ---
    {.name = "File fragment",
     .note = "Crown log shard recovered; arc deepens.",
     .resourceDeltas = {0, 0, 0, 0, -1, 2, 0, 2},
     .setFlags = 0, .trackerIndex = 5, .trackerDelta = 1,
     .repIndex = -1, .repDelta = 0, .weight = 3,
     .requireFlags = F_FILES_READ, .forbidFlags = 0, .tone = CGC::TONE_GOOD},

    {.name = "Identity burned",
     .note = "Trace ceiling hit; handle compromised.",
     .resourceDeltas = {0, -2, 5, 0, 0, 0, -2, 0},
     .setFlags = F_BURNED, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = REP_STREET, .repDelta = -2, .weight = 1,
     .requireFlags = 0, .forbidFlags = F_BURNED, .tone = CGC::TONE_BAD},

    {.name = "Ghost proof",
     .note = "Crown Ledger found: full arc unlocked.",
     .resourceDeltas = {0, 0, 0, 0, -2, 1, 1, 4},
     .setFlags = F_GHOST_PROOF, .trackerIndex = 5, .trackerDelta = 1,
     .repIndex = -1, .repDelta = 0, .weight = 2,
     .requireFlags = F_CONSPIRACY, .forbidFlags = F_GHOST_PROOF, .tone = CGC::TONE_GOOD},

    {.name = "Black market deal",
     .note = "Fence moves surplus; credits arrive.",
     .resourceDeltas = {7, 0, -1, -1, 0, 0, 0, 0},
     .setFlags = 0, .trackerIndex = -1, .trackerDelta = 0,
     .repIndex = REP_STREET, .repDelta = 1, .weight = 2,
     .requireFlags = 0, .forbidFlags = 0, .tone = CGC::TONE_GOOD},
};

// ---------------------------------------------------------------------------
// Endings — most-specific first
// ---------------------------------------------------------------------------
static const CGC::EndingDef ENDINGS[] = {
    // 1. Burned runner — identity compromised, high heat
    {.name = "Burned Runner",
     .note = "Handle is public. You run, not free.",
     .cond = {.resourceIndex = R_HEAT, .resourceMin = 75, .resourceMax = 100,
              .trackerIndex = -1, .trackerMin = 0, .repIndex = -1, .repMin = 0,
              .requireFlags = F_BURNED, .forbidFlags = 0,
              .requiresVictory = true}},

    // 2. Corporate asset — sold out to corp, got rich
    {.name = "Corporate Asset",
     .note = "Corp owns your record. Pay was real.",
     .cond = {.resourceIndex = R_CRED, .resourceMin = 80, .resourceMax = 999,
              .trackerIndex = -1, .trackerMin = 0, .repIndex = REP_CORP, .repMin = 3,
              .requireFlags = F_CORP_DEAL, .forbidFlags = F_SIGNAL_CULT,
              .requiresVictory = true}},

    // 3. Signal Believer — aligned with cult, ghost arc completed
    {.name = "Signal Believer",
     .note = "You broadcast on the cult relay. Gone.",
     .cond = {.resourceIndex = -1, .resourceMin = 0, .resourceMax = 0,
              .trackerIndex = -1, .trackerMin = 0, .repIndex = REP_SIGNAL, .repMin = 4,
              .requireFlags = F_SIGNAL_CULT | F_GHOST_PROOF, .forbidFlags = F_CORP_DEAL,
              .requiresVictory = true}},

    // 4. Ethical Leaker — published files, high rep, no corp deal
    {.name = "Ethical Leaker",
     .note = "Files hit the mesh. Truth costs heat.",
     .cond = {.resourceIndex = R_FILES, .resourceMin = 6, .resourceMax = 99,
              .trackerIndex = -1, .trackerMin = 0, .repIndex = REP_STREET, .repMin = 3,
              .requireFlags = F_GHOST_PROOF, .forbidFlags = F_CORP_DEAL,
              .requiresVictory = true}},

    // 5. Vanished Legend — clean ghost, high rep, no burned flag
    {.name = "Vanished Legend",
     .note = "No trail. No name. The deck goes cold.",
     .cond = {.resourceIndex = R_REP, .resourceMin = 35, .resourceMax = 99,
              .trackerIndex = -1, .trackerMin = 0, .repIndex = -1, .repMin = 0,
              .requireFlags = 0, .forbidFlags = F_BURNED,
              .requiresVictory = true}},

    // 6. Paid Ghost — catch-all victory
    {.name = "Paid Ghost",
     .note = "Clean run. Pocket full. Face unknown.",
     .cond = {.resourceIndex = -1, .resourceMin = 0, .resourceMax = 0,
              .trackerIndex = -1, .trackerMin = 0, .repIndex = -1, .repMin = 0,
              .requireFlags = 0, .forbidFlags = 0,
              .requiresVictory = true}},

    // 7. Deck Dead — catch-all loss
    {.name = "Deck Dead",
     .note = "Hardware fried. Career over. Walk away.",
     .cond = {.resourceIndex = -1, .resourceMin = 0, .resourceMax = 0,
              .trackerIndex = -1, .trackerMin = 0, .repIndex = -1, .repMin = 0,
              .requireFlags = 0, .forbidFlags = 0,
              .requiresVictory = false}},
};

// ---------------------------------------------------------------------------
// Tracker labels
// ---------------------------------------------------------------------------
static const char* const TRACKER_LABELS[] = {
    "Jobs",      // 0: safehouse visits (used as job prep tracker)
    "Contracts", // 1: contracts taken from board
    "Nodes",     // 2: node runs completed
    "Upgrades",  // 3: tool upgrades bought
    "Purchases", // 4: market purchases
    "Files",     // 5: files recovered
    "Acts",      // 6: conspiracy actions taken
    nullptr,
};

// ---------------------------------------------------------------------------
// Game definition
// ---------------------------------------------------------------------------
static const CGC::DeepGameDefinition DEF = {
    .title            = "Cyberdeck RPG",
    .slug             = "cyberdeck-hacker-rpg",
    .subtitle         = "Offline fake node crawl",
    .saveMagic        = "CDR2",
    .resources        = RESOURCES,
    .resourceCount    = sizeof(RESOURCES) / sizeof(RESOURCES[0]),
    .screens          = SCREENS,
    .screenCount      = sizeof(SCREENS) / sizeof(SCREENS[0]),
    .events           = EVENTS,
    .eventCount       = sizeof(EVENTS) / sizeof(EVENTS[0]),
    .endings          = ENDINGS,
    .endingCount      = sizeof(ENDINGS) / sizeof(ENDINGS[0]),
    .targetProgress   = 160,
    .accentColor      = 0x07FF,
    .warningColor     = 0xFD20,
    .trackerLabels    = TRACKER_LABELS,
    .trackerLabelCount = 8,
    .objective        = "Run nodes; recover files; expose arc.",
    .primaryTracker   = 2,
};

const CGC::DeepGameDefinition& gameDefinition() {
  return DEF;
}
