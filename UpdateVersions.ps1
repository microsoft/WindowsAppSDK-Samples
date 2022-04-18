Param(
    [string]$WinAppSDKVersion = "",
    [switch]$WinAppSDKVersionOld = "1.0.0"
)

Get-ChildItem packages.config -Path $PSScriptRoot | foreach-object {
    $newVersionString = 'version="' + $WinAppSDKVersion + '"'
    $oldVersionString = 'version="' + $WinAppSDKVersionOld + '"'
    $_.FullName.replace($oldVersionString, $newVersionString)
  }