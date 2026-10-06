# Local development-tools checkpoint — 2026-10-06

This checkpoint preserves the existing structured BTEST1 logging, capture/parser tooling and deferred battle-test window rendering. The canonical debug-tools checkout additionally retains its debug-menu handoff and separately committed Shiny/Set Var changes; the standalone harness checkout retains its independent configuration and does not acquire Create/Edit Mon tools or Fairy fixtures.

Final checkpoint checks: eight synthetic parser tests passed per generic harness checkout. The combined game additionally passed nineteen protocol/group tests and current Shiny/Set Var regressions. No new ROM build, emulator invocation or save operation is part of this checkpoint. Existing dated build/runtime notes remain the source for earlier artifact evidence; local commits do not expand runtime acceptance.

The Windows capture helper retains its documented local emulator path and requires explicit user launch. Its help-only process regression is retained but was not executed during this no-emulator checkpoint. Powershell syntax is checked without launching the helper. ROMs, runtime logs, local SDK/tool links and helper backups are excluded from commits.
