---
name: porting-uwp-samples-to-winui
description: "Port a UWP sample from microsoft/Windows-universal-samples to WinUI 3 / Windows App SDK the way this repo does it — scaffold on the shared SDKTemplate shell (clone an already-ported sample), keep namespace SDKTemplate, rebuild the scenario list via featureGroups, apply the UWP->WinUI transforms (Windows.UI.Xaml->Microsoft.UI.Xaml, CoreDispatcher->DispatcherQueue, Window.Current->App.MainWindow, GetForCurrentView->HWND interop, pickers InitializeWithWindow, MessageDialog->ContentDialog, MediaElement/CaptureElement->MediaPlayerElement), build clean at Debug/x64, smoke-test, then update the tracker + README. Use when porting or consolidating any Windows-universal-samples sample into this repo."
---

This skill is the concrete, repeatable playbook used to port the samples in this repo. It is
more specific than the generic `winui-uwp-migration` skill: it assumes the **SDKTemplate shell**
that every Windows-universal-samples sample shares, and the exact scaffold/verify loop used here.

Read `AGENTS.md` at the repo root first for cross-sample conventions. Working reference samples
live under `Samples/` (e.g. `RadioManager` for a single-scenario shell, `Camera` and `Sensors`
for consolidated multi-feature shells, `BasicInput` for a flat scenario layout).

## Mental model

Every UWP sample in Windows-universal-samples is built on a shared **SDKTemplate** shell:
`App.xaml` + a `MainPage` that renders a scenario list and a `NotifyUser` status bar, plus
per-scenario pages under `shared\` (XAML) and `cs\` (code-behind). You do **not** rebuild that
shell from scratch per sample. Instead you **clone an already-ported sample** (which already has
a working WinUI 3 shell) and drop the new scenarios into it. This keeps every sample identical in
structure and avoids re-solving shell/lifecycle problems.

Key invariants for every ported sample:

- **`namespace SDKTemplate`** everywhere; `RootNamespace=SDKTemplate` in the csproj. The
  SDK-style project auto-globs `*.cs` / `*.xaml`, so there is no need to list files.
- **Grouped NavigationView** driven by `SampleConfiguration.cs` `featureGroups` (a single group
  auto-flattens the nav). `MainPage.Current` + `NotifyUser(...)` are provided by the shell.
- **Built-in WinUI styles only** — no ported `Styles.xaml`. Use `SubtitleTextBlockStyle` for
  headers and `BodyTextBlockStyle` for descriptions. (Legacy `SampleHeaderTextStyle`,
  `ScenarioDescriptionTextStyle`, `BasicTextStyle`, `BaseMessageStyle` all map to those two.)
- App exposes `static App.MainWindow` (the `Window`) and `static App.MainDispatcherQueue`.

## Step 1 — Scaffold by cloning a ported sample

Do not `dotnet new` per sample. Clone the closest existing sample and rename it.

```powershell
$src = "...\winui-samples\Samples\RadioManager"     # a small, single-scenario reference
$dst = "...\winui-samples\Samples\<Name>"
robocopy $src $dst /E /XD bin obj .vs | Out-Null    # /XD skips build + IDE caches
Rename-Item "$dst\RadioManager.csproj" "<Name>.csproj"
Remove-Item "$dst\Scenarios\RadioManager" -Recurse -Force
New-Item -ItemType Directory "$dst\Scenarios\<Name>" | Out-Null
```

Then edit, per sample:

- `MainWindow.xaml` — Title in both places (window Title + TitleBar text).
- `Package.appxmanifest` — a **fresh** `Identity Name` + `PhoneProductId` GUID (use
  `[guid]::NewGuid()`), `DisplayName`, the `VisualElements` DisplayName + Description, and the
  `Capabilities` the sample actually needs (e.g. `location`, `webcam`, `radios`, `bluetooth`).
- `SampleConfiguration.cs` — set `FEATURE_NAME` and rebuild the `featureGroups` list.

If an open Visual Studio instance re-creates a deleted folder's `.vs` cache, close VS or clean
the stray `.vs` afterward.

## Step 2 — Port the scenarios (mechanical first pass)

Copy scenario XAML + code-behind from the UWP source (`shared\` for XAML, `cs\` for code-behind;
if there is no `shared\`, take XAML from `cs\`) into `Scenarios\<Name>\`, then apply:

- **Namespaces:** `Windows.UI.Xaml*` -> `Microsoft.UI.Xaml*`; `Windows.UI.Colors` ->
  `Microsoft.UI.Colors`; `Windows.UI.Text` -> `Microsoft.UI.Text`; `Windows.UI.Input` ->
  `Microsoft.UI.Input`. Fix `xmlns` in `.xaml` too.
- **Remove `using Windows.UI.Core;`** and the `CoreDispatcher` usage (next step).
- **Styles:** replace `SampleHeaderTextStyle` -> `SubtitleTextBlockStyle`,
  `ScenarioDescriptionTextStyle`/`BasicTextStyle`/`BaseMessageStyle` -> `BodyTextBlockStyle`;
  delete any page `Background="{ThemeResource ...}"` so Mica shows through.
- Keep `private MainPage rootPage = MainPage.Current;` and `rootPage.NotifyUser(...)`.

## Step 3 — Fill SampleConfiguration.cs

`FEATURE_NAME` = friendly title (e.g. `"Radio Manager C# sample"`). Rebuild the scenario list.
Single-feature samples use one `FeatureGroup` (auto-flattens); consolidated samples use one
`FeatureGroup` per feature:

```csharp
public const string FEATURE_NAME = "<Friendly name> C# sample";

