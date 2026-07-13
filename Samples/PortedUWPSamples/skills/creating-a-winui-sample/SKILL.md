---
name: creating-a-winui-sample
description: "Create a brand-new WinUI 3 / Windows App SDK sample from scratch with `dotnet new winui` — pick the right template, strip the scaffolded .github/instructions that conflict with sample code, set up the project (packaged C#, Debug/x64), wire a NavigationView shell + scenario pages, build, and smoke-test with `winapp run` / `dotnet run`. Use when authoring a new WinUI sample (not porting an existing UWP one) in this repo."
---

Use this when you are building a **new** WinUI 3 sample from scratch (no UWP source to port).
If you are porting an existing UWP sample, use the `porting-uwp-samples-to-winui` skill instead —
that clones an existing sample rather than starting from `dotnet new`.

Read `AGENTS.md` at the repo root first for the window-chrome and shell conventions that apply to
every sample in this repo.

## Step 1 — Pick a template

The WinUI templates are installed with the Windows App SDK / Visual Studio workload. List them
with `dotnet new list winui`. The ones that matter:

| Short name | Use for |
|------------|---------|
| `winui` | Blank single-window app — the default starting point for a sample |
| `winui-navview` | App pre-wired with a `NavigationView` shell (good for a multi-scenario sample) |
| `winui-mvvm` | Blank app with an MVVM scaffold (DI, ViewModels) |
| `winui-lib` | Class library |
| `winui-unittest` | Unit Test App (WinUI in Desktop) — use `[UITestMethod]` for XAML tests |

For a scenario-based sample matching this repo, start from `winui` (or `winui-navview` if you
want the NavigationView shell pre-built) and keep code-behind, not MVVM.

## Step 2 — Create the project

```powershell
cd C:\...\winui-samples\Samples
dotnet new winui -n <Name>
```

This produces a **packaged** WinUI 3 app: `App.xaml(.cs)`, `MainWindow.xaml(.cs)`,
`<Name>.csproj`, `Package.appxmanifest`, and an `Assets\` folder. The csproj already has
`<UseWinUI>true</UseWinUI>`, a `net10.0-windows...` TFM, and the `Microsoft.WindowsAppSDK`
package reference.

## Step 3 — Remove the scaffolded instruction files

`dotnet new winui` drops `.github/instructions/*.md` (MVVM / DI / StyleCop / `.resw` guidance)
into the project. These **conflict with faithful code-behind samples** and steer later edits the
wrong way. Delete them:

```powershell
Remove-Item ".\<Name>\.github" -Recurse -Force -ErrorAction SilentlyContinue
```

Confirm no `.github` remains under the sample folder before committing.

## Step 4 — Apply the repo's window chrome and shell

Follow `AGENTS.md`. For a scenario-based sample this means:

- **Mica backdrop** on the window:
  `<Window.SystemBackdrop><MicaBackdrop/></Window.SystemBackdrop>`.
- **App icon + title in the TitleBar** via the `TitleBar` control,
  `ExtendsContentIntoTitleBar = true`, and `SetTitleBar(AppTitleBar)`. No `NavigationView.Header`.
- **No page backgrounds** on scenario pages, so Mica shows through.
- Expose `static App.MainWindow` (the `Window`) from `App`, and navigate the root `Frame` to
  your shell page **after** `App.MainWindow` is assigned (navigating inside the `MainWindow`
  constructor NREs because pages read `App.MainWindow` in `OnNavigatedTo`).
- Shell = a `NavigationView` (`PaneDisplayMode=Left`, `IsSettingsVisible=False`,
  `IsBackButtonVisible=Collapsed`) with your scenarios as menu items and a content `Frame`. Use
  an `InfoBar` for status/error messages.

Tip: the fastest way to get a compliant shell is to copy `App.xaml`, `MainWindow.xaml`,
`MainPage.xaml`, and `SampleConfiguration.cs` from an existing sample under `Samples\` and rename
the namespace/titles, rather than hand-building the chrome.

## Step 5 — Add scenario pages

Add one `Page` per scenario and register them in `SampleConfiguration.cs`. Use built-in WinUI
styles only — `SubtitleTextBlockStyle` for headers, `BodyTextBlockStyle` for descriptions. Do
**not** add a custom `Styles.xaml`.

Declare only the capabilities the sample uses in `Package.appxmanifest`, and give the sample a
unique `Identity Name` + `PhoneProductId` GUID (`[guid]::NewGuid()`).

## Step 6 — Build

```powershell
cd Samples\<Name>
dotnet build <Name>.csproj -c Debug -p:Platform=x64 --nologo -v quiet 2>&1 |
  Select-String ': error|Error\(s\)|Build succeeded'
```

Build to **0 errors** (nullable warnings are acceptable). WinUI has no design surface, so you
verify layout by running, not in a designer.

## Step 7 — Run and smoke-test

Launch the packaged app. `winapp run` registers the AUMID and launches it; `dotnet run` also
works:

```powershell
$job = Start-Job { dotnet run --project <Name>.csproj -c Debug -p:Platform=x64 --no-build }
Start-Sleep 30
Get-Process <Name> -ErrorAction SilentlyContinue   # alive = pass
Stop-Process -Id <pid>; Remove-Job $job
```

`dotnet run`'s launcher exits with an odd code (e.g. 3221226107) **after** launching the packaged
app successfully — that is the launcher detaching, not a crash. Trust `Get-Process` and check the
Application event log for a `<Name>.exe` fault, not the exit code.

## Step 8 — README

Add a `Samples\<Name>\README.md` from `templates\SAMPLE_README_TEMPLATE.md`: what the sample
shows, the APIs featured, build/run steps, and any known limitations.

## APIs to avoid / prefer in new samples

- ❌ `Window.Current`, `CoreDispatcher` / `Dispatcher.RunAsync`, `MessageDialog`,
  `*.GetForCurrentView()`, `MediaElement`, `CaptureElement`, `InkCanvas` — none work in WinUI 3
  desktop. Use `App.MainWindow`, `DispatcherQueue.TryEnqueue`, `ContentDialog` (+ `XamlRoot`),
  HWND interop, and `MediaPlayerElement`.
- ❌ `WebView`/`WebView2` + `HtmlUtilities.ConvertToText` — the legacy IE/Trident engine
  fail-fasts (`iertutil.dll 0xc0000409`) in a packaged app. Prefer native controls + managed
  text handling unless rendering real web content is the point of the sample.
- ✅ Initialize pickers / Share / Print with
  `WinRT.Interop.InitializeWithWindow.Initialize(obj, hwnd)`.
- ✅ Add a toast COM activator to `Package.appxmanifest` before calling
  `AppNotificationManager.Default.Register()`.

## Critical rules

- ❌ Never keep the scaffolded `.github/instructions` in a sample.
- ❌ Never run the raw `.exe` — launch the packaged app via `winapp run` / `dotnet run`.
- ❌ Never add a custom `Styles.xaml` — use WinUI built-in styles.
- ✅ Verify success with `Get-Process`, not the `dotnet run` exit code.
- ✅ Follow the `AGENTS.md` window-chrome standard on every new sample.
