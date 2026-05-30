#include "GameData.h"

namespace CGC = CardputerGameCore;

// Resource indices — keep in sync with RESOURCES order.
enum {
  R_POWER,    // primary pressure: depletion ends the run
  R_FUEL,     // regenerates power; spent by POWER screen
  R_SANITY,   // high is good; reaching min ends run
  R_SIGNAL,   // clues / signal log count
  R_STATION,  // antenna + door + recorder integrity
  R_DREAD,    // high is bad — inverted
  R_CASES,    // resolved cases counter
  R_TRUST,    // caller confidence
};

// Story flags — screens set these; events advance the arc.
enum : uint32_t {
  F_TUNED        = 1u << 0,  // player has tuned at least one band
  F_DECODED      = 1u << 1,  // player has decoded at least one signal
  F_RESPONDED    = 1u << 2,  // player has responded to at least one signal
  F_ENTITY_KNOWN = 1u << 3,  // entity presence confirmed in logs
  F_CASE_LINKED  = 1u << 4,  // two or more cases share a caller
  F_CHANNEL_OPEN = 1u << 5,  // hidden channel unlocked
  F_REFUSED      = 1u << 6,  // player refused entity false instructions
  F_TRUTH_FOUND  = 1u << 7,  // contradictions resolved into truth broadcast
  F_OBEYED       = 1u << 8,  // player followed entity false instructions
};

// --- Resources -----------------------------------------------------------

static const CGC::ResourceDef RESOURCES[] = {
    // label    start  min  max  pDelta  safe  warn  bad   inverted
    {"Power",    75,    0,  100,  -3,     5,   25,   10,  false},  // loss trigger
    {"Fuel",     50,    0,   99,   0,     0,   15,    5,  false},
    {"Sanity",   80,    0,  100,  -2,     4,   30,   15,  false},  // loss trigger (idx=2, pDelta<0)
    {"Signal",    0,    0,   99,   0,     0,   -1,   -1,  false},  // counter
    {"Station",  90,    0,  100,   0,     0,   35,   15,  false},
    {"Dread",     5,    0,  100,   2,    -3,   50,   75,  true},   // inverted: high is bad
    {"Cases",     0,    0,   99,   0,     0,   -1,   -1,  false},  // counter
    {"Trust",    30,    0,  100,   0,     1,   12,    4,  false},
};

// --- Screen rows ---------------------------------------------------------

// SHIFT: begin the nightly shift; costs nothing but sets pressure tone
static const CGC::NamedValue ROWS_SHIFT[] = {
    {"Dawn Watch",     "Pre-dawn: low noise, generator stable."},
    {"Fog Shift",      "Heavy static; some bands smeared."},
    {"Storm Shift",    "Rain on the roof; antenna stressed."},
    {"Cold Night",     "Heater load up; power drains faster."},
    {"Tape Review",    "Re-read logged tapes, plan the night."},
    {"Tower Check",    "Inspect antenna and door before tuning."},
    {"Numbers Night",  "Number station active; decode window."},
    {"Quiet Alert",    "No traffic; entity may be mimicking."},
};

// TUNE: scan bands — costs Power; sets F_TUNED; finds signals
static const CGC::NamedValue ROWS_TUNE[] = {
    {"Low Band",       "Scan 1.6–2.0 MHz locals and callers."},
    {"Ridge Band",     "Sweep 4.0–5.0 MHz mountain relays."},
    {"Lake Channel",   "Tune 6.2 MHz: known distress channel."},
    {"Numbers Band",   "Seek 7.4 MHz: one-way ciphered strings."},
    {"Dawn Freq",      "Listen 9.1 MHz: rare, hidden callers."},
    {"Replay Scan",    "Compare tonight vs. last-shift scan."},
    {"Carrier Sweep",  "Sweep full range for unknown carriers."},
    {"Silent Tune",    "Monitor 11.0 MHz without transmitting."},
};

