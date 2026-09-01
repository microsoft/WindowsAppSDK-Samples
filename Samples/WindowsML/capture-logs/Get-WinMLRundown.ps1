# Copyright (C) Microsoft Corporation. All rights reserved.
<#
.SYNOPSIS
    Converts a Windows ML ETL trace into a readable rundown log.

.DESCRIPTION
    Uses Windows Performance Analyzer Exporter and the supplied WPA profile to
    export Windows ML events, group them by process, and write
    WinmlRundown.log. Windows Performance Toolkit must already be installed.

.PARAMETER EtlFilePath
    Path to the ETL file captured with the Windows ML WPR profile.

.PARAMETER WpaProfilePath
    Path to WindowsMLProfile.wpaProfile.

.PARAMETER OutputFolder
    Folder in which to create WinmlRundown.log. Temporary CSV files are
    isolated in a unique subfolder and removed after processing.

.PARAMETER WpaExporterPath
    Optional path to wpaexporter.exe. By default, the script searches PATH and
    the standard Windows Performance Toolkit installation folders.

.EXAMPLE
    .\Get-WinMLRundown.ps1 -EtlFilePath '.\winml_trace.etl' `
        -WpaProfilePath '.\WindowsMLProfile.wpaProfile' `
        -OutputFolder '.\rundown_output'
#>

[CmdletBinding()]
param (
    [Parameter(Mandatory = $true)]
    [string]$EtlFilePath,

    [Parameter(Mandatory = $true)]
    [string]$WpaProfilePath,

    [Parameter(Mandatory = $true)]
    [string]$OutputFolder,

    [string]$WpaExporterPath
)

Set-StrictMode -Version 2.0
$ErrorActionPreference = 'Stop'

function Get-WpaExporterPath {
    $command = Get-Command 'wpaexporter.exe' -ErrorAction SilentlyContinue
    if ($command) {
        return $command.Source
    }

    $candidatePaths = @(
        (Join-Path $env:ProgramFiles `
            'Windows Kits\10\Windows Performance Toolkit\wpaexporter.exe'),
        (Join-Path ${env:ProgramFiles(x86)} `
            'Windows Kits\10\Windows Performance Toolkit\wpaexporter.exe')
    )

    foreach ($candidatePath in $candidatePaths) {
        if (Test-Path -LiteralPath $candidatePath -PathType Leaf) {
            return $candidatePath
        }
    }

    throw @'
Windows Performance Analyzer Exporter was not found. Install Windows
Performance Toolkit before running this script. For instructions, see:
https://learn.microsoft.com/windows/ai/new-windows-ml/logs
'@
}

if (-not (Test-Path -LiteralPath $EtlFilePath -PathType Leaf)) {
    throw "ETL file not found: $EtlFilePath"
}

if (-not (Test-Path -LiteralPath $WpaProfilePath -PathType Leaf)) {
    throw "WPA profile not found: $WpaProfilePath"
}

$resolvedEtlFilePath = (Resolve-Path -LiteralPath $EtlFilePath).Path
$resolvedWpaProfilePath = (Resolve-Path -LiteralPath $WpaProfilePath).Path
$resolvedOutputFolder = (
    New-Item -ItemType Directory -Path $OutputFolder -Force
).FullName
$logFilePath = Join-Path $resolvedOutputFolder 'WinmlRundown.log'
$temporaryOutputFolder = Join-Path $resolvedOutputFolder (
    '.winml-rundown-{0}' -f [guid]::NewGuid().ToString('N')
)
$null = New-Item -ItemType Directory -Path $temporaryOutputFolder

