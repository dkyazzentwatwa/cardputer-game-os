#include "GameData.h"

namespace CGC = CardputerGameCore;

// Resource indices — keep in sync with RESOURCES order.
enum { R_CASH, R_TIME, R_CLUES, R_LEADS, R_HEAT, R_REP, R_TRUST, R_LINKS };

// Story flags — screens/events set these to gate later screens and events.
enum : uint32_t {
  F_CASE_OPEN   = 1u << 0,  // a case is active on the board
  F_CANVASSED   = 1u << 1,  // at least one location visited
  F_INTERVIEWED = 1u << 2,  // at least one suspect interviewed
  F_LINKED      = 1u << 3,  // first clue link found via COMPARE
  F_DIRTY_MONEY = 1u << 4,  // accepted a dirty bribe
  F_META_CLUE   = 1u << 5,  // found a city-corruption meta-clue
  F_ACCUSED     = 1u << 6,  // made at least one accusation
  F_EXPOSED     = 1u << 7,  // corrupt ring exposed (endgame unlock)
};

// ── Resources ────────────────────────────────────────────────────────────────
// Cash:  operations currency (spend via INFORMANTS, no passive drain)
// Time:  deadline pressure — primaryDelta -1; hitting 0 → loss
// Clues: evidence counter — no drain, endings read it
// Leads: sourced from informants — gates COMPARE + ACCUSE
// Heat:  inverted (high is bad): bad leads, press leaks drive it up
// Rep:   public standing — endings read it
// Trust: client/witness confidence — inverted warn (lose trust fast = bad)
// Links: contradiction links found — endings read it

static const CGC::ResourceDef RESOURCES[] = {
    // label   start min  max  prim safe  warn bad  inverted
    {"Cash",    45,  0,  999,  0,   0,   12,   4,  false},
    {"Time",    40,  0,   99, -1,   3,   14,   6,  false},  // loss trigger
    {"Clues",    0,  0,   99,  0,   0,   -1,  -1,  false},
    {"Leads",    3,  0,   30,  0,   0,   -1,  -1,  false},
    {"Heat",     4,  0,  100,  0,   0,   55,  80,  true},   // high is bad
    {"Rep",     30,  0,  100,  0,   0,   12,   4,  false},
    {"Trust",   30,  0,  100,  0,   0,   10,   4,  false},
    {"Links",    0,  0,   30,  0,   0,   -1,  -1,  false},
};

// ── Screen row tables ─────────────────────────────────────────────────────────

// AGENCY — take a new case; each case entry is a real case name + client hook.
static const CGC::NamedValue ROWS_AGENCY[] = {
    {"Glass Cat",    "Stolen statuette; art dealer client."},
    {"Missing Key",  "Landlord locked out; tenant vanished."},
    {"Red Receipt",  "Faked expense; corporate whistleblower."},
    {"Old Names",    "Cold-case; widow wants the truth."},
    {"Pier Debt",    "Dock boss says someone skimmed the take."},
    {"Night Shift",  "Factory guard found dead at 3 a.m."},
    {"Silver Fork",  "Heirloom missing; suspect list is long."},
    {"Review Board", "Revisit open case notes; plan next move."},
};

// CANVASS — visit city locations; costs Time, yields Clues.
static const CGC::NamedValue ROWS_CANVASS[] = {
    {"Corner Diner",   "Wait for the morning regulars to talk."},
    {"Back Alley",     "Check for dropped evidence near the bins."},
    {"City Archives",  "Pull property and warrant records."},
    {"Waterfront Pier","Watch the cargo crews for familiar faces."},
    {"Pawnshop Row",   "See if stolen goods surfaced for sale."},
    {"Rail Depot",     "Ask the night crew about unusual cargo."},
    {"Park Bench",     "Old regulars see everything and forget nothing."},
};

