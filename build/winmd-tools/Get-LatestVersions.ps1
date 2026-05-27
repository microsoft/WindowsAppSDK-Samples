<#
.SYNOPSIS
    Query NuGet for the latest stable and experimental WinAppSDK versions.

.DESCRIPTION
    Calls the NuGet v3 flat-container API to retrieve all available versions of
    Microsoft.WindowsAppSDK, then picks:
      - Latest stable: highest version without any prerelease label
      - Latest experimental: highest version whose prerelease label contains
        "experimental" (case-insensitive)

.PARAMETER AsJson
    Output as JSON instead of formatted text. Useful for piping into other scripts.

.EXAMPLE
    .\Get-LatestVersions.ps1
    Latest Stable:       2.0.1
    Latest Experimental: 2.0.0-Experimental7

.EXAMPLE
    $v = .\Get-LatestVersions.ps1 -AsJson | ConvertFrom-Json
    .\Compare-WinAppSdkApis.ps1 -StableVersion $v.Stable -ExperimentalVersion $v.Experimental
#>
[CmdletBinding()]
param(
    [switch]$AsJson
)

$ErrorActionPreference = 'Stop'

$indexUrl = 'https://api.nuget.org/v3-flatcontainer/microsoft.windowsappsdk/index.json'

Write-Verbose "Fetching version list from: $indexUrl"
$response = Invoke-RestMethod -Uri $indexUrl -UseBasicParsing
$versions = $response.versions

if (-not $versions -or $versions.Count -eq 0) {
    Write-Error "No versions returned from NuGet."
    exit 1
}

# Helper: parse SemVer into a sortable object
function Get-SemVerInfo {
    param([string]$Version)

    # SemVer regex: major.minor.patch(-prerelease)?(+build)?
    if ($Version -match '^(\d+)\.(\d+)\.(\d+)(?:-([0-9A-Za-z\-\.]+))?(?:\+[0-9A-Za-z\-\.]+)?$') {
        return [pscustomobject]@{
            Raw        = $Version
            Major      = [int]$Matches[1]
            Minor      = [int]$Matches[2]
            Patch      = [int]$Matches[3]
            PreRelease = $Matches[4]
            IsStable   = [string]::IsNullOrEmpty($Matches[4])
        }
    }
    return $null
}

$parsed = $versions | ForEach-Object { Get-SemVerInfo $_ } | Where-Object { $_ }

# Latest stable: highest version with no prerelease
$latestStable = $parsed |
    Where-Object { $_.IsStable } |
    Sort-Object Major, Minor, Patch |
    Select-Object -Last 1

# Latest experimental: highest version whose prerelease contains "experimental"
$latestExp = $parsed |
    Where-Object { -not $_.IsStable -and $_.PreRelease -match 'experimental' } |
    Sort-Object Major, Minor, Patch, PreRelease |
    Select-Object -Last 1

if (-not $latestStable) {
    Write-Error "No stable version found."
    exit 1
}
if (-not $latestExp) {
    Write-Error "No experimental version found."
    exit 1
}

$result = [pscustomobject]@{
    Stable       = $latestStable.Raw
    Experimental = $latestExp.Raw
    Source       = 'nuget.org'
    QueriedAt    = (Get-Date).ToString('o')
}

if ($AsJson) {
    $result | ConvertTo-Json
}
else {
    Write-Host "Latest Stable:       $($result.Stable)" -ForegroundColor Green
    Write-Host "Latest Experimental: $($result.Experimental)" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "Run comparison with:"
    Write-Host "  .\Compare-WinAppSdkApis.ps1 -StableVersion `"$($result.Stable)`" -ExperimentalVersion `"$($result.Experimental)`"" -ForegroundColor Cyan
}
