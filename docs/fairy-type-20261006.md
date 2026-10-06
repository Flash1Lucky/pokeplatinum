> Superseded by [Fairy v2 correction and runtime record](fairy-v2-fixes-20261006.md). V1 built successfully but its actual runtime suite failed four Plate/Judgment cases; see the v2 record for fixes and current artifacts.

# Fairy feature candidate — build validation

Recorded 2026-10-06 04:13 UTC. Both combined and minimal ROM builds and automated checks passed. Runtime acceptance is pending. This is an uncommitted, extracted candidate; it has not been promoted or released.

## Worktrees and dependency boundary

- Combined: `C:/Games/Pokemon/Decomps/pokeplatinum-fairy`, branch `work/fairy-type`, base HEAD `573c1c9bf`. Includes the preserved debug/harness working snapshot and optional Fairy fixture/menu adapter.
- Minimal: `C:/Games/Pokemon/Decomps/pokeplatinum-fairy-minimal`, branch `work/fairy-extraction`, clean upstream base `c248fb3f8cc9934ded800e489567c5c0eeee92eb`. Only Fairy runtime/data/UI/assets and host contracts; no debug-menu, battle-harness, roster expansion, or 96x96 dependency.
- Source changes remain uncommitted in these worktrees. No commits, pushes, merges, emulator control, or save changes were performed.
- Existing original worktrees/branches remain preserved. All 14 original working-file SHA-256 hashes match `fairy-start-snapshot/preserved-files.json` in the task workspace.
- The initial `fairy-core.patch` is an intermediate extraction snapshot; it omits later fixes. Use the complete current worktree diff and untracked feature assets when preparing commits or a distribution patch.

## Implemented candidate

- Fairy appended as type 18; Mystery remains 9. All 12 non-neutral Fairy chart interactions added before the Foresight boundary. All 324 preexisting type pairs retained, including Steel resisting Ghost/Dark. Curse and Hidden Power semantics retained.
- Twenty existing species retyped: Azumarill, Azurill, Clefable, Clefairy, Cleffa, Gardevoir, Granbull, Igglybuff, Jigglypuff, Kirlia, Marill, Mawile, Mime Jr., Mr. Mime, Ralts, Snubbull, Togekiss, Togepi, Togetic, Wigglytuff.
- Pixie Plate appended as item 468; hold effect appended as 147. Current JSON item generator, 20% move-power boost, description, native plate tile and 16-color palette. Existing debug item bounds automatically include it. No campaign placement.
- Judgment script, prediction functions, AI evaluation, in-battle Multitype and boxed/party Arceus type lookup extended.
- Arceus form count 19; no saved-structure widening. Normal/shiny art appended to `pl_otherpoke` at members 253–256, after the original shadows. Both Platinum and DP sprite callers use those entries for Fairy. Fairy shares the base Arceus height and existing icon; original form/archive IDs are unchanged.
- Battle icons use the existing Cute pink palette ramp. Move selection uses the existing pink Psychic palette. Type-name text, full Poketch chart and selectable order, Pokedex display and search updated. Pokedex's second type page now has Fairy plus the original None/page controls, with touch/D-pad handling aligned.
- Fairy Pokedex search index is appended after original body-shape indices to preserve existing archive member IDs.
- Battle Hall remains its original 17 selectable categories plus Mystery/boss slot. Dedicated count 18 replaces general type-count loops; rank getter/setter reject indices outside that storage boundary. No Fairy Hall category.
- Combined only: duplicate Battle Result menu entry removed; Battle Tests full screen and R+Y retained. Read-only result fallback remains for a debug-menu build with battle tests disabled.
- Combined only: fixture v4 retains all eight original cases and appends eight Fairy battles. Expected damages: `54,108,27,81,0,38,74,20` for Plate/Judgment neutral, Dragon, Fire, Fairy Arceus STAB, Dragon immunity, Poison weakness, Steel weakness, Fighting resistance. Every case repeats internally. Parser accepts original v3 and new v4 with strict expectations.


