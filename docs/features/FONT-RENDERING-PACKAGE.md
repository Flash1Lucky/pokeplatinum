# Standalone font rendering package

Baseline: upstream/main fe1ea418a7364c93daf0d07ae55e195452d8803c. Provenance: committed source 17a8139c4d3d04327266494098eff20e6a83cd8e. This extraction retains the four hand-edited PNG/JSON families, explicit NFGR resource inputs/order, width-aware plain/color name helpers and manager initialization. Native system/message/subscreen/unown assets and existing archive members0-7 retain their IDs. Family members8-11 are appended.

No record, ability, species, move catalog, saved-name length, save schema, proof application, fixture or debug-menu prerequisite. Native nickname capacity and Pokemon/save structs are unchanged. Existing callers retain their original rendering; callers opt into the new Font_TryFitName/Font_PrintName APIs explicitly. UI buffer/geometry adaptations are separate consumers.

Font source pixels come from the preserved committed checkpoint, not Eric's ongoing dirty font_narrower.png edit. That edit remains in the preferred checkout and preservation snapshot; it can be applied later when he finishes it. This package does not regenerate or overwrite it.

Run tools/tests/test_font_package.py, test_font_color_package.py and validate_font_package.py. They exercise actual C selection/drawing guards, all509 glyph advances and admitted color controls, plus atlas metadata. SDK source checks and resource conversion are retained in the task evidence. Runtime rendering and original-DS dynamic heap/stack checks remain pending; host/SDK results do not establish runtime acceptance. Do not run legacy compact-font generators over the editable family.