try {
    if ($WpaExporterPath) {
        if (-not (Test-Path -LiteralPath $WpaExporterPath -PathType Leaf)) {
            throw "WPA Exporter not found: $WpaExporterPath"
        }

        $wpaExporterPath = (Resolve-Path `
            -LiteralPath $WpaExporterPath).Path
    }
    else {
        $wpaExporterPath = Get-WpaExporterPath
    }

    Write-Host "Exporting ETL data with $wpaExporterPath..."
    & $wpaExporterPath -i $resolvedEtlFilePath `
        -profile $resolvedWpaProfilePath `
        -outputFolder $temporaryOutputFolder

    if ($LASTEXITCODE -ne 0) {
        throw "wpaexporter.exe failed with exit code $LASTEXITCODE."
    }

    $csvFiles = @(Get-ChildItem -LiteralPath $temporaryOutputFolder `
        -Filter '*.csv' -File)
    if ($csvFiles.Count -eq 0) {
        throw 'wpaexporter.exe did not produce any CSV files.'
    }

    $allData = @(foreach ($file in $csvFiles) {
        $prefix = ''
        $columnMap = @{}

        if ($file.Name -like '*onnxversion*') {
            $prefix = 'Onnx Version'
            $columnMap = @{
                'Field 4' = 'Runtime Version'
                'Field 6' = 'Is Redist'
                'Field 7' = 'Framework Name'
            }
        }
        elseif ($file.Name -like '*driverinfo*') {
            $prefix = 'Driver Info'
            $columnMap = @{
                'Field 4' = 'Device Class'
                'Field 5' = 'Driver Name'
                'Field 6' = 'Driver Version'
            }
        }
        elseif ($file.Name -like '*registeredproviders*') {
            $prefix = 'Registered Providers'
            $columnMap = @{
                'Field 1' = 'PackageFamilyName'
            }
        }
        elseif ($file.Name -like '*sessioncreation*') {
            $prefix = 'Session Creation'
            $columnMap = @{
                'Field 3' = 'Schema Version'
                'Field 4' = 'Session ID'
                'Field 5' = 'IR Version'
                'Field 6' = 'ORT Programming Projection'
                'Field 10' = 'Using FP16'
                'Field 14' = 'Model Weight Type'
                'Field 15' = 'Model Graph Hash'
                'Field 16' = 'Model Weight Hash'
                'Field 19' = 'EP ID'
            }
        }
        elseif ($file.Name -like '*epautoselection*') {
            $prefix = 'EP Auto Selection'
            $columnMap = @{
                'Field 3' = 'Schema Version'
                'Field 4' = 'Session ID'
                'Field 5' = 'Selection Policy'
                'Field 6' = 'Requested EP'
                'Field 7' = 'Available EP'
            }
        }
        elseif ($file.Name -like '*winmlonnxerror*') {
            $prefix = '====================WINML ONNX ERROR==================='
            $columnMap = @{
                'Field 3' = 'Schema Version'
                'Field 4' = 'HRESULT'
                'Field 5' = 'Session ID'
                'Field 6' = 'Error Code'
                'Field 7' = 'Error Category'
                'Field 8' = 'Error Message'
                'Field 9' = 'File'
                'Field 10' = 'Function'
                'Field 11' = 'Line'
            }
        }
        elseif ($file.Name -like '*onnxerror*') {
            $prefix = '====================ONNX ERROR==================='
            $columnMap = @{
                'Field 3' = 'Location'
                'Field 4' = 'Message'
            }
        }
        elseif ($file.Name -like '*winmlversion*') {
            $prefix = 'WindowsAppSDK.ML Version'
            $columnMap = @{
                'Field 5' = 'Version'
            }
        }
        elseif ($file.Name -like '*providercertification*') {
            $prefix = 'Provider Certification'
            $columnMap = @{
                'Field 1' = 'Provider'
                'Field 2' = 'Ready State'
                'Field 3' = 'Certification State'
            }
        }

        Import-Csv -LiteralPath $file.FullName | ForEach-Object {
            foreach ($key in $columnMap.Keys) {
                if ($_.PSObject.Properties.Name -contains $key) {
                    $_ | Add-Member `
                        -NotePropertyName $columnMap[$key] `
                        -NotePropertyValue $_.$key
                    $_.PSObject.Properties.Remove($key)
                }
            }

            $_ | Add-Member -NotePropertyName 'Prefix' `
                -NotePropertyValue $prefix
            $_
        }
    })

    if ($allData.Count -eq 0) {
        throw 'The exported CSV files did not contain any data.'
    }

    if ($allData[0].PSObject.Properties.Name -notcontains 'Process') {
        throw "The exported data does not contain a 'Process' column."
    }

    $groupedData = $allData | Group-Object -Property 'Process'
    Set-Content -LiteralPath $logFilePath -Value $null

    foreach ($group in $groupedData) {
        Add-Content -LiteralPath $logFilePath `
            -Value "Process: $($group.Name)"
        Add-Content -LiteralPath $logFilePath `
            -Value '----------------------------------------'

        $sortedItems = $group.Group
        if ($group.Group[0].PSObject.Properties.Name -contains 'Time (Local)') {
            $sortedItems = $group.Group | Sort-Object 'Time (Local)'
        }

        $emittedWasdkVersion = $null
        foreach ($item in $sortedItems) {
            $timeValue = ''
            if (
                $item.PSObject.Properties.Name -contains 'Time (Local)' -and
                $item.'Time (Local)'
            ) {
                $timeValue = " ($($item.'Time (Local)'))"
            }

            $properties = @(
                $item.PSObject.Properties |
                    Where-Object {
                        $_.Name -notin @('Process', 'Prefix', 'Time (Local)')
                    } |
                    Where-Object {
                        $_.Value -and $_.Value.ToString().Trim() -ne ''
                    }
            )

            if ($properties.Count -eq 0) {
                continue
            }

            if ($item.Prefix -eq 'WindowsAppSDK.ML Version') {
                $versionProperty = $properties |
                    Where-Object Name -eq 'Version' |
                    Select-Object -First 1
                $version = $versionProperty.Value
                if ($version -eq $emittedWasdkVersion) {
                    continue
                }

                $emittedWasdkVersion = $version
            }

            Add-Content -LiteralPath $logFilePath `
                -Value "`t$($item.Prefix)$timeValue :"
            foreach ($property in $properties) {
                Add-Content -LiteralPath $logFilePath `
                    -Value "`t`t$($property.Name) = $($property.Value)"
            }
            Add-Content -LiteralPath $logFilePath -Value ''
        }

        Add-Content -LiteralPath $logFilePath -Value ''
    }

    Write-Host "Log file created successfully: $logFilePath"
}
finally {
    if (Test-Path -LiteralPath $temporaryOutputFolder) {
        Remove-Item -LiteralPath $temporaryOutputFolder -Recurse -Force
    }
}
