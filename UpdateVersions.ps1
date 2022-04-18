Param(
    [string]$WinAppSDKVersion = "",
    [string]$WinAppSDKVersionOld = "1.0.0",
    [string]$RootPath = ""
)

Write-Host "Updating"
Get-ChildItem -Recurse packages.config -Path $PSScriptRoot | foreach-object {
    $newVersionString = 'version="' + $WinAppSDKVersion + '"'
    $oldVersionString = 'version="' + $WinAppSDKVersionOld + '"'
    $content = $_.FullName.replace($oldVersionString, $newVersionString)
    Set-Content -Path $_.FullName -Value $content
  }