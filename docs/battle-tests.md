# Platinum automated battle tests — v3 prototype

Development base: `work/debug-tools-recovery`, checkpoint `c08399db9`, on upstream `c248fb3f8cc9934ded800e489567c5c0eeee92eb`. Existing uncommitted fixture work was adapted in place. V1 and v2 source snapshots and diffs are retained under ignored `build/recovery/`. No foundation/reusable branch has been promoted.

## Run and capture

Run `build/debug-tools-battle-v3.nds` after its visible build reports success, using a separate disposable save. In the ordinary overworld, open R+X and choose **Battle Tests** once. Eight cases run twice each (16 battles), without move/escape inputs. Let the suite finish and return to the field.

Open **Battle Result** for the summary. Expected: **PASS**, **Done 8/8**, **Pass 7**, **Probe 1**, **Fail 0**, **Repeat 8**. Up/Down navigates between the summary and eight named case pages, one page at a time. B returns to the menu. A passing repeated case has Repeat 1. The deliberate probe displays FAIL (probe), Bits 8; all other cases should have Bits 0. Both runs must complete before a case counts in the summary.

| Case | Player → opponent | Damage roll | Expected HP loss |
| --- | --- | --- | --- |
| Neutral | Abra → Snorlax | 100% | 28 |
| Resisted | Abra → Bulbasaur | 100% | 14 |
| Super | Abra → Squirtle | 100% | 56 |
| Immune | Abra → Geodude | 100% | 0 |
| STAB | Pikachu → Snorlax | 100% | 42 |
| STAB+Super | Pikachu → Squirtle | 100% | 84 |
| 85% roll | Abra → Snorlax | 85% | 23 |
| Fail probe | Abra → Snorlax | 100% | Deliberately expect 0; actual should be 28 |

Every player uses Shock Wave and every opponent uses Splash. Player PP should finish at 19, opponent PP at 39, exit at 5, and completed turns at 1. The page shows Got/Want damage, failure bits, repeat agreement, player PP and exit. The summary is the primary capture: report any FAIL and the affected detail page. Exact expected failures alone count as probes; unexpected probe passes and extra failure bits count as ordinary failures.

A stuck battle is a harness failure: report the screen and species without advancing the battle manually. There is no external emulator watchdog or unattended CI runner. Optional SDK debug output begins `BTEST v3` and includes case, run, failures, expected failures, exit, turns, HP, damage and player PP. Emulator capture is not assumed. Results exist only in RAM until another suite or ROM restart.

## Controlled fixtures and independent expectations

All temporary Pokemon are level 50, IV 20, zero EVs, fixed Hardy personalities (0/25), healthy, itemless, and have one move. After ordinary species initialization, the temporary party stats are set to HP 300; Attack, Defense, Sp. Attack and Sp. Defense 100; player Speed 200 and opponent Speed 50. This avoids confusing species stat differences with type multipliers and guarantees normal escape. Types still come from real species data; no expected type multiplier or HP result is written into the engine.

The player has Ability None. The opponent has Battle Armor, using vanilla critical-hit prevention. Shock Wave is a special Electric move with power 60, no secondary effect, and an accuracy-bypass effect. These artificial abilities/moves/stats are fixture inputs, not species edits or legal encounter data. No stat recalculation should be added after normalization.

Independent arithmetic for the baseline is `floor((2*50/5 + 2)*60*100/100/50) + 2 = 28`. Half damage is 14, double is 56, STAB is 42, and STAB plus double is 84. The low-roll baseline is `floor(28*85/100) = 23`. These expected constants live in the case table; they are not obtained from the engine's damage routine or type chart. These cases do not claim full chart, dual-type, rounding-order or critical-hit coverage.

Each case stores player species, opponent species, move, damage-roll percentage, expected damage, UI name, seed and expected failure bits. Add a case by extending `sCases`, its text name and `DEBUG_BATTLE_CASE_COUNT`; the summary denominator is automatic. Level/stats, abilities, slot-1 actions and one-turn duration currently remain shared fixture policy. Extend that policy explicitly when a new mechanic requires it.

## Repeats and assertions

The harness rebuilds both parties and resets the seed before each run, including the repeat. It compares exit, turns, both HP values, damage, both PP values, both command counts and raw failure bits. Failures from either run remain visible. A changed outcome adds bit 128, so a repeat cannot conceal a bad first run or an unexpectedly passing probe.

| Bit | Assertion |
| --- | --- |
| 1 | Normal successful player escape (exit 5) |
| 2 | Exactly one completed turn |
| 4 | Player HP unchanged |
| 8 | Exact opponent HP loss equals the case's independent expected constant |
| 16 | Exactly one PP consumed on each side |
| 32 | Exactly two command selections per battler |
| 64 | Launch refused: disabled tooling, running task/child, or no active field map |
| 128 | The two completed runs differ |

