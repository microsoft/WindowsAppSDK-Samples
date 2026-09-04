<#
.SYNOPSIS
    Updates Windows App SDK versions across the repository's central package files.
.DESCRIPTION
    Restores the requested Microsoft.WindowsAppSDK package and its dependency
    closure, then updates matching declarations in every Directory.Packages.props
    file. A specific set of central package files can be selected when needed.
.PARAMETER WinAppSDKVersion
    Microsoft.WindowsAppSDK version to restore and apply.
.PARAMETER NuGetPackagesFolder
    Package folder used to discover the resolved dependency versions. Defaults
    to the repository's packages folder.
.PARAMETER DirectoryPackagesPropsPath
    Repository-relative or absolute paths to Directory.Packages.props files.
    When omitted, all Directory.Packages.props files in the repository are used.
.EXAMPLE
    .\UpdateVersions.ps1 -WinAppSDKVersion 2.4.1
.EXAMPLE
    .\UpdateVersions.ps1 -WinAppSDKVersion 2.1.4 `
        -DirectoryPackagesPropsPath 'Samples\Directory.Packages.props'
#>
[CmdletBinding()]
param(
    [Parameter(Mandatory)]
    [ValidateNotNullOrEmpty()]
    [ValidatePattern('^[0-9A-Za-z][0-9A-Za-z.+-]*$')]
    [string]$WinAppSDKVersion,

    [Parameter()]
    [string]$NuGetPackagesFolder = '',

    [Parameter()]
    [string[]]$DirectoryPackagesPropsPath = @()
)

Set-StrictMode -Version 2.0
$ErrorActionPreference = 'Stop'

if ($NuGetPackagesFolder -eq '') {
    $NuGetPackagesFolder = Join-Path $PSScriptRoot 'packages'
}

$NuGetPackagesFolder = [System.IO.Path]::GetFullPath($NuGetPackagesFolder)
if (-not (Test-Path -LiteralPath $NuGetPackagesFolder)) {
    New-Item -ItemType Directory -Path $NuGetPackagesFolder -Force | Out-Null
}
$resolvedPackagesFolder = Join-Path $NuGetPackagesFolder $WinAppSDKVersion

$nugetToolDirectory = Join-Path $PSScriptRoot '.nuget'
$nugetExecutable = Join-Path $nugetToolDirectory 'nuget.exe'
if (-not (Test-Path -LiteralPath $nugetExecutable)) {
    New-Item -ItemType Directory -Path $nugetToolDirectory -Force | Out-Null
    Write-Verbose 'Downloading nuget.exe.'
    Invoke-WebRequest `
        -Uri 'https://dist.nuget.org/win-x86-commandline/latest/nuget.exe' `
        -OutFile $nugetExecutable
}

Write-Verbose "Restoring Microsoft.WindowsAppSDK $WinAppSDKVersion."
& $nugetExecutable install Microsoft.WindowsAppSDK `
    -Version $WinAppSDKVersion `
    -OutputDirectory $resolvedPackagesFolder `
    -Prerelease `
    -DependencyVersion Lowest `
    -NonInteractive

if ($LASTEXITCODE -ne 0) {
    throw "nuget.exe failed with exit code $LASTEXITCODE."
}

$packageVersions = @{
    'Microsoft.WindowsAppSDK' = $WinAppSDKVersion
}

Get-ChildItem -LiteralPath $resolvedPackagesFolder -Directory |
    Sort-Object -Property Name |
    ForEach-Object {
        $patterns = @(
            '^(Microsoft\.WindowsAppSDK\.[A-Za-z]+)\.([0-9].*)$',
            '^(Microsoft\.Windows\.SDK\.BuildTools(?:\.MSIX)?)\.([0-9].*)$',
            '^(Microsoft\.Web\.WebView2)\.([0-9].*)$'
        )

        foreach ($pattern in $patterns) {
            if ($_.Name -match $pattern) {
                $packageVersions[$Matches[1]] = $Matches[2]
                break
            }
        }
    }

