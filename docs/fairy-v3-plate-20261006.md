# Fairy v3: Pixie Plate icon and accepted v2 evidence

Recorded 2026-10-06T05:29:14.797008+00:00. Both v3 ROM builds and resource checks passed. The current battle-test menu and Fairy battle-icon artwork were not edited. No emulator/save actions, commits, pushes or merges were performed.

## Latest observed runtime — v2 Fairy group PASS

`pokeplatinum-fairy/build/runtime-20261006-010309-120-3f25a564` identifies combined v2 SHA-1 `fae025b2dd7bff27255edb23b819fef54db662fc`. Started October 6 at 01:03:09 -04:00; finished 01:12:38 -04:00; state exited, emulator exit 0.

The complete fixture-v5 group=1 capture contains cases 9–16 and END: **8 completed, 8 passes, 0 probes, 0 failures, 8 matching repeats**. Actual damages match 54, 108, 27, 81, 0, 38, 74, 20. The current parser returns PASS. There is no Run All session in this capture; do not report full-suite acceptance from it.

Eric explicitly confirms Fairy icons load in battle and Pokedex, and giving Arceus Pixie Plate produces Fairy typing and the desired sprite. These observations are accepted. Removal, PC persistence, ordinary save-close-reload, shiny views and full front/back coverage are not individually confirmed.

## Editable Fairy battle/summary icon

Edit this active combined SOURCE IMAGE:

`C:/Games/Pokemon/Decomps/pokeplatinum-fairy/res/graphics/battle/type_icons/fairy.png`

It is **32×16 pixels, PNG, indexed/paletted mode P**. Its current PNG container is 8-bit indexed; the build explicitly emits native 4-bit graphics. Preserve the existing indexed palette and use indices 0, 8, 9, 10, 14, 15 (index 0 is transparent at runtime). Avoid RGB/RGBA export, antialiasing/new colors or dimension changes. A correctly exported 4-bit indexed PNG with the same indices is also compatible.

Runtime color comes from bank 1 of `res/graphics/battle/type_icons/shared.pal`, not the embedded PNG preview palette. In that bank: index 8 pink fill, 9 light edge, 10 dark edge, 14 letter shadow, 15 white lettering. Editing only embedded RGB palette values will not recolor the game. Shared palette changes would affect other users of that bank and are outside this artwork touch-up.

Build linkage: `res/graphics/battle/type_icons/meson.build` includes `fairy.png` with `-bitdepth 4`; `res/graphics/battle/sprites.order` appends `type_icons/fairy.NCGR.lz`; `src/type_icon.c` maps TYPE_FAIRY to that member and palette bank 1. Generated output is `build/res/graphics/battle/sprites.narc.p/type_icons/fairy.NCGR.lz`, packed into `build/res/graphics/battle/sprites.narc` at member 343. Do not edit generated outputs.

The independent minimal counterpart is `C:/Games/Pokemon/Decomps/pokeplatinum-fairy-minimal/res/graphics/battle/type_icons/fairy.png`. No artwork was copied between worktrees in this task. For later synchronization, inspect Eric's current combined PNG and compare with the recorded hash, preserve a backup, copy only the approved final source PNG to minimal, then rebuild and verify both. Do not restore a cached copy over Eric's edits.

The separate Pokedex image is `res/graphics/pokedex/type_icons.png`; editing the battle icon does not edit that sheet.

## Pixie Plate correction

The item mapping was already correct: item 468 → data record 446, shared `flame_plate_NCGR` tile at icon member 619, `pixie_plate_NCLR` at appended palette member 711. The wrong appearance came from the brown palette imported from March's `5b6024694` candidate.

V3 restores the intended pink palette from February's `f48e64975:res/graphics/item_icons/pixie_plate.pal`. That original branch also used the shared flame-plate tile, confirming the intended shape. The active source is `res/items/icons/pixie_plate.pal`; its first 16 original colors are retained as a 16-color CRLF JASC palette, matching the current converter. No new art was invented, no item or archive IDs shifted, and no other item's palette was changed.

## Artifacts and verification

| Variant | Preserved ROM | SHA-1 |
|---|---|---|
| combined | `C:\Games\Pokemon\Decomps\pokeplatinum-fairy\build\fairy-20261006-combined-v3.nds` | `46236258b384b1ec39b4cdb2d4a502e609b1269e` |
| minimal | `C:\Games\Pokemon\Decomps\pokeplatinum-fairy-minimal\build\fairy-20261006-minimal-v3.nds` | `3a82c4ec1671d95eb65ce85a3c87607083835c09` |

Both are 134,217,728 bytes. Task workspace `fairy-v3-artifacts-20261006.json` records SHA-256 and source-art hashes. Prior v1/v2 delivered ROMs and all 14 preserved original working files still match their hashes.

Both `build/recovery/fairy-v3-<variant>.log`/`.exit` show successful builds and resource checks. Existing Arceus/shadow, item-icon and item-data prefixes remain byte-identical to clean upstream. The new dedicated check verifies the actual built Pixie Plate mapping, original pink colors at member 711 and unchanged 712-member icon archive length. Existing Fairy resource checks also pass. Both diffs pass whitespace checks.

The combined archive's actual mapped plate tile and palette were decoded at their native 32×32 size and visually inspected as the intended pink plate. The palette generator retains the upstream 256-color padded binary layout, with the intended 16 colors in the first bank. This is asset-level verification, not emulator acceptance.

The v2 gameplay/runtime fixes and current All/Fairy test menu are unchanged. The prior nine host contracts and nineteen protocol tests remain their recorded v2 evidence; this palette-only iteration runs relevant packed-resource checks rather than claiming new emulator results.

## Remaining acceptance and direction

Check the Pixie Plate bag/item-preview icon in the named v3 ROM. That new palette has not yet been visually accepted in the emulator. Eric's accepted v2 Fairy group and icon/equip observations remain accepted evidence for that exact v2 artifact; v3 has not been run in the emulator by this task.

Unconfirmed boundaries: removal returns Arceus to Normal, PC deposit/retrieve persistence, ordinary save-close-reload, shiny and complete front/back views, the minimal ROM's runtime behavior, and a complete Run All capture on v2/v3. Do not silently mark these passed or require repetition of already accepted observations.

Leave the current test menu unchanged. Future testing should prioritize checks the assistant can execute directly; manual testing should focus on visuals and custom situations. This task adds only a packed-icon regression check and does not redesign the harness.