// DECODE: run decode tools — costs Power; needs a signal; sets F_DECODED
static const CGC::NamedValue ROWS_DECODE[] = {
    {"Callsign Match", "Match tonight's sign against the log."},
    {"Reverse Phrase", "Run the recording backward for tell."},
    {"Grid Locate",    "Convert numbers to map coordinates."},
    {"Tap Code",       "Transcribe the tap pattern by rhythm."},
    {"Weather Cross",  "Cross-ref barometric log for cover."},
    {"Replay Diff",    "Diff two recordings frame by frame."},
    {"Cipher Sub",     "Apply substitution key from Case 4."},
    {"Freq Hop Trace", "Follow the hopping carrier to source."},
};

// RESPOND: relay/ignore/lie/triangulate — moral; sets F_RESPONDED; flag effects
static const CGC::NamedValue ROWS_RESPOND[] = {
    {"Relay Help",     "Forward distress call to field team."},
    {"Ignore Signal",  "Log it; take no action this shift."},
    {"Send Decoy",     "Broadcast false position to confuse."},
    {"Triangulate",    "Cross two bearings to fix true source."},
    {"Correct Coords", "Broadcast corrected grid to caller."},
    {"Refuse Entity",  "Decline the false instruction packet."},
    {"Stay Silent",    "Kill transmitter; monitor only."},
    {"Warn Caller",    "Tell caller the signal may be faked."},
};

// POWER: route generator / fuel — costs Fuel; restores Power; sets station
static const CGC::NamedValue ROWS_POWER[] = {
    {"Top Up Generator","Add fuel; restore power headroom."},
    {"Shed Recorder",  "Cut recorder draw; save 8W of load."},
    {"Shed Floodlight","Kill exterior lights; save power."},
    {"Reroute Antenna","Reduce antenna draw to bare minimum."},
    {"Emergency Batt", "Tap battery reserve; one-time boost."},
    {"Heater Off",     "Sacrifice warmth; sanity will drop."},
    {"Full Restore",   "Refuel + reroute for full headroom."},
    {"Ration Mode",    "Throttle all systems to low draw."},
};

// CASES: resolve linked mysteries — gated on Signal clue count
static const CGC::NamedValue ROWS_CASES[] = {
    {"Hiker Missing",  "Lost hiker called Ridge Band Day 3."},
    {"Mill Transmit",  "Old mill keys itself every 04:00."},
    {"Camp Nine",      "Camp 9 beacon misfired with a voice."},
    {"Lake Voice",     "Caller at lake matches two prior logs."},
    {"Tunnel Ping",    "Ping from Tunnel 7 has no listed owner."},
    {"Numbers Caller", "Number station cited a real name twice."},
    {"Black Pine",     "Black Pine calls loop with wrong year."},
    {"Link Cases",     "Connect cross-referencing case pairs."},
};

// ENTITY (hidden channel) — locked until F_CHANNEL_OPEN
static const CGC::NamedValue ROWS_ENTITY[] = {
    {"Listen Only",    "Monitor entity channel without reply."},
    {"Demand Source",  "Challenge entity to identify itself."},
    {"Broadcast Truth","Transmit truth log on entity channel."},
    {"Seal Channel",   "Lock out entity band permanently."},
    {"Lure Pattern",   "Replay its own signal back at it."},
    {"Jam Frequency",  "Burn power to drown entity carrier."},
};

// --- Screens -------------------------------------------------------------

