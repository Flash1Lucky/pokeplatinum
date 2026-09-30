# Standalone battle harness extraction — validation in progress

## Source contract

Extraction base: clean `pret/pokeplatinum` US revision 1 at `c248fb3f8cc9934ded800e489567c5c0eeee92eb`. Development reference: tested v3 checkpoint `057757266` on `work/debug-tools-recovery`. Extraction checkout: `C:\Games\Pokemon\Decomps\pokeplatinum-battle-tests`, branch `work/battle-harness-extraction`.

This branch contains only the battle harness, controller and damage-variance adapters, standalone field window, text bank, build option, reporting tests and documentation. It does not include the recovered debug editor, its cheats/startup preset, Fairy, species/sprite expansion or campaign content. Build prerequisites are the baseline's pinned toolchain and SDK libraries. The validation launcher reuses local copies of these existing dependencies; their local storage paths are not distribution requirements.

Enable with `-Dbattle_tests=true` (default false). This is independent of `debug_menu`. The latter exists only in the combined checkout. Disabled builds retain linked support/text but reject harness entry and leave ordinary RNG behavior unchanged; byte-identical retail matching is not promised. No save layout is changed.

In the normal overworld, **hold R and press Y** to open the standalone test window. **A** starts the suite, **B** closes, **Up/Down** browses results. After completion, reopen R+Y. Expected summary remains PASS, Done 8/8, Pass 7, Probe 1, Fail 0, Repeat 8. The core scenarios and assertions are unchanged from v3. The new field window and independent build gate require runtime acceptance.

The combined checkout additionally retains R+X Battle Tests/Battle Result as an optional interface. Its result view now delegates to the standalone renderer so there is one reporting implementation. `res/text/battle_tests.json` is a separate bank; existing bank entries are appended, not renumbered. Check text-bank append ordering when combining unrelated features that also add banks.

The harness remains a development-only tool. Gameplay feature branches must not require it. No foundation or reusable feature branch is promoted until minimal and combined builds and user-run tests pass. No push/publication is authorized by this document.

## Build and acceptance matrix

The visible sequential launcher in the combined checkout is `build/recovery/validate-harness.ps1`. It retains `harness-validation-<timestamp>.log` and a `.sha1` manifest. It stops on failure and never overwrites named test ROMs or touches saves.

| Stage | Configuration | New ROM after successful build | User acceptance |
| --- | --- | --- | --- |
| Disabled | Combined source, both options false | Combined `build/battle-tests-v4-disabled.nds` | R+X and R+Y do not open tools; normal field and battle behavior |
| Combined | `debug_menu=true`, `battle_tests=true` | Combined `build/battle-tests-v4-combined.nds` | R+Y suite and result browsing; R+X menu/editor still works and its suite view agrees |
| Minimal | Clean-upstream extraction, `battle_tests=true`; no debug-menu implementation | Extraction `build/battle-tests-v4-standalone.nds` | R+Y suite reports expected summary; B restores field input; original party/bag/location retained |

For new test ROM names, use separate disposable saves. Preserve old ROMs and save files. An existing suitable save may be copied to a new ROM basename if needed, never moved over the original. Codex does not launch/control Eric's emulator.

Current status: extraction source prepared; host reporting tests passed; both source trees pass `git diff --check`; static dependency scan found no recovered debug-menu dependencies in the standalone harness. The visible build matrix is running. Compilation/runtime success must be recorded separately per stage. Tested v3 ROM remains `debug-tools-battle-v3.nds`, SHA-1 `f9cb71b373b454de84f3c37a7212681a58be5213`.

## Continuing work

Apply the standing feature workflow: preserve local work, develop alongside tools, separate true prerequisites from tooling/adapters/campaign content, extract onto clean upstream, validate minimal and combined configurations, document evidence, and only then promote. Do not merge the combined branch wholesale into the reusable branch. The extraction worktree contains no copied ROM/save binaries in tracked files; SDK/tooling sources and build outputs remain ignored.