// INTERVIEW — question suspects; costs Time, yields statements (Trust fuel).
static const CGC::NamedValue ROWS_INTERVIEW[] = {
    {"Ask about alibi",   "Where were you that night?"},
    {"Ask about debts",   "Who do you owe money to?"},
    {"Ask about noises",  "Did you hear anything unusual?"},
    {"Show the receipt",  "Can you explain this transaction?"},
    {"Ask about the key", "Who else had access to the room?"},
    {"Probe the motive",  "Why would someone want this done?"},
    {"Name a witness",    "Who can confirm your story?"},
    {"Press on timing",   "Walk me through the exact timeline."},
};

// COMPARE — cross-examine clue pairs; gated on F_CANVASSED + F_INTERVIEWED.
// Costs a Lead each time. Yields Links and surfaces contradictions.
static const CGC::NamedValue ROWS_COMPARE[] = {
    {"Receipt vs Alibi",  "Timestamp on receipt kills the alibi."},
    {"Photo vs Timeline", "Camera angle narrows the window."},
    {"Key vs Witness",    "Witness never saw the key returned."},
    {"Ledger vs Debt",    "Numbers on the ledger don't match claims."},
    {"Ticket vs Location","Ticket places suspect across town."},
    {"Thread vs Coat",    "Fiber matches only one suspect's coat."},
    {"Note vs Statement", "Handwriting in note differs from form."},
    {"Door log vs Time",  "Building log says door was sealed early."},
};

// INFORMANTS — pay cash for leads; no flag gate, but costly.
static const CGC::NamedValue ROWS_INFORMANTS[] = {
    {"Dock Contact",     "Waterfront source, knows cargo secrets."},
    {"Press Stringer",   "Reporter trades tips for exclusives."},
    {"Ex-cop Remy",      "Retired sergeant, still has friends."},
    {"Neighborhood Kid", "Sees everything, asks for very little."},
    {"City Hall Clerk",  "Paper trail in exchange for a favor."},
    {"Pawn Broker Sal",  "Knows where stolen goods end up."},
};

// ACCUSE — resolve a case; gated on F_LINKED (need at least one link found).
static const CGC::NamedValue ROWS_ACCUSE[] = {
    {"Full Disclosure",  "Present all evidence; let police decide."},
    {"Confrontation",    "Face the suspect directly with the proof."},
    {"Quiet Settlement", "Hand evidence to client; skip the press."},
    {"Tip the Press",    "Give the story to the stringer instead."},
    {"Partial Filing",   "File what you have; case stays open."},
    {"Drop the Case",    "Walk away; protect yourself for now."},
};

// COURTROOM — meta-case endgame; gated on F_META_CLUE + F_ACCUSED + F_LINKED.
// High reward but maximum risk. Unlocks the true-case ending.
static const CGC::NamedValue ROWS_COURTROOM[] = {
    {"Present Dossier",  "Hand the full corruption file to DA."},
    {"Testify Directly", "Take the stand; no more hiding."},
    {"Leak to Press",    "Blow it wide open; let city decide."},
    {"Broker a Deal",    "Trade evidence for witness protection."},
    {"Archive the File", "Lock it away safe; wait for better day."},
};

