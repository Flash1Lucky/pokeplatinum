> Follow-up: [accepted v2 Fairy runtime and v3 Pixie Plate correction](fairy-v3-plate-20261006.md). The Fairy group now has an actual passing capture and Eric has confirmed the listed icon/equip observations.

# Fairy v2: runtime and visual corrections

Recorded 2026-10-06T04:59:56.399559+00:00. Both v2 builds and automated regression checks passed. V2 emulator acceptance has not been run. Changes remain uncommitted; no promotion, push or merge.

## Observed v1 runtime result — FAIL

Capture: `build/runtime-20261006-002026-370-377c6bfa` in the combined checkout. Session metadata identifies combined v1 SHA-1 `1aa6d5f77d08fd3c5e6346328437c11407b511fe`; started October 6 at 00:20:26 -04:00, finished 00:32:21 -04:00, normal emulator exit 0. The stdout log contains the complete fixture-v4 BEGIN, all 16 CASE records and END.

Actual totals: 16 completed, 11 passes, 1 expected probe, 4 unexpected failures, 16 matching repeats. Cases 1–7 and 13–16 passed. Case 8 correctly produced the deliberate damage-failure bit (8), actual damage 28 versus deliberate target 0. Cases 9/10/11 dealt 46 instead of 54/108/27; case 12 dealt 69 instead of 81. Each failed only the damage check. These are real observed failures, not changed expectations.

## Root causes and changes

- `ItemTable_Load` treated the final zero-based data index as the record count. Pixie Plate is the new final record (item 468 → archive record 446), so the battle item cache omitted it. The loader now includes `lastIndex + 1`. The built item data contains hold effect 147 and parameter 20. The Judgment/Plate damage oracles are unchanged.
- The v1 Fairy battle/summary PNG itself contained `???`. February commits `15f29ec6c` and `fa4b50b71` explicitly document the Mystery placeholder; the March `5b6024694` PNG still used that lettering. V2 composes FAIRY from the existing native F/A/I/R/Y battle-icon glyphs and retains the pink palette. Mystery and every preexisting battle/contest archive member are unchanged.
- Actual inspection of both supplied screenshots confirmed the pink `???` summary icon and overlapping Fairy-label/background graphics in the Pokedex. Its PNG scan order no longer matched the reordered cells. V2 retains all 18 original cells, animation IDs and original tile bytes, and appends Fairy after the background. No old type or background ID is shifted.
- Arceus: February `f48e64975` contains pink Fairy front/back artwork and normal/shiny palettes. March `5b6024694`, used for v1, changed these to green-ring palettes. V2 restores the February art/palettes, retaining the current append-only resource IDs 253/254/255/256 and encoding keys. February's commit notes report an old form-offset bug; its old index-shifting code was not imported. The original PNGs used 8-bit storage despite only 16 colors; v2 normalizes them to native 4-bit storage without altering indices or colors. A built-resource decoding check exposed and corrected this import detail before delivery.
- Full-size Arceus sprite callers retain the Fairy mapping for front/back and normal/shiny views; party/PC mini-icons intentionally share the existing base Arceus icon. No new mini-icon asset was requested or imported. Equip/remove and PC/save persistence still require runtime acceptance.

The screenshots show Clefairy, not Arceus. Arceus findings are grounded in the user report, original source history, palette comparison, generated archive IDs and decoded build assets; they are not an observed v2 emulator result.

## Focused test selection

Existing Battle Tests screen: **A = Run All**, **X = Fairy**, **B = Close**, Up/Down = results. R+X → Battle Tests and R+Y retain their entry behavior. Opening the screen does not automatically run a group.

- All runs original case IDs 1–16: expect 16 completed / 15 pass / 1 deliberate probe / 0 unexpected failures / 16 repeats.
- Fairy runs original case IDs 9–16: expect 8 completed / 8 pass / 0 probes / 0 failures / 8 repeats. It covers Plate/Judgment neutral, super-effective and resisted damage, Fairy Arceus STAB, Dragon immunity, Poison/Steel weakness and Fighting resistance.
- Fixture v5 BEGIN logs include `group=0` (All, 16 cases) or `group=1` (Fairy, 8 cases). Case IDs and damage targets are unchanged. The parser retains v3/v4 compatibility and rejects mismatched groups, totals, missing cases/end markers and incorrect damage. Group and fixture identity are included in parsed output.
- The selected group stays visible with its result summary. All remains the regression gate; Fairy provides focused iteration. No new general test framework was added.