## Build artifacts

| Variant | Preserved ROM | SHA-1 | Bytes |
|---|---|---|---:|
| combined | `C:\Games\Pokemon\Decomps\pokeplatinum-fairy\build\fairy-20261006-combined-v1.nds` | `1aa6d5f77d08fd3c5e6346328437c11407b511fe` | 134217728 |
| minimal | `C:\Games\Pokemon\Decomps\pokeplatinum-fairy-minimal\build\fairy-20261006-minimal-v1.nds` | `5c3b9b4ce707452d300243452da21915a0b45387` | 134217728 |

The task workspace `fairy-artifacts-20261006.json` also records SHA-256 hashes. ROMs remain local under ignored build directories.

## Automated evidence — PASS

- Both full ROM builds exited 0. Combined's final incremental build reported `ninja: no work to do`. Minimal's incremental check repeated prebuilt-file copy steps, with no compilation, linking or ROM regeneration. Both verification runs exited 0 and both ROM hashes remained unchanged.
- Seven host contracts passed in each checkout. They cover all 361 battle/Poketch chart pairs, unchanged original type IDs/chart entries, 20 species, Plate data, Judgment mapping coverage, archive-order prefixes, and preserved Pokedex animation IDs.
- Host C execution tests the actual Battle Hall rank functions with canaries and invalid indices, and actual Arceus equip/remove form helpers. The Arceus test mocks item loading/data access; it is not a DS save-format or emulator test.
- Twelve synthetic logging/parser tests passed in combined: eight original protocol cases plus four v4 cases, including rejection of incorrect Fairy damage, missing cases, and altered oracles. These are not observed runtime battle results.
- Archive comparison passed in both checkouts against the verified clean baseline build: original 253 Arceus/shadow members, 711 item-icon members, and 446 item-data members are byte-identical; only 4/1/1 new members append. Both Pokedex archives preserve original non-type indices and append exactly 20 Fairy species.
- Both worktrees pass `git diff --check`; original-file preservation check passed.

Logs in each worktree: `build/recovery/fairy-<variant>-verification.log` and `.exit` (0). Build logs: combined `fairy-combined-rebuild.log`; minimal `fairy-minimal.log`, with matching `.exit` and `.sha1` files.

## Resolved build failures

The first combined attempt rejected LF JASC palettes; recovered/new palettes now use CRLF as required by nitrogfx. The next attempt encountered upstream's generated sound-header ordering race. The header completed afterward and the incremental retry succeeded. Minimal explicitly generated `res/sound/pl_sound_data.naix` before the full build. Prior failure logs/markers were preserved; final success supersedes earlier running/failure notes.

## Build and installation contract

Supported source baseline is the exact clean commit above. No additional gameplay feature branch is required. Use upstream's pinned build dependencies and local toolchain; development used Meson 1.12.0, NitroSDK 4.2.30001, NitroSystem 071126.1, NitroDWC 2.2.30008, NitroWiFi 2.1.30003, libvct 1.3.1, rapidjson 1.1.0, yyjson 0.12.0, and the existing metang/metroskrew tools. Minimal's local dependency symlinks are build-environment conveniences, not distributable feature files.

Minimal uses upstream configuration with no debug options. Combined enables `debug_menu`, `battle_tests`, and `battle_tests_log`. Preserve appended item/type/archive IDs when integrating; review conflicts with other type/item/form/UI changes. No campaign placement is supplied for Pixie Plate.

Type 9 remains Mystery, Fairy appends at 18, Pixie Plate at item 468 and hold effect 147. Existing Arceus form/held-item save fields and Hall storage layout are unchanged. This reduces migration risk but does not substitute for runtime save/load acceptance. Compatibility with older ROMs reading Fairy-bearing saves or vanilla multiplayer is not claimed.

## Remaining acceptance — NOT RUN

