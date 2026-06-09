# Using Windows AI Foundry in C++ with CMake

This sample shows how to use Windows App SDK and the [Windows AI Foundry](https://developer.microsoft.com/windows/ai/) 
APIs from a C++ app built using CMake. You can use the CMakeLists and vcpkg ports
it contains to build your own apps.

While the sample is a simple console mode application, you can also use it with the UI framework of
your choice.

Topics and concepts in this example include:

-   Referencing the Windows App SDK and C++/WinRT vcpkgs in CMake
-   Accessing the Windows App SDK implementation from an unpackaged native (C++) app
-   Calling the Microsoft.Windows.AI.Text.LanguageModel API to generate some text
-   Calling the Microsoft.Windows.AI.Imaging.ImageScaler API to upscale an image

> [!NOTE]
> The sample uses a local vcpkg overlay port for Windows App SDK packages.
> For general vcpkg guidance, see the
> [vcpkg getting started guide](https://learn.microsoft.com/vcpkg/get_started/get-started).

> [!NOTE]
> This sample is pinned to the current **stable 2.x** Windows App SDK line:
> `Microsoft.WindowsAppSDK` **2.1.3**. The sparse package depends on the matching
> **Windows App Runtime 2** framework package (`Microsoft.WindowsAppRuntime.2`, minimum
> version `2.1.3.0`).

## Prerequisites

You can build this project and change it to see examples of using the Windows AI Foundry
features of Windows App SDK and Copilot+ PCs.

### Prerequisites

1. Install vcpkg
   [Microsoft vcpkg CMake integration](https://learn.microsoft.com/vcpkg/get_started/get-started) to
   get vcpkg cloned and ready for use.
2. Install [CMake](https://cmake.org/download/). You can also `winget install Kitware.CMake`.
3. Clone this repo to your development environment.
4. Use a compiler toolchain supported by CMake and vcpkg. This example uses Visual Studio 2022.
   You can install the community edition with
   `winget install Microsoft.VisualStudio.2022.Community`.
5. A [Windows Copilot+ PC](https://learn.microsoft.com/windows/ai/npu-devices/) to run this sample
6. Install **Windows App Runtime 2** on the test machine. This sample is verified against
   `Microsoft.WindowsAppRuntime.2` version `2.1.3.0`.
7. Verify the runtime is present before building/running:

   ```powershell
   Get-AppxPackage Microsoft.WindowsAppRuntime.2 |
     Select-Object Name, Version, Architecture, PackageFullName
   ```

   If the command prints no rows, install the stable Windows App Runtime 2 package before
   continuing.

## Using in your Own App

Copy the content of `vcpkg_ports` into your build tree for windowsappsdk.

Update your `vcpkg.json` to include the `windowsappsdk` package ([example](./vcpkg.json)):

```json
{
    "dependencies": ["wil", "windowsappsdk"]
}
```

Update your `vcpkg-configuration` to include the vcpkg_port overlays you copied:

```json
{
    "overlay-ports": ["vcpkg_ports"]
}
```

In your `CMakeLists.txt` add:

```cmake
find_package(windowsappsdk CONFIG REQUIRED)
```

Then add the `windowsappsdk` target to your target's dependencies:

```cmake
target_link_libraries(
    your_target
    PRIVATE
        Microsoft::WindowsAppSdk
)
```

### Packaging

Using Windows AI Foundry generative-AI APIs requires an app have package identity. The sample
has a simple [AppxManifest.xml](./AppxManifest.xml) providing identity and a reference to the
Windows App SDK:

```xml
<Package>
    <!-- Define the identity of this package. Be sure to replace with your publisher identity
        or the one assigned by the Microsoft Store for your app. -->
    <Identity
    Name="WindowsAISampleForCppCMakeSparse"
    Publisher="CN=Fabrikam Corporation, O=Fabrikam Corporation, L=Redmond, S=Washington, C=US"
    Version="1.0.0.0" />
```

Add the reference to the Windows App Runtime 2 framework package:

```xml
<Package>
  <Dependencies>
    <PackageDependency
        Name="Microsoft.WindowsAppRuntime.2"
        Publisher="CN=Microsoft Corporation, O=Microsoft Corporation, L=Redmond, S=Washington, C=US"
        MinVersion="2.1.3.0" />
```

Be sure to update the `Name` and `MinVersion` if you retarget the sample to a different
Windows App Runtime line.

## Build

> **Note:** The rest of these instructions assume you're using Visual Studio 2022. Your build tools'
> instructions may be different.

Open this sample directory in Visual Studio 2022 with _File > Open > Folder_.

Change the target build type to match your test machine's architecture:

- `arm64-debug` for Snapdragon / ARM64 Copilot+ PCs
- `x64-debug` for x64 machines

Use _Build > Build all_ - this pulls down Windows App SDK `2.1.3` and its transitive
dependencies through the local `vcpkg_ports/windowsappsdk` overlay.

You can also build from a Developer PowerShell:

```powershell
cmake --preset arm64-debug   # or x64-debug
cmake --build out/build/arm64-debug   # or out/build/x64-debug
```


## Register the sparse package

Windows AI APIs require package identity. This sample gets package identity by registering
`AppxManifest.xml` as a **sparse package** whose external location is the build output folder.

The sample's `CMakeLists.txt` already performs this registration automatically as a
**post-build step**. If registration succeeds, you're ready to run immediately from the
output directory.

For a normal dev loop, re-register manually after rebuilding:

```powershell
Add-AppxPackage `
  -Path .\out\build\arm64-debug\AppxManifest.xml `
  -ExternalLocation .\out\build\arm64-debug `
  -Register `
  -ForceUpdateFromAnyVersion
```

Replace `arm64-debug` with `x64-debug` if you built the x64 preset.

To confirm the sparse package is registered:

```powershell
Get-AppxPackage *WindowsAISampleForCppCMakeSparse* |
  Select-Object Name, Version, PackageFullName
```

When done with the sample, remove the registration with:

```powershell
Get-AppxPackage *WindowsAISampleForCppCMakeSparse* | Remove-AppxPackage
```

## Run

To debug the registered sparse package in Visual Studio, select
**Debug > Other Debug Targets > Debug Installed App Package...**, browse to the
app named **WindowsAISampleForCppCMakeSparse**, and click Start.

To run from PowerShell or Command Prompt, invoke the built exe directly from the output
directory after registration:

```powershell
.\out\build\arm64-debug\cmake-ai-generator.exe --image "C:\path\to\photo.jpg"
.\out\build\arm64-debug\cmake-ai-generator.exe --image "C:\path\to\photo.jpg" --scale 2
.\out\build\arm64-debug\cmake-ai-generator.exe "tell me a dragon story"
.\out\build\arm64-debug\cmake-ai-generator.exe --progress "tell me a dragon story"
```

Replace `arm64-debug` with `x64-debug` if you built the x64 preset.

The app should produce the following console output:
```
Generating response...
Response: In the emerald valleys of Valoria, there dwelled a benevolent dragon named ...
```

(Note that the output generated on your system may be different.)

If `--image` fails immediately with `0x80040154` (`REGDB_E_CLASSNOTREG`), the sparse package
is registered but the required framework package is not in the package graph. Re-check that:

1. `Get-AppxPackage Microsoft.WindowsAppRuntime.2` shows version `2.1.3.0` or later.
2. `Get-AppxPackage *WindowsAISampleForCppCMakeSparse*` shows the sparse package registration.
3. You are running the exe from the same build output directory you registered as
   `-ExternalLocation`.

## Console modes

### Storyteller mode

Use the original LanguageModel flow:

```powershell
cmake-ai-generator.exe "tell me a dragon story"
cmake-ai-generator.exe --progress "tell me a dragon story"
```

`LanguageModel.EnsureReadyAsync()` downloads and installs required language-model components when
needed. However, on current public Learn docs the `Microsoft.Windows.AI.Text.LanguageModel`
(`Phi Silica`) flow is documented as a **Limited Access Feature** on stable releases. If
storyteller mode fails with `0x80070005` (`E_ACCESSDENIED`), verify:

1. The sparse package is registered and the manifest still declares `systemAIModels`.
2. **Settings > System > AI Components** shows the required text model installed.
3. **AI Dev Gallery > AI APIs > Phi Silica > Text Generation** works on the same machine.
4. The current Phi Silica / troubleshooting docs for any LAF requirements on your target setup.

For more detail, see:

- [Get started with Phi Silica in the Windows App SDK](https://learn.microsoft.com/en-us/windows/ai/apis/phi-silica)
- [Windows API troubleshooting](https://learn.microsoft.com/en-us/windows/ai/apis/troubleshooting)
- [LanguageModel.EnsureReadyAsync](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.text.languagemodel.ensurereadyasync?view=windows-app-sdk-2.0)

### Image Scaler mode

In addition to the storyteller (`LanguageModel`) flow above, the sample can also call the
`Microsoft.Windows.AI.Imaging.ImageScaler` API to upscale an image. Pass `--image` (with an
optional `--scale` factor) instead of a prompt:

```
cmake-ai-generator --image "C:\path\to\photo.jpg"
cmake-ai-generator --image "C:\path\to\photo.png" --scale 2
```

The upscaled image is written next to the input as `<name>_x<factor><ext>`.
`--scale` defaults to `4` and must not exceed `ImageScaler::MaxSupportedScaleFactor`.

Image mode prints a `[Step N/6]` header so failures are easy to localize. Common HRESULTs:

| HRESULT | What it usually means |
| --- | --- |
| `0x80040154` (`REGDB_E_CLASSNOTREG`) | The WinAppSDK framework package is not in this process's package graph. The sparse package must declare the matching `Microsoft.WindowsAppRuntime.*` `<PackageDependency>` and the runtime must be installed. |
| `0x80070005` (`E_ACCESSDENIED`) | The app has no package identity (sparse package not registered) or the `systemAIModels` capability is missing from the manifest. |
| `0x80073D06` | Package not registered. Run `Add-AppxPackage -ExternalLocation` against the build directory. |
| `0x80004005` (`E_FAIL`) | Often "Not declared by app" — verify `MaxVersionTested >= 10.0.26226.0` and that the `systemai` capability namespace is present. |
| `EnsureReadyAsync` returns non-success | The ImageScaler model is not yet installed. Check **Settings > System > AI Components** and **Settings > Windows Update**. |

For more detail behind those checks, see:

- [Image Super Resolution with Windows AI APIs](https://learn.microsoft.com/en-us/windows/ai/apis/image-super-resolution) for `systemAIModels`, `MaxVersionTested`, readiness, and the end-to-end ImageScaler flow.
- [Grant package identity by packaging with external location manually](https://learn.microsoft.com/en-us/windows/apps/desktop/modernize/grant-identity-to-nonpackaged-apps) for sparse package / external-location registration.
- [Add-AppxPackage](https://learn.microsoft.com/en-us/powershell/module/appx/add-appxpackage?view=windowsserver2025-ps) for the `-Register` and `-ExternalLocation` options used in the dev loop.
- [ImageScaler.MaxSupportedScaleFactor](https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.windows.ai.imaging.imagescaler.maxsupportedscalefactor?view=windows-app-sdk-2.0) for the supported scale-factor limit.

The ImageScaler model only ships on Copilot+ PCs. For Windows AI platform details, see
[Windows AI Foundry](https://developer.microsoft.com/windows/ai/).
