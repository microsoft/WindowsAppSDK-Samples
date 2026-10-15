<#
.SYNOPSIS
    Updates WinAppSDK-related package references across the samples to a specified version.
.DESCRIPTION
    Ensures the requested Microsoft.WindowsAppSDK NuGet package (and key dependencies) is available locally,
    then rewrites packages.config, project files, and Directory.Packages.props entries so they all target the
    provided version. This script is intended to be the single place to bump WinAppSDK versions in the repo.
.PARAMETER WinAppSDKVersion
    Version of Microsoft.WindowsAppSDK to apply (for example, 1.8.251106002). You can discover the latest
    stable, servicing, or preview versions at https://www.nuget.org/packages/Microsoft.WindowsAppSDK/.
.PARAMETER NuGetPackagesFolder
    Optional path to a NuGet packages directory that already contains the desired packages. When omitted the
    script restores the packages into the local ./packages folder.
.EXAMPLE
    .\UpdateVersions.ps1 -WinAppSDKVersion 1.8.251106002
    Updates all projects to reference Microsoft.WindowsAppSDK version 1.8.251106002, restoring packages into
    the default ./packages directory when needed.
#>
Param(
    [string]$WinAppSDKVersion = "",
    [string]$NuGetPackagesFolder = ""
)

# Ensure a local packages cache exists when the caller does not provide one.
# A lightweight restore keeps this script self-contained for version updates.
if ($NuGetPackagesFolder -eq "") {
    $NuGetPackagesFolder = Join-Path $PSScriptRoot "packages"
    Write-Host "NuGetPackagesFolder not supplied. Using default: $NuGetPackagesFolder"
}

if (!(Test-Path $NuGetPackagesFolder)) {
    New-Item -ItemType Directory -Path $NuGetPackagesFolder -Force | Out-Null
}

