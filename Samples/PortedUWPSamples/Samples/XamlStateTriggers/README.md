# XamlStateTriggers

Ported to WinUI 3 from the UWP [XamlStateTriggers](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/XamlStateTriggers) sample.

## What it shows

A single scenario that demonstrates building responsive, adaptive UI with built-in and custom XAML state triggers. Four trigger types are shown:

1. **AdaptiveTrigger** (built-in): the `RelativePanel` reflows its layout when the window crosses 720 effective pixels wide. Resize the window to see elements rearrange.
2. **InputTypeTrigger** (custom, `StateTriggerBase`): open the "Color Palette Button" flyout. The color swatches use small hit targets when invoked with a mouse and larger hit targets when invoked with touch or pen.
3. **ControlSizeTrigger** (custom, `StateTriggerBase`): in the list, an item whose content grows past 150 effective pixels tall (Item3) switches to a dark background.
4. **StateTrigger bound to a view model property**: an item flips its colors when its `IsColorEnabled` view model property is `true` (Item2). No custom trigger needed, just an inline `StateTrigger` with `IsActive="{Binding IsColorEnabled}"`.

The custom triggers live under `Scenarios/XamlStateTriggers/CustomTriggers/`.

## APIs featured

- `Microsoft.UI.Xaml.StateTriggerBase` (custom trigger authoring)
- `AdaptiveTrigger`, `StateTrigger`, `VisualStateManager`
- `RelativePanel` with simplified `Setter` targets
- `Microsoft.UI.Input.PointerDeviceType` (input-type detection)

## Build & run

```powershell
dotnet build -c Debug -p:Platform=x64
dotnet run -c Debug -p:Platform=x64
```

## Migration notes

Standard WinUI 3 transforms apply: `Windows.UI.Xaml` namespaces updated to `Microsoft.UI.Xaml`, the page background removed so the template Mica backdrop shows through, all namespaces unified to `SDKTemplate`, and the UWP `HeaderTextBlockStyle` replaced with the built-in `TitleTextBlockStyle`.

Two concrete API changes:

- The UWP `WrapGrid` items panel is replaced with `ItemsWrapGrid`, which is the WinUI 3 equivalent for an items control panel (same `Orientation` / `ItemHeight` / `ItemWidth`).
- In the custom `InputTypeTrigger`, `PointerDeviceType` now comes from `Microsoft.UI.Input` instead of the UWP `Windows.Devices.Input`.

## Known differences / limitations

The UWP sample included a fifth demonstration, a custom `DeviceFamilyTrigger` that showed an Xbox-only avatar. That scenario was dropped in this port because these samples do not target Xbox, so the trigger never activated on the supported desktop platform.
