---
page_type: sample
languages:
    - csharp
    - cpp
products:
    - windows
    - windows-app-sdk
name: "Windows AI Samples"
urlFragment: WindowsAISamples
description: "Shows how to use the Windows AI APIs"
extendedZipContent:
    - path: LICENSE
      target: LICENSE
---

# Windows AI Samples

An app that demonstrates how to use the Windows AI APIs with WinUI.

## Releases
Stable features can be found in the [main](https://github.com/microsoft/WindowsAppSDK-Samples/tree/main/Samples/WindowsAIFoundry/cs-winui) branch. 

Experimental features can be found in the [release/experimental](https://github.com/microsoft/WindowsAppSDK-Samples/tree/release/experimental/Samples/WindowsAIFoundry/cs-winui) branch

## Prerequisites

-   See
    [System requirements for Windows app development](https://docs.microsoft.com/windows/apps/windows-app-sdk/system-requirements).
-   Make sure that your development environment is set up correctly&mdash;see
    [Install tools for developing apps for Windows 10 and Windows 11](https://docs.microsoft.com/windows/apps/windows-app-sdk/set-up-your-development-environment).
-   Use a Copilot+ PC
-   Detailed instructions : [Get started building an app with Windows AI APIs](https://learn.microsoft.com/en-us/windows/ai/apis/model-setup)


To get the latest updates to Windows and the development tools, and to help shape their development,
join the [Windows Insider Program](https://insider.windows.com).

## Building and running the sample

-   Open the solution file (`.sln`) in Visual Studio.
-   From Visual Studio, either **Start Without Debugging** (Ctrl+F5) or **Start Debugging** (F5).

See [additional instructions](./cpp-console-unpackaged/README.md) for using [the C++ & CMake sample](./cpp-console-unpackaged/CMakeLists.txt).

### Building against the stable Windows App SDK

The sample is built against the experimental Windows App SDK by default so that every navigation
entry has a working backing API. Two of the entries — **Image Foreground Extractor** and
**Video Scaler** — depend on APIs that only ship in the experimental flavor and live as extension
projects under `Extensions/`. The host app references those projects only when
`IncludeExperimentalApis=true`.

To build the same sample against the latest stable Windows App SDK, pass
`IncludeExperimentalApis=false`:

```powershell
dotnet build WindowsAISample.csproj -p:Platform=x64 -p:IncludeExperimentalApis=false
```

In that mode:

- `Directory.Packages.props` resolves `Microsoft.WindowsAppSDK` to the latest stable version
  instead of the `-experimental` one.
- `WindowsAISample.csproj` references only the stable extensions declared in `Extensions.props`.
- `MainWindow` only probes and loads the extension assemblies emitted into the generated manifest for
  the active build flavor.

Switching back to the default (experimental) build does not require any other change — simply omit
the property or pass `-p:IncludeExperimentalApis=true`.

#### Layout for extension contributors

The split is structured so that no shared file (csproj, `MainWindow.xaml.cs`, root view-model)
contains any `#if` guards. The convention is:

| Location | Purpose |
| --- | --- |
| `Contracts/WindowsAISample.Contracts.csproj` | Tiny shared contract assembly (`NavPageAttribute`, `IModelManager`) |
| `Extensions/<Feature>/WindowsAISample.Ext.<Feature>.csproj` | Per-feature extension project |
| `Extensions/<Feature>/<Feature>Model.cs` | Direct calls to the feature APIs |
| `Extensions/<Feature>/<Feature>ViewModel.cs` | Feature-local view-model |
| `Extensions/<Feature>/<Feature>Page.xaml[.cs]` | Feature UI and navigation registration |
| `Extensions.props` | Declares which extensions are stable vs experimental |

To add a new experimental feature, create a new folder under `Extensions/`, reference
`Contracts/WindowsAISample.Contracts.csproj`, decorate the page with `[NavPage]`, and add the
feature name to `Extensions.props`. To promote a feature to stable, move its entry from
`<ExperimentalExtension>` to `<StableExtension>` and rebuild.

#### Full walkthrough and standalone demo

For a framework-agnostic walkthrough of this extension model &mdash; the empty-DLL technique,
reflection-based navigation discovery, and per-flavor build isolation &mdash; together with a
runnable end-to-end demo, see the standalone demo repository *(link to be added)*.

## Special Considerations for Unpackaged and Self-Contained modes with Windows AI APIs

- Unpackaged app configuration is no longer supported. Every app using Windows AI APIs must have a package identity which can be granted to [apps with an external location](https://learn.microsoft.com/en-us/windows/apps/desktop/modernize/grant-identity-to-nonpackaged-apps) to achieve that with unpackaged binaries.
- Self-contained mode is fully supported by Windows AI APIs as well.
- The following command demonstrates how to run the app as an ARM64 application in packaged self-contained mode (for both WinAppSDK and .NET):
```powershell
dotnet run -p:Configuration=Debug -p:Platform=ARM64 -p:WindowsAppSDKSelfContained=true -p:SelfContained=true.
```