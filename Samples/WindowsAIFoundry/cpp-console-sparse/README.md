# Using Microsoft Foundry on Windows in C++ with CMake

This sample shows how to use Windows App SDK and the [Microsoft Foundry on Windows](https://developer.microsoft.com/windows/ai/) 
APIs from a C++ app built using CMake. You can use the CMakeLists and vcpkg ports
it contains to build your own apps.

While the sample is a simple console mode application, you can also use it with the UI framework of
your choice.

Topics and concepts in this example include:

-   Referencing the Windows App SDK and C++/WinRT vcpkgs in CMake
-   Accessing the Windows App SDK implementation from an unpackaged native (C++) app
-   Calling the Microsoft.Windows.AI.Text.LanguageModel API to generate some text

> [!NOTE]
> The vcpkg defined for the Windows App SDK is under development as of 28th
> May 2025. We're working on making them "offcial."

> [!NOTE]
> Microsoft Foundry on Windows is currently an Experimental feature. Consult its license agreement to
> see how you can use it. The APIs are subject to change. Your customers will not have the
> [Windows App SDK 1.8-experimental2](https://learn.microsoft.com/windows/apps/windows-app-sdk/experimental-channel#version-18-experimental-180-experimental2)
> framework package for production use.

## Building

You can build this project and change it to see examples of using the Microsoft Foundry on Windows
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
6. Install the
   [Windows App SDK 1.8-experimental2](https://learn.microsoft.com/windows/apps/windows-app-sdk/experimental-channel#version-18-experimental-180-experimental2)
   framework package on your Copilot+ PC (Note: installing the Experimental package will not impact
   your production apps).

### Using in your Own App

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

Using Microsoft Foundry on Windows generative-AI APIs requires an app have package identity. The sample
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

Add the reference to the Windows App SDK framework package:

```xml
<Package>
  <Dependencies>
    <PackageDependency
        Name="Microsoft.WindowsAppRuntime.1.8-experimental2"
        Publisher="CN=Microsoft Corporation, O=Microsoft Corporation, L=Redmond, S=Washington, C=US"
        MinVersion="8000.500.1427.0" />
```

Be sure to update the `Name` and `MinVersion` for your target Windows App SDK version.

### Building

> **Note:** The rest of these instructions assume you're using Visual Studio 2022. Your build tools'
> instructions may be different.

Open this sample directory in Visual Studio 2022 with _File > Open > Folder_.

Change the target build type to match your Copilot+ PC's architecture, like `arm64-debug`.

Use _Build > Build all_ - this pulls down the Windows App SDK and its dependencies.

### Deploying

With Windows App SDK 1.8 Experimental 2, the Windows AI APIs now require package identity.
Unpackaged configurations are no longer supported, whether self-contained or loading the
Windows App Runtime via the bootstrapper.  This sample provides package identity via 
"sparse packaging" the app from an external location.

To loose deploy (directly from an AppxManifest.xml) a sparse package for the sample,
run the **install.ps1** script after a successful build.

The AppxManifest.xml contains a PackageReference to the Windows App Runtime, 
which Windows will automatically reference and initialize for the app.

Once complete, the output is registered as a package with external location (a "sparse package.")
When done with the sample, remove this package by running this in PowerShell:

```powershell
Get-AppxPackage *WindowsAISampleForCppCMakeSparse* | Remove-AppxPackage
```

### Running

To debug the deployed sparse package for the sample (see above) in Visual Studio,
select **Debug|Other Debug Targets|Debug Installed App Package...**, browse to the
app named **WindowsAISampleForCppCMakeSparse**, and click start.  Any previously set
breakpoints should be enabled and hit, when appropriate.

The app should produce the following console output:
```
Generating response...
Response: In the emerald valleys of Valoria, there dwelled a benevolent dragon named ...
```

(Note that the output generated on your system may be different.)

## Structure

### Configuration

On entry, the `--progress` argument enables incremental output while content is being
generated. Any other parameters are treated as part of the story prompt.

### Using the Language Model

To use Microsoft Foundry on Windows Generative AI features, your app must ensure the model is available,
then create an instance of the model to use. Your customers' systems may not have the models yet,
and the `EnsureReadyAsync` method will acquire and install them for your app to use.

Calling `LanguageModel::CreateAsync` loads the model and returns an instance of the model ready to
generate output. When ready, use `LanguageModel::GenerateResponseAsync` with a prompt, options, and
content moderation settings.

Change the parameters used in code and recompile if you'd like to see other results.

The sample combines a system prompt ("You are a clever storyteller...") with a user-provided prompt
on the commandline that is the kind of story to tell about what a dragon might say.
