# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

<#
.SYNOPSIS
    Validates the PhotoEditor sample build output for correctness.

.DESCRIPTION
    Checks that the PhotoEditor sample (both C# and C++ variants) built
    successfully by verifying expected binaries, resources, and assets
    exist in the build output directory.

.PARAMETER OutputPath
    Path to the PhotoEditor sample build output directory.

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

function Check-BuildOutput
{
    param(
        [string]$Variant,
        [string]$VariantPath
    )

    if (-not (Test-Path $VariantPath))
    {
        Write-Warning "[$Variant] Build output directory not found: $VariantPath"
        return $false
    }

    $ok = $true

    # Find the executable in the build output
    $exeFiles = Get-ChildItem -Path $VariantPath -Filter "PhotoEditor.exe" -Recurse -ErrorAction SilentlyContinue
    if (-not $exeFiles)
    {
        Write-Warning "[$Variant] PhotoEditor.exe not found in build output"
        $ok = $false
    }
    else
    {
        Write-Host "[$Variant] Found PhotoEditor.exe: $($exeFiles[0].FullName)"

        # Verify the binary is non-trivial (not a 0-byte stub)
        foreach ($exe in $exeFiles)
        {
            if ($exe.Length -lt 1024)
            {
                Write-Warning "[$Variant] PhotoEditor.exe is suspiciously small: $($exe.Length) bytes"
                $ok = $false
            }
        }

        # Verify platform targeting if helpers are available
        if (Get-Command Test-BinaryPlatform -ErrorAction SilentlyContinue)
        {
            $platformArg = $Platform
            if ($platformArg -eq 'arm64ec') { $platformArg = 'arm64' }
            $result = Test-BinaryPlatform -FilePath $exeFiles[0].FullName -ExpectedPlatform $platformArg
            if ($result)
            {
                Write-Host "[$Variant] Platform check passed ($Platform)"
            }
            else
            {
                Write-Warning "[$Variant] Platform check failed"
                $ok = $false
            }
        }

        # Verify PDB exists for the executable
        if (Get-Command Test-SymbolFiles -ErrorAction SilentlyContinue)
        {
            $result = Test-SymbolFiles -BinaryPath $exeFiles[0].FullName
            if ($result)
            {
                Write-Host "[$Variant] PDB symbol file found"
            }
            else
            {
                Write-Warning "[$Variant] PDB symbol file missing"
                $ok = $false
            }
        }
    }

    # Check for resources.pri (compiled resources)
    $priFiles = Get-ChildItem -Path $VariantPath -Filter "resources.pri" -Recurse -ErrorAction SilentlyContinue
    if (-not $priFiles)
    {
        # C++ variant uses PhotoEditor.pri
        $priFiles = Get-ChildItem -Path $VariantPath -Filter "PhotoEditor.pri" -Recurse -ErrorAction SilentlyContinue
    }
    if (-not $priFiles)
    {
        Write-Warning "[$Variant] No .pri resource file found in build output"
        $ok = $false
    }
    else
    {
        Write-Host "[$Variant] Found resource file: $($priFiles[0].Name)"
    }

    # Check for AppxManifest.xml in build output (indicates packaging succeeded)
    $manifests = Get-ChildItem -Path $VariantPath -Filter "AppxManifest.xml" -Recurse -ErrorAction SilentlyContinue
    if ($manifests)
    {
        Write-Host "[$Variant] Found AppxManifest.xml in build output"

        # Validate manifest has correct dependency on WindowsAppRuntime
        if (Get-Command Test-AppxManifest -ErrorAction SilentlyContinue)
        {
            $result = Test-AppxManifest -ManifestPath $manifests[0].FullName -ExpectedDependency "Microsoft.WindowsAppRuntime*"
            if ($result)
            {
                Write-Host "[$Variant] AppxManifest dependency check passed"
            }
            else
            {
                Write-Warning "[$Variant] AppxManifest missing WindowsAppRuntime dependency"
                $ok = $false
            }
        }

        # Check for appxsym symbol packages (packaged apps)
        if (Get-Command Test-AppxSymbols -ErrorAction SilentlyContinue)
        {
            $result = Test-AppxSymbols -Path $VariantPath
            if ($result)
            {
                Write-Host "[$Variant] AppxSymbols found"
            }
            else
            {
                Write-Host "[$Variant] Note: No .appxsym files found (may not be generated in this config)"
            }
        }
    }

    # Check for key asset files in the output
    $assetPatterns = @(
        "Square44x44Logo*",
        "Square150x150Logo*",
        "SplashScreen*"
    )
    foreach ($pattern in $assetPatterns)
    {
        $assets = Get-ChildItem -Path $VariantPath -Filter $pattern -Recurse -ErrorAction SilentlyContinue
        if (-not $assets)
        {
            Write-Warning "[$Variant] Asset not found in output: $pattern"
            $ok = $false
        }
    }

    # Check for WinAppSDK runtime DLLs in the output
    $runtimeDlls = Get-ChildItem -Path $VariantPath -Filter "Microsoft.WindowsAppRuntime*.dll" -Recurse -ErrorAction SilentlyContinue
    if (-not $runtimeDlls)
    {
        Write-Host "[$Variant] Note: WindowsAppRuntime DLLs not found in build output (may be framework-dependent)"
    }
    else
    {
        Write-Host "[$Variant] Found $($runtimeDlls.Count) WindowsAppRuntime DLL(s)"
    }

    if ($ok)
    {
        Write-Host "[$Variant] All checks passed" -ForegroundColor Green
    }
    else
    {
        Write-Host "[$Variant] Some checks failed" -ForegroundColor Red
    }

    return $ok
}

# Validate C# variant
$csPath = Join-Path $OutputPath "cs-winui"
if (Test-Path $csPath)
{
    Write-Host "=== Validating C# WinUI variant ===" -ForegroundColor Cyan
    $csResult = Check-BuildOutput -Variant "cs-winui" -VariantPath $csPath
    if (-not $csResult) { $failed = $true }
}
else
{
    Write-Host "C# variant not found at $csPath - skipping" -ForegroundColor Yellow
}

Write-Host ""

# Validate C++ variant
$cppPath = Join-Path $OutputPath "cpp-winui"
if (Test-Path $cppPath)
{
    Write-Host "=== Validating C++ WinUI variant ===" -ForegroundColor Cyan
    $cppResult = Check-BuildOutput -Variant "cpp-winui" -VariantPath $cppPath
    if (-not $cppResult) { $failed = $true }
}
else
{
    Write-Host "C++ variant not found at $cppPath - skipping" -ForegroundColor Yellow
}

# At least one variant must exist and pass
if (-not (Test-Path $csPath) -and -not (Test-Path $cppPath))
{
    Write-Error "No PhotoEditor variants found in $OutputPath"
    exit 1
}

if ($failed)
{
    Write-Host "`nPhotoEditor validation FAILED" -ForegroundColor Red
    exit 1
}

Write-Host "`nPhotoEditor validation PASSED" -ForegroundColor Green
# Clear $Error to prevent PowerShell from overriding exit code 0.
# Under $ErrorActionPreference='Stop', suppressed errors from Get-ChildItem
# -ErrorAction SilentlyContinue still accumulate in $Error.
$global:Error.Clear()
exit 0
