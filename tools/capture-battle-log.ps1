[CmdletBinding(DefaultParameterSetName='Game')]
param(
    [Parameter(Mandatory=$true, ParameterSetName='Game')][string]$Rom,
    [Parameter(Mandatory=$true, ParameterSetName='Verify')][switch]$VerifyLaunch,
    [string]$LogRoot
)
$ErrorActionPreference = 'Stop'
$emulatorFile = Get-Item -LiteralPath 'C:\Games\Emulation\[DS] melonDS-1.1-windows-x86_64\melonDS.exe'
if ($VerifyLaunch) {
    $launchArgument = '--help'
    if (-not $LogRoot) { $LogRoot = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..\build')) }
} else {
    $romFile = Get-Item -LiteralPath $Rom
    if ($romFile.PSIsContainer -or $romFile.Extension -ne '.nds') { throw 'Expected a built .nds test ROM' }
    if (Get-Process melonDS -ErrorAction SilentlyContinue) { throw 'Close melonDS normally before starting a separate logged test session.' }
    # Windows filenames cannot contain quotes. Quoting the absolute filename
    # keeps spaces and brackets in a single argument, including on PowerShell 5.1.
    $launchArgument = '"' + $romFile.FullName + '"'
    if (-not $LogRoot) { $LogRoot = $romFile.DirectoryName }
}
$logRootItem = Get-Item -LiteralPath $LogRoot
if (-not $logRootItem.PSIsContainer) { throw 'LogRoot must be an existing directory' }
$logDir = Join-Path $logRootItem.FullName ('runtime-' + (Get-Date -Format 'yyyyMMdd-HHmmss-fff') + '-' + [guid]::NewGuid().ToString('N').Substring(0,8))
[IO.Directory]::CreateDirectory($logDir) | Out-Null
$metadataPath = Join-Path $logDir 'session.json'
$metadata = [ordered]@{
    mode = $(if ($VerifyLaunch) { 'help-only launch verification' } else { 'user game session' })
    emulator = $emulatorFile.FullName
    version = $emulatorFile.VersionInfo.ProductVersion
    started = (Get-Date).ToString('o')
    state = 'starting'
    acceptance = 'not established by launcher'
}
if (-not $VerifyLaunch) {
    $metadata.rom = $romFile.FullName
    $metadata.sha1 = (Get-FileHash -LiteralPath $romFile.FullName -Algorithm SHA1).Hash
}
$metadata | ConvertTo-Json | Set-Content -LiteralPath $metadataPath
Write-Host "Logs: $logDir"
if (-not $VerifyLaunch) {
    Write-Host 'Run R+Y, then A in the overworld. Close melonDS normally to flush stdout.'
    Write-Host 'Use a disposable save for this separately named ROM. This launcher never copies, replaces, or deletes saves.'
}
$process = New-Object System.Diagnostics.Process
$info = New-Object System.Diagnostics.ProcessStartInfo
$info.FileName = $emulatorFile.FullName
$info.WorkingDirectory = $emulatorFile.DirectoryName
$info.Arguments = $launchArgument
$info.UseShellExecute = $false
$info.RedirectStandardOutput = $true
$info.RedirectStandardError = $true
$info.CreateNoWindow = [bool]$VerifyLaunch
$process.StartInfo = $info
$stdoutFile = $null
$stderrFile = $null
$exitCode = 1
try {
    # .NET treats these paths literally, unlike Start-Process WorkingDirectory.
    $stdoutFile = [IO.File]::Open((Join-Path $logDir 'stdout.log'), [IO.FileMode]::CreateNew, [IO.FileAccess]::Write, [IO.FileShare]::Read)
    $stderrFile = [IO.File]::Open((Join-Path $logDir 'stderr.log'), [IO.FileMode]::CreateNew, [IO.FileAccess]::Write, [IO.FileShare]::Read)
    if (-not $process.Start()) { throw 'melonDS process did not start' }
    $metadata.state = 'running'
    $metadata.pid = $process.Id
    $metadata | ConvertTo-Json | Set-Content -LiteralPath $metadataPath
    # Drain both streams concurrently: neither a full stdout nor stderr pipe
    # can block the emulator while WaitForExit waits for the user to close it.
    $stdoutCopy = $process.StandardOutput.BaseStream.CopyToAsync($stdoutFile)
    $stderrCopy = $process.StandardError.BaseStream.CopyToAsync($stderrFile)
    $process.WaitForExit()
    [void]$stdoutCopy.GetAwaiter().GetResult()
    [void]$stderrCopy.GetAwaiter().GetResult()
    $exitCode = $process.ExitCode
    $metadata.state = 'exited'
    $metadata.exitCode = $exitCode
} catch {
    $metadata.state = 'launcher_failed'
    $metadata.error = $_.Exception.Message
    throw
} finally {
    if ($stdoutFile) { $stdoutFile.Dispose() }
    if ($stderrFile) { $stderrFile.Dispose() }
    $metadata.finished = (Get-Date).ToString('o')
    $metadata | ConvertTo-Json | Set-Content -LiteralPath $metadataPath
    $process.Dispose()
}
Write-Host "melonDS exited with code $exitCode. Retained stdout.log is not by itself a test pass."
exit $exitCode
