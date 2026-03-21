# Copyright (C) Microsoft Corporation. All rights reserved.
<#
.SYNOPSIS
    Builds the WinML EP Catalog Sample using CMake.

.DESCRIPTION
    This script automates the build process for the WinMLEpCatalog sample.
    It handles:
    - Checking prerequisites (CMake, NuGet, Visual Studio)
    - Setting up Visual Studio developer environment
    - Configuring (with automatic NuGet restore) and building the project

.PARAMETER Configuration
    Build configuration: Debug, Release, RelWithDebInfo, MinSizeRel. Default: RelWithDebInfo

.PARAMETER Generator
    Build generator: Ninja or VisualStudio. Default: Ninja

.PARAMETER Clean
    If specified, reconfigures with `cmake --fresh` before building.

.EXAMPLE
    .\build.ps1
    # Builds RelWithDebInfo for the current platform

.EXAMPLE
    .\build.ps1 -Configuration Release -Platform arm64
    # Builds Release for ARM64

.EXAMPLE
    .\build.ps1 -Generator VisualStudio -Configuration Debug
    # Builds Debug using the Visual Studio generator

.EXAMPLE
    .\build.ps1 -Clean -Configuration Debug
    # Cleans and rebuilds Debug configuration
#>

[CmdletBinding()]
param(
    [ValidateSet('Debug', 'Release', 'RelWithDebInfo', 'MinSizeRel')]
    [string]$Configuration = 'RelWithDebInfo',
    
    [ValidateSet('x64', 'arm64')]
    [string]$Platform,

    [ValidateSet('Ninja', 'VisualStudio')]
    [string]$Generator = 'Ninja',
    
    [switch]$Clean
)

Set-StrictMode -Version 2.0
$ErrorActionPreference = 'Stop'

# ============================================================================
# Helper Functions
# ============================================================================

function Test-CommandExists {
    param([string]$Command)
    $null -ne (Get-Command $Command -ErrorAction SilentlyContinue)
}

function Get-HostArchitecture {
    $arch = [System.Runtime.InteropServices.RuntimeInformation]::OSArchitecture
    switch ($arch) {
        'X64' { return 'x64' }
        'Arm64' { return 'arm64' }
        default { return 'x64' }
    }
}

function Write-Header {
    param([string]$Message)
    Write-Host ""
    Write-Host "========================================" -ForegroundColor Cyan
    Write-Host "  $Message" -ForegroundColor Cyan
    Write-Host "========================================" -ForegroundColor Cyan
}

function Write-Step {
    param([string]$Message)
    Write-Host ""
    Write-Host "[*] $Message" -ForegroundColor Yellow
}

function Write-Success {
    param([string]$Message)
    Write-Host "[OK] $Message" -ForegroundColor Green
}

function Write-ErrorMessage {
    param([string]$Message)
    Write-Error $Message
}

function Enter-VsDevEnvironment {
    param([string]$Arch)
    
    # Find Visual Studio installation
    $vswherePath = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
    if (-not (Test-Path $vswherePath)) {
        $vswherePath = "${env:ProgramFiles}\Microsoft Visual Studio\Installer\vswhere.exe"
    }
    
    if (-not (Test-Path $vswherePath)) {
        Write-ErrorMessage "vswhere not found. Please install Visual Studio 2022 or later."
        return $false
    }
    
    $vsPath = & $vswherePath -latest -property installationPath
    if (-not $vsPath) {
        Write-ErrorMessage "Visual Studio installation not found."
        return $false
    }
    
    $devShellModule = Join-Path $vsPath "Common7\Tools\Microsoft.VisualStudio.DevShell.dll"
    if (-not (Test-Path $devShellModule)) {
        Write-ErrorMessage "VS DevShell module not found at: $devShellModule"
        return $false
    }
    
    Write-Host "  Visual Studio: $vsPath"
    
    # Import the DevShell module and enter the environment
    Import-Module $devShellModule
    Enter-VsDevShell -VsInstallPath $vsPath -DevCmdArguments "-arch=$Arch" -SkipAutomaticLocation
    
    return $true
}

# ============================================================================
# Main Script
# ============================================================================

