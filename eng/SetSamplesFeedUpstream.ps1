[CmdletBinding()]
param(
    [Parameter(Mandatory)]
    [ValidateSet("Enable", "Disable")]
    [string]$Action,

    [string]$AccessToken = $env:SYSTEM_ACCESSTOKEN,

    [string]$Organization = "shine-oss",
    [string]$Project = "WinAppSDK-Samples",
    [string]$FeedId = "4c0236bb-5661-46ec-a487-808515ccdbd2"
)

Set-StrictMode -Version 3.0
$ErrorActionPreference = "Stop"

if ([string]::IsNullOrWhiteSpace($AccessToken))
{
    throw "An Azure Pipelines OAuth token is required."
}

$projectName = [Uri]::EscapeDataString($Project)
$feedUri = "https://feeds.dev.azure.com/$Organization/$projectName/_apis/packaging/feeds/$FeedId`?api-version=7.0"
$headers = @{
    Authorization = "Bearer $AccessToken"
}

$upstreamSources = @()
if ($Action -eq "Enable")
{
    $upstreamSources = @(
        @{
            name = "NuGet Gallery"
            protocol = "nuget"
            location = "https://api.nuget.org/v3/index.json"
            displayLocation = "https://api.nuget.org/v3/index.json"
            upstreamSourceType = "public"
        }
    )
}

$body = @{
    upstreamSources = $upstreamSources
} | ConvertTo-Json -Depth 4

Invoke-RestMethod `
    -Method Patch `
    -Uri $feedUri `
    -Headers $headers `
    -ContentType "application/json" `
    -Body $body | Out-Null

$feed = Invoke-RestMethod -Method Get -Uri $feedUri -Headers $headers
$activeUpstreams = @(
    $feed.upstreamSources | Where-Object {
        !$_.PSObject.Properties["deletedDate"] -or !$_.deletedDate
    }
)

if ($Action -eq "Enable" -and $activeUpstreams.Count -ne 1)
{
    throw "Expected one active upstream after enabling NuGet Gallery, but found $($activeUpstreams.Count)."
}

if ($Action -eq "Disable" -and $activeUpstreams.Count -ne 0)
{
    throw "Expected no active upstreams after disabling NuGet Gallery, but found $($activeUpstreams.Count)."
}

Write-Host "$Action completed for feed '$($feed.name)'. Active upstream count: $($activeUpstreams.Count)."
