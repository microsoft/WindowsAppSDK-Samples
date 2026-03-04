# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

<#
.SYNOPSIS
    Validates the MultiHeadedPackage sample build output.

.DESCRIPTION
    Checks that both multi-headed package variants (cs-winui-msix and cs-wpf-sparse)
    built successfully. Multi-headed packages contain multiple executables in a
    single package identity, so we verify both PrimaryApp and SecondaryApp are produced,
    along with their DLLs, resources, PDBs, and manifest entries.

.PARAMETER OutputPath
    Path to the MultiHeadedPackage sample build output directory.

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

function Check-MultiHeadedVariant
{
    param(
        [string]$VariantName,
        [string]$VariantPath,
        [string]$PrimaryExe,
        [string]$SecondaryExe,
        [string]$PrimaryDll,         # WinUI apps compile to DLL + exe host
        [bool]$ExpectResourcesPri,
        [string]$ExpectedIdentity    # Expected package identity name in manifest
    )

    if (-not (Test-Path $VariantPath))
    {
        return $false
    }

    $script:variantsFound++
    Write-Host "=== Validating $VariantName ===" -ForegroundColor Cyan

    $ok = $true

    # Check PrimaryApp executable
    $primaryFiles = Get-ChildItem -Path $VariantPath -Filter $PrimaryExe -Recurse -ErrorAction SilentlyContinue
    if (-not $primaryFiles)
    {
        Write-Warning "[$VariantName] $PrimaryExe not found"
        $ok = $false
    }
    else
    {
        Write-Host "[$VariantName] Found $PrimaryExe"

        if ($primaryFiles[0].Length -lt 1024)
        {
            Write-Warning "[$VariantName] $PrimaryExe is suspiciously small: $($primaryFiles[0].Length) bytes"
            $ok = $false
        }

        # Platform check
        if (Get-Command Test-BinaryPlatform -ErrorAction SilentlyContinue)
        {
            $platformArg = $Platform
            if ($platformArg -eq 'arm64ec') { $platformArg = 'arm64' }
            $result = Test-BinaryPlatform -FilePath $primaryFiles[0].FullName -ExpectedPlatform $platformArg
            if ($result)
            {
                Write-Host "[$VariantName] $PrimaryExe platform check passed ($Platform)"
            }
            else
            {
                Write-Warning "[$VariantName] $PrimaryExe platform check failed"
                $ok = $false
            }
        }

        # PDB check
        if (Get-Command Test-SymbolFiles -ErrorAction SilentlyContinue)
        {
            $result = Test-SymbolFiles -BinaryPath $primaryFiles[0].FullName
            if ($result)
            {
                Write-Host "[$VariantName] $PrimaryExe PDB found"
            }
            else
            {
                Write-Warning "[$VariantName] $PrimaryExe PDB missing"
                $ok = $false
            }
        }
    }

    # Check PrimaryApp DLL (WinUI apps)
    if ($PrimaryDll)
    {
        $dllFiles = Get-ChildItem -Path $VariantPath -Filter $PrimaryDll -Recurse -ErrorAction SilentlyContinue
        if (-not $dllFiles)
        {
            Write-Warning "[$VariantName] $PrimaryDll not found"
            $ok = $false
        }
        else
        {
            Write-Host "[$VariantName] Found $PrimaryDll"
        }
    }

    # Check SecondaryApp executable
    $secondaryFiles = Get-ChildItem -Path $VariantPath -Filter $SecondaryExe -Recurse -ErrorAction SilentlyContinue
    if (-not $secondaryFiles)
    {
        Write-Warning "[$VariantName] $SecondaryExe not found (multi-headed package incomplete)"
        $ok = $false
    }
    else
    {
        Write-Host "[$VariantName] Found $SecondaryExe"

        if ($secondaryFiles[0].Length -lt 1024)
        {
            Write-Warning "[$VariantName] $SecondaryExe is suspiciously small: $($secondaryFiles[0].Length) bytes"
            $ok = $false
        }

        # PDB check for secondary
        if (Get-Command Test-SymbolFiles -ErrorAction SilentlyContinue)
        {
            $result = Test-SymbolFiles -BinaryPath $secondaryFiles[0].FullName
            if ($result)
            {
                Write-Host "[$VariantName] $SecondaryExe PDB found"
            }
            else
            {
                Write-Warning "[$VariantName] $SecondaryExe PDB missing"
                $ok = $false
            }
        }
    }

    # Check resources.pri
    if ($ExpectResourcesPri)
    {
        $priFiles = Get-ChildItem -Path $VariantPath -Filter "resources.pri" -Recurse -ErrorAction SilentlyContinue
        if (-not $priFiles)
        {
            Write-Warning "[$VariantName] resources.pri not found"
            $ok = $false
        }
        else
        {
            Write-Host "[$VariantName] Found resources.pri"
        }
    }

    # Check AppxManifest has two Application entries (multi-headed)
    $manifests = Get-ChildItem -Path $VariantPath -Filter "AppxManifest.xml" -Recurse -ErrorAction SilentlyContinue
    if ($manifests)
    {
        Write-Host "[$VariantName] Found AppxManifest.xml"

        try
        {
            [xml]$manifest = Get-Content $manifests[0].FullName
            $ns = New-Object System.Xml.XmlNamespaceManager($manifest.NameTable)
            $ns.AddNamespace("pkg", $manifest.DocumentElement.NamespaceURI)

            $apps = $manifest.SelectNodes("//pkg:Application", $ns)
            if ($apps.Count -ge 2)
            {
                Write-Host "[$VariantName] Manifest declares $($apps.Count) Application entries (multi-headed OK)"
            }
            else
            {
                Write-Warning "[$VariantName] Expected 2+ Application entries but found $($apps.Count)"
                $ok = $false
            }
        }
        catch
        {
            Write-Warning "[$VariantName] Failed to parse AppxManifest.xml: $_"
        }

        # Dependency check
        if (Get-Command Test-AppxManifest -ErrorAction SilentlyContinue)
        {
            $result = Test-AppxManifest -ManifestPath $manifests[0].FullName -ExpectedDependency "Microsoft.WindowsAppRuntime*"
            if ($result)
            {
                Write-Host "[$VariantName] AppxManifest dependency check passed"
            }
            else
            {
                Write-Warning "[$VariantName] AppxManifest missing WindowsAppRuntime dependency"
                $ok = $false
            }
        }
    }

    if ($ok)
    {
        Write-Host "[$VariantName] All checks passed" -ForegroundColor Green
    }
    else
    {
        Write-Host "[$VariantName] Some checks failed" -ForegroundColor Red
    }
    Write-Host ""

    return $ok
}