static const CGC::DeepScreenDef SCREENS[] = {
    // SHIFT: free; sets a flag, minor stress on power/sanity, boosts trust/station
    {.title = "SHIFT", .verb = "Begin", .rows = ROWS_SHIFT, .rowCount = 8,
     .resourceDeltas = {-1, 0, -1, 0, 1, 1, 0, 1}, .progressDelta = 2,
     .trackerIndex = 0,  // Shifts tracker
     .flagMask = 0, .costResource = -1, .costAmount = 0, .requireFlags = 0,
     .trackerDelta = 1, .objective = "Start shift; review station status"},

    // TUNE: costs 6 Power; sets F_TUNED; gains Signal
    {.title = "TUNE", .verb = "Tune", .rows = ROWS_TUNE, .rowCount = 8,
     .resourceDeltas = {-6, 0, -1, 1, 0, 1, 0, 0}, .progressDelta = 3,
     .trackerIndex = 1,  // Signals tracker
     .flagMask = F_TUNED, .costResource = R_POWER, .costAmount = 6, .requireFlags = 0,
     .trackerDelta = 1, .objective = "Scan bands for signals (-6 Power)"},

    // DECODE: costs 8 Power; needs F_TUNED; sets F_DECODED; gains Signal clues
    {.title = "DECODE", .verb = "Decode", .rows = ROWS_DECODE, .rowCount = 8,
     .resourceDeltas = {-8, 0, -2, 2, 0, 0, 0, 1}, .progressDelta = 4,
     .trackerIndex = 2,  // Decodes tracker
     .flagMask = F_DECODED, .costResource = R_POWER, .costAmount = 8, .requireFlags = F_TUNED,
     .trackerDelta = 1, .objective = "Decode signals (needs TUNE, -8 Power)"},

    // RESPOND: costs 5 Power; needs F_TUNED; sets F_RESPONDED; moral effects
    {.title = "RESPOND", .verb = "Respond", .rows = ROWS_RESPOND, .rowCount = 8,
     .resourceDeltas = {-5, 0, 1, 0, 0, -1, 0, 2}, .progressDelta = 3,
     .trackerIndex = 4,  // Responses tracker
     .flagMask = F_RESPONDED, .costResource = R_POWER, .costAmount = 5, .requireFlags = F_TUNED,
     .trackerDelta = 1, .objective = "Respond to callers (needs TUNE, -5 Power)"},

    // POWER: costs 8 Fuel; restores Power; protects Station
    {.title = "POWER", .verb = "Reroute", .rows = ROWS_POWER, .rowCount = 8,
     .resourceDeltas = {8, -8, 0, 0, 2, -2, 0, 0}, .progressDelta = 1,
     .trackerIndex = 6,  // spare tracker slot; not labeled
     .flagMask = 0, .costResource = R_FUEL, .costAmount = 8, .requireFlags = 0,
     .trackerDelta = 0, .objective = "Manage generator and fuel (-8 Fuel)"},

    // CASES: free; needs F_DECODED + F_RESPONDED; big progress; gains Cases
    {.title = "CASES", .verb = "Resolve", .rows = ROWS_CASES, .rowCount = 8,
     .resourceDeltas = {0, 0, 2, -1, 0, -2, 1, 3}, .progressDelta = 7,
     .trackerIndex = 3,  // Cases tracker
     .flagMask = F_CASE_LINKED, .costResource = -1, .costAmount = 0,
     .requireFlags = F_DECODED | F_RESPONDED,
     .trackerDelta = 1, .objective = "Resolve cases (needs DECODE+RESPOND)"},

    // ENTITY: hidden channel — locked until F_CHANNEL_OPEN
    {.title = "ENTITY", .verb = "Confront", .rows = ROWS_ENTITY, .rowCount = 6,
     .resourceDeltas = {-10, 0, -3, 0, -3, 5, 0, -2}, .progressDelta = 8,
     .trackerIndex = 5,  // Channels tracker
     .flagMask = F_TRUTH_FOUND, .costResource = R_POWER, .costAmount = 10,
     .requireFlags = F_CHANNEL_OPEN,
     .trackerDelta = 1, .objective = "Confront entity channel (gated)"},
};

// --- Events --------------------------------------------------------------