// ── Screens ───────────────────────────────────────────────────────────────────
static const CGC::DeepScreenDef SCREENS[] = {
    // AGENCY: open a case; sets F_CASE_OPEN; free but costs 1 Time.
    {.title = "AGENCY", .verb = "Open", .rows = ROWS_AGENCY, .rowCount = 8,
     .resourceDeltas = {0, -1, 0, 0, 0, 0, 1, 0}, .progressDelta = 1, .trackerIndex = 0,
     .flagMask = F_CASE_OPEN, .costResource = -1, .costAmount = 0, .requireFlags = 0,
     .trackerDelta = 1, .objective = "Take a new case"},

    // CANVASS: visit locations; costs 2 Time, gains 2 Clues; sets F_CANVASSED.
    {.title = "CANVASS", .verb = "Visit", .rows = ROWS_CANVASS, .rowCount = 7,
     .resourceDeltas = {0, -2, 2, 0, 1, 0, 1, 0}, .progressDelta = 2, .trackerIndex = 1,
     .flagMask = F_CANVASSED, .costResource = R_TIME, .costAmount = 3, .requireFlags = F_CASE_OPEN,
     .trackerDelta = 1, .objective = "Visit locations (-3 Time, +Clues)"},

    // INTERVIEW: question suspects; costs 2 Time, gains Trust fuel; sets F_INTERVIEWED.
    {.title = "INTERVIEW", .verb = "Ask", .rows = ROWS_INTERVIEW, .rowCount = 8,
     .resourceDeltas = {0, -2, 1, 0, 1, 0, 2, 0}, .progressDelta = 2, .trackerIndex = 2,
     .flagMask = F_INTERVIEWED, .costResource = R_TIME, .costAmount = 2, .requireFlags = F_CASE_OPEN,
     .trackerDelta = 1, .objective = "Interview suspects (-2 Time)"},

    // COMPARE: cross-examine clue pairs; gated on canvass + interview; costs 1 Lead.
    {.title = "COMPARE", .verb = "Link", .rows = ROWS_COMPARE, .rowCount = 8,
     .resourceDeltas = {0, -1, 0, -1, -1, 1, 1, 1}, .progressDelta = 3, .trackerIndex = 3,
     .flagMask = F_LINKED, .costResource = R_LEADS, .costAmount = 1,
     .requireFlags = F_CANVASSED | F_INTERVIEWED,
     .trackerDelta = 1, .objective = "Compare clue pairs (-1 Lead)"},

    // INFORMANTS: buy leads with cash; no flag gate.
    {.title = "INFORMANTS", .verb = "Pay", .rows = ROWS_INFORMANTS, .rowCount = 6,
     .resourceDeltas = {-10, -1, 0, 3, 0, 0, 0, 0}, .progressDelta = 1, .trackerIndex = 4,
     .flagMask = 0, .costResource = R_CASH, .costAmount = 10, .requireFlags = 0,
     .trackerDelta = 1, .objective = "Buy leads (-$10, +3 Leads)"},

    // ACCUSE: resolve case; gated on at least one clue link found (F_LINKED).
    {.title = "ACCUSE", .verb = "Close", .rows = ROWS_ACCUSE, .rowCount = 6,
     .resourceDeltas = {12, -3, 0, 0, 2, 3, 2, 0}, .progressDelta = 6, .trackerIndex = 5,
     .flagMask = F_ACCUSED, .costResource = -1, .costAmount = 0, .requireFlags = F_LINKED,
     .trackerDelta = 1, .objective = "Make accusation (needs Links)"},

    // COURTROOM: meta-case endgame; gated on meta-clue + accused + linked.
    {.title = "COURTROOM", .verb = "Expose", .rows = ROWS_COURTROOM, .rowCount = 5,
     .resourceDeltas = {0, -4, 0, 0, 5, 5, 0, 2}, .progressDelta = 10, .trackerIndex = 5,
     .flagMask = F_EXPOSED, .costResource = -1, .costAmount = 0,
     .requireFlags = F_META_CLUE | F_ACCUSED | F_LINKED,
     .trackerDelta = 1, .objective = "Expose corruption (endgame)"},
};

