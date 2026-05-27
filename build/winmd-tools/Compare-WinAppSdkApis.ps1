<#
.SYNOPSIS
    Compare APIs between stable and experimental WinAppSDK packages to find exp-only APIs.

.DESCRIPTION
    Downloads specified stable and experimental WinAppSDK NuGet packages via dotnet restore,
    parses WinMD files using a C# tool, and computes the difference to identify 
    experimental-only APIs.

.PARAMETER StableVersion
    The stable WinAppSDK version (e.g., "2.0.1")

.PARAMETER ExperimentalVersion
    The experimental WinAppSDK version (e.g., "2.0.0-Experimental7")

.PARAMETER OutputDir
    Directory for output files. Defaults to "build\winmd-tools\output"

.EXAMPLE
    .\Compare-WinAppSdkApis.ps1 -StableVersion "2.0.1" -ExperimentalVersion "2.0.0-Experimental7"
#>
[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$StableVersion,

    [Parameter(Mandatory = $true)]
    [string]$ExperimentalVersion,

    [string]$OutputDir = (Join-Path $PSScriptRoot 'output')
)

$ErrorActionPreference = 'Stop'

# Paths
$tempDir = Join-Path $OutputDir 'temp'
$apiComparerDir = Join-Path $PSScriptRoot 'api-comparer'

# Create directories
New-Item -ItemType Directory -Path $OutputDir -Force | Out-Null
New-Item -ItemType Directory -Path $tempDir -Force | Out-Null

Write-Host "=== WinAppSDK API Comparison Tool ===" -ForegroundColor Cyan
Write-Host "Stable:       $StableVersion"
Write-Host "Experimental: $ExperimentalVersion"
Write-Host "Output:       $OutputDir"
Write-Host ""

# ---------------------------------------------------------------------------
# Step 1: Download NuGet packages via dotnet restore
# ---------------------------------------------------------------------------

Write-Host "Step 1: Downloading NuGet packages..." -ForegroundColor Cyan

# NuGet global packages folder
$globalPackages = Join-Path $env:USERPROFILE ".nuget\packages"
if ($env:NUGET_PACKAGES) {
    $globalPackages = $env:NUGET_PACKAGES
}

# Function to ensure package is in NuGet cache
function Ensure-PackageInCache {
    param([string]$Version)
    
    $pkgDir = Join-Path $globalPackages "microsoft.windowsappsdk" $Version.ToLower()
    if (Test-Path $pkgDir) {
        Write-Host "  [cached] Microsoft.WindowsAppSDK@$Version" -ForegroundColor Gray
        return $pkgDir
    }
    
    Write-Host "  [download] Microsoft.WindowsAppSDK@$Version" -ForegroundColor Yellow
    
    # Create a temp project to download the package
    $tempProj = Join-Path $tempDir "download-$Version"
    New-Item -ItemType Directory -Path $tempProj -Force | Out-Null
    
    $csprojContent = @"
<Project Sdk="Microsoft.NET.Sdk">
  <PropertyGroup>
    <TargetFramework>net8.0-windows10.0.19041.0</TargetFramework>
    <Platforms>x64</Platforms>
  </PropertyGroup>
  <ItemGroup>
    <PackageReference Include="Microsoft.WindowsAppSDK" Version="$Version" ExcludeAssets="all" />
  </ItemGroup>
</Project>
"@
    $csprojPath = Join-Path $tempProj "download.csproj"
    $csprojContent | Set-Content $csprojPath -Encoding UTF8
    
    Push-Location $tempProj
    try {
        dotnet restore --nologo -v q 2>&1 | Out-Null
        if ($LASTEXITCODE -ne 0) {
            Write-Error "Failed to restore Microsoft.WindowsAppSDK@$Version"
            return $null
        }
    }
    finally {
        Pop-Location
    }
    
    # Clean up temp project
    Remove-Item $tempProj -Recurse -Force -ErrorAction SilentlyContinue
    
    # Verify package is now in cache
    if (Test-Path $pkgDir) {
        return $pkgDir
    }
    
    Write-Error "Package not found in cache after restore: $pkgDir"
    return $null
}

$stablePkgDir = Ensure-PackageInCache -Version $StableVersion
$expPkgDir = Ensure-PackageInCache -Version $ExperimentalVersion

if (-not $stablePkgDir -or -not $expPkgDir) {
    Write-Error "Failed to download packages"
    exit 1
}

# ---------------------------------------------------------------------------
# Step 2: Collect all WinMD files from packages and dependencies
# ---------------------------------------------------------------------------

Write-Host ""
Write-Host "Step 2: Collecting WinMD files..." -ForegroundColor Cyan

