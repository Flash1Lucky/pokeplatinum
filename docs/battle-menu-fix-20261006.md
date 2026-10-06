# Battle menu opening fix — 2026-10-06 UTC

## Cause and change

The R+Y harness window was drawn synchronously inside FieldInput_Process. After that function returned TRUE, HandleFieldInput (src/field_system.c) called MapNamePopUp_Hide and removed any signpost. Popup hide unconditionally clears BG3 rows 0–4, even when no popup is active; signpost removal can clear rows 18–23. The harness occupies rows 1–20 on the same layer. Its upper text/frame could therefore appear briefly and then be erased. This is a source-confirmed opening-order defect; the submitted screenshot could not be inspected.

Both worktrees now initialize the window on the first menu SysTask callback, after the field-input call and its cleanup have returned. Open only allocates menu state, pauses field processing and queues the callback. The first callback draws and returns without consuming an action; subsequent callbacks handle input normally. No per-frame redraw workaround was added. Existing page refresh, B cleanup, A test launch, and battle assertions are unchanged. The shared renderer used by the combined debug-menu result panel is unchanged.

## Evidence and boundaries

- Both worktrees had identical menu source before this fix and received the same focused change.
- Inspected SysTaskManager_AddTask: registration does not synchronously invoke the callback.
- Inspected map popup and signpost clear regions and immediate caller order.
- git diff --check passed.
- Existing standalone SHA-1 4300cee9cfbd314c9e408d0f03b4de6791ce286f and combined SHA-1 92112fb6005cc317cec4accf7c7e01551733a066 were rechecked unchanged.
- Current original ROMs, logs, and saves are preserved; no emulator was launched or controlled.
- Screenshot Library access failed after one supported local retry because the current transfer helper requires os.setxattr, unavailable in the host Windows Python. No screenshot contents were inferred.
- Builds use the existing configured caches with battle tests/logging enabled; combined also has debug tools enabled. Separate named ROMs prevent replacing Eric's active test artifact.
- Build log/result/manifest: combined build/recovery/menu-fix-20261006.log, .exit, .sha1. The build result must be checked separately; this document alone does not assert success.

## User runtime acceptance still needed

After completing the current test and closing it normally, use the separately named corrected ROM with a disposable save. For both standalone and combined: open R+Y, wait several seconds, confirm upper status/frame and lower controls stay visible; cycle result pages, close with B, reopen, run with A, and reopen results after field return. Repeat when opening soon after a location popup and while a signpost was visible. Confirm ordinary field restoration. Expected battle suite remains 8 complete, 7 pass, 1 deliberate probe, 0 unexpected failures, 8 matching repeats.

No commit, push, or promotion.

## Actual original-standalone runtime capture — 2026-10-06 UTC

Eric completed and closed melonDS normally. Session `C:\Games\Pokemon\Decomps\pokeplatinum-battle-tests\build\runtime-20261005-203623-332-a3ca4db1` (not the earlier failed launcher session) records ROM SHA-1 `4300cee9cfbd314c9e408d0f03b4de6791ce286f`, started 2026-10-05 20:36:23 -04:00 and finished 20:47:38 -04:00, emulator exit 0. stdout contains one complete BTEST1 suite and END; stderr is empty. The existing parser exited 0/PASS: 8 completed, 7 pass, 1 expected probe, 0 unexpected failures, 8 matched internal repeats. Damage values: 28, 14, 56, 0, 42, 84, 23, 28. Case 8 has expected and actual failure bit 8; its deliberate target is zero.

This is actual logging and battle-suite runtime evidence for the original standalone ROM, not acceptance of the subsequently corrected menu build, not a separate restart run, and not proof of all party/bag/save-preservation checks. Logs were read only, and no emulator was controlled.

## Corrected build results

Both incremental builds completed successfully; menu-fix-20261006.exit is 0. Recomputed ROM SHA-1 values:
- Standalone: build/battle-tests-20261006-standalone-menu-fix.nds — eb7a2ad132439d8dff2a889ef6fe4b0c22edf0f3
- Combined: build/battle-tests-20261006-combined-menu-fix.nds — 5eb213be4300edbb3775bd6b19cecf96f9cffbd6

The original standalone/combined named ROM hashes still match. Corrected ROM runtime/visual acceptance remains pending; compile/link/package success is not visual proof. No save was copied or changed by this work.

## Corrected standalone acceptance update — 2026-10-06 UTC

Eric reported at 21:05 EDT on October 5: "just ran that. The menu looked normal now". Record menu appearance as user-confirmed; page navigation, B close/reopen, popup/signpost edge cases, and party/bag/location preservation are not individually confirmed by that statement.

