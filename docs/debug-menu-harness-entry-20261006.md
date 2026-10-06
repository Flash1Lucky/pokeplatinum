# Debug-menu harness entry and local Emerald reference — 2026-10-06 UTC

## User workflow and implementation

The regular combined workflow is R+X -> Battle Tests. The existing Battle Tests entry previously started the fixed suite immediately; Battle Result was a separate results-only entry. Battle Tests now opens the same full harness interface as R+Y: A runs the suite, Up/Down browse summary/cases, B returns to the overworld. Battle Result remains available. R+Y remains unchanged.

The callback caches FieldSystem, frees the debug menu and its task/windows, then queues the harness menu. It does not resume the field between menus. The previously fixed deferred draw prevents shared BG3 window cleanup from erasing the new screen, and its first callback returns without processing A. B resumes field processing; successful A hands lifecycle to the existing battle task, which restores field processing at completion. Failed launch stays in the harness and renders its result. With battle tests disabled, the entry retains the previous results fallback instead of closing into a paused field.

This is an optional debug-menu adapter. The independent harness checkout was a dependency-isolation verification fixture, not a required user-facing product. Eric can use the combined debug menu for ordinary development. Keeping reusable core logic separate allows debug-menu/testing updates without forcing unrelated feature dependencies; it does not require maintaining multiple user experiences.

Changed implementation: src/overlay005/debug_menu.c in the combined worktree only. No fixture, assertion, save format, or R+Y change. Existing source and named ROMs preserved. No commits, pushes, or emulator launch/control.

## Local Emerald checkout inspected

C:\Games\Pokemon\Decomps\pokeemerald-expansion
Branch: battle-status-menu
HEAD: 03827e31854130bfcd0496339144e8b6e9299948
Commit date: 2026-03-06; subject: Turning off tera
Working tree already has a modification to src/battle_util.c. Research reads the local working tree; it is not a claim about current upstream. No fetch, checkout, build, or edit was performed there. No repository/root .agents or AGENTS.md instructions were found in scoped inspection.

## Selection and execution

The inspected automated runner has no in-game category-selection menu. Selection is supplied at build/invocation time using TESTS. The test ELF automatically enumerates registered tests, filters them, resets state, runs them, and reports results.

Supported examples, from this checkout's shell/build environment:
- make check -j
- make check TESTS="Spikes"
- make check TESTS="*Intimidate"
- make check TESTS="test/battle/move_effect/spikes.c"
- make pokeemerald-test.elf TESTS="Spikes" (then open the test ELF in mGBA for visual execution)

test/test_runner.c detects exact filename matching for patterns ending in .c, substring matching for a leading *, and test-name prefix matching otherwise. This is not unrestricted glob syntax. Test registration stores __FILE__; compilation uses the test/battle/... source path. Makefile patches the filter into gTestRunnerArgv in the test ELF.

make check patches a headless ELF and invokes tools/mgba-rom-test-hydra with mgba-rom-test. Hydra is a host-side coordinator of multiple emulator processes and reported results. The battle mechanics and test assertions execute in the GBA test program under emulation; these are not merely native host unit tests.

The separate src/debug.c "Start Debug Battle" action clears/rebuilds parties from src/data/debug_trainers.h and starts a trainer battle. "Try Battle" under trainer tools is also manual setup. Those menus are not the automated test selector.

## Actual coverage/examples read

