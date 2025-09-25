Param(
    [Parameter(Mandatory=$true)]
    [string]$WinAppSDKVersion,
    [string]$NuGetPackagesFolder = ""
)

# Prerequisites
# If the NuGetPackagesFolder parameter wasn't provided, 
# Install the version of WinAppSDK in 'packages' folder first,
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

    # If the WinAppSDK package (any variant) isn't already present, install just that
    # package (and its dependencies) at the requested version directly.
    $hasWinAppSdk = Get-ChildItem -Path $NuGetPackagesFolder -ErrorAction SilentlyContinue | Where-Object { $_.Name -like "Microsoft.WindowsAppSDK.*" } | Select-Object -First 1
    if (-not $hasWinAppSdk) {
        if ([string]::IsNullOrWhiteSpace($WinAppSDKVersion)) {
            Write-Warning "WinAppSDKVersion not supplied; cannot install Microsoft.WindowsAppSDK package automatically."
        } else {
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
    } else {
        Write-Host "Detected existing Microsoft.WindowsAppSDK package; skipping install."
    }
}

# First, add the metapackage
$nugetPackageToVersionTable = @{"Microsoft.WindowsAppSDK" = $WinAppSDKVersion}

# Next, if the nuget packages folder is specified containing the latest versions,
# go through them to get the versions of all dependency packages.
if (!($NuGetPackagesFolder -eq ""))
{
    Get-ChildItem $NuGetPackagesFolder | 
    Where-Object { $_.Name -like "Microsoft.WindowsAppSDK.*" -or 
                   $_.Name -like "Microsoft.Windows.SDK.BuildTools.*" -or 
                   $_.Name -like "Microsoft.Web.WebView2.*" } | 
    Where-Object { $_.Name -notlike "*.nupkg" } |
    ForEach-Object { 
        if ($_.Name -match "^(Microsoft\.WindowsAppSDK\.[a-zA-Z]+)\.([0-9].*)$" -or
            $_.Name -match "^(Microsoft\.Windows\.SDK\.BuildTools\.MSIX)\.([0-9].*)$" -or
            $_.Name -match "^(Microsoft\.Windows\.SDK\.BuildTools)\.([0-9].*)$" -or
            $_.Name -match "^(Microsoft\.Web\.WebView2)\.([0-9].*)$")
        {
            $nugetPackageToVersionTable[$Matches[1]] = $Matches[2]
            Write-Host "Found $($Matches[1]) - $($Matches[2])"
        } 
    }
}

Get-ChildItem -Recurse packages.config -Path $PSScriptRoot | foreach-object {
    $content = Get-Content $_.FullName -Raw

    foreach ($nugetPackageToVersion in $nugetPackageToVersionTable.GetEnumerator())
    {
        $newVersionString = 'package id="' + $nugetPackageToVersion.Key + '" version="' + $nugetPackageToVersion.Value + '"'
        $oldVersionString = 'package id="' + $nugetPackageToVersion.Key + '" version="[-.0-9a-zA-Z]*"'
        $content = $content -replace $oldVersionString, $newVersionString
    }

    Set-Content -Path $_.FullName -Value $content
    Write-Host "Modified " $_.FullName 
}

Get-ChildItem -Recurse *.vcxproj -Path $PSScriptRoot | foreach-object {
    $content = Get-Content $_.FullName -Raw

    foreach ($nugetPackageToVersion in $nugetPackageToVersionTable.GetEnumerator())
    {
        $newVersionString = "\$($nugetPackageToVersion.Key)." + $nugetPackageToVersion.Value + '\'
        $oldVersionString = "\\$($nugetPackageToVersion.Key).[0-9][-.0-9a-zA-Z]*\\"
        $content = $content -replace $oldVersionString, $newVersionString
    }

    Set-Content -Path $_.FullName -Value $content
    Write-Host "Modified " $_.FullName 
}

Get-ChildItem -Recurse *.wapproj -Path $PSScriptRoot | foreach-object {
    $newVersionString = 'PackageReference Include="Microsoft.WindowsAppSDK" Version="'+ $WinAppSDKVersion + '"'
    $oldVersionString = 'PackageReference Include="Microsoft.WindowsAppSDK" Version="[-.0-9a-zA-Z]*"'
    $content = Get-Content $_.FullName -Raw
    $content = $content -replace $oldVersionString, $newVersionString
    Set-Content -Path $_.FullName -Value $content
    Write-Host "Modified " $_.FullName 
}

Get-ChildItem -Recurse *.csproj -Path $PSScriptRoot | foreach-object {
    $newVersionString = 'PackageReference Include="Microsoft.WindowsAppSDK" Version="'+ $WinAppSDKVersion + '"'
    $oldVersionString = 'PackageReference Include="Microsoft.WindowsAppSDK" Version="[-.0-9a-zA-Z]*"'
    $content = Get-Content $_.FullName -Raw
    $content = $content -replace $oldVersionString, $newVersionString
    Set-Content -Path $_.FullName -Value $content
    Write-Host "Modified " $_.FullName 
}
