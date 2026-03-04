# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

<#
.SYNOPSIS
    Validates the SelfContainedDeployment sample build output.

.DESCRIPTION
    Checks that the SelfContainedDeployment sample variants built successfully
    by verifying expected executables exist in the build output. For self-contained
    apps, also verifies that WindowsAppRuntime DLLs are bundled in the output.

.PARAMETER OutputPath
    Path to the SelfContainedDeployment sample build output directory.

.PARAMETER Platform
    Target platform (x64, arm64).

.PARAMETER Configuration
    Build configuration (Debug, Release).

.PARAMETER HelpersModulePath
    Optional path to the SampleValidation.Common.psm1 shared helpers module.
#>
param(
    [Parameter(Mandatory)]
    [string]$OutputPath,

    [Parameter(Mandatory)]
    [string]$Platform,

    [Parameter(Mandatory)]
    [string]$Configuration,

    [string]$HelpersModulePath
)

Set-StrictMode -Version 3.0
$ErrorActionPreference = 'Stop'

if ($HelpersModulePath -and (Test-Path $HelpersModulePath))
{
    Import-Module $HelpersModulePath
}

$failed = $false
$variantsFound = 0

# Define expected variants. All produce SelfContainedDeployment.exe
$variants = @(
    @{ Path = "cpp-console-unpackaged";  Exe = "SelfContainedDeployment.exe"; SelfContained = $true }
    @{ Path = "cpp-winui-packaged";      Exe = "SelfContainedDeployment.exe"; SelfContained = $true }
    @{ Path = "cpp-winui-unpackaged";    Exe = "SelfContainedDeployment.exe"; SelfContained = $true }
    @{ Path = "cs-console-unpackaged";   Exe = "SelfContainedDeployment.exe"; SelfContained = $true }
    @{ Path = "cs-wpf-unpackaged";       Exe = "SelfContainedDeployment.exe"; SelfContained = $true }
    @{ Path = "cs-winui-packaged";       Exe = "SelfContainedDeployment.exe"; SelfContained = $true }
    @{ Path = "cs-winui-unpackaged";     Exe = "SelfContainedDeployment.exe"; SelfContained = $true }
    @{ Path = "cs-winui-packaged-wap";   Exe = "SelfContainedDeployment.exe"; SelfContained = $true }
)

foreach ($variant in $variants)
{
    $variantPath = Join-Path $OutputPath $variant.Path
    $variantName = $variant.Path
    $exeName = $variant.Exe

    if (-not (Test-Path $variantPath))
    {
        continue
    }

    $variantsFound++
    Write-Host "=== Validating $variantName ===" -ForegroundColor Cyan

    $ok = $true

    # Find the executable
    $exeFiles = Get-ChildItem -Path $variantPath -Filter $exeName -Recurse -ErrorAction SilentlyContinue
    if (-not $exeFiles)
    {
        Write-Warning "[$variantName] $exeName not found in build output"
        $ok = $false
    }
    else
    {
        Write-Host "[$variantName] Found $exeName"

        # Verify non-trivial size
        if ($exeFiles[0].Length -lt 1024)
        {
            Write-Warning "[$variantName] $exeName is suspiciously small: $($exeFiles[0].Length) bytes"
            $ok = $false
        }

        # Platform check
        if (Get-Command Test-BinaryPlatform -ErrorAction SilentlyContinue)
        {
            $platformArg = $Platform
            if ($platformArg -eq 'arm64ec') { $platformArg = 'arm64' }
            $result = Test-BinaryPlatform -FilePath $exeFiles[0].FullName -ExpectedPlatform $platformArg
            if ($result)
            {
                Write-Host "[$variantName] Platform check passed ($Platform)"
            }
            else
            {
                Write-Warning "[$variantName] Platform check failed"
                $ok = $false
            }
        }

        # PDB symbol check
        if (Get-Command Test-SymbolFiles -ErrorAction SilentlyContinue)
        {
            $result = Test-SymbolFiles -BinaryPath $exeFiles[0].FullName
            if ($result)
            {
                Write-Host "[$variantName] PDB symbol file found"
            }
            else
            {
                Write-Warning "[$variantName] PDB symbol file missing"
                $ok = $false
            }
        }
    }

    # Self-contained apps should bundle WindowsAppRuntime DLLs
    if ($variant.SelfContained)
    {
        $runtimeDlls = Get-ChildItem -Path $variantPath -Filter "Microsoft.WindowsAppRuntime*.dll" -Recurse -ErrorAction SilentlyContinue
        if ($runtimeDlls)
        {
            Write-Host "[$variantName] Found $($runtimeDlls.Count) bundled WindowsAppRuntime DLL(s)"
        }
        else
        {
            Write-Warning "[$variantName] Self-contained app missing bundled WindowsAppRuntime DLLs"
            $ok = $false
        }
    }

    if ($ok)
    {
        Write-Host "[$variantName] All checks passed" -ForegroundColor Green
    }
    else
    {
        Write-Host "[$variantName] Some checks failed" -ForegroundColor Red
        $failed = $true
    }
    Write-Host ""
}

if ($variantsFound -eq 0)
{
    Write-Host "No SelfContainedDeployment variants found in $OutputPath - skipping" -ForegroundColor Yellow
    $global:Error.Clear()
    exit 0
}

Write-Host "Validated $variantsFound variant(s)"

if ($failed)
{
    Write-Host "`nSelfContainedDeployment validation FAILED" -ForegroundColor Red
    exit 1
}

Write-Host "`nSelfContainedDeployment validation PASSED" -ForegroundColor Green
$global:Error.Clear()
exit 0
