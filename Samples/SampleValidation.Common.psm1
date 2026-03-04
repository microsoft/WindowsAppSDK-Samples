# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

<#
.SYNOPSIS
    Shared helper module for sample validation scripts.

.DESCRIPTION
    Provides utility functions that Validate.ps1 authors can optionally import
    to simplify common validation tasks such as checking file presence, validating
    MSIX packages, and inspecting AppxManifest.xml entries.

    This module is imported by Invoke-SampleValidation.ps1 and made available to
    per-sample Validate.ps1 scripts via the -HelpersModulePath parameter.

.NOTES
    Usage from a Validate.ps1 script:
        if ($HelpersModulePath -and (Test-Path $HelpersModulePath))
        {
            Import-Module $HelpersModulePath
        }
#>

Set-StrictMode -Version 3.0
$ErrorActionPreference = 'Stop'

function Test-FilePresence
{
    <#
    .SYNOPSIS
        Asserts that expected files exist in the specified directory.

    .PARAMETER Path
        The directory to check.

    .PARAMETER ExpectedFiles
        Array of file names or wildcard patterns expected to exist.

    .PARAMETER Recurse
        If set, searches subdirectories for expected files.

    .OUTPUTS
        Returns $true if all files found, $false otherwise. Writes errors for missing files.
    #>
    [CmdletBinding()]
    param(
        [Parameter(Mandatory)]
        [string]$Path,

        [Parameter(Mandatory)]
        [string[]]$ExpectedFiles,

        [switch]$Recurse
    )

    $allFound = $true

    if (-not (Test-Path $Path))
    {
        Write-Error "Output directory does not exist: $Path"
        return $false
    }

    foreach ($pattern in $ExpectedFiles)
    {
        $searchPath = Join-Path $Path $pattern
        $found = Get-ChildItem -Path $searchPath -ErrorAction SilentlyContinue

        if ($Recurse -and -not $found)
        {
            $found = Get-ChildItem -Path $Path -Filter $pattern -Recurse -ErrorAction SilentlyContinue
        }

        if (-not $found)
        {
            Write-Warning "Expected file not found: $pattern in $Path"
            $allFound = $false
        }
    }

    return $allFound
}

function Test-FileSizeRange
{
    <#
    .SYNOPSIS
        Validates that a file's size is within the expected range.

    .PARAMETER FilePath
        Path to the file to check.

    .PARAMETER MinSizeBytes
        Minimum expected file size in bytes.

    .PARAMETER MaxSizeBytes
        Maximum expected file size in bytes. If 0, no upper bound is checked.

    .OUTPUTS
        Returns $true if file size is within range, $false otherwise.
    #>
    [CmdletBinding()]
    param(
        [Parameter(Mandatory)]
        [string]$FilePath,

        [long]$MinSizeBytes = 1,

        [long]$MaxSizeBytes = 0
    )

    if (-not (Test-Path $FilePath))
    {
        Write-Warning "File not found for size check: $FilePath"
        return $false
    }

    $fileInfo = Get-Item $FilePath
    $size = $fileInfo.Length

    if ($size -lt $MinSizeBytes)
    {
        Write-Warning "File '$($fileInfo.Name)' is too small: $size bytes (minimum: $MinSizeBytes bytes)"
        return $false
    }

    if ($MaxSizeBytes -gt 0 -and $size -gt $MaxSizeBytes)
    {
        Write-Warning "File '$($fileInfo.Name)' is too large: $size bytes (maximum: $MaxSizeBytes bytes)"
        return $false
    }

    return $true
}

function Test-MsixPackage
{
    <#
    .SYNOPSIS
        Extracts and validates the contents of an MSIX package.

    .PARAMETER Path
        Path to the .msix or .appx file.

    .PARAMETER ExpectedDependency
        Optional dependency name pattern to verify in the AppxManifest.xml.

    .PARAMETER ExpectedFiles
        Optional array of file patterns expected to exist in the package payload.

    .OUTPUTS
        Returns $true if all checks pass, $false otherwise.
    #>
    [CmdletBinding()]
    param(
        [Parameter(Mandatory)]
        [string]$Path,

        [string]$ExpectedDependency,

        [string[]]$ExpectedFiles
    )

    if (-not (Test-Path $Path))
    {
        Write-Warning "MSIX package not found: $Path"
        return $false
    }

    $allPassed = $true
    $tempDir = Join-Path ([System.IO.Path]::GetTempPath()) "SampleValidation_$([System.Guid]::NewGuid().ToString('N'))"

    try
    {
        New-Item -ItemType Directory -Path $tempDir -Force | Out-Null

        # Extract MSIX as ZIP
        $zipPath = "$tempDir\package.zip"
        Copy-Item -Path $Path -Destination $zipPath
        Expand-Archive -Path $zipPath -DestinationPath "$tempDir\contents" -Force

        $contentsPath = "$tempDir\contents"

        # Validate manifest exists
        $manifestPath = Join-Path $contentsPath "AppxManifest.xml"
        if (-not (Test-Path $manifestPath))
        {
            Write-Warning "AppxManifest.xml not found in MSIX package: $Path"
            return $false
        }

        # Check dependency if specified
        if ($ExpectedDependency)
        {
            $result = Test-AppxManifest -ManifestPath $manifestPath -ExpectedDependency $ExpectedDependency
            if (-not $result)
            {
                $allPassed = $false
            }
        }

        # Check expected files in package
        if ($ExpectedFiles)
        {
            $result = Test-FilePresence -Path $contentsPath -ExpectedFiles $ExpectedFiles
            if (-not $result)
            {
                $allPassed = $false
            }
        }
    }
    finally
    {
        if (Test-Path $tempDir)
        {
            Remove-Item -Path $tempDir -Recurse -Force -ErrorAction SilentlyContinue
        }
    }

    return $allPassed
}

