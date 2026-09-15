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

for ($attempt = 1; $attempt -le 5; $attempt++)
{
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

    $expectedCount = if ($Action -eq "Enable") { 1 } else { 0 }
    if ($activeUpstreams.Count -eq $expectedCount)
    {
        break
    }

    if ($attempt -lt 5)
    {
        Write-Warning "$Action attempt $attempt left $($activeUpstreams.Count) active upstreams; retrying."
        Start-Sleep -Seconds 5
    }
}

if ($activeUpstreams.Count -ne $expectedCount)
{
    throw "Expected $expectedCount active upstreams after $Action, but found $($activeUpstreams.Count)."
}

Write-Host "$Action completed for feed '$($feed.name)'. Active upstream count: $($activeUpstreams.Count)."
