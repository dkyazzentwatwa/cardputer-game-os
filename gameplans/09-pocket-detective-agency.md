# Pocket Detective Agency

## Pitch

Pocket Detective Agency is a mystery investigation RPG where the player runs a
small agency, accepts cases, visits locations, interviews suspects, collects
clues, compares contradictions, and makes accusations. It fits the Cardputer
because mysteries are naturally text-first, keyboard-friendly, and episodic.

## Target Experience

- Total playtime target: 5 to 10+ hours across 8 to 12 cases.
- Session length: 10 to 30 minutes per case segment.
- Tone: noir, clever, readable, lightly playful.
- Player fantasy: "My Cardputer is a detective notebook."
- Campaign shape: solve cases that gradually reveal a larger city corruption
  story.
- Hardware constraints: Arduino CLI, M5Cardputer keyboard, 240x135 text UI,
  SD saves, fixed case/clue/suspect tables, no graphics dependency.

## Core Loop

1. Select a case from the agency board.
2. Read brief, client notes, time pressure, and reward.
3. Visit locations and interview suspects.
4. Collect clues, statements, alibis, and contradictions.
5. Compare clue pairs or challenge a suspect statement.
6. Make accusation or close the case with partial truth.
7. Earn money, reputation, informants, and links to larger cases.

## Controls And UI

- `1`-`5`: choose case, location, suspect, question, or accusation.
- `w/s` and `;/.`: scroll clue database, suspect list, and statements.
- `Enter`: inspect or present highlighted clue.
- `Backspace/q`: return to previous screen.
- Main screens: `AGENCY`, `CASE`, `MAP`, `TALK`, `CLUES`, `COMPARE`,
  `ACCUSE`.
- Use short clue titles and open a detail screen for longer text.

## Major Systems

- Case structure: each case has locations, suspects, clue IDs, required facts,
  false leads, optional evidence, and outcome levels.
- Interviewing: suspects have topics, statements, hidden lies, and reaction to
  specific clue IDs.
- Clue database: stores discovered clues, tags, source, reliability, and whether
  it was used in a contradiction.
- Compare mode: selecting two clues can reveal a link, contradiction, or dead
  end if the pair is defined.
- Time and money: some actions cost time; late cases may close poorly or reduce
  reward.
- Reputation: police, underworld, press, neighborhood. Reputation opens or
  blocks informants.
- Recurring cast: clients, rivals, informants, and suspects can reappear.
- Meta-case: hidden corruption flags connect solved cases into final courtroom
  or city-hall confrontation.

## Progression

Early cases are small and teach clue collection. Midgame adds false leads,
recurring suspects, informants, and time-limited choices. Late game requires
linking clues across cases. Endings depend on accuracy, reputation, corruption
evidence, and whether the agency stayed honest or took dirty money.

## Data Model

Recommended compiled tables:

- `CaseDef`: id, name, rank, client, reward, locationStart, locationCount,
  requiredClueMask, culpritId.
- `LocationDef`: id, name, caseId, clueMask, suspectMask, unlockFlag.
- `SuspectDef`: id, name, caseMask, motiveTag, alibiClue, lieClue.
- `ClueDef`: id, caseId, title, tag, sourceType, reliability, detailTextId.
- `TopicDef`: suspectId, topicId, statementTextId, revealClue, contradictionClue.
- `CompareDef`: clueA, clueB, resultClue, resultTextId.

Recommended save fields:

- seed, day, money, reputation[4], currentCase, caseState[12].
- discoveredCluesMask per case, suspectTrust[24], topicAskedMask.
- metaFlags[3], informantFlags, accusationHistory, ending.

## Content Tables

V1 content target:

- 8 cases.
- 24 locations.
- 28 suspects.
- 96 clues.
- 120 interview topics.
- 40 clue comparison pairs.
- 4 reputation groups.
- 6 endings.

## Events And Encounters

- Suspect lies until shown a receipt or witness clue.
- Informant demands money, favor, or reputation before helping.
- Police lock down a location if too much time passes.
- Rival detective plants a false clue.
- Client asks you to hide the real culprit.
- Press leak improves fame but hurts police access.
- Final case requires clues from at least three earlier cases.

## Implementation Plan

1. Create a separate `PocketDetective` sketch.
2. Implement agency board, case brief, locations, suspects, and clue list.
3. Add interview topics and clue discovery.
4. Add clue comparison pairs.
5. Add accusation outcomes and case scoring.
6. Add reputation, money, and informant unlocks.
7. Add meta-case flags and final case.
8. Balance 8 cases with clear solvability and no hidden impossible links.

## Testing Checklist

- Compile with Arduino CLI.
- Confirm clue list and interview rows fit on screen.
- Solve each case with minimum required clues.
- Test wrong accusations and partial outcomes.
- Save/load mid-case with asked topics and discovered clues intact.
- Verify every required contradiction has an available clue path.
- Confirm reputation unlocks do not block mandatory progression.
- Reach honest, corrupt, failed, and true endings.

## Prompt For Next Codex Session

```text
In /Users/cypher/Documents/GitHub/poke-trail, create a separate Arduino CLI
Cardputer prototype for Pocket Detective Agency using
docs/gameplans/09-pocket-detective-agency.md. Use compact M5Cardputer text
screens, SD saves, fixed case/clue/suspect tables, locations, interviews,
clue database, compare mode, accusations, reputation, and an 8-case campaign.
```

