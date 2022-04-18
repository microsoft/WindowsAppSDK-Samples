Param(
    [string]$WinAppSDKVersion = "",
    [string]$WinAppSDKVersionOld = "1.0.0",
    [string]$RootPath = ""
)

Get-ChildItem *packages.config -Path $RootPath | foreach-object {
    $newVersionString = 'version="' + $WinAppSDKVersion + '"'
    $oldVersionString = 'version="' + $WinAppSDKVersionOld + '"'
    $_.FullName.replace($oldVersionString, $newVersionString)
  }