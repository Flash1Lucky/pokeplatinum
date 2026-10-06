# Fairy icon color check — 2026-10-06

Supplied tutorial image: Library `libfile_482fce089ce081918675ac8a6d980407`, `image(20261006-053907).png`. Actual file is 32×16 RGBA, not indexed. Exactly five opaque colors and transparent black are used; no antialiasing or partial alpha occurs.

All supplied colors match the existing shared palette exactly, even before RGB555 conversion. All fit together in zero-based bank 1:

| Index | RGB8 | RGB555 | Pixel count |
|---|---|---|---|
| 0 | transparent black | 0,0,0 | 68 |
| 9 | 255,197,180 | 31,24,22 | 32 |
| 11 | 164,65,164 | 20,8,20 | 32 |
| 12 | 222,131,189 | 27,16,23 | 237 |
| 14 | 131,123,115 | 16,15,14 | 77 |
| 15 | 255,255,255 | 31,31,31 | 66 |

No missing colors; no palette expansion, shared palette changes, or loader changes. Prior diagnosis described the previous project's icon, whose pink fill reused Psychic index 8; it did not describe this supplied tutorial icon, whose fill uses index 12.

Current Poison icon uses indices 11/12, while Psychic and Cute use index 9 along with 8/10. The supplied icon combines 9/11/12 from the same bank; it does not require a special contest palette.

`fairy-editable.png` is a 4-bit indexed conversion with all 512 rendered RGBA pixels identical to the supplied original. The original is preserved as `tutorial-fairy-original.png`. Previous project artwork and source files are backed up in `before/<worktree>/`. Supplied lettering is provisional: Eric will edit it to match the other icons. No lettering redesign was performed.

Editable active source: `C:\Games\Pokemon\Decomps\pokeplatinum-fairy\res\graphics\battle\type_icons\fairy.png`; identical minimal source under `pokeplatinum-fairy-minimal`. Keep 32×16 indexed PNG, preserve palette ordering, use transparent index 0 and existing colors (especially 14 shadow / 15 white lettering). Disable antialiasing, resampling and automatic palette optimization/reordering. The build reads pixel indices and uses shared.pal; changing only PNG palette colors does not change in-game colors. Make later lettering edits in the active source and synchronize the minimal source before rebuilding.

Icon bank selection already maps Fairy 18 to bank 1. Existing helpers load three banks; all relevant UI consumers already select through TypeIcon_GetPltt. Pokedex uses its separate type_icons.png/type_icons.pal and was not altered in this correction.

The separate move-selection border now uses the tutorial's 16-entry sMovePaletteFairy array, appended at table index 18. Mystery remains index 9, and every original move palette and table entry is preserved. Source: https://github.com/pret/pokeplatinum/wiki/Tutorial%3A-Adding-Fairy-Type (page edited May 2, 2026; checked October 6, 2026). The tutorial's replacement of Mystery was intentionally not adopted.

Library editable copy: `libfile_f79ee8a9287081919f7f9a68e0f3945d`, `fairy-editable.png`, version 0. This is a separate editable copy; the supplied reference remains untouched.

## Built v4 artifacts

- combined: `C:\Games\Pokemon\Decomps\pokeplatinum-fairy\build\fairy-20261006-combined-v4.nds`; SHA1 `f361a2ed0a6c2a3253bd25aff205b9b697551d46`.
- minimal: `C:\Games\Pokemon\Decomps\pokeplatinum-fairy-minimal\build\fairy-20261006-minimal-v4.nds`; SHA1 `fa0446431bc0518eafb36dcc65dd3716952c750f`.

Both builds and packed-resource checks passed, including exact supplied icon pixel/color matching, all prior battle/contest/Mystery archive members unchanged, unchanged Pokedex assets, the compiled tutorial move-button palette, nine host Fairy contracts per build, Pixie Plate palette, and Arceus asset checks. This is build/resource validation, not a new emulator acceptance result. v1/v2/v3 ROMs are retained. No commit, push, merge, game/save operation or debug-menu changes were performed. Supplied lettering remains provisional for Eric to edit.

Build-launcher incident: the first v4 shell passed Windows CR characters into Bash, producing invalid pipefail option / command-not-found before Ninja. The launcher now strips CR before invocation. The screenshot is retained locally as fairy-color-review/build-error.png. No persistent security setting changes were made.