## Artifacts

| Variant | Preserved ROM | SHA-1 |
|---|---|---|
| combined | `C:\Games\Pokemon\Decomps\pokeplatinum-fairy\build\fairy-20261006-combined-v2.nds` | `fae025b2dd7bff27255edb23b819fef54db662fc` |
| minimal | `C:\Games\Pokemon\Decomps\pokeplatinum-fairy-minimal\build\fairy-20261006-minimal-v2.nds` | `15a1797a7fba4f82a650f4d3317e56e271f015e5` |

Each ROM is 134,217,728 bytes. `fairy-v2-artifacts-20261006.json` in the task workspace records SHA-256 too. Both delivered v1 ROMs and all 14 original working files still match their recorded hashes. A complete v1 working-source snapshot was saved under the task workspace's `fairy-v1-source-snapshot` before this correction.

## Automated checks — PASS

- Both final builds exited 0. Logs/exit/hash: `build/recovery/fairy-v2-<variant>-final.log`, `.exit`, `.sha1` in each checkout. Earlier `fairy-v2-<variant>.log` records the intermediate build before 4-bit normalization; only the final files/artifacts above are the deliverable.
- Nine host contracts passed in each checkout, including executing the actual item-cache loader with an assertion that the last item record is included, actual Hall storage functions and actual Arceus form helpers.
- Twelve existing parser tests plus seven new v5 group tests passed in combined (19 total). These synthetic protocol tests do not claim runtime battle success.
- Original Arceus/shadow, item-icon and item-data archive prefixes remain byte-identical. Existing Pokedex non-type indices remain unchanged.
- New generated-resource checks prove all original Pokedex character tile bytes remain unchanged, Fairy appends 384 tile bytes, original battle/Mystery/contest members remain unchanged, and the Fairy glyph is distinct from Mystery. Pixie Plate's actual built mapping/effect/boost and Arceus's four resource IDs, palettes and native 4-bit/two-frame dimensions pass.
- The final combined archive's normal/shiny front/back images were decoded and visually inspected. All four decoded index buffers exactly match their corresponding source sprite; pink Fairy normal and golden/pink shiny art are present. This validates encoded assets, not in-game selection or animation behavior.
- Both worktrees pass `git diff --check`. Minimal contains the runtime/assets fixes and contracts without the optional group/menu/logging adapter.

## Remaining manual acceptance

Use the distinctly named combined v2 ROM and a disposable test save. Run X: Fairy first and capture all eight case records plus END; then A: Run All for baseline regression. Close melonDS normally to flush the capture. V1's failing log is preserved as evidence and does not establish v2 results.

Use the current combined checkout's `tools/capture-battle-log.ps1` and `tools/parse_battle_log.py`; the parser in an older debug-tools checkout may not understand fixture v5. This task did not launch the capture or emulator.

Check Clefairy's summary shows FAIRY and the Pokedex entry has one correctly placed label without the overlaid box artifacts. Check the search filter with touch and D-pad, plus old filters and icons.

Create Arceus 493 at level 50, Form 0, ability Multitype, no held item; optionally give Judgment 449. Add Pixie Plate 468, then use ordinary Bag Give/party Take to change Normal ↔ Fairy. Do not force form 18 in the editor for this lifecycle test. Check pink Fairy normal/shiny front and back views, PC deposit/retrieve, ordinary save-close-reload and subsequent Take/Give. For deterministic art setup, Personality 0 with full OT ID 8 is non-shiny; OT ID 0 is shiny. Set those last and avoid subsequently changing personality/nature; START commits editor changes. The automatic suite makes its own temporary combatants and needs no manual setup.

The minimal ROM has no debug tools. Accept corresponding feature paths using a separately prepared disposable save; this task did not copy or modify saves. Stable type/item/save fields and the prior core feature scope remain as documented in `fairy-type-20261006.md`; v2 adds no campaign placement, roster expansion or sprite-size dependency.

No emulator control, save changes, commits, pushes or merges were performed during this correction.