// ── Events ────────────────────────────────────────────────────────────────────
// Reputation channels: 0=Police, 1=Underworld, 2=Press, 3=Neighborhood
static const CGC::EventDef EVENTS[] = {
    // ── Common filler (no flag gate) ─────────────────────────────────────────
    {.name = "Quiet afternoon",
     .note = "No leads; you reorganize your notes.",
     .resourceDeltas = {0, 1, 0, 0, -1, 0, 0, 0}, .setFlags = 0, .trackerIndex = -1,
     .trackerDelta = 0, .repIndex = -1, .repDelta = 0, .weight = 4,
     .requireFlags = 0, .forbidFlags = 0, .tone = CGC::TONE_NEUTRAL},

    {.name = "Street tip",
     .note = "A stranger hands you a name. +1 Lead.",
     .resourceDeltas = {0, 0, 0, 1, 0, 0, 1, 0}, .setFlags = 0, .trackerIndex = -1,
     .trackerDelta = 0, .repIndex = 3, .repDelta = 1, .weight = 3,
     .requireFlags = 0, .forbidFlags = 0, .tone = CGC::TONE_GOOD},

    {.name = "Dead end",
     .note = "Trail goes cold; wasted half a day.",
     .resourceDeltas = {0, -2, 0, 0, 1, 0, -1, 0}, .setFlags = 0, .trackerIndex = -1,
     .trackerDelta = 0, .repIndex = -1, .repDelta = 0, .weight = 3,
     .requireFlags = 0, .forbidFlags = 0, .tone = CGC::TONE_BAD},

    {.name = "Client check-in",
     .note = "Client is patient. Rep holds steady.",
     .resourceDeltas = {0, 0, 0, 0, 0, 1, 2, 0}, .setFlags = 0, .trackerIndex = -1,
     .trackerDelta = 0, .repIndex = 3, .repDelta = 1, .weight = 3,
     .requireFlags = F_CASE_OPEN, .forbidFlags = 0, .tone = CGC::TONE_GOOD},

    // ── Canvass-gated beats ───────────────────────────────────────────────────
    {.name = "Witness surfaces",
     .note = "Bystander saw it all; adds a clue.",
     .resourceDeltas = {0, 0, 2, 0, 0, 1, 1, 0}, .setFlags = 0, .trackerIndex = 1,
     .trackerDelta = 1, .repIndex = 3, .repDelta = 2, .weight = 3,
     .requireFlags = F_CANVASSED, .forbidFlags = 0, .tone = CGC::TONE_GOOD},

    {.name = "Police lockdown",
     .note = "Cops seal a location; Time lost.",
     .resourceDeltas = {0, -3, 0, 0, 3, 0, 0, 0}, .setFlags = 0, .trackerIndex = -1,
     .trackerDelta = 0, .repIndex = 0, .repDelta = -1, .weight = 2,
     .requireFlags = F_CANVASSED, .forbidFlags = 0, .tone = CGC::TONE_BAD},

    {.name = "Rival plants clue",
     .note = "False evidence slips into your notes.",
     .resourceDeltas = {0, -1, 0, 0, 2, -1, -1, 0}, .setFlags = 0, .trackerIndex = -1,
     .trackerDelta = 0, .repIndex = 2, .repDelta = -1, .weight = 2,
     .requireFlags = F_CANVASSED, .forbidFlags = 0, .tone = CGC::TONE_BAD},

    // ── Interview-gated beats ─────────────────────────────────────────────────
    {.name = "False alibi cracked",
     .note = "Suspect's story doesn't hold. +1 Link.",
     .resourceDeltas = {0, 0, 1, 0, 0, 1, 0, 1}, .setFlags = 0, .trackerIndex = 3,
     .trackerDelta = 1, .repIndex = 0, .repDelta = 1, .weight = 3,
     .requireFlags = F_INTERVIEWED, .forbidFlags = 0, .tone = CGC::TONE_GOOD},

    {.name = "Suspect goes silent",
     .note = "Lawyer called; interview ends early.",
     .resourceDeltas = {0, -1, 0, 0, 2, 0, -2, 0}, .setFlags = 0, .trackerIndex = -1,
     .trackerDelta = 0, .repIndex = 0, .repDelta = -1, .weight = 2,
     .requireFlags = F_INTERVIEWED, .forbidFlags = 0, .tone = CGC::TONE_BAD},

    {.name = "Bribe offered",
     .note = "Suspect slides cash across the table.",
     .resourceDeltas = {8, 0, 0, 0, 0, -2, -1, 0}, .setFlags = F_DIRTY_MONEY,
     .trackerIndex = -1, .trackerDelta = 0, .repIndex = 1, .repDelta = 2, .weight = 1,
     .requireFlags = F_INTERVIEWED, .forbidFlags = F_DIRTY_MONEY, .tone = CGC::TONE_BAD},

    // ── Compare-gated beats ───────────────────────────────────────────────────
    {.name = "Contradiction found",
     .note = "Two clues clash; the truth emerges.",
     .resourceDeltas = {0, 0, 0, 0, -1, 2, 1, 1}, .setFlags = 0, .trackerIndex = 3,
     .trackerDelta = 1, .repIndex = 2, .repDelta = 1, .weight = 3,
     .requireFlags = F_LINKED, .forbidFlags = 0, .tone = CGC::TONE_GOOD},

    {.name = "Press leak",
     .note = "Story hits the papers before you're ready.",
     .resourceDeltas = {0, -1, 0, 0, 4, 2, -1, 0}, .setFlags = 0, .trackerIndex = -1,
     .trackerDelta = 0, .repIndex = 2, .repDelta = 3, .weight = 2,
     .requireFlags = F_LINKED, .forbidFlags = 0, .tone = CGC::TONE_BAD},

    // ── Meta-corruption beat (unlocks the courtroom endgame flag) ─────────────
    {.name = "City hall document",
     .note = "A sealed memo links three old cases.",
     .resourceDeltas = {0, -1, 1, 0, 2, 1, 0, 0}, .setFlags = F_META_CLUE,
     .trackerIndex = -1, .trackerDelta = 0, .repIndex = 0, .repDelta = 1, .weight = 2,
     .requireFlags = F_ACCUSED, .forbidFlags = F_META_CLUE, .tone = CGC::TONE_GOOD},

    // ── Client-pressure event ─────────────────────────────────────────────────
    {.name = "Client pressure",
     .note = "Client demands result; wants truth buried.",
     .resourceDeltas = {5, 0, 0, 0, 0, -1, -2, 0}, .setFlags = F_DIRTY_MONEY,
     .trackerIndex = -1, .trackerDelta = 0, .repIndex = 1, .repDelta = 2, .weight = 1,
     .requireFlags = F_CASE_OPEN, .forbidFlags = F_DIRTY_MONEY, .tone = CGC::TONE_BAD},

    // ── Late-game breakthrough ────────────────────────────────────────────────
    {.name = "Breakthrough moment",
     .note = "Everything clicks; notebook fills fast.",
     .resourceDeltas = {0, 0, 2, 1, -1, 1, 1, 1}, .setFlags = 0, .trackerIndex = 3,
     .trackerDelta = 1, .repIndex = -1, .repDelta = 0, .weight = 2,
     .requireFlags = F_LINKED, .forbidFlags = 0, .tone = CGC::TONE_GOOD},

    // ── Underworld favor ──────────────────────────────────────────────────────
    {.name = "Underworld favor",
     .note = "A fixer clears your heat; costs nothing.",
     .resourceDeltas = {0, 0, 0, 1, -4, 0, 0, 0}, .setFlags = 0, .trackerIndex = -1,
     .trackerDelta = 0, .repIndex = 1, .repDelta = 2, .weight = 1,
     .requireFlags = F_DIRTY_MONEY, .forbidFlags = 0, .tone = CGC::TONE_NEUTRAL},
};