1. Eric opens the combined ROM in melonDS. R+X → Battle Tests (or R+Y) must open without auto-running; B returns to field, reopen, A runs, result pages and return work. Duplicate Battle Result menu entry should be absent.
2. Runtime fixture v4: 16 completed, 15 pass, 1 intentional probe, 0 unexpected failures, 16 matching repeats. Eight new cases exercise actual Judgment/Plate neutral, super-effective and resisted damage, Fairy Arceus STAB, Dragon immunity, Poison/Steel weakness and Fighting resistance. Expected damage values are 54, 108, 27, 81, 0, 38, 74, 20. Retain actual BTEST1 logs; no Fairy runtime pass has been observed yet.
3. Pixie Plate item 468: icon/text, give/take, Arceus with Multitype changes to Fairy and back; inspect normal/shiny front/back sprites, summary and party/PC deposit/retrieve; ordinary save, close, reopen and reload.
4. Fairy Pokedex search via touch and D-pad, original filters, Poketch chart/None/wrap, battle and contest type icons, Hall original categories/ranks. Confirm the corresponding feature paths in the minimal ROM as appropriate; its debug tools are intentionally absent.
5. Review complete source diff and commit boundaries, then record runtime acceptance before any promotion into foundation or independently validated/release-ready claim.

## Concrete manual setup in the combined ROM

Use Eric's disposable test save with room in the party. No save was copied or changed by this task.

1. In the field, R+X → Create Mon. Set Species to Arceus (493), Level 50, Form 0, Held item None, Ability Multitype (its first ability). Use L/R for pages; A selects/confirms a field; START creates the Pokemon. Set Move 1 to Judgment (449) for an ordinary-battle visual check. Do not force Form 18 in the editor: the test must exercise item-driven form selection.
2. R+X → Add Item → Pixie Plate (468) → quantity 1. Exit the tools, open the ordinary bag and Give Pixie Plate to Arceus. Check Fairy typing and sprite in its summary. Take the item through the ordinary party menu and verify return to Normal; give it again before PC and save/reload checks.
3. For deterministic shiny/non-shiny art checks, the editor exposes the full 32-bit OT ID and Personality. With Personality 0, OT ID 8 is non-shiny and OT ID 0 is shiny under the current source's XOR < 8 rule. Set these last, avoid changing Nature/Personality afterward, and verify the displayed shiny status. START commits an edit. Then use ordinary bag Take/Give to exercise the form transition again. These values are a setup recipe, not an observed runtime result.
4. Summary/PC views show the front sprite; an ordinary battle with Arceus leading shows the back sprite. Check both palettes, then deposit/retrieve and perform an ordinary save-close-reload while holding Pixie Plate. Repeat Take/Give after reloading.
5. The automated battle fixture creates temporary combatants itself; no Arceus or Plate setup is required to run it. Open R+X → Battle Tests or R+Y, press A, then expect 16 completed / 15 pass / 1 deliberate probe / 0 unexpected failures / 16 matching repeats. Its fixed damage checks do not replace manual sprite, menu, PC or persistent-save tests.

The minimal ROM has no creation/item tools. Use an independently prepared disposable test save when accepting its feature paths; this task has not created or transferred one.

## Asset provenance

Recovered selected assets and species typings from local `fairy-type-testing` tip `5b6024694099cee774f5bd5968f6791bd52bb715`; existing old branch is preserved. Battle icon glyph retained while native pixel indices were mapped to the existing Cute palette. Pokedex animation IDs 0–17 retained; Fairy gets 18. Arceus and item palette files normalized to current converter requirements. No blanket old-branch merge or legacy item/archive renumbering.


## Proposed commit boundaries — no commits made

1. Runtime type IDs/chart/species and Hall save boundary with contracts.
2. Type presentation, Poketch/Pokedex UI/data and assets.
3. Pixie Plate, Judgment/AI, Multitype and appended Arceus resources.
4. Optional combined debug adapter: menu cleanup, fixture v4 and protocol checks.

Only the first three belong in the minimal feature. The fourth remains development-only.
