Param(
    [string]$WinAppSDKVersion = "",
    [string]$NuGetPackagesFolder = ""
)

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
