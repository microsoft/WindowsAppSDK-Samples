# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

<#
.SYNOPSIS
    Registers and launches a packaged MSIX app with package identity.
.PARAMETER AppxManifestPath
    Path to the AppxManifest.xml in the build output directory.
.PARAMETER PackageNameFilter
    Wildcard filter to find the registered package (e.g., *CMake-PackagedSelfContained*).
.EXAMPLE
    .\Launch-PackagedApp.ps1 -AppxManifestPath .\build\vs2022-x64\PackagedSelfContained\Debug\AppxManifest.xml -PackageNameFilter *CMake-PackagedSelfContained*
#>

param(
    [Parameter(Mandatory)]
    [string]$AppxManifestPath,

    [Parameter(Mandatory)]
    [string]$PackageNameFilter
)

Set-StrictMode -Version 2.0
$ErrorActionPreference = 'Stop'

# Step 1: Register
Write-Host "Registering MSIX layout: $AppxManifestPath"
Add-AppxPackage -Register $AppxManifestPath

# Step 2: Activate via COM IApplicationActivationManager (provides package identity)
Add-Type -TypeDefinition @"
using System;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;

[ComImport, Guid("2e941141-7f97-4756-ba1d-9decde894a3d"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
public interface IAppActivation {
    IntPtr ActivateApplication([In] String appUserModelId, [In] String arguments, [In] UInt32 options, [Out] out UInt32 processId);
}

[ComImport, Guid("45BA127D-10A8-46EA-8AB7-56EA9078943C")]
public class AppActivation : IAppActivation {
    [MethodImpl(MethodImplOptions.InternalCall, MethodCodeType = MethodCodeType.Runtime)]
    public extern IntPtr ActivateApplication([In] String appUserModelId, [In] String arguments, [In] UInt32 options, [Out] out UInt32 processId);
}
"@ -ErrorAction SilentlyContinue

$pkg = Get-AppxPackage | Where-Object { $_.Name -like $PackageNameFilter }
if (-not $pkg) {
    Write-Error "Package not found matching '$PackageNameFilter'"
    exit 1
}

$aumid = "$($pkg.PackageFamilyName)!App"
Write-Host "Launching: $aumid"

$aam = New-Object AppActivation
$processId = [uint32]0
$aam.ActivateApplication($aumid, $null, 0, [ref]$processId) | Out-Null

Write-Host "App launched with PID: $processId"
Write-Host ""
Write-Host "To unregister when done:"
Write-Host "  Get-AppxPackage $PackageNameFilter | Remove-AppxPackage"

exit 0
