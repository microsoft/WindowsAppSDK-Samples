# PackagedFrameworkDependent

A native C++ WinAppSDK app that is **packaged** (MSIX) and **framework-dependent**, built with CMake and NuGetCMakePackage.

## What this app demonstrates

- MSIX-packaged deployment with package identity
- Framework-dependent WinAppSDK deployment — the WinAppSDK Framework package is resolved via the MSIX package graph
- Standard dynamic CRT linking
- WinAppSDK EnvironmentManager API via C++/WinRT projections
- Manual AppxManifest with explicit `PackageDependency` on the WinAppSDK framework

## Configuration

- CMake target: `Microsoft.WindowsAppSDK.Foundation_Framework`
- CRT: `MultiThreadedDLL` / `MultiThreadedDebugDLL` (standard dynamic CRT)
- MSIX packaging via manual `AppxManifest.xml` with `PackageDependency` on `Microsoft.WindowsAppRuntime.2.0-preview1`

## Build

From the `CMakeBaselineTests/` directory:

```powershell
# Configure (one-time)
cmake --preset vs2022-x64

# Build
cmake --build --preset vs2022-x64-debug
```

Or for Release:

```powershell
cmake --build --preset vs2022-x64-release
```

## Launch

This app requires MSIX registration to get package identity and resolve the framework package:

```powershell
# Step 1: Register the MSIX package layout
Add-AppxPackage -Register ".\build\vs2022-x64\PackagedFrameworkDependent\Debug\AppxManifest.xml"

# Step 2: Launch via shell activation (provides package identity + framework resolution)
$pfn = (Get-AppxPackage *CMake-PackagedFrameworkDependent*).PackageFamilyName
Start-Process "shell:AppsFolder\${pfn}!App"

# Step 3: Cleanup when done
Get-AppxPackage *CMake-PackagedFrameworkDependent* | Remove-AppxPackage
```

**Expected result:** A message box showing "Packaged Framework-Dependent App (CMake)" with the full package identity string, EnvironmentManager status, and the `PROCESSOR_ARCHITECTURE` value.

> **Note:** The app must be launched through the MSIX activation mechanism to have package identity. In Visual Studio, use F5 with the packaging project as startup.

## Prerequisites

- **WinAppSDK runtime** (`Microsoft.WindowsAppRuntime.2.0-preview1`) must be installed on the machine

## Notes

- The `AppxManifest.xml` declares a `PackageDependency` on `Microsoft.WindowsAppRuntime.2.0-preview1` which causes Windows to resolve and load the framework package when the MSIX is activated.
- Unlike self-contained, the app has a smaller footprint since WinAppSDK binaries come from the shared framework package.
- No bootstrapper is used — the MSIX package graph handles framework resolution.
