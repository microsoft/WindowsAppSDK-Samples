<#
.SYNOPSIS
  Builds all WindowsAppSDK Samples and produces a structured pass/fail report.

.DESCRIPTION
  Wraps the same MSBuild logic as build.ps1 but:
  - Continues on failure (never exits early).
  - Classifies each solution as 'stable' or 'experimental'.
  - Filters via -Mode to build only what you need.
  - Accepts -ExtraProperties for ad-hoc MSBuild overrides (e.g. removing NoWarn).
  - Writes a JSON report and prints a console summary table at the end.

.PARAMETER Mode
  Which samples to build: all (default), stable-only, experimental-only.

.PARAMETER Platform
  Target platform: x86, x64, arm64, auto (default). 'auto' picks arm64 on ARM64 OS, else x64.

.PARAMETER Configuration
  Build configuration: Debug or Release (default).

.PARAMETER ReportPath
  Path for the JSON report file. Defaults to build-report-<timestamp>.json in the repo root.

.PARAMETER ExtraProperties
  Extra MSBuild properties appended verbatim, e.g. '/p:NoWarn= /p:TreatWarningsAsErrors=false'.
  Useful when experimenting with warning suppression changes.

.PARAMETER Sample
  Restrict to a single sample folder name under Samples/, e.g. 'AppLifecycle' or 'WindowsML'.

.EXAMPLE
  # Baseline: build everything, continue on failure
  ./build-report.ps1

.EXAMPLE
  # Build only stable samples
  ./build-report.ps1 -Mode stable-only

.EXAMPLE
  # Build experimental samples without the CS8305 suppression to surface hidden warnings
  ./build-report.ps1 -Mode experimental-only -ExtraProperties '/p:NoWarn='
#>