static const CGC::EventDef EVENTS[] = {
    // Common fillers
    {.name = "Quiet static", .note = "Bands clear; generator holds steady.",
     .resourceDeltas = {2, 1, 1, 0, 0, -1, 0, 0}, .setFlags = 0,
     .trackerIndex = -1, .trackerDelta = 0, .repIndex = -1, .repDelta = 0,
     .weight = 4, .requireFlags = 0, .forbidFlags = 0, .tone = CGC::TONE_NEUTRAL},

    {.name = "Fuel drum found", .note = "Reserve drum in shed still half-full.",
     .resourceDeltas = {0, 8, 0, 0, 0, 0, 0, 0}, .setFlags = 0,
     .trackerIndex = -1, .trackerDelta = 0, .repIndex = -1, .repDelta = 0,
     .weight = 3, .requireFlags = 0, .forbidFlags = 0, .tone = CGC::TONE_GOOD},

    {.name = "Clear caller", .note = "A real voice; coordinates confirmed clean.",
     .resourceDeltas = {0, 0, 1, 1, 0, -1, 0, 2}, .setFlags = 0,
     .trackerIndex = 1, .trackerDelta = 1, .repIndex = 0, .repDelta = 1,
     .weight = 3, .requireFlags = F_TUNED, .forbidFlags = 0, .tone = CGC::TONE_GOOD},

    {.name = "Power spike", .note = "Surge trips the recorder breaker.",
     .resourceDeltas = {-6, 0, -1, 0, -4, 2, 0, 0}, .setFlags = 0,
     .trackerIndex = -1, .trackerDelta = 0, .repIndex = -1, .repDelta = 0,
     .weight = 2, .requireFlags = 0, .forbidFlags = 0, .tone = CGC::TONE_BAD},

    {.name = "Antenna ice", .note = "Ice load bends the mast; signal degrades.",
     .resourceDeltas = {-3, 0, -2, 0, -6, 3, 0, -1}, .setFlags = 0,
     .trackerIndex = -1, .trackerDelta = 0, .repIndex = -1, .repDelta = 0,
     .weight = 2, .requireFlags = 0, .forbidFlags = 0, .tone = CGC::TONE_BAD},

    {.name = "False bearing", .note = "Caller's grid is off by one digit.",
     .resourceDeltas = {0, 0, -2, 0, 0, 2, 0, -2}, .setFlags = 0,
     .trackerIndex = -1, .trackerDelta = 0, .repIndex = 1, .repDelta = -1,
     .weight = 2, .requireFlags = F_TUNED, .forbidFlags = 0, .tone = CGC::TONE_BAD},

    {.name = "Number station", .note = "Ciphered string loops on the 7.4 band.",
     .resourceDeltas = {0, 0, -1, 2, 0, 1, 0, 0}, .setFlags = 0,
     .trackerIndex = 1, .trackerDelta = 1, .repIndex = -1, .repDelta = 0,
     .weight = 3, .requireFlags = F_TUNED, .forbidFlags = 0, .tone = CGC::TONE_NEUTRAL},

    {.name = "Familiar voice", .note = "Voice matches a prior caller; sign is wrong.",
     .resourceDeltas = {0, 0, -3, 0, 0, 3, 0, -2}, .setFlags = F_ENTITY_KNOWN,
     .trackerIndex = -1, .trackerDelta = 0, .repIndex = 2, .repDelta = -1,
     .weight = 2, .requireFlags = F_DECODED, .forbidFlags = F_ENTITY_KNOWN,
     .tone = CGC::TONE_BAD},

    {.name = "Log contradiction", .note = "Two callsigns match; one caller is fake.",
     .resourceDeltas = {0, 0, -2, 1, 0, 2, 0, 0}, .setFlags = F_ENTITY_KNOWN,
     .trackerIndex = 1, .trackerDelta = 1, .repIndex = -1, .repDelta = 0,
     .weight = 2, .requireFlags = F_RESPONDED, .forbidFlags = F_ENTITY_KNOWN,
     .tone = CGC::TONE_BAD},

    {.name = "Correct rescue", .note = "Field team confirms the grid; hiker safe.",
     .resourceDeltas = {0, 0, 3, 0, 0, -3, 1, 4}, .setFlags = 0,
     .trackerIndex = 3, .trackerDelta = 1, .repIndex = 0, .repDelta = 2,
     .weight = 2, .requireFlags = F_RESPONDED | F_DECODED, .forbidFlags = 0,
     .tone = CGC::TONE_GOOD},

    {.name = "Weather surge", .note = "Storm drives static through every band.",
     .resourceDeltas = {-4, 0, -1, 0, -2, 4, 0, 0}, .setFlags = 0,
     .trackerIndex = -1, .trackerDelta = 0, .repIndex = -1, .repDelta = 0,
     .weight = 2, .requireFlags = 0, .forbidFlags = 0, .tone = CGC::TONE_BAD},

    {.name = "Entity log attack", .note = "Tape counter spins; entry corrupted.",
     .resourceDeltas = {-2, 0, -3, -1, -3, 4, 0, -1}, .setFlags = 0,
     .trackerIndex = -1, .trackerDelta = 0, .repIndex = 2, .repDelta = -2,
     .weight = 2, .requireFlags = F_ENTITY_KNOWN, .forbidFlags = 0,
     .tone = CGC::TONE_BAD},

    {.name = "Entity power drain", .note = "Carrier bleeds 20W off the bus.",
     .resourceDeltas = {-8, -3, -2, 0, 0, 5, 0, 0}, .setFlags = 0,
     .trackerIndex = -1, .trackerDelta = 0, .repIndex = -1, .repDelta = 0,
     .weight = 2, .requireFlags = F_ENTITY_KNOWN, .forbidFlags = F_REFUSED,
     .tone = CGC::TONE_BAD},

    {.name = "Cases link", .note = "Caller from Case 2 matches Case 5 grid.",
     .resourceDeltas = {0, 0, 1, 1, 0, -1, 0, 1}, .setFlags = F_CASE_LINKED,
     .trackerIndex = 3, .trackerDelta = 1, .repIndex = -1, .repDelta = 0,
     .weight = 2, .requireFlags = F_DECODED | F_RESPONDED, .forbidFlags = F_CASE_LINKED,
     .tone = CGC::TONE_GOOD},

    {.name = "Hidden channel open", .note = "Third linked log unlocks the band.",
     .resourceDeltas = {0, 0, 0, 1, 0, 2, 0, 0}, .setFlags = F_CHANNEL_OPEN,
     .trackerIndex = 5, .trackerDelta = 1, .repIndex = -1, .repDelta = 0,
     .weight = 3, .requireFlags = F_CASE_LINKED, .forbidFlags = F_CHANNEL_OPEN,
     .tone = CGC::TONE_GOOD},

    {.name = "Entity false order", .note = "Packet orders you to relay wrong coords.",
     .resourceDeltas = {0, 0, -3, 0, 0, 4, 0, -3}, .setFlags = F_OBEYED,
     .trackerIndex = -1, .trackerDelta = 0, .repIndex = 3, .repDelta = -2,
     .weight = 2, .requireFlags = F_CHANNEL_OPEN, .forbidFlags = F_REFUSED,
     .tone = CGC::TONE_BAD},

    {.name = "Breakthrough decode", .note = "Cipher solved: source is the old tower.",
     .resourceDeltas = {0, 0, 2, 2, 0, -2, 0, 2}, .setFlags = F_TRUTH_FOUND,
     .trackerIndex = 2, .trackerDelta = 1, .repIndex = 1, .repDelta = 2,
     .weight = 3, .requireFlags = F_CASE_LINKED | F_REFUSED, .forbidFlags = F_TRUTH_FOUND,
     .tone = CGC::TONE_GOOD},
};