function Test-AppxManifest
{
    <#
    .SYNOPSIS
        Validates entries in an AppxManifest.xml file.

    .PARAMETER ManifestPath
        Path to the AppxManifest.xml file.

    .PARAMETER ExpectedDependency
        Optional dependency name pattern to check in PackageDependency elements.

    .PARAMETER ExpectedCapabilities
        Optional array of capability names expected in the manifest.

    .PARAMETER ExpectedEntryPoint
        Optional entry point string to verify in the Application element.

    .OUTPUTS
        Returns $true if all checks pass, $false otherwise.
    #>
    [CmdletBinding()]
    param(
        [Parameter(Mandatory)]
        [string]$ManifestPath,

        [string]$ExpectedDependency,

        [string[]]$ExpectedCapabilities,

        [string]$ExpectedEntryPoint
    )

    if (-not (Test-Path $ManifestPath))
    {
        Write-Warning "Manifest not found: $ManifestPath"
        return $false
    }

    $allPassed = $true
    [xml]$manifest = Get-Content $ManifestPath

    # Check dependency
    if ($ExpectedDependency)
    {
        $ns = New-Object System.Xml.XmlNamespaceManager($manifest.NameTable)
        $ns.AddNamespace("pkg", $manifest.DocumentElement.NamespaceURI)

        $dependencies = $manifest.SelectNodes("//pkg:PackageDependency", $ns)
        $found = $false
        foreach ($dep in $dependencies)
        {
            if ($dep.Name -like $ExpectedDependency)
            {
                $found = $true
                break
            }
        }

        if (-not $found)
        {
            Write-Warning "Expected dependency '$ExpectedDependency' not found in manifest"
            $allPassed = $false
        }
    }

    # Check capabilities
    if ($ExpectedCapabilities)
    {
        $ns = New-Object System.Xml.XmlNamespaceManager($manifest.NameTable)
        $ns.AddNamespace("pkg", $manifest.DocumentElement.NamespaceURI)

        $capabilities = $manifest.SelectNodes("//pkg:Capability", $ns)
        $capNames = @()
        foreach ($cap in $capabilities)
        {
            $capNames += $cap.Name
        }

        foreach ($expected in $ExpectedCapabilities)
        {
            if ($expected -notin $capNames)
            {
                Write-Warning "Expected capability '$expected' not found in manifest"
                $allPassed = $false
            }
        }
    }

    # Check entry point
    if ($ExpectedEntryPoint)
    {
        $ns = New-Object System.Xml.XmlNamespaceManager($manifest.NameTable)
        $ns.AddNamespace("pkg", $manifest.DocumentElement.NamespaceURI)

        $apps = $manifest.SelectNodes("//pkg:Application", $ns)
        $found = $false
        foreach ($app in $apps)
        {
            if ($app.EntryPoint -eq $ExpectedEntryPoint -or $app.Id -eq $ExpectedEntryPoint)
            {
                $found = $true
                break
            }
        }

        if (-not $found)
        {
            Write-Warning "Expected entry point '$ExpectedEntryPoint' not found in manifest"
            $allPassed = $false
        }
    }

    return $allPassed
}

function Test-BinaryPlatform
{
    <#
    .SYNOPSIS
        Checks that a PE binary targets the expected platform.

    .PARAMETER FilePath
        Path to the binary (.exe or .dll).

    .PARAMETER ExpectedPlatform
        Expected platform: x64, x86, arm64.

    .OUTPUTS
        Returns $true if platform matches, $false otherwise.
    #>
    [CmdletBinding()]
    param(
        [Parameter(Mandatory)]
        [string]$FilePath,

        [Parameter(Mandatory)]
        [ValidateSet('x64', 'x86', 'arm64')]
        [string]$ExpectedPlatform
    )

    if (-not (Test-Path $FilePath))
    {
        Write-Warning "Binary not found for platform check: $FilePath"
        return $false
    }

    try
    {
        $bytes = [System.IO.File]::ReadAllBytes($FilePath)

        # Read PE signature offset from DOS header at offset 0x3C
        $peOffset = [System.BitConverter]::ToInt32($bytes, 0x3C)

        # Machine type is at PE offset + 4
        $machineType = [System.BitConverter]::ToUInt16($bytes, $peOffset + 4)

        $platformMap = @{
            'x86'   = 0x14C   # IMAGE_FILE_MACHINE_I386
            'x64'   = 0x8664  # IMAGE_FILE_MACHINE_AMD64
            'arm64' = 0xAA64  # IMAGE_FILE_MACHINE_ARM64
        }

        $expectedMachine = $platformMap[$ExpectedPlatform]
        if ($machineType -ne $expectedMachine)
        {
            $actualPlatform = ($platformMap.GetEnumerator() | Where-Object { $_.Value -eq $machineType }).Key
            if (-not $actualPlatform) { $actualPlatform = "Unknown (0x$($machineType.ToString('X4')))" }
            Write-Warning "Binary '$FilePath' targets '$actualPlatform' but expected '$ExpectedPlatform'"
            return $false
        }

        return $true
    }
    catch
    {
        Write-Warning "Failed to read PE header from '$FilePath': $_"
        return $false
    }
}

