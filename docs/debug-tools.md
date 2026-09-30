# Debug tools recovery

Status: **prototype under validation**. The five basic user-run smoke checks passed; deeper validation remains pending; this is not yet the validated `feature/debug-tools` or foundation release.

## Source and installation contract

- Base: `pret/pokeplatinum` at `c248fb3f8cc9934ded800e489567c5c0eeee92eb` (US revision 1).
- Recovered from [RavePossum's debug_menu branch](https://github.com/ravepossum/pokeplatinum/tree/debug_menu) at `82ef02910ac0f58f7a4ad1553c8bdb628c52b8d7`, refreshed 2026-09-29. The source includes Eric's Pokedex contribution, [PR #2](https://github.com/ravepossum/pokeplatinum/pull/2), and the author's newer Fly adaptations. Original branch history is retained separately.
- This recovery is reconstructed on clean main; it does not merge the old feature branch's unrelated historical base. There are no Fairy, expanded roster, sprite-size, campaign, or other feature prerequisites.
- Use the baseline's pinned build dependencies. Configure with `-Ddebug_menu=true`; default is `false`. Build the modified ROM target directly (`ninja -C build pokeplatinum.us.nds`), since retail checksum matching is not expected for feature builds.
- Both configurations include the resources and linked code. The setting gates title/field entry, collision/sight/step-encounter hooks, and the new-game script preset. Disabled does **not** promise a byte-identical retail ROM.

## Controls

Use DS button mappings in your emulator, not necessarily the keyboard letters below.

- Field: hold **R**, press **X** in the ordinary overworld. D-pad moves through entries, A selects, B backs out. Special field load modes are excluded.
- Title: **R+X** starts the original Sinnoh developer preset only when no save exists. An existing save takes the continue path. Use a separately named ROM/save for testing.
- Pokemon editor: A begins/accepts a field edit; B cancels that field edit, or exits the editor without committing when browsing. Up/down changes a value and left/right selects a decimal step. L/R changes pages. **Start commits** the displayed Pokemon; X/Y selects party slots in edit mode. Switching slots discards uncommitted changes.
- Gender is displayed read-only and is derived from species/personality. Nature edits change the personality while preserving its low byte; shiny status and personality-based appearance can change. The editor is for game testing and does not enforce encounter-legality rules.
- Moves set to zero are cleared with their PP and PP Ups. Changed moves receive base PP; unchanged moves retain current PP and PP Ups. IVs are capped at 31; each edited EV is capped at 255 and the remaining part of the total 510 budget. An already invalid imported EV spread is not automatically rewritten in unrelated fields.
- Form bounds and available ability slots use vanilla data. Battle-only or item-dependent forms may be reset by normal game logic. Expanded forms need an explicit adapter.
- New Pokemon are healed. Existing Pokemon retain damage across stat changes (at least 1 HP if previously alive, 0 if fainted) and retain status counters unless status is explicitly edited. Editing uses a clone until Start.
- Add Item selects an item, then a quantity; A adds it. The result reports the amount actually added, including zero for failure/full capacity. B returns to the main menu. Placeholder item IDs 113-134 are skipped.
- Flags and variables display **decimal IDs**. Flags range from 1 through 2911; vars use the baseline's saved variable range. Special/temporary ranges are excluded. A toggles a flag or commits a var value.
- Fly uses the town map and requires a nonempty party. Named safe warps and a safe-return bookmark have not been implemented.
- Camera adjustment uses D-pad for angle and L/R for field of view; B or Start exits. These changes last for the current field session.
- **Reset Controls** clears collision bypass, trainer-sight suppression, and random-step-encounter suppression.

## Save and behavior boundaries

No save structure is widened. Reserved existing flag slots are `0x0974` (collision), `0x0975` (trainer sight), `0x0976` (step encounters), and `0x0990` (debug startup). These are unused on the recorded baseline; other hacks must check allocations. Toggle flags persist if you save; disabled builds ignore their cheat behavior. Item/Pokemon/progression edits themselves persist normally and are not reverted by disabling debug support.

Collision bypass applies on foot in the normal-overworld movement handler while moving in a direction. Trainer sight and step encounters have separate switches. Fishing, scripted/static battles, honey trees, and other encounter paths are not covered by the step-encounter switch.

The recovered startup preset is Sinnoh-specific and separate from the generic editor. It sets trainer identity, supplies two level-100 utility Pokemon and travel items, and grants all badges as in the original feature. It is intended for a disposable development save. It is not evidence of a completed campaign. Startup seeds both RNGs to `0x504C4154`, but **does not make later battles deterministic**. The separate Battle Tests suite resets the battle-local RNG seed immediately before each battle; see `battle-tests.md`.

## Validation record

Completed so far:

- Original source fetched and compared; recovered editor and Pokedex work retained, newer Fly APIs included.
- Host regression tests exercise the actual numeric, EV/HP, and nature/PID helper functions, including 250,000 PID/nature combinations and unsigned boundary cases.
- The recovered menu, Pokemon editor, debug startup, and movement hook compile with the pinned DS toolchain.
- Debug-enabled ROM build completed (2026-09-29); test copy: `build/debug-tools-test.nds`. SHA-1: `5ce45af78989df1410f0a7d767def040894b82fb`.
- Eric reports all five smoke checks passed in standalone melonDS: repeated menu entry/exit with movement restored; item additions; Pokemon edits and cancellation; normal save/close/reload persistence; Fly and encounter/reset controls. These are user-confirmed functional checks, not exhaustive boundary coverage.
- Debug-disabled new-game script preprocessing matches the baseline tokens.
- Debug-disabled checkpoint ROM built successfully: `build/debug-tools-disabled.nds`, SHA-1 `099dbc4ba309440f0f120bded9e749f7711e39cd`, source checkpoint `c08399db9`.
- Eric confirms R+X does not open debug tools in the disabled checkpoint ROM. This confirms the entry gate; saved-cheat behavior is not yet separately verified.
- Battle fixture v1 ROM built successfully: `build/debug-tools-battle-v1.nds`, SHA-1 `78f7bdc3da3372afa2ed41073819a3c46674d9a9`. Eric tested it: Test Battle did nothing. The launch guard incorrectly rejected the ordinary overworld parent process. Superseded by the automated harness below.
- Text banks parse and contain unique message IDs.
- Reviewed cancellation, empty-party guards, item capacity reporting, and removed menu/editor/camera use-after-free paths and the egg met-location array-index error.

Pending:

- Remaining disabled-ROM checks: normal menu/movement and persistence, and inactive cheat hooks with saved toggles enabled.
- Additional runtime edge cases: full bag and party/PC capacity, numeric flag/var boundaries, and unsupported contexts.
- Capacity/egg/form/PP/metadata round trips in summary, battle and PC storage.
- Automated battle suite: Eric confirmed PASS/PASS/FAIL (probe) on v2, then reported the repeat/cold-start and original party/bag/location checks worked properly. V3 type/STAB acceptance remains pending. See `battle-tests.md`.
- Minimal and initial combined acceptance gates, then promotion to reusable feature/foundation branches. Fairy/sprite combined checks remain future work.

A successful compile or visible menu alone does not satisfy these runtime gates.

## Automated battle harness v3

The manual v1 fixture is superseded by the controlled type/STAB suite in [battle-tests.md](battle-tests.md). The recovered editing menu remains available. Select **Battle Tests** once; no move or escape inputs are required. **Battle Result** pages through the actual assertion results with Up/Down.

The lifecycle guard now allows the ordinary field parent process and rejects a concurrent child application/task. A rejected launch opens the result screen with failure bit 64 instead of silently doing nothing. Eric confirmed the v2 runtime milestone and subsequent reproducibility/isolation checks. V3 adds exact type/STAB assertions, automatic repeats and a summary; it requires its own runtime acceptance.
