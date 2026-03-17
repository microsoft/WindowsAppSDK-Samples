# UnpackagedFrameworkDependent

A native C++ WinAppSDK app that is **unpackaged** and **framework-dependent**, built with CMake and NuGetCMakePackage.

## What this app demonstrates

- Unpackaged desktop deployment (no MSIX, no package identity)
- Framework-dependent WinAppSDK deployment — requires the WinAppSDK runtime to be installed on the machine
- Standard dynamic CRT linking
- Automatic bootstrapper initialization via the Foundation_Framework target
- WinAppSDK EnvironmentManager API via C++/WinRT projections

## Configuration

- CMake target: `Microsoft.WindowsAppSDK.Foundation_Framework`
- CMake target property: `WindowsAppSdkBootstrapInitialize = TRUE`
- CRT: `MultiThreadedDLL` / `MultiThreadedDebugDLL` (standard dynamic CRT)
- No MSIX packaging, no AppxManifest

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

Run the executable directly:

```powershell
.\build\vs2022-x64\UnpackagedFrameworkDependent\Debug\UnpackagedFrameworkDependent.exe
```

**Expected result:** A message box showing "Unpackaged Framework-Dependent App (CMake)" with EnvironmentManager status and the `PROCESSOR_ARCHITECTURE` value.

## Prerequisites

- **WinAppSDK runtime** (`Microsoft.WindowsAppRuntime.2.0-preview1`) must be installed on the machine
- **Visual C++ Redistributable** must be installed (typically already present)

## Notes

- The bootstrapper (`MddBootstrapAutoInitializer.cpp`) is automatically compiled and linked by the Foundation_Framework target when `WindowsAppSdkBootstrapInitialize` is set to `TRUE`.
- The bootstrapper locates and loads the WinAppSDK Framework package dynamically at runtime.
- Unlike self-contained, the WinAppSDK binaries are NOT bundled — smaller deployment but requires the runtime installer on the target machine.
