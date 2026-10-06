# Battle harness log capture (2026-10-05)

Status: all three configurations built successfully; see [the verified build checkpoint](foundation-checkpoint-20261005.md). User-run emulator capture and runtime acceptance remain pending.

## Existing evidence

Eric confirmed v2 repeatability, restart and party/bag/location isolation, and v3's eight cases with seven passes and one intentional failure probe. The v3 ROM SHA-1 is `f9cb71b373b454de84f3c37a7212681a58be5213`. These tests are substantial but do not validate v4's standalone R+Y window or extraction.

The September 29 matrix built disabled and combined v4 ROMs, then stopped at standalone Meson configuration: missing `tools/metroskrew/lib/metroskrew/sdk/ds/2.0/sp2/msl/MSL_C/MSL_ARM/Lib/MSL_C_NITRO_Ai_LE.a`. Compiler commands already used the main checkout's toolchain, while the library paths were relative to the standalone checkout. A local WSL symlink now connects standalone `tools/metroskrew` to the existing baseline toolchain. No SDK/library download or distribution is involved. This machine-specific symlink is untracked and must not be committed; it is local build setup, not a feature dependency to publish.

## Logging design

Enable `-Dbattle_tests=true -Dbattle_test_logging=true`. Both default false. The generated logging gate requires both; disabling battle tests also suppresses all harness log calls. General `logging_enabled` is unnecessary. In-game results remain available. No assertions, battle fixtures, RNG adapters, save layout, or menu behavior are changed by logging.

The older harness called `OS_Printf`, which is a no-op under this build's `SDK_FINALROM`. The new adapter uses a static main-RAM buffer and `OS_VSNPrintf` to format bounded ASCII lines and the no$gba debug-string register `0x04FFFA18` to emit them. Messages stay below melonDS's 120-byte input limit; formatting overflow produces an explicit error record. The emitted values come from the existing completed assertions and repeat comparison, not emulator startup diagnostics.

Protocol:

```
BTEST1 BEGIN fixture=3 cases=8
BTEST1 CASE id=1 fail=0 expected=0 repeat=1 damage=28 target=28
...
BTEST1 CASE id=8 fail=8 expected=8 repeat=1 damage=28 target=0
BTEST1 END completed=8 pass=7 probe=1 fail=0 repeat=8
```

This is an illustrative expected trace, not observed runtime output. A CASE is emitted only after both runs and their comparison. END is emitted after field processing resumes. A deliberate failure probe counts as successful detection only when its exact expected mask is present; unexpected pass/additional failures fail the suite. Absence of END is incomplete, never a pass. This is not yet an automatic suite launcher, emulator driver, timeout/recovery framework, or full battle coverage.

## Installed emulator and supported route

Verified installed executable: `C:\Games\Emulation\[DS] melonDS-1.1-windows-x86_64\melonDS.exe`, file/product version 1.1. Its current config has JIT disabled and GDB disabled. No emulator config changes, plugin installs, listening ports, or game sessions were started for this investigation. A `melonDS.exe --help` subprocess exited 0 and successfully wrote its 1.1 help text to redirected stdout, verifying basic Windows stdout capture without loading a ROM.

Official melonDS 1.1 source handles ARM9 writes to `0x04FFFA18`, reads up to 120 bytes, and calls the frontend logger, which uses `vprintf` to stdout. The installed version therefore supports the transport in source; successful Windows stdout capture of these game messages still requires a real runtime check. Buffered stdout may not be complete until melonDS closes normally.

- [Official 1.1 NDS source: no$gba debug registers and NocashPrint](https://github.com/melonDS-emu/melonDS/blob/1.1/src/NDS.cpp)
- [Official 1.1 frontend logger](https://github.com/melonDS-emu/melonDS/blob/1.1/src/frontend/qt_sdl/Platform.cpp)
- Local upstream precedent: `src/debug.c` and `include/debug.h` use the same debug-register family for general emulator logging.

## User-run capture

Close any existing melonDS session normally first. Use a separately named test ROM and disposable save. The helper neither copies nor overwrites saves and does not alter emulator settings. It launches the existing interactive emulator only when Eric runs it:

```powershell
& .\tools\capture-battle-log.ps1 -Rom 'C:\full\path\to\battle-tests-TIMESTAMP-standalone-log.nds'
```

It records ROM SHA-1, emulator version, stdout and stderr in a new `runtime-TIMESTAMP` directory beside the ROM. In the normal overworld hold DS R and press Y, then A to run. Reopen R+Y afterward to compare the summary, test B and movement, and inspect the original party, bag and location. Close melonDS normally to flush the log. Repeat once after restarting.

```powershell
python .\tools\parse_battle_log.py 'C:\full\path\to\runtime-TIMESTAMP\stdout.log'
```

Exit 0 means a complete supported trace whose records and expected assertions agree; exit 1 means completed test failure; exit 2 means missing/malformed/unsupported output. These results do not certify menu/editor operation, save persistence, or general gameplay. The parser's synthetic tests are protocol tests, not emulator evidence.

Also check the disabled ROM: both R+X and R+Y remain closed and normal movement/battle/save behavior works. In combined logging ROM, check both R+Y and the R+X editor/menu route, including agreement of their suite views. Keep each acceptance result tied to the exact ROM hash.

## Promotion gate

Keep both work branches and the validated v3 checkpoint. No feature/foundation promotion, merge, or push until minimal and combined runtime acceptance is recorded. Then resolve remaining debug-tool edge cases and publish the dependency contract before the data/save audit and Fairy recovery.

Host parser validation: eight synthetic protocol tests passed on 2026-10-05. Run from the repository with PYTHONPATH=tools and python -m unittest discover -s tools/tests -p test_parse_battle_log.py. These are not runtime traces.

## Literal-path launcher fix — 2026-10-06 UTC

The original Start-Process WorkingDirectory handling interpreted [DS] as a wildcard. Both repository helpers now use System.Diagnostics.ProcessStartInfo with literal executable and working-directory paths, concurrent stdout/stderr draining, unique retained logs, actual exit-code propagation, and failed-start metadata. No ROM rebuild is required.

Actual installed melonDS 1.1 --help passed through both deployed helpers under normal PowerShell 7.6.5 permissions, without loading a ROM. Executable/working-directory and log paths included spaces and brackets; stdout help and stderr files were captured; synthetic save sentinels remained unchanged. Latest evidence: temp directories `battle capture [literal] ac2456d1ee77458a9289943cdf2a44e1` and `battle capture [literal] 337a319ec32543afa5a32724f19b9c43`.

A temporary copy of the same capture code using a PowerShell fixture also captured nonempty stdout and stderr, propagated exit 7, and recorded a missing-executable failure with nonzero status. Evidence: `C:\Users\Eric\AppData\Local\Temp\capture failure [paths] 29589188c98547b8a53b5bfd8add2cf9`. This tests launcher failures, not game behavior. Real BTEST runtime capture remains pending. No game was launched and no existing saves or failed-session logs were changed.
