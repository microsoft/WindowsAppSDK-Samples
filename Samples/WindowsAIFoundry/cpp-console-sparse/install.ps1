# In the current script directory, find the path to appxmanifest.xml and register it
$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Definition
$appxManifestPath = Join-Path $scriptDir "appxmanifest.xml" 
Add-AppxPackage -Path $appxManifestPath -Register -ForceUpdateFromAnyVersion