[CmdletBinding()]
param(
    [ValidateSet('all', 'stable-only', 'experimental-only')]
    [string]$Mode = 'all',

    [ValidateSet('x86', 'x64', 'arm64', 'auto')]
    [string]$Platform = 'auto',

    [ValidateSet('Debug', 'Release')]
    [string]$Configuration = 'Release',

    [string]$ReportPath = '',

    [string]$ExtraProperties = '',

    [string]$Sample = ''
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Continue'   # never abort the script on non-terminating errors

# ---------------------------------------------------------------------------
# Experimental sample roots (relative to Samples\, forward- or back-slash)
# Any .sln whose path begins with one of these prefixes is classified 'experimental'.
# Update this list when new experimental samples are added.
# ---------------------------------------------------------------------------
$script:ExperimentalPrefixes = @(
    'AppContentSearch',
    'WindowsAIFoundry',
    'WindowsML',
    'WinUI\ConditionalPredicate',
    'WinUI/ConditionalPredicate'
)

# ---------------------------------------------------------------------------
# Helper functions
# ---------------------------------------------------------------------------

function Initialize-VSEnvironment {
    if (Get-Command msbuild -ErrorAction SilentlyContinue) {
        Write-Host 'MSBuild already on PATH.' -ForegroundColor DarkGray
        return
    }
    if ($env:VSINSTALLDIR) {
        Write-Host "VS environment already initialized: $env:VSINSTALLDIR" -ForegroundColor DarkGray
        return
    }
    # vswhere is always installed at the x86 Program Files path regardless of where VS itself lives
    $vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
    if (-not (Test-Path $vswhere)) {
        # Fallback: try the 64-bit Program Files path
        $vswhere = Join-Path $env:ProgramFiles 'Microsoft Visual Studio\Installer\vswhere.exe'
    }
    if (-not (Test-Path $vswhere)) {
        Write-Error 'vswhere.exe not found. Install Visual Studio 2022 with MSBuild.'
        exit 1
    }
    $installPath = & $vswhere -latest -prerelease -products * -requires Microsoft.Component.MSBuild -property installationPath |
                   Select-Object -First 1
    if (-not $installPath) {
        Write-Error 'No suitable Visual Studio installation found.'
        exit 1
    }
    $devShellDll = Join-Path $installPath 'Common7\Tools\Microsoft.VisualStudio.DevShell.dll'
    if (Test-Path $devShellDll) {
        Write-Host "Initializing VS DevShell: $installPath" -ForegroundColor Yellow
        try {
            Import-Module $devShellDll -ErrorAction Stop
            Enter-VsDevShell -VsInstallPath $installPath -SkipAutomaticLocation | Out-Null
        } catch {
            Write-Warning "DevShell init failed: $($_.Exception.Message)"
        }
    }
}

function Resolve-Platform {
    param([string]$Value)
    if ($Value -ne 'auto') { return $Value }
    try {
        $arch = [System.Runtime.InteropServices.RuntimeInformation]::OSArchitecture.ToString().ToLowerInvariant()
        switch ($arch) {
            'arm64' { return 'arm64' }
            'x86'   { return 'x64'  }   # WOW64 – prefer x64
            default { return 'x64'  }
        }
    } catch { return 'x64' }
}

function Initialize-NuGet {
    if (-not (Test-Path $script:nugetDir))   { New-Item -ItemType Directory -Path $script:nugetDir   | Out-Null }
    if (-not (Test-Path $script:packagesDir)) { New-Item -ItemType Directory -Path $script:packagesDir | Out-Null }
    if (-not (Test-Path $script:nugetExe)) {
        Write-Host 'Downloading nuget.exe...' -ForegroundColor Yellow
        Invoke-WebRequest -UseBasicParsing https://dist.nuget.org/win-x86-commandline/latest/nuget.exe -OutFile $script:nugetExe
    }
}

function Get-SolutionCategory {
    param([string]$SolutionFullPath)
    $rel = $SolutionFullPath.Substring($script:samplesRoot.Length).TrimStart('\', '/')
    foreach ($prefix in $script:ExperimentalPrefixes) {
        if ($rel.StartsWith($prefix, [System.StringComparison]::OrdinalIgnoreCase)) {
            return 'experimental'
        }
    }
    return 'stable'
}

function Get-Solutions {
    param([string]$SampleFilter)
    if (-not [string]::IsNullOrWhiteSpace($SampleFilter)) {
        $dir = Join-Path $script:samplesRoot $SampleFilter
        if (-not (Test-Path $dir)) { Write-Error "Sample not found: $dir"; exit 1 }
        return Get-ChildItem -Path $dir -Filter *.sln -Recurse | Sort-Object FullName
    }
    return Get-ChildItem -Path $script:samplesRoot -Filter *.sln -Recurse | Sort-Object FullName
}

function Invoke-NuGetRestore {
    param([string]$SolutionPath)
    $hasPackagesConfig = @(Get-ChildItem -Path (Split-Path $SolutionPath -Parent) -Filter packages.config -Recurse -ErrorAction SilentlyContinue).Count -gt 0
    if (-not $hasPackagesConfig) { return $true }

    Write-Host "  NuGet restore (packages.config)..." -ForegroundColor DarkCyan
    & $script:nugetExe restore $SolutionPath `
        -ConfigFile (Join-Path $script:samplesRoot 'nuget.config') `
        -PackagesDirectory $script:packagesDir | Out-Null
    return ($LASTEXITCODE -eq 0)
}

function Invoke-MSBuild {
    param([string]$SolutionPath, [string]$ResolvedPlatform, [string]$ResolvedConfig, [string]$BinLogPath)

    $args = @(
        '/m',
        '/restore',
        "/p:Platform=$ResolvedPlatform",
        "/p:Configuration=$ResolvedConfig",
        "/p:NugetPackageDirectory=$script:packagesDir",
        "/bl:`"$BinLogPath`""
    )
    if (-not [string]::IsNullOrWhiteSpace($ExtraProperties)) {
        # Split on spaces that are not inside quotes, then append each token
        $ExtraProperties.Trim() -split '\s+(?=/)' | Where-Object { $_ } | ForEach-Object { $args += $_ }
    }
    $args += "`"$SolutionPath`""

    $output = & msbuild @args 2>&1
    $exitCode = $LASTEXITCODE

    # Extract warning/error counts from MSBuild summary line
    $warnings = 0; $errors = 0
    $output | Select-String '(\d+) Warning\(s\)'  | ForEach-Object { $warnings = [int]$_.Matches[0].Groups[1].Value }
    $output | Select-String '(\d+) Error\(s\)'    | ForEach-Object { $errors   = [int]$_.Matches[0].Groups[1].Value }

    return [PSCustomObject]@{
        ExitCode = $exitCode
        Warnings = $warnings
        Errors   = $errors
        Output   = $output
    }
}

# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

Initialize-VSEnvironment
if (-not (Get-Command msbuild -ErrorAction SilentlyContinue)) {
    Write-Error 'msbuild not found. Run from a VS Developer PowerShell or ensure MSBuild is on PATH.'
    exit 1
}

$resolvedPlatform = Resolve-Platform -Value $Platform
$repoRoot         = Split-Path -Parent $MyInvocation.MyCommand.Path
$script:nugetDir    = Join-Path $repoRoot '.nuget'
$script:nugetExe    = Join-Path $script:nugetDir 'nuget.exe'
$script:packagesDir = Join-Path $repoRoot 'packages'
$script:samplesRoot = Join-Path $repoRoot 'Samples'

Initialize-NuGet

$allSolutions = Get-Solutions -SampleFilter $Sample
if (-not $allSolutions -or $allSolutions.Count -eq 0) {
    Write-Warning 'No solutions found.'
    exit 0
}

# Apply Mode filter
$solutions = $allSolutions | Where-Object {
    $cat = Get-SolutionCategory -SolutionFullPath $_.FullName
    switch ($Mode) {
        'stable-only'       { $cat -eq 'stable' }
        'experimental-only' { $cat -eq 'experimental' }
        default             { $true }
    }
}

$total = $solutions.Count
Write-Host ''
Write-Host "=== build-report.ps1 ===" -ForegroundColor Cyan
Write-Host "Mode=$Mode  Platform=$resolvedPlatform  Configuration=$Configuration" -ForegroundColor Cyan
if ($ExtraProperties) { Write-Host "ExtraProperties=$ExtraProperties" -ForegroundColor Cyan }
Write-Host "Solutions to build: $total" -ForegroundColor Cyan
Write-Host ''

$results  = [System.Collections.Generic.List[PSCustomObject]]::new()
$scriptSw = [System.Diagnostics.Stopwatch]::StartNew()
$idx      = 0

foreach ($sln in $solutions) {
    $idx++
    $relPath  = $sln.FullName.Substring($script:samplesRoot.Length).TrimStart('\', '/')
    $category = Get-SolutionCategory -SolutionFullPath $sln.FullName
    $binlog   = Join-Path (Split-Path $sln.FullName -Parent) ([IO.Path]::GetFileNameWithoutExtension($sln.Name) + '.binlog')
    $logFile  = $binlog -replace '\.binlog$', '.log'

    Write-Host "[$idx/$total] $relPath  ($category)" -ForegroundColor $(if ($category -eq 'experimental') { 'Yellow' } else { 'White' })

    $sw = [System.Diagnostics.Stopwatch]::StartNew()

    # NuGet restore (packages.config only)
    $restoreOk = Invoke-NuGetRestore -SolutionPath $sln.FullName
    if (-not $restoreOk) {
        $sw.Stop()
        Write-Host "  FAIL (NuGet restore)" -ForegroundColor Red
        $results.Add([PSCustomObject]@{
            Solution  = $relPath
            Category  = $category
            Status    = 'FAIL'
            Stage     = 'nuget-restore'
            ExitCode  = 1
            Warnings  = 0
            Errors    = 0
            Duration  = $sw.Elapsed.TotalSeconds
            BinLog    = $binlog
        })
        continue
    }

    # MSBuild
    $build = Invoke-MSBuild -SolutionPath $sln.FullName -ResolvedPlatform $resolvedPlatform -ResolvedConfig $Configuration -BinLogPath $binlog

    # Write text log
    $build.Output | Out-File -FilePath $logFile -Encoding utf8

    $sw.Stop()
    $status = if ($build.ExitCode -eq 0) { 'PASS' } else { 'FAIL' }
    $color  = if ($status -eq 'PASS') { 'Green' } else { 'Red' }
    Write-Host ("  {0}  ({1:F1}s)  warnings={2}  errors={3}" -f $status, $sw.Elapsed.TotalSeconds, $build.Warnings, $build.Errors) -ForegroundColor $color

    $results.Add([PSCustomObject]@{
        Solution  = $relPath
        Category  = $category
        Status    = $status
        Stage     = 'msbuild'
        ExitCode  = $build.ExitCode
        Warnings  = $build.Warnings
        Errors    = $build.Errors
        Duration  = $sw.Elapsed.TotalSeconds
        BinLog    = $binlog
    })
}

$scriptSw.Stop()

# ---------------------------------------------------------------------------
# Console summary
# ---------------------------------------------------------------------------
$passed      = @($results | Where-Object Status -eq 'PASS')
$failed      = @($results | Where-Object Status -eq 'FAIL')
$stablePassed   = @($passed | Where-Object Category -eq 'stable').Count
$stableFailed   = @($failed | Where-Object Category -eq 'stable').Count
$expPassed      = @($passed | Where-Object Category -eq 'experimental').Count
$expFailed      = @($failed | Where-Object Category -eq 'experimental').Count
$totalWarnings  = ($results | Measure-Object Warnings -Sum).Sum
$totalDuration  = [System.String]::Format('{0:hh\:mm\:ss}', $scriptSw.Elapsed)

Write-Host ''
Write-Host '======================================================' -ForegroundColor Cyan
Write-Host ' BUILD REPORT SUMMARY' -ForegroundColor Cyan
Write-Host '======================================================' -ForegroundColor Cyan
Write-Host ("  Total:        {0,3} solutions   ({1} elapsed)" -f $total, $totalDuration)
Write-Host ("  Passed:       {0,3}" -f $passed.Count) -ForegroundColor Green
Write-Host ("  Failed:       {0,3}" -f $failed.Count) -ForegroundColor $(if ($failed.Count -gt 0) { 'Red' } else { 'Green' })
Write-Host ("  Warnings:     {0,3}" -f $totalWarnings)
Write-Host ''
Write-Host ("  Stable:       PASS={0}  FAIL={1}" -f $stablePassed, $stableFailed)
Write-Host ("  Experimental: PASS={0}  FAIL={1}" -f $expPassed, $expFailed)

if ($failed.Count -gt 0) {
    Write-Host ''
    Write-Host '  Failed solutions:' -ForegroundColor Red
    foreach ($r in $failed) {
        Write-Host ("    [{0}] {1}" -f $r.Category, $r.Solution) -ForegroundColor Red
        Write-Host ("           binlog: {0}" -f $r.BinLog) -ForegroundColor DarkRed
    }
}
Write-Host '======================================================' -ForegroundColor Cyan

# ---------------------------------------------------------------------------
# JSON report
# ---------------------------------------------------------------------------
if ([string]::IsNullOrWhiteSpace($ReportPath)) {
    $timestamp  = Get-Date -Format 'yyyyMMdd-HHmmss'
    $ReportPath = Join-Path $repoRoot "build-report-$timestamp.json"
}

$reportData = [PSCustomObject]@{
    GeneratedAt    = (Get-Date -Format 'o')
    Mode           = $Mode
    Platform       = $resolvedPlatform
    Configuration  = $Configuration
    ExtraProperties = $ExtraProperties
    TotalSolutions = $total
    Passed         = $passed.Count
    Failed         = $failed.Count
    TotalWarnings  = $totalWarnings
    ElapsedSeconds = $scriptSw.Elapsed.TotalSeconds
    Results        = $results
}
$reportData | ConvertTo-Json -Depth 5 | Out-File -FilePath $ReportPath -Encoding utf8
Write-Host ''
Write-Host "JSON report: $ReportPath" -ForegroundColor DarkGray
