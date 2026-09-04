[CmdletBinding()]
param(
    [string]$SamplesRoot = (Join-Path $PSScriptRoot "..\Samples"),
    [string]$NuGetConfigPath = (Join-Path $PSScriptRoot "..\Samples\nuget.config"),
    [Parameter(Mandatory)]
    [string]$PackagesDirectory
)

Set-StrictMode -Version 3.0
$ErrorActionPreference = "Stop"

$nuget = Get-Command nuget -ErrorAction Stop
$solutions = @(Get-ChildItem -Path $SamplesRoot -Recurse -File -Filter *.sln)
if ($solutions.Count -eq 0)
{
    throw "No solutions were found under '$SamplesRoot'."
}

New-Item -ItemType Directory -Path $PackagesDirectory -Force | Out-Null
$originalPackagesDirectory = $env:NUGET_PACKAGES
$env:NUGET_PACKAGES = $PackagesDirectory

try
{
    foreach ($solution in $solutions)
    {
        Write-Host "Restoring $($solution.FullName)"
        & $nuget.Source restore $solution.FullName `
            -ConfigFile $NuGetConfigPath `
            -PackagesDirectory $PackagesDirectory `
            -NoHttpCache `
            -NonInteractive `
            -ForceEnglishOutput `
            -Verbosity quiet

        if ($LASTEXITCODE -ne 0)
        {
            throw "NuGet restore failed for '$($solution.FullName)' with exit code $LASTEXITCODE."
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

Write-Host "Restored $($solutions.Count) sample solutions and all declared CMake NuGet dependencies."
