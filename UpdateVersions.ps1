<#
.SYNOPSIS
    Updates WinAppSDK-related package references across the samples to a specified version.
.DESCRIPTION
    Ensures the requested Microsoft.WindowsAppSDK NuGet package and key dependencies are available locally,
    then updates selected Directory.Packages.props entries. The root Samples/Directory.Packages.props file is
    the default and additional central package files must be selected explicitly.
.PARAMETER WinAppSDKVersion
    Version of Microsoft.WindowsAppSDK to apply (for example, 1.8.251106002). You can discover the latest
    stable, servicing, or preview versions at https://www.nuget.org/packages/Microsoft.WindowsAppSDK/.
.PARAMETER NuGetPackagesFolder
    Optional path to a NuGet packages directory that already contains the desired packages. When omitted the
    script restores the packages into the local ./packages folder.
.PARAMETER DirectoryPackagesPropsPath
    Repository-relative or absolute paths to central package files to update. Defaults to the root
    Samples/Directory.Packages.props file. Child package files are never discovered or modified implicitly.
.EXAMPLE
    .\UpdateVersions.ps1 -WinAppSDKVersion 1.8.251106002
    Updates the root central package file, restoring packages into the default ./packages directory when
    needed.
.EXAMPLE
    .\UpdateVersions.ps1 -WinAppSDKVersion 2.4.0 `
        -DirectoryPackagesPropsPath 'Samples\Directory.Packages.props', 'Samples\WindowsML\Directory.Packages.props'
    Explicitly updates both the root and WindowsML central package files.
#>
Param(
    [string]$WinAppSDKVersion = "",
    [string]$NuGetPackagesFolder = "",
    [string[]]$DirectoryPackagesPropsPath = @('Samples\Directory.Packages.props')
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
        throw "Failed to download nuget.exe: $($_.Exception.Message)"
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
            -DependencyVersion Lowest

        if ($LASTEXITCODE -ne 0) {
            throw "nuget.exe failed with exit code $LASTEXITCODE"
        }
    }
    catch {
        throw
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

$repositoryRoot = [System.IO.Path]::GetFullPath($PSScriptRoot).TrimEnd('\') + '\'
if ($DirectoryPackagesPropsPath.Count -eq 0) {
    throw 'At least one Directory.Packages.props path is required.'
}

$directoryPackagesPropsFiles = foreach ($path in $DirectoryPackagesPropsPath) {
    $candidate = if ([System.IO.Path]::IsPathRooted($path)) {
        $path
    }
    else {
        Join-Path $PSScriptRoot $path
    }

    $resolved = Resolve-Path -LiteralPath $candidate -ErrorAction Stop
    $fullPath = [System.IO.Path]::GetFullPath($resolved.Path)
    if (!$fullPath.StartsWith($repositoryRoot, [System.StringComparison]::OrdinalIgnoreCase)) {
        throw "Package file must be inside the repository: $path"
    }
    if ([System.IO.Path]::GetFileName($fullPath) -ne 'Directory.Packages.props') {
        throw "Expected a Directory.Packages.props file: $path"
    }

    $item = Get-Item -LiteralPath $fullPath
    if ($item.PSIsContainer) {
        throw "Expected a file but found a directory: $path"
    }

    $item
}

$directoryPackagesPropsFiles | Sort-Object FullName -Unique | ForEach-Object {
    $content = Get-Content $_.FullName -Raw

    foreach ($nugetPackageToVersion in $nugetPackageToVersionTable.GetEnumerator()) {
        $newVersionString = 'PackageVersion Include="' + $nugetPackageToVersion.Key + '" Version="' + $nugetPackageToVersion.Value + '"'
        $oldVersionString = 'PackageVersion Include="' + $nugetPackageToVersion.Key + '" Version="[-.0-9a-zA-Z]*"'
        $content = $content -replace $oldVersionString, $newVersionString
    }

    Set-Content -Path $_.FullName -Value $content
    Write-Host "Modified " $_.FullName 
}