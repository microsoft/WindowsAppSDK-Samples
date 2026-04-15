# Windows App SDK — CMake Samples

These samples demonstrate how to consume Windows App SDK NuGet packages from CMake-based C++ projects. Each sample covers a different deployment scenario in the WinAppSDK deployment matrix.

> **This feature is experimental.** CMake support in Windows App SDK is under active development and community feedback will directly shape its direction. See [Feedback](#feedback) below.

---

## Prerequisites

- **CMake 3.31+**
- **MSVC C++ Build Tools**
- **Build Tool e.g. Ninja**
- **Windows SDK** (included with the C++ Build Tools — the minimum target platform supported is 10.0.17763.0)

---

## Samples Overview

| Sample | Deployment Mode | Packaging | Description |
|--------|----------------|-----------|-------------|
| [UnpackagedSelfContained](UnpackagedSelfContained/) | Self-Contained | None | Runtime DLLs bundled alongside the exe. No MSIX, no framework package dependency. |
| [UnpackagedFrameworkDependent](UnpackagedFrameworkDependent/) | Framework-Dependent | None | DLLs loaded from the installed WinAppSDK Framework package. Bootstrap auto-init locates the framework at startup. |
| [PackagedSelfContained](PackagedSelfContained/) | Self-Contained | MSIX | Runtime DLLs bundled with MSIX package identity. |
| [PackagedFrameworkDependent](PackagedFrameworkDependent/) | Framework-Dependent | MSIX | Framework package declared as a PackageDependency in AppxManifest.xml. DeploymentManager auto-init enabled. |
| [MetapackageFrameworkDependent](MetapackageFrameworkDependent/) | Framework-Dependent | MSIX | Same as PackagedFrameworkDependent but uses the single `Microsoft.WindowsAppSDK` metapackage instead of listing individual component packages. |

---

## Building with Ninja

### Component Package Samples (4 apps)

The four deployment matrix samples share a root `CMakeLists.txt` with presets.

1. **Open an x64 Developer Command Prompt:**
   ```
   "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" x64
   ```
   Or: Start Menu → "x64 Native Tools Command Prompt for VS 2022"

2. **Navigate to the samples root:**
   ```
   cd Samples\CMake
   ```

3. **Configure:**
   ```
   cmake --preset ninja-x64
   ```

4. **Build:**
   ```
   cmake --build --preset ninja-x64-debug
   ```

> **Before building:** Review `CMakePresets.json` and update the `binaryDir` field to control where build output is generated. The default uses a sample path (`D:\b\nx64`).

### Metapackage Sample (standalone)

The metapackage sample has its own `CMakeLists.txt` and presets.

1. Open an x64 Developer Command Prompt (same as above).

2. **Navigate to the metapackage sample:**
   ```
   cd Samples\CMake\MetapackageFrameworkDependent
   ```

3. **Configure:**
   ```
   cmake --preset ninja-x64
   ```

4. **Build:**
   ```
   cmake --build --preset ninja-x64-debug
   ```

---

## Launching Apps

### Unpackaged Apps

Run the exe directly from the build output:

```
D:\b\nx64\UnpackagedSelfContained\Debug\UnpackagedSelfContained.exe
D:\b\nx64\UnpackagedFrameworkDependent\Debug\UnpackagedFrameworkDependent.exe
```

### Packaged Apps (MSIX)

Packaged apps require MSIX registration before launching. Use the provided `Launch-PackagedApp.ps1` script:

```powershell
# PackagedSelfContained
.\Launch-PackagedApp.ps1 -AppxManifestPath D:\b\nx64\PackagedSelfContained\Debug\AppxManifest.xml -PackageNameFilter *CMake-PackagedSelfContained*

# PackagedFrameworkDependent
.\Launch-PackagedApp.ps1 -AppxManifestPath D:\b\nx64\PackagedFrameworkDependent\Debug\AppxManifest.xml -PackageNameFilter *CMake-PackagedFrameworkDependent*

# MetapackageFrameworkDependent (from the CMake root directory)
.\Launch-PackagedApp.ps1 -AppxManifestPath D:\b\meta-nx64\Debug\AppxManifest.xml -PackageNameFilter *CMake-MetapackageFrameworkDependent*
```

To unregister a packaged app after testing:
```powershell
Get-AppxPackage *CMake-PackagedSelfContained* | Remove-AppxPackage
```

### Run All Tests

```powershell
.\Test-AllApps.ps1
.\Test-AllApps.ps1 -Configuration Release
```

This script builds, registers, launches, and verifies all 4 deployment matrix apps automatically.

---

## How It Works

### NuGetCMakePackage Infrastructure

These samples use [NuGetCMakePackage](https://github.com/mschofie/NuGetCMakePackage), an open-source CMake module fetched via `FetchContent`. It provides:

- **`add_nuget_packages()`** — Downloads and restores NuGet packages at configure time. Each Windows App SDK NuGet package embeds a cmake config at `build/cmake/<package>-config.cmake` that is automatically probed.
- **`find_package()`** — Standard CMake mechanism to load the package's cmake config, which defines targets for headers, libraries, DLLs, and WinRT projections.
- **C++/WinRT projection generation** — Runs `cppwinrt.exe` on `.winmd` files to generate C++ projection headers.
- **SxS manifest generation** — Transforms `package.appxfragment` into activation manifests for self-contained deployment.
- **AppxManifest generation** — `wasdk_generate_appx_manifest()` generates `AppxManifest.xml` with framework `PackageDependency` for packaged framework-dependent apps.

### Package Listing

Packages are listed in `add_nuget_packages()` with explicit versions:

```cmake
add_nuget_packages(
    CONFIG_FILE ${CMAKE_SOURCE_DIR}/nuget.config
    LOCK_FILE ${CMAKE_SOURCE_DIR}/packages.lock.json
    FRAMEWORK native
    PACKAGES
        Microsoft.Windows.CppWinRT 2.0.251203.1
        Microsoft.WindowsAppSDK.Base 2.0.3-experimental1
        Microsoft.WindowsAppSDK.Foundation 2.0.19-experimental
        Microsoft.WindowsAppSDK.InteractiveExperiences 2.0.11-experimental
        Microsoft.WindowsAppSDK.DWrite 2.0.2-experimental
        Microsoft.WindowsAppSDK.Runtime 2.0.0-experimental7
        Microsoft.WindowsAppSDK.Widgets 2.0.4-experimental
        Microsoft.WindowsAppSDK.AI 2.0.179-experimental
        Microsoft.WindowsAppSDK.ML 2.0.325-experimental
        Microsoft.Web.WebView2 1.0.3719.77
        Microsoft.WindowsAppSDK.WinUI 2.0.10-experimental
)
```

Alternatively, the **MetapackageFrameworkDependent** sample shows that you can list only `Microsoft.WindowsAppSDK` and all component packages are pulled transitively. Individual `find_package()` calls per component are still required.

---

## Available CMake Targets Per Component

Each component package provides targets for different deployment modes. Link against the appropriate target in `target_link_libraries()`:

### Component Packages

| Component | Base Target | Self-Contained | Framework |
|-----------|------------|----------------|-----------|
| **Foundation** | `Microsoft.WindowsAppSDK.Foundation` | `Microsoft.WindowsAppSDK.Foundation_SelfContained` | `Microsoft.WindowsAppSDK.Foundation_Framework` |
| **InteractiveExperiences (IXP)** | `Microsoft.WindowsAppSDK.InteractiveExperiences` | `Microsoft.WindowsAppSDK.InteractiveExperiences_SelfContained` |
| **DWrite** | `Microsoft.WindowsAppSDK.DWrite` | `Microsoft.WindowsAppSDK.DWrite_SelfContained` | `Microsoft.WindowsAppSDK.DWrite_Framework` |
| **Widgets** | `Microsoft.WindowsAppSDK.Widgets` | `Microsoft.WindowsAppSDK.Widgets_SelfContained` | `Microsoft.WindowsAppSDK.Widgets_Framework` |
| **AI** | `Microsoft.WindowsAppSDK.AI` | `Microsoft.WindowsAppSDK.AI_SelfContained` | `Microsoft.WindowsAppSDK.AI_Framework` |
| **ML** | `Microsoft.WindowsAppSDK.ML` | `Microsoft.WindowsAppSDK.ML_SelfContained` | `Microsoft.WindowsAppSDK.ML_Framework` |
| **WinUI** | `Microsoft.WindowsAppSDK.WinUI` | `Microsoft.WindowsAppSDK.WinUI_SelfContained` | `Microsoft.WindowsAppSDK.WinUI_Framework` |

### Dependency Packages (not component packages)

| Package | Target | Purpose |
|---------|--------|---------|
| **Base** | `Microsoft.WindowsAppSDK.Base` | Shared CMake infrastructure: CppWinRT projection generation, platform detection, SxS manifest transformation. Required by all component configs. |
| **Runtime** | `Microsoft.WindowsAppSDK.Runtime` | Version info headers (`WindowsAppSDK-VersionInfo.h`). Used by Foundation for bootstrap auto-init and AppxManifest generation. |

**Notes:**
- **IXP** does not have a `_Framework` target — use the base target (`Microsoft.WindowsAppSDK.InteractiveExperiences`) for framework-dependent apps.
- Self-contained apps should link `_SelfContained` targets; framework-dependent apps should link `_Framework` targets (or base target for IXP).

### Example: Self-Contained App

```cmake
target_link_libraries(MyApp
    PRIVATE
        Microsoft.WindowsAppSDK.Foundation_SelfContained
        Microsoft.WindowsAppSDK.InteractiveExperiences_SelfContained
        Microsoft.WindowsAppSDK.DWrite_SelfContained
        Microsoft.WindowsAppSDK.Widgets_SelfContained
        Microsoft.WindowsAppSDK.AI_SelfContained
        Microsoft.WindowsAppSDK.WinUI_SelfContained
)
```

### Example: Framework-Dependent App

```cmake
target_link_libraries(MyApp
    PRIVATE
        Microsoft.WindowsAppSDK.Foundation_Framework
        Microsoft.WindowsAppSDK.InteractiveExperiences
        Microsoft.WindowsAppSDK.DWrite_Framework
        Microsoft.WindowsAppSDK.Widgets_Framework
        Microsoft.WindowsAppSDK.AI_Framework
        Microsoft.WindowsAppSDK.WinUI_Framework
)
```

---

## Target Properties

The Foundation cmake config supports target properties on the consuming executable to control behavior. Set these via `set_target_properties()`. We'd love feedback on whether these properties cover your use cases — if you need additional controls or find the defaults don't match your expectations, please let us know via [Issues](https://github.com/microsoft/WindowsAppSDK/issues).

| Property | Default | Description |
|----------|---------|-------------|
| `WindowsPackageType` | `"MSIX"` | Set to `"None"` for unpackaged apps. Controls which auto-initializers are enabled by default. |
| `WindowsAppSdkBootstrapInitialize` | `TRUE` when `WindowsPackageType=None` | Enables Bootstrap auto-init for unpackaged framework-dependent apps. Set `FALSE` to opt out. |
| `WindowsAppSdkDeploymentManagerInitialize` | `TRUE` when `WindowsPackageType=MSIX` | Enables DeploymentManager auto-init for packaged framework-dependent apps. Set `FALSE` to opt out. |
| `WindowsAppSdkUndockedRegFreeWinRTInitialize` | `TRUE` when SDK < 10.0.18362.0 | Enables manifest-based WinRT activation for self-contained apps on downlevel devices. Set `TRUE` to force on modern OS. |

### Example

```cmake
# Unpackaged framework-dependent app
set_target_properties(MyApp PROPERTIES
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
    WindowsPackageType "None"
    # Bootstrap is on by default for unpackaged apps — setting explicitly as an example
    WindowsAppSdkBootstrapInitialize TRUE
)
```

```cmake
# Packaged framework-dependent app
set_target_properties(MyApp PROPERTIES
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
    # DeploymentManager is on by default for packaged apps — setting explicitly as an example
    WindowsAppSdkDeploymentManagerInitialize TRUE
)
```

---

## WinUI Header Note

The Windows SDK defines a `GetCurrentTime` macro that conflicts with WinUI XAML. When including WinUI headers, use:

```cpp
#pragma push_macro("GetCurrentTime")
#undef GetCurrentTime
#include <winrt/Microsoft.UI.Xaml.h>
#pragma pop_macro("GetCurrentTime")
```

---

## Project Structure

```
Samples/CMake/
├── CMakeLists.txt                          # Root — FetchContent + add_nuget_packages + macros
├── CMakePresets.json                       # VS2022 and Ninja presets
├── Configuration.cmake                    # Shared compiler settings
├── nuget.config                           # NuGet package sources
├── Launch-PackagedApp.ps1                 # Script to register + COM-activate packaged apps
├── Test-AllApps.ps1                       # Script to build, launch, and verify all 4 apps
├── README.md                              # This file
├── UnpackagedSelfContained/               # Self-contained, no MSIX
├── UnpackagedFrameworkDependent/          # Framework-dependent, no MSIX
├── PackagedSelfContained/                 # Self-contained with MSIX identity
├── PackagedFrameworkDependent/            # Framework-dependent with MSIX identity
└── MetapackageFrameworkDependent/         # Standalone sample using the metapackage
```

---

## Feedback

**CMake support in Windows App SDK is experimental and under active development.** Your feedback will help shape the future of this feature. We want to hear about:

- Scenarios that aren't currently covered
- Issues with the developer experience or configuration
- Suggestions for improvement
- Missing deployment modes or component coverage

Please file feedback under [Issues](https://github.com/microsoft/WindowsAppSDK/issues) or start a conversation in [Discussions](https://github.com/microsoft/WindowsAppSDK/discussions) in the Windows App SDK open-source repository. Thank you!