// --- Endings -------------------------------------------------------------
// Most-specific first; first match wins. Victory flag true = win context.

static const CGC::EndingDef ENDINGS[] = {
    // 1. Truth Broadcast — best: solved cases, refused entity, found truth
    {.name = "Truth Broadcast",
     .note = "You broadcast the tower source on every band. They hear you.",
     .cond = {.resourceIndex = R_TRUST, .resourceMin = 40, .resourceMax = 100,
              .trackerIndex = 3, .trackerMin = 4,
              .repIndex = -1, .repMin = 0,
              .requireFlags = F_TRUTH_FOUND | F_REFUSED,
              .forbidFlags = F_OBEYED,
              .requiresVictory = true}},

    // 2. Silent Evacuation — survived without confronting; cautious win
    {.name = "Silent Evacuation",
     .note = "You sealed the channel, packed the logs, and walked out at dawn.",
     .cond = {.resourceIndex = R_SANITY, .resourceMin = 50, .resourceMax = 100,
              .trackerIndex = -1, .trackerMin = 0,
              .repIndex = -1, .repMin = 0,
              .requireFlags = F_REFUSED,
              .forbidFlags = F_TRUTH_FOUND,
              .requiresVictory = true}},

    // 3. Possessed Station — obeyed entity; station condition survives but corrupted
    {.name = "Possessed Station",
     .note = "The logs say your voice. The orders were not yours.",
     .cond = {.resourceIndex = R_STATION, .resourceMin = 50, .resourceMax = 100,
              .trackerIndex = -1, .trackerMin = 0,
              .repIndex = -1, .repMin = 0,
              .requireFlags = F_OBEYED,
              .forbidFlags = F_REFUSED,
              .requiresVictory = true}},

    // 4. Wrong Rescue — responded without decoding; sent help to wrong location
    {.name = "Wrong Rescue",
     .note = "The team found an empty grid. Someone went the wrong way.",
     .cond = {.resourceIndex = R_TRUST, .resourceMin = 0, .resourceMax = 20,
              .trackerIndex = -1, .trackerMin = 0,
              .repIndex = -1, .repMin = 0,
              .requireFlags = F_RESPONDED,
              .forbidFlags = F_DECODED,
              .requiresVictory = true}},

    // 5. Sealed Channel — reached entity channel but locked it without broadcast
    {.name = "Sealed Channel",
     .note = "Band 11 is cold. Whatever was there cannot key up again.",
     .cond = {.resourceIndex = -1, .resourceMin = 0, .resourceMax = 0,
              .trackerIndex = 5, .trackerMin = 1,
              .repIndex = -1, .repMin = 0,
              .requireFlags = F_CHANNEL_OPEN,
              .forbidFlags = F_TRUTH_FOUND,
              .requiresVictory = true}},

    // 6. Catch-all victory
    {.name = "Dawn Operator",
     .note = "You held the shift. The mystery fades with the static at dawn.",
     .cond = {.resourceIndex = -1, .resourceMin = 0, .resourceMax = 0,
              .trackerIndex = -1, .trackerMin = 0,
              .repIndex = -1, .repMin = 0,
              .requireFlags = 0, .forbidFlags = 0,
              .requiresVictory = true}},

    // 7. Catch-all loss — Lost Operator
    {.name = "Lost Operator",
     .note = "Power fell. Static rose. No one heard the last transmission.",
     .cond = {.resourceIndex = -1, .resourceMin = 0, .resourceMax = 0,
              .trackerIndex = -1, .trackerMin = 0,
              .repIndex = -1, .repMin = 0,
              .requireFlags = 0, .forbidFlags = 0,
              .requiresVictory = false}},
};