- test/battle/move_effect/spikes.c: one/two/three-layer entry damage, failure on a fourth layer, scripted switching and expected messages/HP changes.
- test/battle/ability/intimidate.c: Attack reduction after switching and after a KO, parameter comparison against another ability, doubles cases.
- test/battle/status1/paralysis.c: generation-dependent Speed penalties, parameterized move order, tagged-RNG 25% full-paralysis checks, AI cases.
- test/battle/weather/rain.c: Fire damage x0.5 and Water damage x1.5 using baseline-vs-weather parameter runs.
- test/battle/ai/ai_check_viability.c: AI selects Facade rather than Body Slam when status makes it stronger.
- Other actual test/battle directories: form_change, gimmick, hold_effect, item_effect, move_animations, move_effect_secondary, move_effects_combined, move_flags, starting_status, volatiles.
- Framework supports single, double, multi, 2v1 and 1v2 battles and equivalent AI-controlled scenarios.
- Nonbattle test files include bag, daycare, Pokemon data, random, save, script and text checks. The docs explicitly limit overworld testing: commands affecting party/state can be tested, but map object events are not loaded.

## Ideas for Platinum, not implemented here

Our eight fixed cases are an initial deterministic smoke suite: neutral/resisted/super-effective/immune damage, STAB, STAB plus super-effective, low damage roll, and an intentional assertion probe. They verify a narrow foundation and do not establish modern mechanics coverage.

Good next design options are a named case registry with groups plus Run all/Run selected in the debug interface; scripted turn/setup helpers; parameterized interaction cases; and assertions on visible outcomes such as HP/PP/status/message events. Preserve explicit expected-failure reporting and make skips/assumption failures distinguishable from passes. CLI selection could later use the same registry if useful.

Emerald's GBA macros, hooks, RNG controls and mGBA runner cannot be dropped directly into the DS engine. They require Platinum-specific lifecycle and observation hooks. Probabilistic/tagged-RNG tests differ from replaying a fixed seed. The local runner's 60-second headless timeout should not be copied blindly into visibly paced DS tests. No new groups, test language, or broad runner redesign was implemented.

## Checks and pending acceptance

Source review confirmed callback returns immediately after dispatch, no use of the freed debug menu, paused-field handoff, existing A/B behavior, and deferred initial draw. git diff --check passed. Separate build log/result/manifest: build/recovery/debug-entry-20261006.log, .exit and .sha1. Check the final result before claiming build success.

Runtime still needs Eric to use R+X -> Battle Tests, verify it opens without immediately running, B back to field, reopen, A run, then browse results. R+Y should still open the same screen. Earlier combined/standalone logged suite results remain valid for their exact named ROMs; this new entry needs its own UI handoff check.

## Main references

docs/tutorials/how_to_testing_system.md
Makefile (test ELF/check targets and test compilation)
test/test_runner.c
test/test_runner_args.c
include/test/test.h
tools/mgba-rom-test-hydra/main.c
src/debug.c

## Build result

Build completed successfully (debug-entry-20261006.exit = 0); new artifact `C:\Games\Pokemon\Decomps\pokeplatinum-debug-tools\build\battle-tests-20261006-combined-debug-entry.nds`, recomputed SHA-1 `67a4dedca96e2cb936050033dc91784780b616b3`. Previous accepted combined-menu-fix ROM remains unchanged at `5eb213be4300edbb3775bd6b19cecf96f9cffbd6`. Runtime handoff verification is pending. No additional standalone build is needed for this adapter-only change.

## Acceptance correction — user evidence from October 5, 21:34 EDT

Eric explicitly confirmed the CURRENT disabled artifact `battle-tests-20261005-191143-disabled.nds`, SHA-1 `af03eef308cbfc18cfd811f605aafeb775d19209`, opens none of the tools. Both R+X and R+Y gating are accepted as user-confirmed. Source: message Sentinel_6e40722fe718819183be8b6330156a57, following the exact artifact path supplied at 21:24 EDT. This is additional current-artifact evidence, not merely the older R+X-only checkpoint. Earlier statements here listing current shortcut gating as pending are superseded.

Eric later confirmed combined R+Y and R+X both open. Record that opening behavior as user-confirmed without inferring page navigation, editor functionality, or the newest `combined-debug-entry` ROM's Battle Tests handoff/run/result/return details. Those latest handoff details still await explicit confirmation. Ordinary battle/save/reload checks on the current disabled ROM remain unconfirmed; shortcut gating does not require repeating.
