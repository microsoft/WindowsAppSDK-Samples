# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

<#
.SYNOPSIS
    Builds, registers, launches, and verifies all 4 CMake baseline test apps.
.DESCRIPTION
    Runs the full test cycle: cmake build, launch each app, verify output via log files.
    Requires cmake --preset vs2022-x64 to have been run first (configure step).
.EXAMPLE
    .\Test-AllApps.ps1
    .\Test-AllApps.ps1 -Configuration Release
#>

param(
    [string]$Configuration = "Debug"
)

Set-StrictMode -Version 2.0
$ErrorActionPreference = 'Stop'

$buildDir = Join-Path $PSScriptRoot "build\vs2022-x64"

# COM activation type for packaged apps
Add-Type -TypeDefinition @"
using System;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;

[ComImport, Guid("2e941141-7f97-4756-ba1d-9decde894a3d"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
public interface IAppActivationTest {
    IntPtr ActivateApplication([In] String appUserModelId, [In] String arguments, [In] UInt32 options, [Out] out UInt32 processId);
}

[ComImport, Guid("45BA127D-10A8-46EA-8AB7-56EA9078943C")]
public class AppActivationTest : IAppActivationTest {
    [MethodImpl(MethodImplOptions.InternalCall, MethodCodeType = MethodCodeType.Runtime)]
    public extern IntPtr ActivateApplication([In] String appUserModelId, [In] String arguments, [In] UInt32 options, [Out] out UInt32 processId);
}
"@ -ErrorAction SilentlyContinue

# Build
Write-Host "=== Building $Configuration ==="
$presetName = "vs2022-x64-$($Configuration.ToLower())"
cmake --build --preset $presetName
if ($LASTEXITCODE -ne 0) { Write-Error "Build failed"; exit 1 }

$results = @()

# --- Test 1: UnpackagedSelfContained ---
Write-Host ""
Write-Host "=== Test: UnpackagedSelfContained ==="
$exe = Join-Path $buildDir "UnpackagedSelfContained\$Configuration\UnpackagedSelfContained.exe"
$p = Start-Process -FilePath $exe -PassThru
Start-Sleep -Seconds 4
$p.Refresh()
if ($p.HasExited) {
    $results += [PSCustomObject]@{ App = "UnpackagedSelfContained"; Status = "FAIL"; Detail = "Exited with $($p.ExitCode)" }
} else {
    $results += [PSCustomObject]@{ App = "UnpackagedSelfContained"; Status = "PASS"; Detail = "Running" }
    [System.Diagnostics.Process]::GetProcessById($p.Id).Kill()
}

# --- Test 2: UnpackagedFrameworkDependent ---
Write-Host ""
Write-Host "=== Test: UnpackagedFrameworkDependent ==="
$exe = Join-Path $buildDir "UnpackagedFrameworkDependent\$Configuration\UnpackagedFrameworkDependent.exe"
$p = Start-Process -FilePath $exe -PassThru
Start-Sleep -Seconds 4
$p.Refresh()
if ($p.HasExited) {
    $results += [PSCustomObject]@{ App = "UnpackagedFrameworkDependent"; Status = "FAIL"; Detail = "Exited with $($p.ExitCode)" }
} else {
    $results += [PSCustomObject]@{ App = "UnpackagedFrameworkDependent"; Status = "PASS"; Detail = "Running" }
    [System.Diagnostics.Process]::GetProcessById($p.Id).Kill()
}

Start-Sleep -Seconds 2

# --- Test 3: PackagedSelfContained ---
Write-Host ""
Write-Host "=== Test: PackagedSelfContained ==="
$manifest = Join-Path $buildDir "PackagedSelfContained\$Configuration\AppxManifest.xml"
$logFile = Join-Path $env:TEMP "CMake_PackagedSC.log"
Remove-Item $logFile -Force -ErrorAction SilentlyContinue

Get-AppxPackage *CMake-PackagedSelfContained* | Remove-AppxPackage -ErrorAction SilentlyContinue
Start-Sleep -Seconds 3
Add-AppxPackage -Register $manifest

$pfn = (Get-AppxPackage *CMake-PackagedSelfContained*).PackageFamilyName
$aam = New-Object AppActivationTest
$pid_out = [uint32]0
$aam.ActivateApplication("${pfn}!App", $null, 0, [ref]$pid_out) | Out-Null
Start-Sleep -Seconds 5
Get-Process -Id $pid_out -ErrorAction SilentlyContinue | ForEach-Object { [System.Diagnostics.Process]::GetProcessById($_.Id).Kill() }
Start-Sleep -Seconds 1

if ((Test-Path $logFile) -and (Get-Content $logFile -Raw) -match "Result=SUCCESS") {
    $identity = (Get-Content $logFile | Select-String "PackageIdentity=(.+)").Matches.Groups[1].Value
    $results += [PSCustomObject]@{ App = "PackagedSelfContained"; Status = "PASS"; Detail = $identity }
} else {
    $results += [PSCustomObject]@{ App = "PackagedSelfContained"; Status = "FAIL"; Detail = if (Test-Path $logFile) { Get-Content $logFile -Raw } else { "No log" } }
}
Get-AppxPackage *CMake-PackagedSelfContained* | Remove-AppxPackage -ErrorAction SilentlyContinue

# --- Test 4: PackagedFrameworkDependent ---
Write-Host ""
Write-Host "=== Test: PackagedFrameworkDependent ==="
$manifest = Join-Path $buildDir "PackagedFrameworkDependent\$Configuration\AppxManifest.xml"
$logFile = Join-Path $env:TEMP "CMake_PackagedFW.log"
Remove-Item $logFile -Force -ErrorAction SilentlyContinue

Get-AppxPackage *CMake-PackagedFrameworkDependent* | Remove-AppxPackage -ErrorAction SilentlyContinue
Start-Sleep -Seconds 3
Add-AppxPackage -Register $manifest

$pfn = (Get-AppxPackage *CMake-PackagedFrameworkDependent*).PackageFamilyName
$pid_out = [uint32]0
$aam.ActivateApplication("${pfn}!App", $null, 0, [ref]$pid_out) | Out-Null
Start-Sleep -Seconds 5
Get-Process -Id $pid_out -ErrorAction SilentlyContinue | ForEach-Object { [System.Diagnostics.Process]::GetProcessById($_.Id).Kill() }
Start-Sleep -Seconds 1

if ((Test-Path $logFile) -and (Get-Content $logFile -Raw) -match "Result=SUCCESS") {
    $identity = (Get-Content $logFile | Select-String "PackageIdentity=(.+)").Matches.Groups[1].Value
    $results += [PSCustomObject]@{ App = "PackagedFrameworkDependent"; Status = "PASS"; Detail = $identity }
} else {
    $results += [PSCustomObject]@{ App = "PackagedFrameworkDependent"; Status = "FAIL"; Detail = if (Test-Path $logFile) { Get-Content $logFile -Raw } else { "No log" } }
}
Get-AppxPackage *CMake-PackagedFrameworkDependent* | Remove-AppxPackage -ErrorAction SilentlyContinue

# Summary
Write-Host ""
Write-Host "=== RESULTS ==="
$results | Format-Table -AutoSize

$failed = ($results | Where-Object { $_.Status -eq "FAIL" }).Count
if ($failed -gt 0) {
    Write-Host "$failed test(s) FAILED" -ForegroundColor Red
    exit 1
} else {
    Write-Host "All tests PASSED" -ForegroundColor Green
    exit 0
}
