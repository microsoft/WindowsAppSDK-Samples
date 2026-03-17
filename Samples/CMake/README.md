# CMake Baseline Tests

WinAppSDK deployment matrix baseline tests built with **CMake** and **NuGetCMakePackage**.

## Deployment Matrix

|                    | **Packaged (MSIX)**         | **Unpackaged**                |
|--------------------|----------------------------|-------------------------------|
| **Self-Contained** | PackagedSelfContained       | UnpackagedSelfContained       |
| **Framework**      | PackagedFrameworkDependent   | UnpackagedFrameworkDependent   |

Each app uses the WinAppSDK `EnvironmentManager` API to demonstrate SDK functionality.

## Prerequisites

- CMake 3.31+
- Visual Studio 2022 (with C++ desktop workload)
- WinAppSDK runtime (`Microsoft.WindowsAppRuntime.2.0-preview1`) installed for framework-dependent apps

## Build

### Visual Studio generator (recommended)

```powershell
cmake --preset vs2022-x64
cmake --build --preset vs2022-x64-debug
```

### Ninja generator (from VS Developer Command Prompt)

```powershell
# Step 1: Open x64 Native Tools Command Prompt, or set up the environment:
& "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat"

# Step 2: Configure
cmake --preset ninja-x64

# Step 3: Build Debug
cmake --build --preset ninja-x64-debug

# Or build Release
cmake --build --preset ninja-x64-release
```

> **Note:** The Ninja presets use a short build directory (`C:\b\nx64`) to avoid Windows path length issues with the auto-generated source files from NuGetCMakePackage. This is a known limitation when NuGet packages are in deep user-profile paths.

Alternatively, run configure and build in a single command from any shell:

```powershell
cmd /c "`"C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat`" && cmake --preset ninja-x64 && cmake --build --preset ninja-x64-debug"
```

Output executables will be in `C:\b\nx64\<AppName>\Debug\` (or `Release\`).

## Run

### Run all tests (recommended)

```powershell
.\Test-AllApps.ps1
.\Test-AllApps.ps1 -Configuration Release
```

This script builds, registers, launches, and verifies all 4 apps automatically. Packaged apps are verified via log files for package identity.

### Unpackaged apps (direct launch)

```powershell
.\build\vs2022-x64\UnpackagedSelfContained\Debug\UnpackagedSelfContained.exe
.\build\vs2022-x64\UnpackagedFrameworkDependent\Debug\UnpackagedFrameworkDependent.exe
```

### Packaged apps (use launch script)

```powershell
.\Launch-PackagedApp.ps1 -AppxManifestPath .\build\vs2022-x64\PackagedSelfContained\Debug\AppxManifest.xml -PackageNameFilter *CMake-PackagedSelfContained*
.\Launch-PackagedApp.ps1 -AppxManifestPath .\build\vs2022-x64\PackagedFrameworkDependent\Debug\AppxManifest.xml -PackageNameFilter *CMake-PackagedFrameworkDependent*

# Cleanup when done
Get-AppxPackage *CMake-Packaged* | Remove-AppxPackage
```

### Verify package identity (check log files)

```powershell
Get-Content $env:TEMP\CMake_PackagedSC.log
Get-Content $env:TEMP\CMake_PackagedFW.log
```

## Project Structure

```
CMakeBaselineTests/
├── CMakeLists.txt              # Root — FetchContent + add_nuget_packages
├── CMakePresets.json           # VS2022 and Ninja presets
├── Configuration.cmake         # Shared compiler settings
├── nuget.config                # Points to ../../LocalNugetCache
├── Launch-PackagedApp.ps1      # Script to register + COM-activate packaged apps
├── Test-AllApps.ps1            # Script to build, launch, and verify all 4 apps
├── UnpackagedSelfContained/
│   ├── CMakeLists.txt          # Links Foundation_SelfContained, HybridCRT
│   ├── main.cpp
│   └── README.md
├── UnpackagedFrameworkDependent/
│   ├── CMakeLists.txt          # Links Foundation_Framework + bootstrapper
│   ├── main.cpp
│   └── README.md
├── PackagedSelfContained/
│   ├── CMakeLists.txt          # Links Foundation_SelfContained, HybridCRT
│   ├── main.cpp
│   ├── AppxManifest.xml
│   ├── Logo.png
│   └── README.md
└── PackagedFrameworkDependent/
    ├── CMakeLists.txt          # Links Foundation_Framework
    ├── main.cpp
    ├── AppxManifest.xml        # Declares framework package dependency
    ├── Logo.png
    └── README.md
```
