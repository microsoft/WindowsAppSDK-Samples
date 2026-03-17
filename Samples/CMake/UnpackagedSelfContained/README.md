# UnpackagedSelfContained

A native C++ WinAppSDK app that is **unpackaged** and **self-contained**, built with CMake and NuGetCMakePackage.

## What this app demonstrates

- Unpackaged desktop deployment (no MSIX, no package identity)
- Self-contained WinAppSDK deployment — all WinAppSDK DLLs are copied alongside the executable
- Hybrid CRT (static VC runtime + dynamic UCRT)
- WinAppSDK EnvironmentManager API via C++/WinRT projections
- UndockedRegFreeWinRT initialization handled by the Foundation_SelfContained target's manifest

## Configuration

- CMake target: `Microsoft.WindowsAppSDK.Foundation_SelfContained`
- CRT: `MultiThreaded` / `MultiThreadedDebug` (Hybrid CRT)
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
.\build\vs2022-x64\UnpackagedSelfContained\Debug\UnpackagedSelfContained.exe
```

**Expected result:** A message box showing "Unpackaged Self-Contained App (CMake)" with EnvironmentManager status and the `PROCESSOR_ARCHITECTURE` value.

## Notes

- Since this is self-contained, all WinAppSDK DLLs are in the output directory alongside the executable.
- The app can be xcopy-deployed to another machine without requiring the WinAppSDK runtime installer.
- No bootstrapper or MSIX registration needed — the simplest deployment scenario.
