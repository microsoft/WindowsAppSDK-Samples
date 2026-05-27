# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

<#
.SYNOPSIS
    Detects which samples are affected by PR changes and outputs build matrix.

.DESCRIPTION
    Analyzes git diff to determine:
    1. Which sample directories were modified
    2. Whether common/shared files were modified (triggers full build)
    3. Generates a JSON matrix for downstream build stages

.PARAMETER TargetBranch
    The target branch to compare against (e.g., main, release/1.0)

.PARAMETER IsStableBuild
    If true, excludes exp-only samples from the output

.PARAMETER ForceFullBuild
    If true, builds all samples regardless of changes

.OUTPUTS
    Sets Azure DevOps pipeline variables:
    - AffectedSamples: JSON array of affected sample paths
    - IsFullBuild: true if all samples should be built
    - SampleCount: number of samples to build
#>

param(
    [Parameter(Mandatory = $false)]
    [string]$TargetBranch = "main",

    [Parameter(Mandatory = $false)]
    [switch]$IsStableBuild,

    [Parameter(Mandatory = $false)]
    [switch]$ForceFullBuild
)

$ErrorActionPreference = "Stop"

# ============================================================
# Configuration
# ============================================================

# Common files that trigger full build when modified
$CommonFiles = @(
    "Samples/Directory.Packages.props",
    "Samples/Directory.Build.props",
    "Samples/nuget.config",
    "build/*"
)

# Exp-only samples (skip in stable build)
$ExpOnlySamples = @(
    "AppContentSearch",
    "WinUI/ConditionalPredicate"
)

# All known sample directories (top-level feature areas)
$AllSampleAreas = @(
    "AppContentSearch",
    "AppLifecycle",
    "BackgroundTask",
    "CameraCaptureUI",
    "Composition",
    "CustomControls",
    "DeploymentManager",
    "DynamicDependenciesSample",
    "Input",
    "Insights",
    "Installer",
    "Islands",
    "Mica",
    "Notifications",
    "OAuth2Manager",
    "PhotoEditor",
    "ResourceManagement",
    "SceneGraph",
    "SelfContainedDeployment",
    "StoragePickers",
    "TextRendering",
    "Unpackaged",
    "Widgets",
    "WinUI",
    "Windowing",
    "WindowsAIFoundry",
    "WindowsML"
)

# ============================================================
# Functions
# ============================================================

function Get-ChangedFiles {
    param([string]$TargetBranch)
    
    Write-Host "Comparing against branch: $TargetBranch"
    
    # Get list of changed files
    $changedFiles = git diff --name-only "origin/$TargetBranch...HEAD" 2>$null
    if ($LASTEXITCODE -ne 0) {
        # Fallback: compare against target branch directly
        $changedFiles = git diff --name-only "origin/$TargetBranch" 2>$null
    }
    
    if ($changedFiles) {
        $files = $changedFiles -split "`n" | Where-Object { $_ -ne "" }
        Write-Host "Found $($files.Count) changed files"
        return $files
    }
    
    return @()
}

function Test-CommonFileChanged {
    param([string[]]$ChangedFiles)
    
    foreach ($file in $ChangedFiles) {
        foreach ($pattern in $CommonFiles) {
            if ($pattern.EndsWith("*")) {
                $prefix = $pattern.TrimEnd('*')
                if ($file.StartsWith($prefix)) {
                    Write-Host "Common file changed: $file (matches $pattern)"
                    return $true
                }
            }
            elseif ($file -eq $pattern) {
                Write-Host "Common file changed: $file"
                return $true
            }
        }
    }
    return $false
}

function Get-AffectedSamples {
    param([string[]]$ChangedFiles)
    
    $affected = @{}
    
    foreach ($file in $ChangedFiles) {
        # Check if file is under Samples/
        if ($file -match "^Samples/([^/]+)") {
            $sampleArea = $Matches[1]
            if ($sampleArea -in $AllSampleAreas) {
                $affected[$sampleArea] = $true
            }
        }
    }
    
    return $affected.Keys | Sort-Object
}

function Filter-StableSamples {
    param([string[]]$Samples)
    
    $filtered = $Samples | Where-Object {
        $sample = $_
        $isExpOnly = $ExpOnlySamples | Where-Object { $sample -like "$_*" }
        if ($isExpOnly) {
            Write-Host "Excluding exp-only sample from stable build: $sample"
            return $false
        }
        return $true
    }
    
    return $filtered
}

# ============================================================
# Main
# ============================================================

Write-Host "=========================================="
Write-Host "Detect Affected Samples"
Write-Host "=========================================="
Write-Host "Target Branch: $TargetBranch"
Write-Host "Is Stable Build: $IsStableBuild"
Write-Host "Force Full Build: $ForceFullBuild"
Write-Host ""

$isFullBuild = $false
$affectedSamples = @()

if ($ForceFullBuild) {
    Write-Host "Force full build requested"
    $isFullBuild = $true
    $affectedSamples = $AllSampleAreas
}
else {
    # Get changed files
    $changedFiles = Get-ChangedFiles -TargetBranch $TargetBranch
    
    if ($changedFiles.Count -eq 0) {
        Write-Host "No changed files detected"
        $affectedSamples = @()
    }
    elseif (Test-CommonFileChanged -ChangedFiles $changedFiles) {
        Write-Host "Common file changed - triggering full build"
        $isFullBuild = $true
        $affectedSamples = $AllSampleAreas
    }
    else {
        $affectedSamples = Get-AffectedSamples -ChangedFiles $changedFiles
        Write-Host "Affected samples: $($affectedSamples -join ', ')"
    }
}

# Filter out exp-only samples for stable build
if ($IsStableBuild -and $affectedSamples.Count -gt 0) {
    $affectedSamples = Filter-StableSamples -Samples $affectedSamples
}

# Output results
Write-Host ""
Write-Host "=========================================="
Write-Host "Results"
Write-Host "=========================================="
Write-Host "Is Full Build: $isFullBuild"
Write-Host "Sample Count: $($affectedSamples.Count)"
Write-Host "Samples: $($affectedSamples -join ', ')"

# Convert to JSON for pipeline
$samplesJson = $affectedSamples | ConvertTo-Json -Compress
if ($affectedSamples.Count -eq 1) {
    # Single item needs to be wrapped in array
    $samplesJson = "[$samplesJson]"
}
elseif ($affectedSamples.Count -eq 0) {
    $samplesJson = "[]"
}

# Set Azure DevOps pipeline variables
if ($env:TF_BUILD) {
    Write-Host "##vso[task.setvariable variable=AffectedSamples;isOutput=true]$samplesJson"
    Write-Host "##vso[task.setvariable variable=IsFullBuild;isOutput=true]$isFullBuild"
    Write-Host "##vso[task.setvariable variable=SampleCount;isOutput=true]$($affectedSamples.Count)"
}

# Return for local testing
return @{
    AffectedSamples = $affectedSamples
    IsFullBuild = $isFullBuild
    SampleCount = $affectedSamples.Count
    SamplesJson = $samplesJson
}
