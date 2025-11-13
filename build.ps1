<#!
.SYNOPSIS
  Builds WindowsAppSDK Samples solutions (or local solution(s)).
.DESCRIPTION
  PowerShell port of the original build.cmd logic.
  * Ensures Visual Studio Developer environment (DevShell) is initialized.
  * Downloads a local nuget.exe if not present and restores packages.
  * Builds one or more .sln files with MSBuild producing a .binlog per solution.

  Solution discovery order:
    1. If -Sample is specified: build all .sln files under Samples/<Sample>.
    2. Else if the current working directory (where you invoked the script) contains one or more .sln files: build those only (no recursion).
    3. Else build all .sln files under Samples (recursive).

.PARAMETER Platform
  Target platform (x86, x64, arm64, auto). Default: auto (arm64 on ARM64 OS, else x64).
.PARAMETER Configuration
  Build configuration (Debug or Release). Default: Release.
.PARAMETER Sample
  Optional sample folder name (child of Samples) to restrict the build scope.
.PARAMETER Help
  Show usage information.

.EXAMPLE
  ./build.ps1
  (Auto-detect platform, build local solutions or all samples.)

.EXAMPLE
  ./build.ps1 -Platform arm64 -Configuration Debug -Sample AppLifecycle
  (Build only the AppLifecycle sample solutions for arm64 Debug.)
#>
# Parameter definitions
[CmdletBinding()] param(
  [Parameter(Position = 0)] [ValidateSet('x86', 'x64', 'arm64', 'auto')] [string]$Platform = 'auto',
  [Parameter(Position = 1)] [ValidateSet('Debug', 'Release')] [string]$Configuration = 'Release',
  [Parameter(Position = 2)] [string]$Sample = '',
  [switch]$Help
)

# --- Functions (grouped at top for clarity) ---

# Show-Usage: Display help/usage information.
function Show-Usage {
  Write-Host 'Usage:'
  Write-Host '  build.ps1 [-Platform x86|x64|arm64|auto] [-Configuration Debug|Release] [-Sample <SampleFolder>]'
  Write-Host ''
  Write-Host 'Platform:'
  Write-Host '  auto (default)  Detects host OS arch: arm64 on ARM64, else x64.'
  Write-Host ''
  Write-Host 'Solution selection:'
  Write-Host '  -Sample <Name>  Build solutions under Samples/<Name>'
  Write-Host '  (no Sample)     If current directory has .sln file(s), build only those; otherwise build all Samples solutions.'
  Write-Host ''
  Write-Host 'Examples:'
  Write-Host '  ./build.ps1'
  Write-Host '  ./build.ps1 -Platform arm64 -Configuration Debug'
  Write-Host '  ./build.ps1 -Sample AppLifecycle'
}

# Initialize-VSEnvironment: Ensure VS dev environment & MSBuild are available.
function Initialize-VSEnvironment {
  if (Get-Command msbuild -ErrorAction SilentlyContinue) { Write-Host 'MSBuild already on PATH; assuming VS env initialized.'; return }
  if ($env:VSINSTALLDIR) { Write-Host "VS environment already initialized: $env:VSINSTALLDIR"; return }
  $vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
  if (-not (Test-Path $vswhere)) { Write-Error 'vswhere.exe not found. Install Visual Studio 2022 with MSBuild.'; exit 1 }
  $installationPath = & $vswhere -latest -prerelease -products * -requires Microsoft.Component.MSBuild -property installationPath | Select-Object -First 1
  if (-not $installationPath) { Write-Error 'Unable to locate a suitable Visual Studio installation.'; exit 1 }
  $devShellModule = Join-Path $installationPath 'Common7\Tools\Microsoft.VisualStudio.DevShell.dll'
  if (Test-Path $devShellModule) {
    Write-Host "Initializing VS DevShell from: $devShellModule" -ForegroundColor Yellow
    try { Import-Module $devShellModule -ErrorAction Stop; Enter-VsDevShell -VsInstallPath $installationPath -SkipAutomaticLocation | Out-Null }
    catch { Write-Warning "DevShell initialization failed: $($_.Exception.Message)" }
  }
}

# Resolve-Platform: Turn 'auto' into a concrete platform value.
function Resolve-Platform {
  param([string]$PlatformValue)
  if ($PlatformValue -ne 'auto') { return $PlatformValue }
  $detected = 'x64'
  try {
    $osArch = [System.Runtime.InteropServices.RuntimeInformation]::OSArchitecture.ToString().ToLowerInvariant()
    switch ($osArch) { 'arm64' { $detected = 'arm64' } 'x86' { $detected = 'x86' } default { $detected = 'x64' } }
    if ($detected -eq 'x86' -and $env:PROCESSOR_ARCHITEW6432) { $detected = 'x64' }
  }
  catch {}
  Write-Host "Auto-detected platform: $detected" -ForegroundColor Yellow
  return $detected
}

