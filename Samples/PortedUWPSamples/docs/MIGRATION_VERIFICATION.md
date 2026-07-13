# Migration & verification guide

Every sample follows the same loop, driven by the `winui-uwp-migration` skill
(win-dev-skills PR #88). A sample is **not done** until manual verification passes.

## Per-sample loop

1. **Audit** the UWP source for UWP-only / moved APIs:
   ```powershell
   Select-String -Path (Get-ChildItem -Recurse -Include *.cs,*.xaml | Where-Object { $_.FullName -notlike "*\obj\*" -and $_.FullName -notlike "*\bin\*" }) -Pattern "Windows\.UI\.Xaml|Windows\.UI\.Core|Windows\.UI\.Popups|Window\.Current|GetForCurrentView|CoreDispatcher|MessageDialog|IBackgroundTask|MediaElement|InkCanvas|WebAuthenticationBroker"
   ```
2. **Scaffold** a WinUI 3 project; set `<RootNamespace>` to match the UWP namespace; build empty shell first.
3. **Port** code + XAML, applying the migration rules (below).
4. **Build** until clean.
5. **Verify manually** (gate — see checklist).
6. **Document**: per-sample README + link the Learn doc(s) it serves.
7. **Sign-off**: mark done only after manual verification passes; log any gaps.

## Migration rules (from the skill — most common breakages)

- `Windows.UI.Xaml.*` → `Microsoft.UI.Xaml.*` (and `Windows.UI.Colors/Text/Composition/Input` → `Microsoft.UI.*`).
- `CoreDispatcher` / `Dispatcher.RunAsync` → `DispatcherQueue.TryEnqueue`.
- `Window.Current` → `App.MainWindow` static; `ApplicationView` / `CoreWindow` → `AppWindow`.
- `MessageDialog` → `ContentDialog`; every `ContentDialog` needs `XamlRoot`.
- Pickers / Share / Print → `WinRT.Interop.InitializeWithWindow.Initialize(obj, hwnd)`.
- `GetForCurrentView()` → window-scoped `AppWindow` equivalents.
- `MediaElement` → `MediaPlayerElement`.
- Activation → `AppInstance.GetActivatedEventArgs` (AppLifecycle).
- Resources → MRT Core `ResourceLoader`; DirectWrite → DWriteCore.

## Post-migration check (no UWP-only APIs left)

```powershell
Select-String -Path (Get-ChildItem -Recurse -Include *.cs,*.xaml | Where-Object { $_.FullName -notlike "*\obj\*" -and $_.FullName -notlike "*\bin\*" }) -Pattern "Windows\.UI\.Xaml|Windows\.UI\.Popups\.MessageDialog|Window\.Current|CoreDispatcher|GetForCurrentView|IBackgroundTask|\bMediaElement\b"
```

## Manual verification checklist (per sample — the gate)

- [ ] Project builds clean (Debug, x64), packaged and unpackaged where applicable.
- [ ] App launches via `winapp run` (not the raw `.exe`).
- [ ] Every scenario from the original UWP sample is present and reachable.
- [ ] Each scenario produces the **same observable result** as the UWP original.
- [ ] No runtime exceptions (dialogs show, pickers open, dispatcher updates land).
- [ ] Visual parity reviewed; intentional styling/UX differences noted in the README.
- [ ] Post-migration grep returns no UWP-only APIs.
- [ ] Per-sample README written, with links to the Learn doc(s) it serves.
- [ ] Gaps (missing/changed APIs, behavior deltas) recorded in the tracker.
