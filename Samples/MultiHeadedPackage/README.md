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

These samples demonstrate how to create **multi-headed packages** — whether MSIX or sparse-packaged — by defining multiple `<Application>` elements in a single `Package.appxmanifest`. Each application entry gets its own Start menu tile and can be launched independently, while sharing the same package identity and installation lifecycle.

## Samples

### MSIX sample (`cs/cs-winui-msix/`)

A WinUI 3 solution using single-project MSIX packaging with two projects:

- **PrimaryApp** — The main WinUI app that owns the package manifest. Uses `$targetnametoken$` tokens so the build system fills in the correct executable name.
- **SecondaryApp** — A console app included in the same MSIX package. Its executable name is hardcoded in the manifest as `SecondaryApp.exe` with `EntryPoint="Windows.FullTrustApplication"`.

After deployment, both apps appear in the Start menu as separate entries.

### Sparse sample (`cs/cs-wpf-sparse/`)

A WPF solution demonstrating multi-headed sparse packages with runtime registration:

- **PrimaryApp** — A WPF app with `WindowsPackageType=Sparse` that registers the sparse package at runtime using the `PackageManager` API. After registration, it uses `IApplicationActivationManager` to relaunch with package identity via the app's AUMID (Application User Model ID).
- **SecondaryApp** — A minimal WPF app that detects whether it has package identity. Once the sparse package is registered, the primary app can launch this secondary app with identity using the `SecondaryApp` AUMID entry point.

Key implementation details:
- The manifest uses `uap10:AllowExternalContent="true"` to enable sparse packaging.
- Application entries use `uap10:RuntimeBehavior="win32App"` instead of `EntryPoint` (these are mutually exclusive).
- Restart/launch uses `IApplicationActivationManager.ActivateApplication()` with the package AUMID to ensure the process receives package identity. Launching the exe directly with `Process.Start` would bypass the package activation and the process would not have identity.
- A test certificate (`PrimaryApp_TemporaryKey.pfx`) is included because sparse packages must be signed to be registered.

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
3. Trust the test certificate: run `certutil -addstore TrustedPeople cs\cs-wpf-sparse\PrimaryApp\PrimaryApp_TemporaryKey.pfx` from an elevated terminal.
4. Press **Ctrl+F5** to build and run without debugging.
5. Click **Register Package** to register the sparse package with the OS.
6. Click **Restart** to relaunch the app with package identity (the status should now show the package full name).
7. Click **Launch Secondary App** to launch the secondary app entry point with package identity.

To clean up, click **Unregister Package** and restart.

## Related links

- [Windows App SDK](https://docs.microsoft.com/windows/apps/windows-app-sdk/)
- [Single-project MSIX packaging](https://docs.microsoft.com/windows/apps/windows-app-sdk/single-project-msix)
- [Grant package identity by packaging with external location](https://docs.microsoft.com/windows/apps/desktop/modernize/grant-identity-to-nonpackaged-apps)
- [IApplicationActivationManager::ActivateApplication](https://learn.microsoft.com/windows/win32/api/shobjidl_core/nf-shobjidl_core-iapplicationactivationmanager-activateapplication)