# Initialize-NuGetEnvironment: Create folders/download nuget.exe if needed.
function Initialize-NuGetEnvironment {
  if (-not (Test-Path $nugetDir)) { New-Item -ItemType Directory -Path $nugetDir | Out-Null }
  if (-not (Test-Path $nugetExe)) { Write-Host 'Downloading nuget.exe...'; Invoke-WebRequest -UseBasicParsing https://dist.nuget.org/win-x86-commandline/latest/nuget.exe -OutFile $nugetExe }
  if (-not (Test-Path $packagesDir)) { New-Item -ItemType Directory -Path $packagesDir | Out-Null }
}

# Get-Solutions: Discover which solutions to build.
function Get-Solutions {
  param([string]$SampleFilter)
  $targetRoot = $null; $solutions = @()
  if (-not [string]::IsNullOrWhiteSpace($SampleFilter)) {
    $targetRoot = Join-Path $samplesRoot $SampleFilter
    if (-not (Test-Path $targetRoot)) { Write-Error "Sample path not found: $targetRoot"; exit 1 }
    $solutions = Get-ChildItem -Path $targetRoot -Filter *.sln -Recurse | Sort-Object FullName
  }
  else {
    $currentDir = Get-Location
    $localSolutions = Get-ChildItem -Path $currentDir -Filter *.sln -File -ErrorAction SilentlyContinue | Sort-Object FullName
    if ($localSolutions) { Write-Host "Detected $($localSolutions.Count) solution(s) in current directory: $currentDir" -ForegroundColor Yellow; $solutions = $localSolutions }
    else { $targetRoot = $samplesRoot; $solutions = Get-ChildItem -Path $targetRoot -Filter *.sln -Recurse | Sort-Object FullName }
  }
  return $solutions
}

# Restore-Solution: Perform NuGet restore for a solution (.sln) using repo nuget.exe.
function Restore-Solution {
  param([string]$SolutionPath)
  Write-Host "Restoring: $SolutionPath" -ForegroundColor Cyan
  & $nugetExe restore $SolutionPath -ConfigFile (Join-Path $samplesRoot 'nuget.config') -PackagesDirectory $packagesDir
  if ($LASTEXITCODE -ne 0) { Write-Error 'NuGet restore failed.'; exit $LASTEXITCODE }
}

# Build-Solution: Invoke MSBuild on a solution with platform/config and emit binlog.
function Build-Solution {
  param([string]$SolutionPath, [string]$Platform, [string]$Configuration)
  $binlog = Join-Path (Split-Path $SolutionPath -Parent) ("{0}.binlog" -f ([IO.Path]::GetFileNameWithoutExtension($SolutionPath)))
  Write-Host "Building:  $SolutionPath" -ForegroundColor Cyan
  & msbuild /warnaserror /p:Platform=$Platform /p:Configuration=$Configuration /p:NugetPackageDirectory=$packagesDir /bl:"$binlog" "$SolutionPath"
  if ($LASTEXITCODE -ne 0) { Write-Error 'MSBuild failed.'; exit $LASTEXITCODE }
}

# --- Main Execution Flow ---
if ($Help -or $PSBoundParameters.ContainsKey('?')) { Show-Usage; return }
Initialize-VSEnvironment

if (-not $env:VSINSTALLDIR) { Write-Warning 'VSINSTALLDIR environment variable not found. Run this from a VS Developer PowerShell prompt.'; Show-Usage; exit 1 }

$Platform = Resolve-Platform -PlatformValue $Platform
$repoRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$nugetDir = Join-Path $repoRoot '.nuget'
$nugetExe = Join-Path $nugetDir 'nuget.exe'
$packagesDir = Join-Path $repoRoot 'packages'
$samplesRoot = Join-Path $repoRoot 'Samples'

Initialize-NuGetEnvironment
$solutions = Get-Solutions -SampleFilter $Sample

if (-not $solutions -or $solutions.Count -eq 0) {
  Write-Warning 'No solution files found to build.'
  exit 0
}

$stopwatch = [System.Diagnostics.Stopwatch]::StartNew()
$startTime = Get-Date

Write-Host "Building $($solutions.Count) solution(s) for Platform=$Platform Configuration=$Configuration" -ForegroundColor Green
foreach ($sln in $solutions) {
  Write-Host '---' -ForegroundColor Cyan
  Restore-Solution -SolutionPath $sln.FullName
  Build-Solution -SolutionPath $sln.FullName -Platform $Platform -Configuration $Configuration
}

$stopwatch.Stop()
$endTime = Get-Date

Write-Host '---'
Write-Host ("Start time: {0}. End time: {1}" -f $startTime.ToLongTimeString(), $endTime.ToLongTimeString())
Write-Host ("   Elapsed: {0}" -f [System.String]::Format('{0:hh\\:mm\\:ss\.ff}', $stopwatch.Elapsed))