// --- Tracker labels ------------------------------------------------------

static const char* const TRACKER_LABELS[] = {
    "Shifts",    // 0 — bumped by SHIFT
    "Signals",   // 1 — bumped by TUNE + certain events
    "Decodes",   // 2 — bumped by DECODE
    "Cases",     // 3 — bumped by CASES
    "Responses", // 4 — bumped by RESPOND
    "Channels",  // 5 — bumped by ENTITY
    nullptr,
    nullptr,
};

// --- Game definition -----------------------------------------------------

static const CGC::DeepGameDefinition DEF = {
    .title          = "Haunted Radio",
    .slug           = "haunted-radio-operator",
    .subtitle       = "Fictional station mystery",
    .saveMagic      = "HRO2",
    .resources      = RESOURCES,
    .resourceCount  = sizeof(RESOURCES) / sizeof(RESOURCES[0]),
    .screens        = SCREENS,
    .screenCount    = sizeof(SCREENS) / sizeof(SCREENS[0]),
    .events         = EVENTS,
    .eventCount     = sizeof(EVENTS) / sizeof(EVENTS[0]),
    .endings        = ENDINGS,
    .endingCount    = sizeof(ENDINGS) / sizeof(ENDINGS[0]),
    .targetProgress = 165,
    .accentColor    = 0xBDF7,
    .warningColor   = 0xFD20,
    .trackerLabels      = TRACKER_LABELS,
    .trackerLabelCount  = 8,
    .objective      = "Solve the mystery; refuse the entity.",
    .primaryTracker = 3,  // Cases resolved shown as hub headline
};

const CGC::DeepGameDefinition& gameDefinition() {
  return DEF;
}
