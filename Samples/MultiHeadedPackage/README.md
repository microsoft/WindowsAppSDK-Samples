---
page_type: sample
languages:
- csharp
products:
- windows
- windows-app-sdk
name: "Multi-headed MSIX package sample"
urlFragment: multi-headed-package
description: "Shows how to package multiple executables in a single MSIX or sparse package"
extendedZipContent:
- path: LICENSE
  target: LICENSE
---
# Multi-headed MSIX package sample

These samples demonstrate how to create **multi-headed packages** — MSIX or sparse packages that contain multiple `<Application>` elements in a single `Package.appxmanifest`. Each application entry gets its own Start menu tile and can be launched independently, while sharing the same package identity and installation lifecycle.

## Samples

### MSIX sample (`cs/cs-winui-msix/`)

A WinUI 3 solution using single-project MSIX packaging with two projects:

- **PrimaryApp** — The main WinUI app that owns the package manifest. Uses `$targetnametoken$` tokens so the build system fills in the correct executable name.
- **SecondaryApp** — A secondary WinUI app referenced by PrimaryApp. Its executable name is hardcoded in the manifest as `SecondaryApp.exe` with `EntryPoint="Windows.FullTrustApplication"`.

After deployment, both apps appear in the Start menu as separate entries.

### Sparse sample (`cs/cs-wpf-sparse/`)

A WPF solution demonstrating multi-headed sparse packages with runtime registration:

- **PrimaryApp** — A WPF app with `WindowsPackageType=Sparse` that includes Register/Unregister/Restart buttons to manage the sparse package at runtime.
- **SecondaryApp** — A minimal WPF app that detects whether it has package identity (which it will once the sparse package is registered).

## Prerequisites

* See [System requirements for Windows app development](https://docs.microsoft.com/windows/apps/windows-app-sdk/system-requirements).
* Make sure that your development environment is set up correctly&mdash;see [Install tools for developing apps for Windows 10 and Windows 11](https://docs.microsoft.com/windows/apps/windows-app-sdk/set-up-your-development-environment).

## Building and running

### MSIX sample

1. Open `cs/cs-winui-msix/MultiHeadedMsix.sln` in Visual Studio.
2. Set **PrimaryApp** as the startup project.
3. Press **F5** to build, deploy, and launch the primary app.
4. Check the Start menu for both "Primary App" and "Secondary App" entries.

### Sparse sample

1. Open `cs/cs-wpf-sparse/MultiHeadedSparse.sln` in Visual Studio.
2. Set **PrimaryApp** as the startup project.
3. Press **Ctrl+F5** to run without debugging.
4. Click **Register Package** to register the sparse package.
5. Click **Restart** to relaunch with package identity.
6. Run **SecondaryApp** separately to verify it also detects the package identity.

## Related links

- [Windows App SDK](https://docs.microsoft.com/windows/apps/windows-app-sdk/)
- [Single-project MSIX packaging](https://docs.microsoft.com/windows/apps/windows-app-sdk/single-project-msix)
- [Sparse packages](https://docs.microsoft.com/windows/apps/desktop/modernize/grant-identity-to-nonpackaged-apps)
- [WindowsAppSDK issue #5586](https://github.com/microsoft/WindowsAppSDK/issues/5586)
