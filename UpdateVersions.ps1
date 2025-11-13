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

    if (!(Test-Path $NuGetPackagesFolder)) {
        Write-Host "Packages folder not found. Will perform a minimal restore to populate it."       
    }

    $nugetToolDir = Join-Path $PSScriptRoot ".nuget"
    $nugetExe = Join-Path $nugetToolDir "nuget.exe"
    if (!(Test-Path $nugetExe)) {
        if (!(Test-Path $nugetToolDir)) { New-Item -ItemType Directory -Path $nugetToolDir | Out-Null }
        Write-Host "Downloading nuget.exe..."
        Invoke-WebRequest https://dist.nuget.org/win-x86-commandline/latest/nuget.exe -OutFile $nugetExe
    }

    # If no Microsoft.WindowsAppSDK package is present, restore the requested version (and dependencies).
    $hasWinAppSdk = Get-ChildItem -Path $NuGetPackagesFolder -ErrorAction SilentlyContinue | Where-Object { $_.Name -like "Microsoft.WindowsAppSDK.*" } | Select-Object -First 1
    if (-not $hasWinAppSdk) {
        if ([string]::IsNullOrWhiteSpace($WinAppSDKVersion)) {
            $winAppSdkNugetUrl = "https://www.nuget.org/packages/Microsoft.WindowsAppSDK/"
            Write-Warning "WinAppSDKVersion not supplied; cannot install Microsoft.WindowsAppSDK package automatically."
            Write-Warning "Visit $winAppSdkNugetUrl to determine the latest version, then rerun the script."
        }
        else {
            if (!(Test-Path $NuGetPackagesFolder)) { New-Item -ItemType Directory -Path $NuGetPackagesFolder | Out-Null }
            Write-Host "Installing Microsoft.WindowsAppSDK $WinAppSDKVersion into $NuGetPackagesFolder (running inside folder)"
            Push-Location $NuGetPackagesFolder
            try {
                & $nugetExe install Microsoft.WindowsAppSDK -Version $WinAppSDKVersion -OutputDirectory . -Prerelease -DependencyVersion Highest | Write-Host
            }
            finally {
                Pop-Location
            }
        }
    }
    else {
        Write-Host "Detected existing Microsoft.WindowsAppSDK package; skipping install."
    }
}

# Seed the package/version map with the WinAppSDK metapackage.
$nugetPackageToVersionTable = @{"Microsoft.WindowsAppSDK" = $WinAppSDKVersion }

# When a populated packages folder is available, harvest dependency versions from it.
if (!($NuGetPackagesFolder -eq "")) {
    Get-ChildItem $NuGetPackagesFolder | 
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
}

Get-ChildItem -Recurse packages.config -Path $PSScriptRoot | foreach-object {
    $content = Get-Content $_.FullName -Raw

    foreach ($nugetPackageToVersion in $nugetPackageToVersionTable.GetEnumerator()) {
        $newVersionString = 'package id="' + $nugetPackageToVersion.Key + '" version="' + $nugetPackageToVersion.Value + '"'
        $oldVersionString = 'package id="' + $nugetPackageToVersion.Key + '" version="[-.0-9a-zA-Z]*"'
        $content = $content -replace $oldVersionString, $newVersionString
    }

    Set-Content -Path $_.FullName -Value $content
    Write-Host "Modified " $_.FullName 
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

Get-ChildItem -Recurse *.vcxproj -Path $PSScriptRoot | foreach-object {
    $content = Get-Content $_.FullName -Raw

    foreach ($nugetPackageToVersion in $nugetPackageToVersionTable.GetEnumerator()) {
        $newVersionString = "\$($nugetPackageToVersion.Key)." + $nugetPackageToVersion.Value + '\'
        $oldVersionString = "\\$($nugetPackageToVersion.Key).[0-9][-.0-9a-zA-Z]*\\"
        $content = $content -replace $oldVersionString, $newVersionString
    }

    Set-Content -Path $_.FullName -Value $content
    Write-Host "Modified " $_.FullName 
}

Get-ChildItem -Recurse *.wapproj -Path $PSScriptRoot | foreach-object {
    $newVersionString = 'PackageReference Include="Microsoft.WindowsAppSDK" Version="' + $WinAppSDKVersion + '"'
    $oldVersionString = 'PackageReference Include="Microsoft.WindowsAppSDK" Version="[-.0-9a-zA-Z]*"'
    $content = Get-Content $_.FullName -Raw
    $content = $content -replace $oldVersionString, $newVersionString
    Set-Content -Path $_.FullName -Value $content
    Write-Host "Modified " $_.FullName 
}

Get-ChildItem -Recurse *.csproj -Path $PSScriptRoot | foreach-object {
    $newVersionString = 'PackageReference Include="Microsoft.WindowsAppSDK" Version="' + $WinAppSDKVersion + '"'
    $oldVersionString = 'PackageReference Include="Microsoft.WindowsAppSDK" Version="[-.0-9a-zA-Z]*"'
    $content = Get-Content $_.FullName -Raw
    $content = $content -replace $oldVersionString, $newVersionString
    Set-Content -Path $_.FullName -Value $content
    Write-Host "Modified " $_.FullName 
}