# Validate cs-winui-msix variant
$msixPath = Join-Path $OutputPath "cs\cs-winui-msix"
$result = Check-MultiHeadedVariant `
    -VariantName "cs-winui-msix" `
    -VariantPath $msixPath `
    -PrimaryExe "PrimaryApp.exe" `
    -SecondaryExe "SecondaryApp.exe" `
    -PrimaryDll "PrimaryApp.dll" `
    -ExpectResourcesPri $true `
    -ExpectedIdentity "MultiHeadedMsixSample"
if (-not $result -and (Test-Path $msixPath)) { $failed = $true }

# Validate cs-wpf-sparse variant
$sparsePath = Join-Path $OutputPath "cs\cs-wpf-sparse"
$result = Check-MultiHeadedVariant `
    -VariantName "cs-wpf-sparse" `
    -VariantPath $sparsePath `
    -PrimaryExe "PrimaryApp.exe" `
    -SecondaryExe "SecondaryApp.exe" `
    -PrimaryDll "" `
    -ExpectResourcesPri $false `
    -ExpectedIdentity "MultiHeadedSparseSample"
if (-not $result -and (Test-Path $sparsePath)) { $failed = $true }

if ($variantsFound -eq 0)
{
    Write-Host "No MultiHeadedPackage variants found in $OutputPath - skipping" -ForegroundColor Yellow
    $global:Error.Clear()
    exit 0
}

Write-Host "Validated $variantsFound variant(s)"

if ($failed)
{
    Write-Host "`nMultiHeadedPackage validation FAILED" -ForegroundColor Red
    exit 1
}

Write-Host "`nMultiHeadedPackage validation PASSED" -ForegroundColor Green
$global:Error.Clear()
exit 0
