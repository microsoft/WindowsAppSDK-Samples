# Basic Input

Ported to WinUI 3 / Windows App SDK from the UWP
[BasicInput](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/BasicInput)
sample.

## What it shows

Basic pointer, gesture, keyboard, mouse, touch, and manipulation input in five scenarios:

1. **Input Events** - pointer pressed/released/entered/exited plus tap, double-tap, holding, and right-tap events.
2. **PointerPoint Properties** - reading pointer location, contact state, wheel/button, touch contact rectangle, and pen pressure properties.
3. **Device Capabilities** - querying keyboard, mouse, and touch capability objects.
4. **XAML Manipulations** - moving and rotating an element with XAML manipulation events and inertia.
5. **Gesture Recognizer** - feeding pointer data to `GestureRecognizer` and applying its manipulation output.

## APIs featured

- `Microsoft.UI.Xaml.Input.PointerRoutedEventArgs`, routed gesture events, and manipulation routed events
- `Microsoft.UI.Input.PointerPoint`, `PointerPointProperties`, `PointerDeviceType`, and `GestureRecognizer`
- `Windows.Devices.Input.KeyboardCapabilities`, `MouseCapabilities`, and `TouchCapabilities`

## Learn docs this serves

- [Handle pointer input](https://learn.microsoft.com/windows/apps/design/input/handle-pointer-input)
- [Touch interactions](https://learn.microsoft.com/windows/apps/design/input/touch-interactions)
- [Input overview](https://learn.microsoft.com/windows/apps/design/input/)

## Build & run

```powershell
dotnet build -c Debug -p:Platform=x64
winapp run
```

(`dotnet run -c Debug -p:Platform=x64` also works.)

## Migration notes

- **Namespaces:** `Windows.UI.Xaml.*` → `Microsoft.UI.Xaml.*`; `Windows.UI.Input.*` → `Microsoft.UI.Input.*`; `Windows.UI.Colors` → `Microsoft.UI.Colors`.
- **Pointer APIs:** `PointerRoutedEventArgs` and XAML input events are under `Microsoft.UI.Xaml.Input`; `PointerPoint`, `PointerPointProperties`, `GestureRecognizer`, and `PointerDeviceType` are under `Microsoft.UI.Input`. WinUI's `PointerPoint` exposes `PointerDeviceType` directly instead of through `PointerDevice.PointerDeviceType`.
- **Shell:** standard NavigationView + InfoBar shell (see repo `AGENTS.md`); Mica backdrop, app icon in the title bar, no page backgrounds. The pointer canvas uses a transparent hit-test surface so it can still receive pointer events without painting over Mica.

### Known differences / limitations

- None. All five scenarios port directly to WinUI 3 / Windows App SDK.
