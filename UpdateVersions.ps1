Param(
    [string]$WinAppSDKVersion = "",
    [string]$WinAppSDKVersionOld = "1.0.0"
)

Get-ChildItem -Recurse packages.config -Path $PSScriptRoot | foreach-object {
    $newVersionString = 'package id="Microsoft.WindowsAppSDK" version="' + $WinAppSDKVersion + '"'
    $oldVersionString = 'package id="Microsoft.WindowsAppSDK" version="' + $WinAppSDKVersionOld + '"'
    $content = Get-Content $_.FullName -Raw
    $content = $content.replace($oldVersionString, $newVersionString)
    Set-Content -Path $_.FullName -Value $content
    Write-Host "Modified " $_.FullName 
}

Get-ChildItem -Recurse *.vcxproj -Path $PSScriptRoot | foreach-object {
    $newVersionString = 'packages\Microsoft.WindowsAppSDK.' + $WinAppSDKVersion
    $oldVersionString = 'packages\Microsoft.WindowsAppSDK.' + $WinAppSDKVersionOld
    $content = Get-Content $_.FullName -Raw
    $content = $content.replace($oldVersionString, $newVersionString)
    Set-Content -Path $_.FullName -Value $content
    Write-Host "Modified " $_.FullName 
}

Get-ChildItem -Recurse *.wapproj -Path $PSScriptRoot | foreach-object {
    $newVersionString = 'PackageReference Include="Microsoft.WindowsAppSDK" Version="'+ $WinAppSDKVersion + '"'
    $oldVersionString = 'PackageReference Include="Microsoft.WindowsAppSDK" Version="'+ $WinAppSDKVersionOld + '"'
    $content = Get-Content $_.FullName -Raw
    $content = $content.replace($oldVersionString, $newVersionString)
    Set-Content -Path $_.FullName -Value $content
    Write-Host "Modified " $_.FullName 
}

Get-ChildItem -Recurse *.csproj -Path $PSScriptRoot | foreach-object {
    $newVersionString = 'PackageReference Include="Microsoft.WindowsAppSDK" Version="'+ $WinAppSDKVersion + '"'
    $oldVersionString = 'PackageReference Include="Microsoft.WindowsAppSDK" Version="'+ $WinAppSDKVersionOld + '"'
    $content = Get-Content $_.FullName -Raw
    $content = $content.replace($oldVersionString, $newVersionString)
    Set-Content -Path $_.FullName -Value $content
    Write-Host "Modified " $_.FullName 
}