Write-Header "WinML EP Catalog Sample Build"

# Auto-detect platform if not specified
if (-not $Platform) {
    $Platform = Get-HostArchitecture
    Write-Host "Auto-detected platform: $Platform"
}

# Determine preset name
$PresetName = "nuget"
Write-Host "Build preset: $PresetName"
Write-Host "Generator: $Generator"

# ============================================================================
# Set up Visual Studio Developer Environment
# ============================================================================
# Enter the dev environment first so that cmake, ninja, and other tools
# bundled with Visual Studio are available on PATH for prerequisite checks.

Write-Step "Setting up Visual Studio Developer environment..."

if (-not (Enter-VsDevEnvironment -Arch $Platform)) {
    Write-ErrorMessage "Failed to set up Visual Studio environment"
    exit 1
}

Write-Success "VS Developer environment configured for $Platform"

# ============================================================================
# Check Prerequisites
# ============================================================================

Write-Step "Checking prerequisites..."

# Check CMake
if (-not (Test-CommandExists 'cmake')) {
    Write-ErrorMessage "CMake not found. Please install CMake 3.21 or later."
    Write-Host "  Install with: winget install Kitware.CMake" -ForegroundColor Gray
    exit 1
}

$cmakeVersion = (cmake --version | Select-Object -First 1)
Write-Host "  CMake: $cmakeVersion"

# Check Ninja (only required for Ninja generator)
if ($Generator -eq 'Ninja') {
    if (Test-CommandExists 'ninja') {
        $ninjaVersion = (ninja --version)
        Write-Host "  Ninja: $ninjaVersion"
    }
    else {
        Write-ErrorMessage "Ninja not found. Install Ninja or use -Generator VisualStudio."
        exit 1
    }
}

# Check NuGet (needed by CMake for automatic package restore)
if (-not (Test-CommandExists 'nuget')) {
    Write-ErrorMessage "NuGet not found. CMake uses nuget.exe to restore the ML package."
    Write-Host "  Install with: winget install Microsoft.NuGet" -ForegroundColor Gray
    exit 1
}
Write-Host "  NuGet: $(nuget help | Select-Object -First 1)"

Write-Success "Prerequisites check passed"

# ============================================================================
# Clean (if requested)
# ============================================================================

if ($Clean) {
    Write-Step "Clean requested: configure will run with --fresh"
}

# ============================================================================
# Configure
# ============================================================================

Write-Step "Configuring with CMake..."

$buildDir = Join-Path $PSScriptRoot "out/build/$PresetName"
$configureArgs = @(
    '-S', $PSScriptRoot,
    '-B', $buildDir
)

if ($Clean) {
    $configureArgs += '--fresh'
}

if ($Generator -eq 'VisualStudio') {
    # Omit -G to let CMake auto-detect the installed Visual Studio version.
    $configureArgs += @('-A', $Platform)
}
else {
    $configureArgs += @('-G', 'Ninja', "-DCMAKE_BUILD_TYPE=$Configuration")
}

& cmake @configureArgs
if ($LASTEXITCODE -ne 0) {
    Write-ErrorMessage "CMake configuration failed"
    exit 1
}
Write-Success "Configuration complete"

# ============================================================================
# Build
# ============================================================================

Write-Step "Building..."

& cmake --build $buildDir --config $Configuration
if ($LASTEXITCODE -ne 0) {
    Write-ErrorMessage "Build failed"
    exit 1
}
Write-Success "Build complete"

# ============================================================================
# Output Information
# ============================================================================

Write-Header "Build Successful"

$exePath = if ($Generator -eq 'VisualStudio') {
    Join-Path $PSScriptRoot "out\build\$PresetName\$Configuration\WinMLEpCatalogSample.exe"
} else {
    Join-Path $PSScriptRoot "out\build\$PresetName\WinMLEpCatalogSample.exe"
}
if (Test-Path $exePath) {
    Write-Host ""
    Write-Host "Output: $exePath" -ForegroundColor Green
    Write-Host ""
    Write-Host "To run the sample:" -ForegroundColor Cyan
    Write-Host "  $exePath"
    Write-Host ""
}

exit 0