$nugetToolDir = Join-Path $PSScriptRoot ".nuget"
$nugetExe = Join-Path $nugetToolDir "nuget.exe"
if (!(Test-Path $nugetExe)) {
    if (!(Test-Path $nugetToolDir)) { New-Item -ItemType Directory -Path $nugetToolDir | Out-Null }
    Write-Host "Downloading nuget.exe..."
    try {
        Invoke-WebRequest https://dist.nuget.org/win-x86-commandline/latest/nuget.exe `
            -OutFile $nugetExe `
            -ErrorAction Stop
    }
    catch {
        Write-Warning "Failed to download nuget.exe: $($_.Exception.Message)"
    }
}

# Always install/refresh the requested Microsoft.WindowsAppSDK version (idempotent if already present).
if ([string]::IsNullOrWhiteSpace($WinAppSDKVersion)) {
    $winAppSdkNugetUrl = "https://www.nuget.org/packages/Microsoft.WindowsAppSDK/"
    Write-Warning "WinAppSDKVersion not supplied; cannot install Microsoft.WindowsAppSDK package automatically."
    Write-Warning "Visit $winAppSdkNugetUrl to determine the latest version, then rerun the script."
    exit 1
}
else {
    if (!(Test-Path $NuGetPackagesFolder)) { New-Item -ItemType Directory -Path $NuGetPackagesFolder | Out-Null }
    Write-Host "Installing Microsoft.WindowsAppSDK $WinAppSDKVersion into $NuGetPackagesFolder (running inside folder)"
    Push-Location $NuGetPackagesFolder
    try {
        & $nugetExe install Microsoft.WindowsAppSDK `
            -Version $WinAppSDKVersion `
            -OutputDirectory . `
            -Prerelease `
            -DependencyVersion Highest

        if ($LASTEXITCODE -ne 0) {
            throw "nuget.exe failed with exit code $LASTEXITCODE"
        }
    }
    catch {
        Write-Warning $_
    }
    finally {
        Pop-Location
    }
}

# Seed the package/version map with the WinAppSDK metapackage.
$nugetPackageToVersionTable = @{"Microsoft.WindowsAppSDK" = $WinAppSDKVersion }

# When a populated packages folder is available, harvest dependency versions from it.
Get-ChildItem $NuGetPackagesFolder |
Sort-Object Name |
Where-Object { $_.Name -like "Microsoft.WindowsAppSDK.*" -or 
    $_.Name -like "Microsoft.Windows.SDK.BuildTools.*" -or 
    $_.Name -like "Microsoft.Web.WebView2.*" } | 
Where-Object { $_.Name -notlike "*.nupkg" } |
ForEach-Object { 
    if ($_.Name -match "^(Microsoft\.WindowsAppSDK\.[a-zA-Z]+)\.([0-9].*)$" -or
        $_.Name -match "^(Microsoft\.Windows\.SDK\.BuildTools\.MSIX)\.([0-9].*)$" -or
        $_.Name -match "^(Microsoft\.Windows\.SDK\.BuildTools)\.([0-9].*)$" -or
        $_.Name -match "^(Microsoft\.Web\.WebView2)\.([0-9].*)$") {
        $nugetPackageToVersionTable[$Matches[1]] = $Matches[2]
        Write-Host "Found $($Matches[1]) - $($Matches[2])"
    } 
}

# Microsoft.Windows.AI.MachineLearning ships on its own version track, independent of Microsoft.WindowsAppSDK,
# so a fixed pin would fall below the range the wrapper requires as the wrapper advances. Instead, read the
# AI.ML floor declared by the Microsoft.WindowsAppSDK.ML wrapper that was just resolved and harvested above and
# pin the samples to that floor. Using the harvested wrapper version (rather than searching the folder) keeps
# this in lockstep with the exact wrapper the samples are being pinned to and is deterministic. A WinAppSDK
# that carries no ML wrapper simply leaves the committed pin untouched.
$mlWrapperId = "Microsoft.WindowsAppSDK.ML"
$aimlId = "Microsoft.Windows.AI.MachineLearning"
if ($nugetPackageToVersionTable.ContainsKey($mlWrapperId)) {
    $mlWrapperFolder = Join-Path $NuGetPackagesFolder "$mlWrapperId.$($nugetPackageToVersionTable[$mlWrapperId])"
    $mlWrapperNuspec = Join-Path $mlWrapperFolder "$mlWrapperId.nuspec"
    if (Test-Path $mlWrapperNuspec) {
        try {
            [xml]$mlWrapperXml = Get-Content -Path $mlWrapperNuspec
            # The AI.ML dependency is declared once per target-framework <group> with an identical range in each,
            # so the first match is representative; fall back to an ungrouped <dependency> list when present.
            $aimlDependency = $null
            foreach ($group in $mlWrapperXml.package.metadata.dependencies.group) {
                $aimlDependency = $group.dependency | Where-Object { $_.id -eq $aimlId } | Select-Object -First 1
                if ($aimlDependency) { break }
            }
            if (-not $aimlDependency) {
                $aimlDependency = $mlWrapperXml.package.metadata.dependencies.dependency | Where-Object { $_.id -eq $aimlId } | Select-Object -First 1
            }
            if ($aimlDependency) {
                # NuGet range notation may be "[min, max)", "[exact]", or a bare "min"; keep the lower bound.
                $aimlFloor = (($aimlDependency.version -replace '[\[\]()]', '') -split ',')[0].Trim()
                if ($aimlFloor) {
                    $nugetPackageToVersionTable[$aimlId] = $aimlFloor
                    Write-Host "Found $aimlId - $aimlFloor (floor from $mlWrapperId $($nugetPackageToVersionTable[$mlWrapperId]))"
                }
            }
        }
        catch {
            Write-Warning "Unable to read $aimlId floor from ${mlWrapperNuspec}: $($_.Exception.Message)"
        }
    }
}

Get-ChildItem -Recurse Directory.Packages.props -Path $PSScriptRoot | foreach-object {
    $content = Get-Content $_.FullName -Raw

    foreach ($nugetPackageToVersion in $nugetPackageToVersionTable.GetEnumerator()) {
        $newVersionString = 'PackageVersion Include="' + $nugetPackageToVersion.Key + '" Version="' + $nugetPackageToVersion.Value + '"'
        $oldVersionString = 'PackageVersion Include="' + $nugetPackageToVersion.Key + '" Version="[-.0-9a-zA-Z]*"'
        $content = $content -replace $oldVersionString, $newVersionString
    }

    Set-Content -Path $_.FullName -Value $content
    Write-Host "Modified " $_.FullName 
}