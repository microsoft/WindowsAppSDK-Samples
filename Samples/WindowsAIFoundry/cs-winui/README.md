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
**Video Scaler** — depend on APIs that only ship in the experimental flavor and live under
`Models/Experimental/`, `ViewModels/Experimental/`, and `Pages/Experimental/`. Their sample-code
snippets live flat in `Examples/` alongside the rest.

To build the same sample against the latest stable Windows App SDK, pass
`IncludeExperimentalApis=false`:

```powershell
dotnet build WindowsAISample.csproj -p:Platform=x64 -p:IncludeExperimentalApis=false
```

In that mode:

- `Directory.Packages.props` resolves `Microsoft.WindowsAppSDK` to the latest stable version
  instead of the `-experimental` one.
- Every file under `**/Experimental/` is excluded from compile / XAML items.
- `MainWindow` removes any `NavigationViewItem` whose `Tag` begins with `Experimental:`, so the
  navigation pane only shows the six features that actually have backing APIs in stable.

Switching back to the default (experimental) build does not require any other change — simply omit
the property or pass `-p:IncludeExperimentalApis=true`.

#### Layout for experimental contributors

The split is structured so that no shared file (csproj, `MainWindow.xaml.cs`, root view-model)
contains any `#if` guards. The convention is:

| Location | Purpose |
| --- | --- |
| `Models/Experimental/<Feature>Model.cs` | Direct calls to experimental APIs |
| `ViewModels/Experimental/<Feature>ViewModel.cs` | View-model wrapper |
| `Pages/Experimental/<Feature>Page.xaml[.cs]` | UI for the feature |
| `ViewModels/Experimental/CopilotRootViewModel.Experimental.cs` | Adds the feature's property to the root view-model via `partial class` + field initializer |
| `Pages/Experimental/MainWindow.Experimental.cs` | Implements `partial void` hooks to keep the `Experimental:`-tagged nav items and to route their navigation |
| `MainWindow.xaml` | Declares the nav item with `Tag="Experimental:<TagName>"` |

To add a new experimental feature, drop the first three files in their `Experimental/` folders, add
a property to `CopilotRootViewModel.Experimental.cs`, add a `<NavigationViewItem
Tag="Experimental:..." ...>` entry to `MainWindow.xaml`, and add a `case` to
`MainWindow.Experimental.cs`. To promote a feature to stable, `git mv` those files out of the
`Experimental/` subfolders, drop the `Experimental:` prefix from its nav item tag, move the property
and case into the corresponding stable files, and you're done.

## Special Considerations for Unpackaged and Self-Contained modes with Windows AI APIs

- Unpackaged app configuration is no longer supported. Every app using Windows AI APIs must have a package identity which can be granted to [apps with an external location](https://learn.microsoft.com/en-us/windows/apps/desktop/modernize/grant-identity-to-nonpackaged-apps) to achieve that with unpackaged binaries.
- Self-contained mode is fully supported by Windows AI APIs as well.
- The following command demonstrates how to run the app as an ARM64 application in packaged self-contained mode (for both WinAppSDK and .NET):
```powershell
dotnet run -p:Configuration=Debug -p:Platform=ARM64 -p:WindowsAppSDKSelfContained=true -p:SelfContained=true.
```