List<FeatureGroup> featureGroups = new()
{
    new FeatureGroup("<Feature>", new()
    {
        new Scenario { Title = "<Scenario title>", ClassType = typeof(Scenario1_Xyz) },
        ...
    }),
};
```

Some samples use a **flat** `List<Scenario> scenarios` with `ClassType = typeof(Scenario1..N)`
(e.g. `BasicInput`). Match whatever the sample you cloned uses; do not mix the two.

## Step 4 — Apply the runtime transforms

These are the changes that compile in UWP but break at runtime in WinUI 3 desktop:

| UWP | WinUI 3 (this repo) |
|-----|---------------------|
| `Dispatcher.RunAsync(priority, () => ...)` | `DispatcherQueue.TryEnqueue(() => ...)` (drop `async`) |
| `Window.Current` / `MainWindow.Current` | `App.MainWindow` |
| `Window.Current.VisibilityChanged` | `App.MainWindow.VisibilityChanged` (`WindowVisibilityChangedEventArgs`) |
| `FileOpenPicker`/`FolderPicker`/`FileSavePicker` | add `WinRT.Interop.InitializeWithWindow.Initialize(picker, hwnd)` |
| `MessageDialog` (`Windows.UI.Popups`) | `ContentDialog` + `XamlRoot = App.MainWindow.Content.XamlRoot` |
| `*.GetForCurrentView()` | HWND interop `*Interop.GetForWindow(hwnd)` (see gap table) |
| `MediaElement` | `MediaPlayerElement` (`Source = MediaSource.CreateFromStream(...)`) |
| `CaptureElement` | `MediaPlayerElement` + `MediaPlayer` + `MediaSource.CreateFromMediaFrameSource` |

`hwnd = WinRT.Interop.WindowNative.GetWindowHandle(App.MainWindow)`.

## Step 5 — Consolidation (only where it makes sense)

Cluster tightly-related samples into a single project with a grouped, expandable NavigationView:
this repo consolidated the 9 camera samples into `Camera`, the sensor samples into `Sensors`,
and Bluetooth into one project. When consolidating:

- Organize code as `Scenarios\<Feature>\`; put shared helpers in a `Common\` folder (dedupe
  helpers that were copy-pasted per sample).
- **Fix class-name collisions.** Scenario classes repeat across samples
  (`Scenario1_DataEvents` etc.). Rename every class + file + `x:Class` to inject the feature
  name: `Scenario<N>_<Feature><Rest>`.
- Move per-sample `MainPage` helper state into a static `<Feature>Helper` class.
- Add `<AllowUnsafeBlocks>true</AllowUnsafeBlocks>` if any scenario uses `unsafe` (camera frame
  samples do).

## Step 6 — Build, smoke-test, then verify

```powershell
cd Samples\<Name>
dotnet build <Name>.csproj -c Debug -p:Platform=x64 --nologo -v quiet 2>&1 |
  Select-String ': error|Error\(s\)|Build succeeded'
