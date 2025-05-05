Param(
    [string]$WinAppSDKVersion = "",
    [string]$NuGetPackagesFolder = ""
)

$nugetPackageToVersionTable = @{"Microsoft.WindowsAppSDK" = $WinAppSDKVersion}
if (!($NuGetPackagesFolder -eq ""))
{
    Get-ChildItem -Recurse $NuGetPackagesFolder | ForEach-Object { Write-Host $_.FullName }

    [xml]$publicNuspec = Get-Content -Encoding utf8 -Path (Join-Path $NuGetPackagesFolder "Microsoft.WindowsAppSDK\$WinAppSDKVersion\Microsoft.WindowsAppSDK.nuspec")
    foreach ($dependency in $publicNuspec.package.metadata.dependencies.dependency)
    {
        $nugetPackageToVersionTable[$dependency.id] = $dependency.version
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
        $newVersionString = "\$($nugetPackageToVersion.Key)." + $WinAppSDKVersion + '\'
        $oldVersionString = "\$($nugetPackageToVersion.Key).[0-9][-.0-9a-zA-Z]*\"
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