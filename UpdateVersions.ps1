Param(
    [string]$WinAppSDKVersion = "",
    [string]$WinAppSDKVersionOld = "1.0.0",
    [string]$RootPath = ""
)

Get-ChildItem -Recurse packages.config -Path $PSScriptRoot | foreach-object {
    $newVersionString = 'version="' + $WinAppSDKVersion + '"'
    $oldVersionString = 'version="' + $WinAppSDKVersionOld + '"'
    $content = Get-Content $_.FullName -Raw
    $content = $content.replace($oldVersionString, $newVersionString)
    Set-Content -Path $_.FullName -Value $content
}

Get-ChildItem -Recurse *.vcxproj -Path $PSScriptRoot | foreach-object {
    $newVersionString = 'Microsoft.WindowsAppSDK.' + $WinAppSDKVersion
    $oldVersionString = 'Microsoft.WindowsAppSDK.' + $WinAppSDKVersionOld
    $content = Get-Content $_.FullName -Raw
    $content = $content.replace($oldVersionString, $newVersionString)
    Set-Content -Path $_.FullName -Value $content
}

Get-ChildItem -Recurse *.wapproj -Path $PSScriptRoot | foreach-object {
    $newVersionString = 'PackageReference Include="Microsoft.WindowsAppSDK" Version="'+ $WinAppSDKVersion + '"'
    $oldVersionString = 'PackageReference Include="Microsoft.WindowsAppSDK" Version="'+ $WinAppSDKVersionOld + '"'
    $content = Get-Content $_.FullName -Raw
    $content = $content.replace($oldVersionString, $newVersionString)
    Set-Content -Path $_.FullName -Value $content
}

Get-ChildItem -Recurse *.csproj -Path $PSScriptRoot | foreach-object {
    $newVersionString = 'PackageReference Include="Microsoft.WindowsAppSDK" Version="'+ $WinAppSDKVersion + '"'
    $oldVersionString = 'PackageReference Include="Microsoft.WindowsAppSDK" Version="'+ $WinAppSDKVersionOld + '"'
    $content = Get-Content $_.FullName -Raw
    $content = $content.replace($oldVersionString, $newVersionString)
    Set-Content -Path $_.FullName -Value $content
}