// ── Endings (most-specific first) ────────────────────────────────────────────
// Reputation channels: 0=Police, 1=Underworld, 2=Press, 3=Neighborhood
static const CGC::EndingDef ENDINGS[] = {
    // 1. True Case: exposed corruption ring, no dirty money, high links.
    {.name = "True Case",
     .note = "The city hears the full story. Justice, at last.",
     .cond = {.resourceIndex = R_LINKS, .resourceMin = 4, .resourceMax = 30,
              .trackerIndex = -1, .trackerMin = 0, .repIndex = -1, .repMin = 0,
              .requireFlags = F_EXPOSED | F_META_CLUE, .forbidFlags = F_DIRTY_MONEY,
              .requiresVictory = true}},

    // 2. Honest Agency: clean run, good rep, no corruption.
    {.name = "Honest Agency",
     .note = "Your name stands for something real in this city.",
     .cond = {.resourceIndex = R_REP, .resourceMin = 45, .resourceMax = 100,
              .trackerIndex = -1, .trackerMin = 0, .repIndex = -1, .repMin = 0,
              .requireFlags = F_ACCUSED, .forbidFlags = F_DIRTY_MONEY,
              .requiresVictory = true}},

    // 3. Famous Sleuth: high rep + high press, dirty money allowed.
    {.name = "Famous Sleuth",
     .note = "Front page every week; the city loves you.",
     .cond = {.resourceIndex = R_REP, .resourceMin = 55, .resourceMax = 100,
              .trackerIndex = 2, .trackerMin = 5, .repIndex = 2, .repMin = 3,
              .requireFlags = F_ACCUSED, .forbidFlags = 0,
              .requiresVictory = true}},

    // 4. Quiet Fixer: used dirty money, cases closed quietly.
    {.name = "Quiet Fixer",
     .note = "Solved it all, your way. No headlines.",
     .cond = {.resourceIndex = R_CASH, .resourceMin = 30, .resourceMax = 999,
              .trackerIndex = -1, .trackerMin = 0, .repIndex = -1, .repMin = 0,
              .requireFlags = F_ACCUSED | F_DIRTY_MONEY, .forbidFlags = 0,
              .requiresVictory = true}},

    // 5. Wrong Arrest: accused without enough links.
    {.name = "Wrong Arrest",
     .note = "Wrong person in cuffs. Case collapses.",
     .cond = {.resourceIndex = R_LINKS, .resourceMin = 0, .resourceMax = 1,
              .trackerIndex = -1, .trackerMin = 0, .repIndex = -1, .repMin = 0,
              .requireFlags = F_ACCUSED, .forbidFlags = 0,
              .requiresVictory = true}},

    // 6. Catch-all victory.
    {.name = "Cold File",
     .note = "A partial truth filed quietly in the archives.",
     .cond = {.resourceIndex = -1, .resourceMin = 0, .resourceMax = 0,
              .trackerIndex = -1, .trackerMin = 0, .repIndex = -1, .repMin = 0,
              .requireFlags = 0, .forbidFlags = 0,
              .requiresVictory = true}},

    // 7. Catch-all loss (Time ran out).
    {.name = "Out of Time",
     .note = "Deadline passed. The case closes without you.",
     .cond = {.resourceIndex = -1, .resourceMin = 0, .resourceMax = 0,
              .trackerIndex = -1, .trackerMin = 0, .repIndex = -1, .repMin = 0,
              .requireFlags = 0, .forbidFlags = 0,
              .requiresVictory = false}},
};