function Get-AllWinMdDirs {
    param([string]$PkgDir, [string]$Label)
    
    $winmdDirs = @()
    
    # Read nuspec for dependencies
    $nuspecPath = Get-ChildItem $PkgDir -Filter "*.nuspec" | Select-Object -First 1 -ExpandProperty FullName
    if (-not $nuspecPath) {
        Write-Warning "No nuspec found in $PkgDir"
        return @()
    }
    
    [xml]$nuspec = Get-Content $nuspecPath
    $deps = @()
    
    # Handle both flat dependencies and grouped dependencies
    if ($nuspec.package.metadata.dependencies.dependency) {
        $deps = $nuspec.package.metadata.dependencies.dependency
    }
    elseif ($nuspec.package.metadata.dependencies.group) {
        foreach ($group in $nuspec.package.metadata.dependencies.group) {
            $deps += $group.dependency
        }
    }
    
    Write-Host "  $Label dependencies: $($deps.Count)"
    
    foreach ($dep in $deps) {
        $depId = $dep.id
        $depVersion = $dep.version
        
        # Strip version range brackets: [1.2.3] -> 1.2.3, [1.2.3, ) -> 1.2.3
        if ($depVersion -match '^\[([^\],]+)') {
            $depVersion = $Matches[1]
        }
        
        $depDir = Join-Path $globalPackages $depId.ToLower() $depVersion.ToLower()
        if (-not (Test-Path $depDir)) {
            # Try exact match without version normalization
            $parentDir = Join-Path $globalPackages $depId.ToLower()
            if (Test-Path $parentDir) {
                $depDir = Get-ChildItem $parentDir -Directory | Where-Object { 
                    $_.Name -eq $depVersion.ToLower() -or $_.Name -eq $depVersion 
                } | Select-Object -First 1 -ExpandProperty FullName
            }
        }
        
        if ($depDir -and (Test-Path $depDir)) {
            $depWinMd = Get-ChildItem $depDir -Filter "*.winmd" -Recurse -ErrorAction SilentlyContinue
            if ($depWinMd) {
                Write-Host "    $depId@$depVersion : $($depWinMd.Count) winmd" -ForegroundColor Gray
                $winmdDirs += $depDir
            }
        }
    }
    
    # Also check main package
    $mainWinMd = Get-ChildItem $PkgDir -Filter "*.winmd" -Recurse -ErrorAction SilentlyContinue
    if ($mainWinMd) {
        $winmdDirs += $PkgDir
    }
    
    return $winmdDirs
}

$stableDirs = Get-AllWinMdDirs -PkgDir $stablePkgDir -Label "Stable"
$expDirs = Get-AllWinMdDirs -PkgDir $expPkgDir -Label "Experimental"

# Create temp directories with all WinMD files
$stableWinMdDir = Join-Path $tempDir "stable-winmd"
$expWinMdDir = Join-Path $tempDir "exp-winmd"

New-Item -ItemType Directory -Path $stableWinMdDir -Force | Out-Null
New-Item -ItemType Directory -Path $expWinMdDir -Force | Out-Null

foreach ($dir in $stableDirs) {
    Get-ChildItem $dir -Filter "*.winmd" -Recurse -ErrorAction SilentlyContinue | ForEach-Object {
        $dest = Join-Path $stableWinMdDir $_.Name
        if (-not (Test-Path $dest)) {
            Copy-Item $_.FullName $dest
        }
    }
}

foreach ($dir in $expDirs) {
    Get-ChildItem $dir -Filter "*.winmd" -Recurse -ErrorAction SilentlyContinue | ForEach-Object {
        $dest = Join-Path $expWinMdDir $_.Name
        if (-not (Test-Path $dest)) {
            Copy-Item $_.FullName $dest
        }
    }
}

$stableCount = (Get-ChildItem $stableWinMdDir -Filter "*.winmd").Count
$expCount = (Get-ChildItem $expWinMdDir -Filter "*.winmd").Count
Write-Host ""
Write-Host "  Total Stable WinMD: $stableCount"
Write-Host "  Total Experimental WinMD: $expCount"

# ---------------------------------------------------------------------------
# Step 3: Build and run API comparer
# ---------------------------------------------------------------------------

Write-Host ""
Write-Host "Step 3: Building API comparer..." -ForegroundColor Cyan

Push-Location $apiComparerDir
try {
    dotnet build -c Release --nologo -v q
    if ($LASTEXITCODE -ne 0) {
        Write-Error "Failed to build API comparer"
        exit 1
    }
}
finally {
    Pop-Location
}

Write-Host ""
Write-Host "Step 4: Running comparison..." -ForegroundColor Cyan

$comparerExe = Join-Path $apiComparerDir "bin\Release\net8.0\ApiComparer.exe"
if (-not (Test-Path $comparerExe)) {
    # Try dll
    $comparerExe = Join-Path $apiComparerDir "bin\Release\net8.0\ApiComparer.dll"
    if (Test-Path $comparerExe) {
        & dotnet $comparerExe $stableWinMdDir $expWinMdDir $OutputDir $StableVersion $ExperimentalVersion
    }
    else {
        Write-Error "ApiComparer not found at $comparerExe"
        exit 1
    }
}
else {
    & $comparerExe $stableWinMdDir $expWinMdDir $OutputDir $StableVersion $ExperimentalVersion
}

if ($LASTEXITCODE -ne 0) {
    Write-Error "API comparison failed"
    exit 1
}

# ---------------------------------------------------------------------------
# Cleanup temp files
# ---------------------------------------------------------------------------

# Remove-Item $tempDir -Recurse -Force -ErrorAction SilentlyContinue

Write-Host ""
Write-Host "=== Complete ===" -ForegroundColor Cyan
