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
        [Parameter(Mandatory=$true)][string]$ExpectedMD5
    )
    
    $needsDownload = $true
    Write-Host "Downloading $OutFile from $Uri" -ForegroundColor Cyan
    
    if (Test-Path $OutFile -PathType Leaf) {
        Write-Host "File exists. Verifying MD5 checksum..." -ForegroundColor Yellow
        $md5 = Get-FileHash -Path $OutFile -Algorithm MD5
        if ($md5.Hash -eq $ExpectedMD5) {
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
Get-FileFromUri -Uri $modelUri -OutFile $modelPath -ExpectedMD5 "3EA0D4AECA899A4F99216FC4BEBD9D0D" | Out-Null

# Download the labels file
$labelsPath = Join-Path $targetPath 'SqueezeNet.Labels.txt'
$labelsUri = 'https://github.com/microsoft/Windows-Machine-Learning/blob/02b586811c8beb1ae2208c8605393267051257ae/Samples/SqueezeNetObjectDetection/Desktop/cpp/Labels.txt?raw=true'
Get-FileFromUri -Uri $labelsUri -OutFile $labelsPath -ExpectedMD5 "7317EA720B83CB3CADB75AD91419F6A8" | Out-Null