// ── Tracker labels ─────────────────────────────────────────────────────────
static const char* const TRACKER_LABELS[] = {
    "Cases", "Canvass", "Interviews", "Links", "Contacts", "Closed", nullptr, nullptr,
};

// ── Game definition ───────────────────────────────────────────────────────────
static const CGC::DeepGameDefinition DEF = {
    .title    = "Pocket Detective",
    .slug     = "pocket-detective-agency",
    .subtitle = "Notebook mystery campaign",
    .saveMagic = "PDA2",
    .resources     = RESOURCES,
    .resourceCount = sizeof(RESOURCES) / sizeof(RESOURCES[0]),
    .screens       = SCREENS,
    .screenCount   = sizeof(SCREENS) / sizeof(SCREENS[0]),
    .events        = EVENTS,
    .eventCount    = sizeof(EVENTS) / sizeof(EVENTS[0]),
    .endings       = ENDINGS,
    .endingCount   = sizeof(ENDINGS) / sizeof(ENDINGS[0]),
    .targetProgress = 140,
    .accentColor  = 0xFFFF,
    .warningColor = 0xFD20,
    .trackerLabels     = TRACKER_LABELS,
    .trackerLabelCount = 8,
    .objective    = "Solve cases; expose city corruption.",
    .primaryTracker = 5,
};

const CGC::DeepGameDefinition& gameDefinition() {
  return DEF;
}
