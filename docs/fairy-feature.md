# Fairy feature contract and local checkpoint

Current as of 2026-10-06. The combined implementation is user-accepted; the minimal implementation is extracted and build/host validated, with standalone runtime acceptance still pending. Earlier dated notes are milestone history, not the current status.

## Installation boundary

The minimal implementation is based on clean upstream `c248fb3f8cc9934ded800e489567c5c0eeee92eb`, on `work/fairy-extraction`. No debug menu, battle harness, later-generation species expansion or sprite-size expansion is a required dependency. Apply the complete feature commit, including new assets and tests, to that supported baseline using the upstream pinned toolchain. The earlier `fairy-core.patch` is incomplete and must not be used as the distributable implementation.

The combined checkout on `work/fairy-type` includes optional debug tools and a Fairy-specific battle-test adapter. Those changes are separate commits and do not belong in a minimal Fairy installation. Original branches are preserved; this checkpoint does not merge, promote, publish or push them.

## Implemented scope

- Fairy appended at type 18; Mystery stays 9. Existing types and matchups retained, including original Steel/Ghost/Dark behavior. Curse and Hidden Power retain their intended original semantics.
- Fairy chart interactions, 20 retyped existing species, Poketch chart/selector, Pokedex display/search and type labels.
- Pixie Plate item 468, hold effect 147, 20 percent boost; Judgment script, type prediction, AI mappings and Multitype/Arceus support. No campaign placement or additional Fairy move roster.
- Arceus form count 19 within existing saved fields; front/back and normal/shiny assets append at pl_otherpoke members 253-256. The PNG .key files are required sprite encoding metadata, not credentials. Mini-icons intentionally reuse base Arceus.
- Battle icon uses existing shared palette bank 1, indices 0/9/11/12/14/15; final lettering is Eric's source artwork (SHA256 09b4b5c4621cbded4b663a3cc6957d16afac0a8cd9bfeebacfa836686e7b49a6). Move-button palette is separate. Pokedex selector label is title-case Fairy.
- Battle Hall keeps its original categories/boss slot and storage size, with bounds guards; no new Fairy Hall category.

No save structure is widened. This is not a guarantee of compatibility with older ROMs reading Fairy-bearing saves, vanilla multiplayer or arbitrary other feature branches. Review conflicts in type/item/form IDs, graphics archives and Pokedex UI when integrating.

## Evidence and remaining acceptance

See [combined user acceptance](fairy-combined-acceptance-20261006.md) for precise user-reported observations and their limits. Run All was reported as 15 passes plus one intentional probe; Arceus PC/save persistence, final visuals, Poketch, prior shiny/Mystery spot checks and Hall operation were accepted. No new emulator or runtime capture was operated for this checkpoint.

Nine current host contracts pass in each Fairy checkout. These include static table/data assertions and extracted C helpers with mocked I/O; they do not substitute for full DS execution or AI decision coverage. Prior ROM builds/packed-resource validation are recorded in dated notes. The combined protocol has 19 passing synthetic parser/group tests; these are optional harness tests, not a minimal dependency.

Minimal standalone runtime acceptance remains pending. A separately reported Take/re-equip sequence, exhaustive shiny views, Pokedex result correctness and AI decisions are not newly asserted. Source review/checkpoint approval was received; this local checkpoint records the implementation without upgrading those evidence boundaries.

Useful host check: from the checkout, set FAIRY_BASELINE_REPO to the clean baseline repository when needed and run `python3 tools/tests/test_fairy_contract.py`. The resource-verification tools under tools/tests consume an existing clean baseline build and feature build; they do not run an emulator.
