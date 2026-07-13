# AssociationLaunching

Ported to WinUI 3 / Windows App SDK from the UWP [AssociationLaunching](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/AssociationLaunching) sample.

## What it shows

How an app launches files and URIs, and how it registers to be launched when other apps
open its file types or custom URI scheme, across 4 scenarios:

1. **Launching a file** (`Launcher.LaunchFileAsync`) - default handler, warn-before-launch,
   Open With dialog, and pick-then-launch.
2. **Launching a URI** (`Launcher.LaunchUriAsync`) - default handler, warn-before-launch,
   and Open With dialog.
3. **Receiving a file** - the app registers the `.alsdk` file type (and `*` for no-extension
   files) and is activated when such a file is opened. Buttons create/remove test files in the
   Pictures library so you can trigger the activation.
4. **Receiving a URI** - the app registers the custom `alsdk:` URI scheme and is activated when
   such a URI is launched (for example enter `alsdk://hello` in scenario 2).

## APIs featured

- `Windows.System.Launcher` (`LaunchFileAsync`, `LaunchUriAsync`, `LaunchFolderAsync`, `LauncherOptions`)
- `Microsoft.Windows.AppLifecycle` (`AppInstance`, `AppActivationArguments`, `ExtendedActivationKind`)
- `Windows.ApplicationModel.Activation` (`IFileActivatedEventArgs`, `IProtocolActivatedEventArgs`)
- `Microsoft.Windows.Storage.Pickers.FileOpenPicker` (Windows App SDK picker)

## Build & run

```powershell
dotnet build -c Debug -p:Platform=x64
dotnet run -c Debug -p:Platform=x64
```

Register the file/protocol associations by deploying the app once (build + run). Then open the
"Receiving a file" scenario, create a test file, and double-click it in the Pictures folder; or
use the "Launching a URI" scenario to launch `alsdk://hello`.

## Migration notes

UWP apps are single-instanced and handle activation through the `App.OnFileActivated` /
`App.OnActivated` overrides. WinUI 3 desktop apps are multi-instanced and have no such overrides,
so this port reproduces the UWP behavior with two pieces:

- A custom entry point (`Program.cs`, enabled by `DISABLE_XAML_GENERATED_MAIN`) uses
  `Microsoft.Windows.AppLifecycle.AppInstance` to register a single shared instance key and
  redirect any additional file/protocol activations to the already-running instance.
- `App` reads the activation payload from `AppInstance.GetCurrent().GetActivatedEventArgs()` on
  cold start (and from the `Activated` event for redirected activations), then routes File
  activations to scenario 3 and Protocol activations to scenario 4, passing the
  `IFileActivatedEventArgs` / `IProtocolActivatedEventArgs` as the navigation parameter.

Other transforms are the standard ones: `Windows.UI.Xaml` namespaces updated to
`Microsoft.UI.Xaml`, page backgrounds removed so the Mica backdrop shows through, and the UWP
`SampleHeaderTextStyle` / `ScenarioDescriptionTextStyle` / `BasicTextStyle` replaced with a
"Description:" header (`SubtitleTextBlockStyle`) plus `BodyTextBlockStyle` body text. The
pick-and-launch file picker uses the Windows App SDK `Microsoft.Windows.Storage.Pickers.FileOpenPicker`
(constructed with `this.XamlRoot.ContentIslandEnvironment.AppWindowId`) instead of the legacy
picker that would require HWND interop.

## Known differences / limitations

- The UWP "Launch with view preference" (split-screen) options, which relied on
  `LauncherOptions.DesiredRemainingView` and `Windows.UI.ViewManagement.ViewSizePreference`, are
  dropped. Those APIs target the UWP single-window/tablet model and have no desktop equivalent.
- The UWP `appUriHandler` (associating an `https://` domain with the app) is dropped; it requires
  a verified domain. Only the custom `alsdk:` scheme is registered.
- The Open With dialog is requested with `LauncherOptions.DisplayApplicationPicker`; the UWP
  positioning hints (`LauncherOptions.UI.InvocationPoint` / `PreferredPlacement`), which are
  CoreWindow-relative, are not set.