The PP checks prevent an unexecuted attack from passing the immunity case. Repeated identical failures still fail the suite. The deliberate bit-8 probe must match exactly in both runs. `include/debug_battle_report.h` contains the shared repeat comparison and summary classification, exercised by `tools/tests/debug_battle_report_test.c` including unexpected-pass and additional-failure cases.

## Engine integration and randomness

`src/debug_battle.c` owns the resident field task, private fixtures and result storage. It allows the ordinary parent field process at launch, rejects an existing task/child, unloads the field once, runs each battle application serially and restores the field and movement. The resident callback survives overlay5 unloading.

The controller adapter in `src/battle/battle_controller.c` substitutes ordinary server replies only while the harness is active and the battle type is exactly debug singles. It avoids enqueuing unanswered client messages. Turn 0 selects Fight/slot 1 for both sides; later turns select Run for the player and Fight for the opponent. The battle engine still performs validation, move execution, type/STAB calculations, status handling, PP, turn order, animations and teardown.

`BattleSystem_CalcDamageVariance` has a narrow test adapter: it consumes the normal battle RNG draw, then uses the configured 100% or 85% roll only for an active debug harness. Ordinary battles retain their original roll. The hook changes randomness, never expected damage or type/STAB logic. The DTO initializes the battle-local seed to `0x504C4154`; the LCRNG is also set before each battle and the prior field LCRNG seed is restored afterward. MTRNG is not reseeded. This supports seeded runs and damage-roll control, not a general semantic RNG scripting system.

## Save and dependency boundaries

Parties, bag, Pokedex, options and trainer data are DTO copies. Records, Poketch and cursor storage are private. The empty bag and scripted move/run actions exclude capture, items, switching and PC writes. Post-battle field updates, progression and blackout handling are omitted. Some DTO pointers still alias saved structures: this is a constrained fixture, not general isolation for arbitrary future actions. Audit new actions before enabling them.

No save schema change, Fairy, species expansion, sprite expansion or campaign content is required. Emerald expansion at `03827e3185` was a read-only conceptual reference; its uncommitted battle-util work remains untouched. No GBA testing code was ported.

Current build switch and launcher/result UI use recovered debug tools (`debug_menu=true`). Resident harness, controller adapter and variance adapter are development-only infrastructure. Later independent extraction should provide a dedicated build option plus optional menu adapter on clean upstream, and validate minimal and combined installations. Feature-under-test distributions must not require this harness. This turn does not promote or claim independent reuse.

Not implemented: arbitrary multi-turn actions, doubles, switching/capture, scene assertions, general timeout recovery, full type chart coverage, Fairy tests or external emulator/CI integration.

## Evidence

- V2 enabled ROM: `build/debug-tools-battle-v2.nds`, SHA-1 `af71699e74c012e18b81fc85c60ca4cfc1837382`; visible build completed all 11 steps in 205.972 seconds.
- Eric confirmed v2 PASS/PASS/FAIL (probe), then completed the requested repeat, emulator-restart and original party/bag/location checks and reported they worked properly. Exact numeric damage values were not supplied. These observations apply to v2, not automatically to v3.
- V3 reporting tests passed under host C99 with `-Wall -Wextra -Werror`. Text JSON parses with 42 unique IDs; `git diff --check` passes.
- V3 visible build started using ignored `build/recovery/visible-build-v3.ps1`. On success it creates `build/debug-tools-battle-v3.nds` without overwriting existing ROMs/saves and writes its SHA-1 to `build/recovery/battle-v3-build-result.txt`.
- Build log: `build/recovery/visible-build-20260929-230013.log`. All changed C objects compiled successfully through step 11/13; final link/ROM packaging were still running at the last check.
- V3 final build and emulator acceptance remain pending until recorded. Disabled build/behavior and minimal/combined extraction remain separate later gates. Codex has not launched or controlled the emulator.

- V3 build completed all 13 steps in 203.079 seconds. Preserved ROM SHA-1: f9cb71b373b454de84f3c37a7212681a58be5213. Eric confirmed eight cases: seven passes and one FAIL (probe). These statuses require the automatic comparisons and assertions to pass (apart from the deliberate bit-8 failure).

## Extraction follow-up

The next-stage standalone build option and R+Y window are implemented. See [battle-tests-extraction.md](battle-tests-extraction.md) for the current dependency contract, build matrix and pending acceptance. This supersedes the earlier statement that a dedicated build option is future work. V3 remains the last user-validated ROM; extraction UI/build combinations are not yet validated.
