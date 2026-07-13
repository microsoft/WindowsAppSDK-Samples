# WinUI samples — porting conventions (agent instructions)

This repo holds UWP samples ported to **WinUI 3 / Windows App SDK**. Read this before
porting or modifying any sample. These conventions come from the Clipboard pilot and from
Niels' (@niels9001) review feedback; they apply to **every** sample.

## Project shape

- Scaffold each sample with `dotnet new winui` (Blank App, packaged, C#), then restructure.
- Namespace: keep the UWP `SDKTemplate` root namespace per sample (RootNamespace=SDKTemplate
  in the csproj). SDK-style project auto-globs `*.xaml` / `*.cs`.
- Build / run (from the sample folder):
  - `dotnet build -c Debug -p:Platform=x64`
  - `dotnet run -c Debug -p:Platform=x64` (or `winapp run`). The `dotnet run` launcher host
    exits with odd codes (e.g. 3221226107) **after** successfully launching the packaged
    app — that is the launcher detaching, not the app crashing. Verify the real app with
    `Get-Process <SampleExe>`.

## Shell (use this, not the UWP SDKTemplate SplitView/ListBox/hamburger)

- **NavigationView + InfoBar shell.** `MainPage` hosts a `NavigationView`
  (`PaneDisplayMode=Left`, `IsSettingsVisible=False`, `IsBackButtonVisible=Collapsed`) with
  the scenarios as menu items, a content `Frame`, and an `InfoBar` for status/error messages
  (replaces the UWP colored status border / `NotifyUser`).
- Do **not** set a `NavigationView.Header`. App identity lives only in the title bar.

## Window chrome (Niels' cross-sample standard — apply to ALL samples)

1. **Mica backdrop:** `<Window.SystemBackdrop><MicaBackdrop/></Window.SystemBackdrop>`.
2. **App icon in the title bar:** use the `TitleBar` control with
   `<TitleBar.IconSource><ImageIconSource ImageSource="ms-appx:///Assets/StoreLogo.png"/></TitleBar.IconSource>`,
   `ExtendsContentIntoTitleBar = true`, and `SetTitleBar(AppTitleBar)`.
3. **No page backgrounds.** Scenario pages must not set an explicit `Background` (no
   `SolidBackgroundFillColorBaseBrush` etc.) so Mica shows through.
4. **Header alignment:** keep the title (and icon) in the `TitleBar` only; don't duplicate it
   in the content area.

## Lifecycle / API migration rules

- **No `Window.Current`.** Track the main window in `App.MainWindow`. Navigate the root
  `Frame` to `MainPage` from `App.OnLaunched` **after** `App.MainWindow` is assigned (a
  `MainWindow.NavigateToMainPage()` helper) — navigating inside the `MainWindow` constructor
  NRE's because scenario pages read `App.MainWindow` in `OnNavigatedTo`.
- **`CoreDispatcher` → `DispatcherQueue`** (`DispatcherQueue.TryEnqueue` /
  `HasThreadAccess`).
- **Pickers** (`FileOpenPicker`, `FolderPicker`, `FileSavePicker`): associate with the window
  via `WinRT.Interop.InitializeWithWindow.Initialize(picker, hwnd)`, where
  `hwnd = WinRT.Interop.WindowNative.GetWindowHandle(App.MainWindow)`.
- **`MessageDialog` → `ContentDialog`** and set `XamlRoot`.
- **Toasts:** `ToastNotificationManager` → `AppNotificationManager` /
  `AppNotificationBuilder`. A packaged app must register a toast COM activator in
  `Package.appxmanifest` (a `windows.toastNotificationActivation` extension + a
  `windows.comServer` `ExeServer`), or `AppNotificationManager.Default.Register()` throws
  "No COM servers are registered".

## AVOID the legacy IE/Trident engine (it fail-fasts in packaged WinAppSDK apps)

`iertutil.dll` crashes with `0xc0000409` for IE/Trident-backed APIs in a packaged process:

- **`WebView` / `WebView2`** — crashed both on teardown and in use during the Clipboard port.
- **`Windows.Data.Html.HtmlUtilities.ConvertToText`** — crashed on call.

Prefer **native controls + managed code**: `RichTextBlock` / `TextBlock` to show formatted
text, a read-only `TextBox` to show markup, and a small managed HTML-to-text helper (regex
strip tags + `System.Net.WebUtility.HtmlDecode`) instead of `ConvertToText`. Only use
`WebView2` if rendering real web content is the actual point of the sample.

## When an API doesn't exist / doesn't work in WinUI

Check the official migration docs on Microsoft Learn before improvising:

- UWP → Windows App SDK migration overview:
  https://learn.microsoft.com/windows/apps/windows-app-sdk/migrate-to-windows-app-sdk/overview
- Mapping UWP features to the Windows App SDK:
  https://learn.microsoft.com/windows/apps/windows-app-sdk/migrate-to-windows-app-sdk/feature-mapping-table

If a feature genuinely cannot be reproduced in WinUI 3 (or needs a different API), document
it in the sample README under "Known differences / limitations" and record the gap in the
session `samples` table / plan notes for the migration thread.

## Per-sample deliverables

- Builds clean (0 errors; nullable warnings are acceptable).
- Launches and runs without crashing (smoke test); full click-through is Niels' manual gate.
- A per-sample `README.md` (use `templates/SAMPLE_README_TEMPLATE.md`): what it shows, APIs
  featured, the Learn doc(s) it serves, build/run, and migration notes + known differences.