Newest capture: `C:\Games\Pokemon\Decomps\pokeplatinum-battle-tests\build\runtime-20261005-205548-917-7893e070`. Metadata identifies `battle-tests-20261006-standalone-menu-fix.nds`, SHA-1 `eb7a2ad132439d8dff2a889ef6fe4b0c22edf0f3` (recomputed and matched). Started 20:55:48.918 -04:00, finished 21:05:16.708 -04:00; state exited, exit code 0, stderr empty. Existing parser: PASS/exit 0, one complete BEGIN + eight CASE + END suite; 8 completed, 7 pass, 1 expected probe, 0 unexpected failures, 8 matching internal repeats. Probe case 8: actual failure bit 8 equals expected 8; damage 28 against intentional target 0.

Compared all 10 BTEST1 records in order with previous accepted session runtime-20261005-203623-332-a3ca4db1: zero differences. This establishes consistent reported outcomes across two fresh emulator launches spanning the UI-only correction. It is not two fresh launches of the exact corrected ROM. Session timestamps, PID, ROM name/hash and ordinary emulator/save log lines are intentionally run-dependent and excluded from battle-result comparison. No BTEST result field differed.

Remaining acceptance: corrected combined build (`battle-tests-20261006-combined-menu-fix.nds`, SHA-1 `5eb213be4300edbb3775bd6b19cecf96f9cffbd6`) needs runtime suite/logging, R+Y window interactions, R+X editor/results agreement, and normal field/save behavior. Final disabled build (`battle-tests-20261005-191143-disabled.nds`, SHA-1 `af03eef308cbfc18cfd811f605aafeb775d19209`) needs both shortcuts gated plus ordinary field/battle/save-load checks; earlier R+X validation applied to a different checkpoint. Record remaining standalone UI/isolation checks before full promotion. No emulator was launched or controlled; captures were read only. No commit, push, or promotion.

## Corrected combined runtime acceptance — 2026-10-06 UTC

Capture `C:\Games\Pokemon\Decomps\pokeplatinum-debug-tools\build\runtime-20261005-211353-853-81e362e1` identifies `battle-tests-20261006-combined-menu-fix.nds`, SHA-1 `5eb213be4300edbb3775bd6b19cecf96f9cffbd6` (recomputed and matched). Session started October 5 21:13:53.854 EDT and ended 21:23:04.059 EDT; state exited, exit code 0, stderr empty. Existing parser exited 0/PASS: one complete suite, 8 cases, 7 passes, one deliberate probe, zero unexpected failures, 8 matching internal repeats, END present. Case 8 actual/expected failure bit 8 matches, damage 28 versus intentional target 0.

All 10 BTEST1 records exactly match both accepted standalone captures (runtime-20261005-203623-332-a3ca4db1 and runtime-20261005-205548-917-7893e070). This verifies consistent logged suite outcomes across fresh launches and minimal/combined installations. Timestamps, PID, ROM hash/path and incidental emulator/save logs differ by session as expected.

Eric's latest statement was only that the run appeared finished. No new explicit combined menu/page/reopen/editor observations were provided, so none are recorded as confirmed. Prior corrected standalone normal-menu appearance remains user-confirmed. Remaining interaction/isolation checks and ordinary save/load behavior remain separate.

Smallest next artifact check: final disabled ROM `C:\Games\Pokemon\Decomps\pokeplatinum-debug-tools\build\battle-tests-20261005-191143-disabled.nds`, SHA-1 `af03eef308cbfc18cfd811f605aafeb775d19209` rechecked. User should confirm R+X and R+Y do not open tools in the overworld, then ordinary field/battle and normal save-close-reload. This build intentionally emits no BTEST suite; absence is expected and cannot prove behavior. No emulator control, commit, push, or promotion occurred during this verification.

## Acceptance correction — user evidence from October 5, 21:34 EDT

Eric explicitly confirmed the CURRENT disabled artifact `battle-tests-20261005-191143-disabled.nds`, SHA-1 `af03eef308cbfc18cfd811f605aafeb775d19209`, opens none of the tools. Both R+X and R+Y gating are accepted as user-confirmed. Source: message Sentinel_6e40722fe718819183be8b6330156a57, following the exact artifact path supplied at 21:24 EDT. This is additional current-artifact evidence, not merely the older R+X-only checkpoint. Earlier statements here listing current shortcut gating as pending are superseded.

Eric later confirmed combined R+Y and R+X both open. Record that opening behavior as user-confirmed without inferring page navigation, editor functionality, or the newest `combined-debug-entry` ROM's Battle Tests handoff/run/result/return details. Those latest handoff details still await explicit confirmation. Ordinary battle/save/reload checks on the current disabled ROM remain unconfirmed; shortcut gating does not require repeating.
