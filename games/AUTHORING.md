# Authoring a deep game (`GameData.cpp`)

Every game is one `CardputerGameCore::DeepGameDefinition` built from static const
tables in `games/<name>/GameData.cpp`. The shared engine
(`libraries/CardputerGameCore`) turns those tables into a real loop:

- The **hub** lists your screens. Each screen is a *system* (patrol, market, deck…).
- Picking a screen opens a **detail** list of action **rows**.
- Choosing a row runs `applyRow`: it spends the screen's cost (if any), applies the
  screen's resource deltas, rolls a **weighted, flag-gated event** that applies its
  own deltas/flags/tracker/reputation, then shows a **result** screen with the real
  per-turn deltas and the event outcome.
- The run **wins** at `targetProgress` and **loses** if one of the first four
  resources with a negative `primaryDelta` hits its `minValue`. The **ending** is
  chosen by scanning `ENDINGS` top-to-bottom for the first matching condition.

Use `games/cyber_ranger/GameData.cpp` as the canonical worked example.

## Hard rules
1. **Use designated initializers** (`.field = value`) for `DeepScreenDef`,
   `EventDef`, `EndingDef`/`EndingCond`, and `DeepGameDefinition`. C++ requires the
   fields in declaration order — keep the order shown below.
2. `resourceDeltas[]` (on screens and events) must have **exactly 8 entries**; pad
   unused trailing slots with `0`. Order matches your `RESOURCES` array.
3. Unused tracker/reputation references use sentinel **`-1`**.
4. Endings: list **most-specific first**; the first match wins. Always end with a
   catch-all **victory** ending (`resourceIndex=-1`, `requireFlags=0`) and a catch-all
   **loss** ending (`requiresVictory=false`).
5. Keep event `resourceDeltas` small (±1..±6) so a single turn cannot drain a primary
   resource from healthy to instant-loss.
6. Keep strings short: row notes ~30–40 chars, event notes ≤ ~40 chars, objective
   lines ≤ ~37 chars (240×135 screen, ~37 chars/line).

## Structures (field order is mandatory)

```cpp
ResourceDef { label, start, minValue, maxValue, primaryDelta, safeDelta,
              warnBelow, badBelow, inverted }
```
- A resource is a **loss trigger** only if it is one of the first four AND has
  `primaryDelta < 0` (then hitting `minValue` ends the run). Give your one survival
  resource a negative `primaryDelta`; set others to `0`/positive so they don't kill.
- Health colors: normal resource is amber at `<= warnBelow`, red at `<= badBelow`.
  Set `inverted=true` for resources where high is bad (Heat, Debt) — then amber is
  `>= warnBelow`, red `>= badBelow`. Use `-1` thresholds to never warn (e.g. counters).

```cpp
DeepScreenDef { title, verb, rows, rowCount, resourceDeltas[8], progressDelta,
                trackerIndex, flagMask, costResource, costAmount, requireFlags,
                trackerDelta, objective }
```
- `flagMask` is set **cleanly** when any row on the screen is taken — use it to mark
  progress (e.g. "gathered evidence") that other screens gate on.
- `costResource`/`costAmount`: if set (`costResource >= 0`), the action needs and
  spends that much; if the player can't afford it the turn fails (no day spent) with a
  "Need X" message. `-1` = free.
- `requireFlags`: screen is locked until ALL these flags are set; taking an action on
  a locked screen fails with "Locked".
- `trackerDelta`: per-action bump of `trackers[trackerIndex]` (0 means 1).
- `objective`: one-line goal shown on the detail footer.

```cpp
EventDef { name, note, resourceDeltas[8], setFlags, trackerIndex, trackerDelta,
           repIndex, repDelta, weight, requireFlags, forbidFlags, tone }
```
- Rolled after every successful action, **weighted** by `weight` and eligible only if
  ALL `requireFlags` are set and NONE of `forbidFlags` are. Gate story beats behind
  flags (e.g. an endgame event with `requireFlags = F_PROOF`).
- `setFlags` advances the arc; `repIndex/repDelta` move a reputation channel (0–3).
- `tone` is `CGC::TONE_GOOD | TONE_NEUTRAL | TONE_BAD` and colors the result line.

```cpp
EndingCond { resourceIndex, resourceMin, resourceMax, trackerIndex, trackerMin,
             repIndex, repMin, requireFlags, forbidFlags, requiresVictory }
EndingDef  { name, note, cond }
```
- A clause is skipped when its index is `-1`. `requiresVictory` must match the run's
  win/loss context. First satisfied ending (top-down) is chosen.

```cpp
DeepGameDefinition { title, slug, subtitle, saveMagic, resources, resourceCount,
                     screens, screenCount, events, eventCount, endings, endingCount,
                     targetProgress, accentColor, warningColor,
                     trackerLabels, trackerLabelCount, objective, primaryTracker }
```
- `trackerLabels`: `const char* const[]` up to 8 (null entries hidden). Shown on the
  Status screen and the hub headline (`primaryTracker`).
- **Do not change** `slug`, `saveMagic`, `accentColor`, `warningColor` — the launcher,
  save paths, and per-slug theme depend on them.

## Design checklist per game
- 5–8 screens that map to the game's real systems (from `gameplans/<n>-<slug>.md`).
- A clear gated progression: early screens set flags that later screens require.
- One survival resource (negative `primaryDelta`) that creates loss pressure; one or
  two "currency" resources spent via screen costs; one or two "evidence/progress"
  counters that endings read.
- 10–16 events: a few common neutral/good fillers plus gated story beats; tuned so the
  economy bites but isn't unfair.
- 6+ endings covering the brief's branches, ordered specific→general, with catch-alls.
- Thematic, non-repeteated row names/notes (no `#1 / #2` filler).
