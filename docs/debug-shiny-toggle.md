# Shiny control — debug tools only

Create Mon and Edit Mon now expose **Shiny: ON/OFF** on page 1, below Nature. Select with A, change with Up/Down, accept with A or undo with B; START creates/saves as before. The current state is derived from the actual PID and full OT ID, including after direct PID, Nature or OT-ID edits.

Toggling changes PID, preserving the full OT ID, nature, actual species gender, ability parity and selected ability, IVs/EVs, moves, held item and selected form. The PID search also preserves Wurmple's evolution branch. PID changes pass through the existing engine re-encryption/reordering helper; existing Pokemon setters maintain checksums. No raw save or checksum writes are introduced. This is a debug editor, not an encounter-legality generator.

The title explicitly warns before editing that PID changes can change Spinda spots. An unsuccessful bounded search leaves everything unchanged and displays a message. Changing Nature or OT ID afterward can change shininess; the displayed Shiny field refreshes accordingly. Use Shiny after those edits when constructing a particular test mon.

This change belongs to canonical `pokeplatinum-debug-tools` and is copied as an optional development-tool change into `pokeplatinum-fairy`. It is excluded from `pokeplatinum-fairy-minimal` and the reusable Fairy feature. It neither requires nor adds Fairy types, items, assets or battle fixtures.

Focused host tests cover 200,000 target shiny states plus inverse toggles across gender ratios, PID/OT extremes and deterministic samples, checking nature, gender, ability parity, Wurmple evolution branch and idempotence. Native editor compilation and text encoding are checked separately. In-game control appearance, cancellation and party/PC persistence still require user acceptance. No emulator or save was operated.
