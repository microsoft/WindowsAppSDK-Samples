[CmdletBinding()]
param(
    [string]$SamplesRoot = (Join-Path $PSScriptRoot "..\Samples"),
    [string]$NuGetConfigPath = (Join-Path $PSScriptRoot "..\Samples\nuget.config"),
    [Parameter(Mandatory)]
    [string]$PackagesDirectory,
    [switch]$DesktopSdkProjectsOnly
)

Set-StrictMode -Version 3.0
$ErrorActionPreference = "Stop"

$nuget = Get-Command nuget -ErrorAction Stop
$restoreTargets = @(Get-ChildItem -Path $SamplesRoot -Recurse -File -Filter *.sln)
$projects = @(Get-ChildItem -Path $SamplesRoot -Recurse -File -Filter *.csproj)
$mauiProjects = @(
    $projects | Select-String -Pattern '<UseMaui>\s*true' -List |
        ForEach-Object { $_.Path }
)
if ($DesktopSdkProjectsOnly)
{
    $restoreTargets = @($projects | Where-Object {
        [xml]$project = Get-Content -LiteralPath $_.FullName -Raw
        $project.DocumentElement.GetAttribute("Sdk") -like "Microsoft.NET.Sdk*" -and
            $_.FullName -notin $mauiProjects
    })
}
if ($restoreTargets.Count -eq 0)
{
    throw "No sample restore targets were found under '$SamplesRoot'."
}

# MAUI projects require the MAUI workload before restore.
if (!$DesktopSdkProjectsOnly -and $mauiProjects.Count -gt 0)
{
    $dotnet = Get-Command dotnet -ErrorAction Stop
    Write-Host "Installing .NET MAUI workload"
    & $dotnet.Source workload install maui `
        --source https://api.nuget.org/v3/index.json `
        --skip-manifest-update

    if ($LASTEXITCODE -ne 0)
    {
        throw "dotnet workload install maui failed with exit code $LASTEXITCODE."
    }
}

New-Item -ItemType Directory -Path $PackagesDirectory -Force | Out-Null
$originalPackagesDirectory = $env:NUGET_PACKAGES
$env:NUGET_PACKAGES = $PackagesDirectory

try
{
    foreach ($restoreTarget in $restoreTargets)
    {
        Write-Host "Restoring $($restoreTarget.FullName)"
        & $nuget.Source restore $restoreTarget.FullName `
            -ConfigFile $NuGetConfigPath `
            -PackagesDirectory $PackagesDirectory `
            -NoHttpCache `
            -NonInteractive `
            -ForceEnglishOutput `
            -Verbosity quiet

        if ($LASTEXITCODE -ne 0)
        {
            throw "NuGet restore failed for '$($restoreTarget.FullName)' with exit code $LASTEXITCODE."
        }
    }

    foreach ($cmakeFile in Get-ChildItem -Path $SamplesRoot -Recurse -File -Filter CMakeLists.txt)
    {
        $content = Get-Content -Path $cmakeFile.FullName -Raw
        $packageMatches = [regex]::Matches(
            $content,
            'set\(\s*(?<prefix>[A-Za-z0-9_]+)_NUGET_ID\s+"(?<id>[^"]+)"')

        foreach ($packageMatch in $packageMatches)
        {
            $prefix = [regex]::Escape($packageMatch.Groups["prefix"].Value)
            $versionMatch = [regex]::Match(
                $content,
                "set\(\s*${prefix}_NUGET_VERSION\s+`"(?<version>[^`"]+)`"")

            if (!$versionMatch.Success)
            {
                throw "No matching NuGet version was found for '$($packageMatch.Groups["id"].Value)' in '$($cmakeFile.FullName)'."
            }

            $packageId = $packageMatch.Groups["id"].Value
            $packageVersion = $versionMatch.Groups["version"].Value
            Write-Host "Installing CMake dependency $packageId $packageVersion"
            & $nuget.Source install $packageId `
                -Version $packageVersion `
                -OutputDirectory $PackagesDirectory `
                -ConfigFile $NuGetConfigPath `
                -NoHttpCache `
                -NonInteractive `
                -DirectDownload `
                -DependencyVersion Ignore `
                -Verbosity quiet

            if ($LASTEXITCODE -ne 0)
            {
                throw "NuGet install failed for '$packageId $packageVersion' with exit code $LASTEXITCODE."
            }
        }
    }
}
finally
{
    $env:NUGET_PACKAGES = $originalPackagesDirectory
}

Write-Host "Restored $($restoreTargets.Count) sample restore targets and all declared CMake NuGet dependencies."
