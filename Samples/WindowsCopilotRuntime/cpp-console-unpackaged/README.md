# Using Windows Copilot Runtime in C++ with CMake

This sample shows how to use Windows App SDK and the Windows Copilot Runtime Generative AI APIs from
a C++ app built using CMake. You can use the CMakeLists and vcpkg ports it contains to build your
own apps.

While the sample is a simple console mode application, you can also use it with the UI framework of
your choice.

Topics and concepts in this example include:

-   Referencing the Windows App SDK, WebView2, and C++/WinRT vcpkgs in CMake
-   Accessing the Windows App SDK implementation from an unpackaged native (C++) app
-   Calling the Microsoft.Windows.AI.Generative.LanguageModel API to generate some text

> **Special note:** The vcpkgs used for C++/WinRT, Windows App SDK, and WebView2 are unofficial as
> of 19th March 2025. We're working on making them widely available.

> **Special note:** Windows Copilot Runtime is currently an Experimental feature. Consult its
> license agreement to see how you can use it. The APIs are subject to change. Your customers will
> not have the
> [Windows App SDK 1.8-experimental1](https://learn.microsoft.com/windows/apps/windows-app-sdk/experimental-channel#version-18-experimental-180-experimental1)
> framework package for production use.

## Building

You can build this project and change it to see examples of using the Windows Copilot Runtime
features of Windows App SDK and Copilot+ PCs.

### Prerequisites

1. Install vcpkg
   [Microsoft vcpkg CMake integration](https://learn.microsoft.com/vcpkg/get_started/get-started) to
   get vcpkg cloned and ready for use.
2. Install [CMake](https://cmake.org/download/). You can also `winget install Kitware.CMake`.
3. Clone this repo to your development environment.
4. Use a compiler toolchain supported by CMake and vcpkg. This example will use Visual Studio 2022.
   You can install the community edition with
   `winget install Microsoft.VisualStudio.2022.Community`.
5. A [Windows Copilot+ PC](https://learn.microsoft.com/windows/ai/npu-devices/) to run this sample
6. Install the
   [Windows App SDK 1.8-experimental1](https://learn.microsoft.com/windows/apps/windows-app-sdk/experimental-channel#version-18-experimental-180-experimental1)
   framework package on your Copilot+ PC (Note: installing the Experimental package will not impact
   your production apps).

### Using in your Own App

> **Note**: More fully-featured CMake integration for C++WinRT is available from other sources.
> Updates to this repo or the vcpkg port are welcome.

Copy the content of `vcpkg_ports` into your build tree for both cppwinrt and windowsappsdk.

Update your `vcpkg.json` to include the `windowsappsdk` package ([example](./vcpkg.json)):

```json
{
  "dependencies": [
    "wil",
    "windowsappsdk"
  ]
}
```

Update your `vcpkg-configuration` to include the vcpkg_port overlays you copied:

```json
{
  "overlay-ports": [
    "vcpkg_ports"
  ]
}
```

In your `CMakeLists.txt` add:

```cmake
find_package(cppwinrt CONFIG REQUIRED)
find_package(windowsappsdk CONFIG REQUIRED)
```

Then add the `windowsappsdk` and `cppwinrt` targets to your target's dependencies:

```cmake
target_link_libraries(
    your_target
    PRIVATE
        Microsoft::CppWinRT
        Microsoft::WindowsAppSdk
)
```

If you are using the "Bootstrapper" support, copy the `install_target_runtime_dlls`
function from [CMakeLists.txt](./CMakeLists.txt) so the 

### Building

> **Note:** The rest of these instructions assume you're using Visual Studio 2022. Your build tools'
> instructions may be different.

Open this sample directory in Visual Studio 2022 with _File > Open > Folder_.

Change the target build type to match your Copilot+ PC's architecture, like `arm64-debug`.

Use _Build > Build all_ - this pulls down the Windows App SDK kit, C++/WinRT, and WebView2.

### Running

Run this project with _Debug > Start Debugging_. You should see output like:

```
Generating response...
Response: In the emerald valleys of Valoria, there dwelled a benevolent dragon named ...
```

(Note that the output generated on your system may be different.)

## Structure

The sample has several phases:

### Configuration

On entry, arguments like `--manual-bootstrap` and `--progress` are pulled out of the command line.
Any remaining text is considered part of the prompt.

### Loading Windows App Runtime

As as an unpackaged framework-bound application, this sample creates a runtime reference to the
Windows App Runtime. Your app can use the built-in `Bootstrapper` type, which finds the correct
version of the runtime and configures the process for its use. Your app's installer must deploy the
`Microsoft.WindowsAppRuntime.Bootstrap.dll` in a place your app can load it at runtime.

Apps that cannot deploy the bootstrapper DLL can use the built-in Windows methods
`TryCreatePackageDependency` and `AddPackageDependency` to find and load the runtime.

> **Note:** Packaged apps should instead specify a `<PackageReference/>` in their MSIX manifest.

### Using the Language Model

To use Windows Copilot Runtime Generative AI features, your app must ensure the model is available,
then create an instance of the model to use. Your customers' systems may not have the models yet,
and the `EnsureReadyAsync` method will acquire and install them for your app to use.

Calling `LanguageModel::CreateAsync` loads the model and returns an instance of the model ready to
generate output. When ready, use `LanguageModel::GenerateResponseAsync` with a prompt, options, and
content moderation settings.

Change the parameters used in code and recompile if you'd like to see other results.

The sample combines a system prompt ("You are a clever storyteller...") with a user-provided prompt
on the commandline that is the kind of story to tell about what a dragon might say.