$repositoryRoot = [System.IO.Path]::GetFullPath($PSScriptRoot).TrimEnd('\')
$repositoryPrefix = $repositoryRoot + '\'

$packageFiles = if ($DirectoryPackagesPropsPath.Count -eq 0) {
    @(Get-ChildItem `
            -LiteralPath $repositoryRoot `
            -Filter 'Directory.Packages.props' `
            -File `
            -Recurse)
}
else {
    @(
        foreach ($path in $DirectoryPackagesPropsPath) {
            $candidate = if ([System.IO.Path]::IsPathRooted($path)) {
                $path
            }
            else {
                Join-Path $repositoryRoot $path
            }

            $resolvedPath = (Resolve-Path -LiteralPath $candidate).Path
            $fullPath = [System.IO.Path]::GetFullPath($resolvedPath)
            if (-not $fullPath.StartsWith(
                    $repositoryPrefix,
                    [System.StringComparison]::OrdinalIgnoreCase)) {
                throw "Package file must be inside the repository: $path"
            }

            if ([System.IO.Path]::GetFileName($fullPath) -ne
                'Directory.Packages.props') {
                throw "Expected a Directory.Packages.props file: $path"
            }

            Get-Item -LiteralPath $fullPath
        }
    )
}

$packageFiles = @($packageFiles | Sort-Object -Property FullName -Unique)
if ($packageFiles.Count -eq 0) {
    throw 'No Directory.Packages.props files were found.'
}

$matchedWindowsAppSdkDeclarations = 0
$matchedDeclarationCount = 0

foreach ($packageFile in $packageFiles) {
    $content = Get-Content -LiteralPath $packageFile.FullName -Raw
    $originalContent = $content
    $matchedPackageNames = @()

    foreach ($packageVersion in $packageVersions.GetEnumerator()) {
        $escapedPackageName = [regex]::Escape($packageVersion.Key)
        $pattern = '(<PackageVersion\b[^>]*\bInclude="' +
            $escapedPackageName + '"[^>]*\bVersion=")([^"]*)(")'
        $matches = [regex]::Matches($content, $pattern)
        if ($matches.Count -eq 0) {
            continue
        }

        $matchedDeclarationCount += $matches.Count
        $matchedPackageNames += $packageVersion.Key
        if ($packageVersion.Key -eq 'Microsoft.WindowsAppSDK') {
            $matchedWindowsAppSdkDeclarations += $matches.Count
        }

        $content = [regex]::Replace(
            $content,
            $pattern,
            '${1}' + $packageVersion.Value + '${3}')
    }

    if ($content -ne $originalContent) {
        Set-Content -LiteralPath $packageFile.FullName -Value $content -NoNewline
    }

    if ($matchedPackageNames.Count -gt 0) {
        Write-Verbose (
            "Processed $($packageFile.FullName): " +
            ($matchedPackageNames -join ', '))
    }
}

if ($matchedWindowsAppSdkDeclarations -eq 0) {
    throw 'No Microsoft.WindowsAppSDK central package declarations were found.'
}

$staleDeclarations = @()
foreach ($packageFile in $packageFiles) {
    $content = Get-Content -LiteralPath $packageFile.FullName -Raw

    foreach ($packageVersion in $packageVersions.GetEnumerator()) {
        $escapedPackageName = [regex]::Escape($packageVersion.Key)
        $pattern = '<PackageVersion\b[^>]*\bInclude="' +
            $escapedPackageName + '"[^>]*\bVersion="([^"]*)"'

        foreach ($match in [regex]::Matches($content, $pattern)) {
            if ($match.Groups[1].Value -ne $packageVersion.Value) {
                $staleDeclarations += (
                    "$($packageFile.FullName): $($packageVersion.Key) " +
                    "is $($match.Groups[1].Value), expected " +
                    $packageVersion.Value)
            }
        }
    }
}

if ($staleDeclarations.Count -gt 0) {
    throw "Central package verification failed:`n$($staleDeclarations -join "`n")"
}

Write-Verbose (
    "Verified $matchedDeclarationCount declarations across " +
    "$($packageFiles.Count) central package files.")

exit 0
