# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License. See LICENSE.md in the repo root for license information.

# Create the model folder
$targetPath = Join-Path $PSScriptRoot 'Model'
if (-not (Test-Path $targetPath -PathType Container)) {
    New-Item -Path $targetPath -ItemType Directory | Out-Null
}

function Get-FileFromUri {
    param (
        [Parameter(Mandatory=$true)][string]$Uri,
        [Parameter(Mandatory=$true)][string]$OutFile,
        [Parameter(Mandatory=$true)][string]$ExpectedHash
    )
    
    $needsDownload = $true
    Write-Host "Downloading $OutFile from $Uri" -ForegroundColor Cyan
    
    if (Test-Path $OutFile -PathType Leaf) {
        Write-Host "File exists. Verifying checksum..." -ForegroundColor Yellow
        $hash = Get-FileHash -Path $OutFile -Algorithm SHA256
        if ($hash.Hash -eq $ExpectedHash) {
            Write-Host "File checksum matches. Using existing file." -ForegroundColor Green
            $needsDownload = $false
        } else {
            Write-Host "File checksum mismatch. Redownloading..." -ForegroundColor Yellow
        }
    }
    if ($needsDownload) {
        try {
            Write-Host "Downloading..." -ForegroundColor Yellow
            Invoke-WebRequest -Uri $Uri -OutFile $OutFile
            Write-Host "File downloaded successfully." -ForegroundColor Green
            return $true
        } catch {
            Write-Error "Failed to download file: $_"
            return $false
        }
    }
    return $true
}

# Download the model file
$modelPath = Join-Path $targetPath 'SqueezeNet.onnx'
$modelUri = 'https://github.com/microsoft/Windows-Machine-Learning/blob/02b586811c8beb1ae2208c8605393267051257ae/SharedContent/models/SqueezeNet.onnx?raw=true'
Get-FileFromUri -Uri $modelUri -OutFile $modelPath -ExpectedHash "D95E2191E056F1912A9B8F6000DA3B9C7818441B9EB48137033C2ADBF6398BC8" | Out-Null

# Download the labels file
$labelsPath = Join-Path $targetPath 'SqueezeNet.Labels.txt'
$labelsUri = 'https://github.com/microsoft/Windows-Machine-Learning/blob/02b586811c8beb1ae2208c8605393267051257ae/Samples/SqueezeNetObjectDetection/Desktop/cpp/Labels.txt?raw=true'
Get-FileFromUri -Uri $labelsUri -OutFile $labelsPath -ExpectedHash "DC1FD0D4747096D3AA690BD65EC2F51FDB3E5117535BFBCE46FA91088A8F93A9" | Out-Null