```

Build to **0 errors** (nullable warnings are fine). Then smoke-test — launch, confirm the
process stays alive ~45s, then stop it:

```powershell
$job = Start-Job { dotnet run --project <Name>.csproj -c Debug -p:Platform=x64 --no-build }
Start-Sleep 45
Get-Process <Name> -ErrorAction SilentlyContinue   # alive = pass
Stop-Process -Id <pid>; Remove-Job $job
```

`dotnet run`'s launcher host exits with odd codes (e.g. 3221226107) **after** successfully
launching the packaged app — that is the launcher detaching, not a crash. Trust `Get-Process`.
Full scenario click-through against the UWP original is the manual sign-off gate (Niels does it).

Self-check before marking done: no `.github/instructions` scaffolding, no `Windows.UI.Xaml` left
in any `*.xaml.cs`, README present, and (camera) no leftover `CaptureElement`.

## Step 7 — Record status

Update `port-tracker.md` (the README generator's input) and regenerate the repo `README.md`.
Document any API gap in the sample README under "Known differences / limitations" and in the
tracker so it reaches the migration thread.

## Known API gaps (real, discovered during these ports)

Do not fight these — drop the affected scenario (keep the rest) and flag it.

- **`CaptureElement`** has no WinUI 3 equivalent -> `MediaPlayerElement` +
  `MediaSource.CreateFromMediaFrameSource` (or frame-reader -> `Image` +
  `SoftwareBitmapSource` for preview-frame samples).
- **`InkCanvas` / `InkToolbar`** are not in `Microsoft.UI.Xaml.Controls` (XamlCompiler
  `WMC0001 Unknown type`). Tracked at microsoft/microsoft-ui-xaml#4099. Drop inking scenarios.
- **`DisplayInformation.GetForCurrentView()`** throws on desktop and `CreateForWindowId` is not
  in the projection used here (10.0.19041) -> drop display-orientation / DPI scenarios (a
  rewrite over `XamlRoot.RasterizationScale` + `XamlRoot.Changed`, not a mechanical port).
- **`RadialController`** interop (`IRadialControllerInterop.CreateForWindow`) fails-fast at
  runtime in the packaged app despite being documented as desktop-supported. Gate it off.
- **IE/Trident (`WebView`/`WebView2`, `HtmlUtilities.ConvertToText`)** fail-fast with
  `iertutil.dll 0xc0000409` in a packaged WinAppSDK process. Use native controls + a managed
  HTML-to-text helper instead.
- **Extended-advertising 2M-PHY Bluetooth GATT server APIs** are absent from the 10.0.26100
  projection -> drop those scenarios.
- **`CoreTextEditContext` / `CoreTextServicesManager`** are not available on desktop -> blocks
  custom-edit-control samples.
- **Virtualized `IItemsRangeInfo`/`IList` returning null placeholders** fail-fasts
  (`combase.dll`, `E_POINTER 0x80004003`) in WinUI 3 (UWP tolerated null). Return a shared
  non-null placeholder from the indexer and swap in the real item via `CacheChanged`.
- **Empty `Image Source=""`** fail-fasts at startup -> point to a real asset or leave unset.
- **`Windows.Web.Http.HttpClient`** is ambiguous with `System.Net.Http.HttpClient` on .NET 10 ->
  fully qualify `Windows.Web.Http.HttpClient`.
- **UWP out-of-process background tasks / `Application.OnBackgroundActivated`** have no WinUI 3
  equivalent -> convert to in-process work or drop the background scenario.

## Critical rules

- ❌ Do not rebuild the shell per sample — clone a ported sample.
- ❌ Do not add a custom `Styles.xaml` — use WinUI built-in styles.
- ❌ Do not keep `Windows.UI.Xaml.*`, `CoreDispatcher`, or `Window.Current`.
- ❌ Do not port `Conditional`/`Rewrite`/`Superseded`-verdict samples into the batch — only
  `Migrate` (and whatever the current review column keeps).
- ✅ Give every sample a fresh Identity GUID in `Package.appxmanifest`.
- ✅ Verify with `Get-Process`, not the `dotnet run` exit code.
- ✅ Drop-and-flag unsupported scenarios; keep the rest of the sample working.
