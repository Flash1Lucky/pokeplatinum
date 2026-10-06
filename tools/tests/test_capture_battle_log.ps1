# Real process regression. Only --help is executed; no ROM/game is launched.
$ErrorActionPreference = 'Stop'
$helper = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..\capture-battle-log.ps1'))
$testRoot = Join-Path ([IO.Path]::GetTempPath()) ('battle capture [literal] ' + [guid]::NewGuid().ToString('N'))
[IO.Directory]::CreateDirectory($testRoot) | Out-Null
$sentinel = Join-Path $testRoot 'preserve [save].sav'
[IO.File]::WriteAllText($sentinel, 'Synthetic sentinel only; never loaded into an emulator.')
$before = (Get-FileHash -LiteralPath $sentinel).Hash
$shellBinary = (Get-Process -Id $PID).Path
& $shellBinary -NoProfile -File $helper -VerifyLaunch -LogRoot $testRoot
if ($LASTEXITCODE -ne 0) { throw "Helper exit $LASTEXITCODE" }
$runs = @(Get-ChildItem -LiteralPath $testRoot -Directory)
if ($runs.Count -ne 1) { throw 'Expected one new log directory' }
$metadata = Get-Content -LiteralPath (Join-Path $runs[0].FullName 'session.json') -Raw | ConvertFrom-Json
$stdout = Get-Content -LiteralPath (Join-Path $runs[0].FullName 'stdout.log') -Raw
if ($metadata.state -ne 'exited' -or $metadata.exitCode -ne 0 -or $metadata.mode -ne 'help-only launch verification') { throw 'Incorrect launch result metadata' }
if ($stdout -notmatch 'melonDS 1\.1' -or $stdout -notmatch 'Usage:') { throw 'Real emulator help was not captured' }
if (-not (Test-Path -LiteralPath (Join-Path $runs[0].FullName 'stderr.log'))) { throw 'Missing stderr file' }
if ((Get-FileHash -LiteralPath $sentinel).Hash -ne $before) { throw 'Save sentinel changed' }
Write-Output "PASS: actual melonDS --help; literal executable, working directory and log paths; stdout/stderr capture; exit 0; save sentinel unchanged. Evidence: $($runs[0].FullName)"
