# PackagedSelfContained

A native C++ WinAppSDK app that is **packaged** (MSIX) and **self-contained**, built with CMake and NuGetCMakePackage.

## What this app demonstrates

- MSIX-packaged deployment with package identity
- Self-contained WinAppSDK deployment — all WinAppSDK DLLs are included in the MSIX layout
- Hybrid CRT (static VC runtime + dynamic UCRT)
- WinAppSDK EnvironmentManager API via C++/WinRT projections
- Manual AppxManifest for CMake-built packaged apps

## Configuration

- CMake target: `Microsoft.WindowsAppSDK.Foundation_SelfContained`
- CRT: `MultiThreaded` / `MultiThreadedDebug` (Hybrid CRT)
- MSIX packaging via manual `AppxManifest.xml` copied to build output

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

This app requires MSIX registration to get package identity:

```powershell
# Step 1: Register the MSIX package layout
Add-AppxPackage -Register ".\build\vs2022-x64\PackagedSelfContained\Debug\AppxManifest.xml"

# Step 2: Launch via shell activation (provides package identity)
$pfn = (Get-AppxPackage *CMake-PackagedSelfContained*).PackageFamilyName
Start-Process "shell:AppsFolder\${pfn}!App"

# Step 3: Cleanup when done
Get-AppxPackage *CMake-PackagedSelfContained* | Remove-AppxPackage
```

**Expected result:** A message box showing "Packaged Self-Contained App (CMake)" with the full package identity string, EnvironmentManager status, and the `PROCESSOR_ARCHITECTURE` value.

> **Note:** The app must be launched through the MSIX activation mechanism to have package identity. In Visual Studio, use F5 with the packaging project as startup.

## Notes

- Since this is self-contained, no WinAppSDK runtime installer is needed on the target machine.
- The `AppxManifest.xml` and `Logo.png` are copied to the build output by a post-build command.
- The app does not declare a framework package dependency in the manifest since all dependencies are self-contained.
