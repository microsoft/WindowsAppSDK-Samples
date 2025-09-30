# Windows ML C++ CMake Samples

## Overview

This folder contains samples showing how to consume WindowsML from C++ using a CMake-based build.

## Getting Started

In addition to the requirements called out in [the repository README.md](../../../README.md), the CMake builds require CMake 3.31 or higher. You can install CMake on Windows using WinGet with:

```powershell
winget install --id Kitware.CMake --source winget
```

To build the samples using Ninja, then Ninja needs to be installed. See <https://ninja-build.org/> for more details. You can install Ninja on Windows using WinGet with:

```powershell
winget install --id Ninja-build.Ninja --source winget
```

## Building

The samples use `CMakePresets.json` to describe the build and configuration presets that can be used. There are two types of presets, those that use the 'Visual Studio 17 2022' generator and those that use the 'Ninja Multi-Config' generator. Visual Studio generator presets can be built from any environment. The Ninja Multi-Config generator presets have to be built from a platform-specific Visual Studio command prompt. Create a platform-specific build prompt by running - for example:

```cmd
C:\Program Files\Microsoft Visual Studio\2022\Enterprise\vc\Auxiliary\Build\vcvars64.bat
```

for an x64 build, or:

```cmd
C:\Program Files\Microsoft Visual Studio\2022\Enterprise\vc\Auxiliary\Build\vcvarsamd64_arm64.bat
```

for an arm64 cross-compiling prompt on an x64 OS.

In order to build a given preset, simply run the CMake 'configuration' step:

```powershell
cmake --preset <preset name>
```

Followed by the CMake 'build' step:

```powershell
cmake --build --preset <preset name>
```