function Write-ValidationResult
{
    <#
    .SYNOPSIS
        Writes a structured validation result message.

    .PARAMETER TestName
        Name of the validation check.

    .PARAMETER Passed
        Whether the check passed.

    .PARAMETER Message
        Optional detail message.

    .OUTPUTS
        Writes formatted output. In Azure Pipelines, uses ##vso logging commands.
    #>
    [CmdletBinding()]
    param(
        [Parameter(Mandatory)]
        [string]$TestName,

        [Parameter(Mandatory)]
        [bool]$Passed,

        [string]$Message = ""
    )

    $icon = if ($Passed) { "[PASS]" } else { "[FAIL]" }
    $color = if ($Passed) { "Green" } else { "Red" }

    $output = "$icon $TestName"
    if ($Message)
    {
        $output += " - $Message"
    }

    Write-Host $output -ForegroundColor $color

    if (-not $Passed -and $env:AzurePipelineBuild -eq $true)
    {
        Write-Host "##vso[task.logissue type=warning]$TestName - $Message"
    }
}

function Test-SymbolFiles
{
    <#
    .SYNOPSIS
        Checks that a matching .pdb file exists for a given binary.

    .PARAMETER BinaryPath
        Path to the .exe or .dll file.

    .PARAMETER MinSizeBytes
        Minimum expected PDB size in bytes. Default: 1024 (1KB).

    .OUTPUTS
        Returns $true if PDB found and valid, $false otherwise.
    #>
    [CmdletBinding()]
    param(
        [Parameter(Mandatory)]
        [string]$BinaryPath,

        [long]$MinSizeBytes = 1024
    )

    if (-not (Test-Path $BinaryPath))
    {
        Write-Warning "Binary not found for symbol check: $BinaryPath"
        return $false
    }

    $binaryName = [System.IO.Path]::GetFileNameWithoutExtension($BinaryPath)
    $binaryDir = [System.IO.Path]::GetDirectoryName($BinaryPath)

    # Look for PDB next to the binary
    $pdbPath = Join-Path $binaryDir "$binaryName.pdb"
    if (-not (Test-Path $pdbPath))
    {
        # Search recursively from binary directory
        $pdbFiles = Get-ChildItem -Path $binaryDir -Filter "$binaryName.pdb" -Recurse -ErrorAction SilentlyContinue
        if ($pdbFiles)
        {
            $pdbPath = $pdbFiles[0].FullName
        }
        else
        {
            Write-Warning "PDB not found for $binaryName (looked in $binaryDir)"
            return $false
        }
    }

    $pdbInfo = Get-Item $pdbPath
    if ($pdbInfo.Length -lt $MinSizeBytes)
    {
        Write-Warning "PDB for $binaryName is suspiciously small: $($pdbInfo.Length) bytes (minimum: $MinSizeBytes)"
        return $false
    }

    return $true
}

function Test-AppxSymbols
{
    <#
    .SYNOPSIS
        Checks that .appxsym files exist in the build output for packaged apps.

    .PARAMETER Path
        Path to search for .appxsym files.

    .OUTPUTS
        Returns $true if appxsym files found, $false otherwise.
    #>
    [CmdletBinding()]
    param(
        [Parameter(Mandatory)]
        [string]$Path
    )

    if (-not (Test-Path $Path))
    {
        Write-Warning "Path not found for appxsym check: $Path"
        return $false
    }

    $symFiles = Get-ChildItem -Path $Path -Filter "*.appxsym" -Recurse -ErrorAction SilentlyContinue
    if (-not $symFiles)
    {
        Write-Warning "No .appxsym files found in $Path"
        return $false
    }

    foreach ($sym in $symFiles)
    {
        if ($sym.Length -lt 1024)
        {
            Write-Warning "appxsym file is suspiciously small: $($sym.Name) ($($sym.Length) bytes)"
            return $false
        }
    }

    return $true
}

Export-ModuleMember -Function @(
    'Test-FilePresence',
    'Test-FileSizeRange',
    'Test-MsixPackage',
    'Test-AppxManifest',
    'Test-BinaryPlatform',
    'Test-SymbolFiles',
    'Test-AppxSymbols',
    'Write-ValidationResult'
)
