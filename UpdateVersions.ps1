Param(
    [string]$WinAppSDKVersion = "",
    [string]$WinAppSDKVersionOld = "1.0.0",
    [string]$RootPath = ""
)

Write-Host "Updating"
Get-ChildItem -Recurse packages.config -File $PSScriptRoot | foreach-object {
    $newVersionString = 'version="' + $WinAppSDKVersion + '"'
    $oldVersionString = 'version="' + $WinAppSDKVersionOld + '"'
    $content = Get-Content $_ -Raw
    $content = $content.replace($oldVersionString, $newVersionString)
    Set-Content -Path $_ -Value $content
  }