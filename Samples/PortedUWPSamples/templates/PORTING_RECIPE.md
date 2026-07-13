# Per-sample porting recipe (UWP → WinUI 3)

Follow this exactly to port one UWP sample. Read `AGENTS.md` (repo root) first for the
conventions and the IE/Trident warning. The **Clipboard** and **DateTimeFormatting** samples
under `Samples/` are working references.

Paths (replace `<Name>` with the sample name, e.g. `Calendar`):

- Template:  `templates\ShellTemplate`
- Source:    `..\Windows-universal-samples\Samples\<Name>` (use the `cs\` and `shared\` folders)
- Target:    `Samples\<Name>`

## 1. Scaffold from the template

```powershell
$tpl="...\winui-samples\templates\ShellTemplate"
$dst="...\winui-samples\Samples\<Name>"
$src="...\Windows-universal-samples\Samples\<Name>"
$feature="<Friendly feature name> C# sample"   # e.g. "Calendar C# sample"

if (Test-Path $dst) { Remove-Item $dst -Recurse -Force }
Copy-Item $tpl $dst -Recurse
Rename-Item "$dst\__SampleName__.csproj" "<Name>.csproj"
foreach ($f in @("MainWindow.xaml","Package.appxmanifest")) {
  $p="$dst\$f"; (Get-Content $p -Raw).Replace('__FEATURE_NAME__',$feature).Replace('__SampleName__','<Name>') | Set-Content $p
}
```

If the source has no `shared\` folder, take XAML from `cs\` instead.

## 2. Port the scenarios (mechanical first pass)

```powershell
# code-behind: UWP -> WinUI namespaces
Get-ChildItem "$src\cs" -Filter "*.xaml.cs" | Where-Object { $_.Name -notmatch 'App\.|MainPage\.' } | ForEach-Object {
  (Get-Content $_.FullName -Raw).Replace('Windows.UI.Xaml','Microsoft.UI.Xaml') | Set-Content "$dst\$($_.Name)"
}
# XAML: strip the page background so Mica shows through
Get-ChildItem "$src\shared" -Filter "Scenario*.xaml" | ForEach-Object {
  (Get-Content $_.FullName -Raw).Replace(' Background="{ThemeResource ApplicationPageBackgroundThemeBrush}"','') | Set-Content "$dst\$($_.Name)"
}
```

## 3. Fill `SampleConfiguration.cs`

Set `FEATURE_NAME` to the friendly title and copy the `scenarios` list from the source
`cs\SampleConfiguration.cs` (keep the same titles and `ClassType` names).

## 4. Build, then fix sample-specific issues

`dotnet build -c Debug -p:Platform=x64` until 0 errors. Common fixes (see AGENTS.md):

- `Windows.UI.Xaml.*` left over → `Microsoft.UI.Xaml.*` (also in any `.xaml` `xmlns` or
  helper `.cs`).
- `rootPage`/`MainPage.Current` + `NotifyUser` already exist in the shell — keep them.
- `CoreDispatcher` → `DispatcherQueue`; `await Dispatcher.RunAsync(...)` →
  `DispatcherQueue.TryEnqueue(...)`.
- `Window.Current` → `App.MainWindow`.
- Pickers (`FileOpenPicker`/`FolderPicker`/`FileSavePicker`): add
  `WinRT.Interop.InitializeWithWindow.Initialize(picker, WinRT.Interop.WindowNative.GetWindowHandle(App.MainWindow));`
- `MessageDialog` → `ContentDialog` (+ `XamlRoot = App.MainWindow.Content.XamlRoot`).
- Toasts: register `AppNotificationManager.Default.Register()` in `App.OnLaunched` AND add
  the toast COM activator to `Package.appxmanifest` (copy the block from Clipboard, set
  `Executable="<Name>.exe"`).
- **AVOID `WebView`/`WebView2` and `Windows.Data.Html.HtmlUtilities`** — use native controls
  + managed HTML-to-text (see CopyText.xaml.cs in Clipboard).
- If an API has no WinUI equivalent, consult MS Learn migration docs (links in AGENTS.md)
  and document the gap in the README "Known differences".

## 5. Smoke-test

`dotnet run -c Debug -p:Platform=x64`, confirm `Get-Process <Name>` shows it running and the
Application event log has no `<Name>.exe` fault. Close it. Full click-through is Niels' gate.

## 6. README

Copy `templates\SAMPLE_README_TEMPLATE.md` → `Samples\<Name>\README.md` and fill in: what it
shows, APIs featured, the Learn doc(s) it serves, build/run, migration notes, known
differences.
