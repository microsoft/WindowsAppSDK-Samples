[CmdletBinding()]
param(
    [string]$PackageVersion = $env:WINDOWS_APP_SDK_VERSION,
    [string]$NuGetConfigPath = (Join-Path $PSScriptRoot "SamplesFeed.nuget.config"),
    [Parameter(Mandatory)]
    [string]$PackagesDirectory
)

Set-StrictMode -Version 3.0
$ErrorActionPreference = "Stop"

if ([string]::IsNullOrWhiteSpace($PackageVersion) -or
    $PackageVersion -notmatch '^[0-9A-Za-z][0-9A-Za-z.+-]*$')
{
    throw "WindowsAppSDKVersion must be a valid non-empty NuGet version."
}

$nuget = Get-Command nuget -ErrorAction Stop
New-Item -ItemType Directory -Path $PackagesDirectory -Force | Out-Null

Write-Host "Restoring Microsoft.WindowsAppSDK $PackageVersion and its dependency closure."
& $nuget.Source install Microsoft.WindowsAppSDK `
    -Version $PackageVersion `
    -OutputDirectory $PackagesDirectory `
    -ConfigFile $NuGetConfigPath `
    -NoHttpCache `
    -NonInteractive `
    -DirectDownload `
    -DependencyVersion Lowest `
    -Verbosity quiet

if ($LASTEXITCODE -ne 0)
{
    throw "NuGet install failed for 'Microsoft.WindowsAppSDK $PackageVersion' with exit code $LASTEXITCODE."
}

Write-Host "Restored Microsoft.WindowsAppSDK $PackageVersion and its dependency closure."
