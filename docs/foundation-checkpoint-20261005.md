# Foundation harness checkpoint - 2026-10-05

Build matrix: **PASSED** (matrix exit 0, corrective combined exit 0; 3/3 final named ROMs recorded). Runtime acceptance: **pending** for all new ROMs. The preserved v3 ROM remains the last Eric-validated harness.

This checkpoint supersedes the September 29 "matrix is running" status. That earlier job completed disabled and combined v4 ROMs, then failed standalone configuration at the relative Metroskrew MSL library path. The standalone checkout now uses an untracked local symlink to the already available baseline toolchain. No dependencies were downloaded, no global settings changed, and no proprietary tools/libraries were published.

| Output | SHA-1 |
| --- | --- |
| `battle-tests-20261005-191143-disabled.nds` | `af03eef308cbfc18cfd811f605aafeb775d19209` |
| `battle-tests-20261005-191143-standalone-log.nds` | `4300cee9cfbd314c9e408d0f03b4de6791ce286f` |
| `battle-tests-20261005-combined-log-ram.nds` | `92112fb6005cc317cec4accf7c7e01551733a066` |

Combined/disabled outputs are under `C:\Games\Pokemon\Decomps\pokeplatinum-debug-tools\build`; standalone is under `C:\Games\Pokemon\Decomps\pokeplatinum-battle-tests\build`. The disabled hash matches the prior v4 disabled ROM exactly (`af03eef308cbfc18cfd811f605aafeb775d19209`). Disabled and combined use the existing build cache; this is not a claim of fresh clean-room installation. Standalone config uses `--wrap-mode=nodownload`; its existing local dependencies are retained.

Evidence: `C:\Games\Pokemon\Decomps\pokeplatinum-debug-tools\build\recovery\checkpoint-20261005-191143.log`, `.sha1` manifest, and `.exit` marker; plus `combined-log-ram-20261005.log`, `.sha1` and `.exit`. Launcher: `build/recovery/validate-harness-20261005.ps1`, followed by `build/recovery/rebuild-combined-log-20261005.ps1` in the combined worktree. Existing v3 and v4 ROMs were preserved and their hashes rechecked. The first new combined ROM (SHA-1 `a5f39bcfff62429df77ab6e66e3a3786d4fe5952`) is superseded: source review showed that no$gba string output reads ordinary RAM, so the formatting buffer was moved from possible DTCM stack memory into static main RAM. Standalone received this correction before its source compilation. Use only the final outputs listed above for acceptance.

The change adds default-off `battle_test_logging`, bounded no$gba debug-string output, completed-case/repeat records and an END summary after field processing resumes. Game assertions and fixtures are unchanged. Both worktrees contain a strict log parser and user-run capture helper. Eight synthetic parser tests passed; existing host C report tests passed before building. A mocked PowerShell helper invocation verified quoted paths, emulator working directory, unique metadata with ROM hash, and an unchanged synthetic save sentinel; it did not launch an emulator. These checks do not establish emulator runtime acceptance. The installed melonDS 1.1 --help process successfully redirected stdout without loading a ROM; real BTEST capture is pending. The corrected combined logger buffer's linked address is `0x021029F4`, ordinary main RAM.

See [capture instructions and protocol](battle-test-logging.md). Eric runs the emulator. Validate standalone R+Y suite and B/field restoration, party/bag/location preservation, restart repeatability; combined R+Y and R+X/editor/results agreement; and disabled R+X/R+Y plus ordinary field/battle/save behavior. Tie reports to ROM SHA-1. Expected suite: 8 complete, 7 pass, 1 intentional probe, 0 unexpected failures, 8 matching repeats.

Source remains uncommitted on `work/debug-tools-recovery` (base tip `573c1c9bf`) and `work/battle-harness-extraction` (base tip `80e497ba8`). No merge, push, promotion, Fairy work, or roster import occurred. Do not commit the standalone `tools/metroskrew` symlink. Preserve the known v3 checkpoint while accepting these candidates.

Final standalone static-buffer check: compiled object contains the local static buffer; the linked buffer address is 0x021028F4 (ordinary main RAM). Both worktrees have identical debug_battle.c SHA-256 0a8bc00035eed365feeb90e99a3d4fb063af36a0f86aa76800b911ecaf148edb. All three ROMs are 134,217,728 bytes. Final hashes were recomputed from the delivered files.

## Acceptance correction — user evidence from October 5, 21:34 EDT

Eric explicitly confirmed the CURRENT disabled artifact `battle-tests-20261005-191143-disabled.nds`, SHA-1 `af03eef308cbfc18cfd811f605aafeb775d19209`, opens none of the tools. Both R+X and R+Y gating are accepted as user-confirmed. Source: message Sentinel_6e40722fe718819183be8b6330156a57, following the exact artifact path supplied at 21:24 EDT. This is additional current-artifact evidence, not merely the older R+X-only checkpoint. Earlier statements here listing current shortcut gating as pending are superseded.

Eric later confirmed combined R+Y and R+X both open. Record that opening behavior as user-confirmed without inferring page navigation, editor functionality, or the newest `combined-debug-entry` ROM's Battle Tests handoff/run/result/return details. Those latest handoff details still await explicit confirmation. Ordinary battle/save/reload checks on the current disabled ROM remain unconfirmed; shortcut gating does not require repeating.
