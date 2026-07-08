# XamlFocusVisuals

Ported to WinUI 3 / Windows App SDK from the UWP [XamlFocusVisuals](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/XamlFocusVisuals) sample.

## What it shows

This sample demonstrates XAML focus visuals in WinUI, including custom focus visuals for an in-box control and applying system focus visuals to a custom templated control.

## APIs featured

- `Microsoft.UI.Xaml.Controls.Control.UseSystemFocusVisuals`
- `Microsoft.UI.Xaml.Controls.Control.IsTemplateFocusTarget`
- `Microsoft.UI.Xaml.VisualStateManager`
- `Microsoft.UI.Xaml.Controls.HyperlinkButton`

## Learn docs this serves

- [Keyboard interactions](https://learn.microsoft.com/windows/apps/design/input/keyboard-interactions)
- [Focus navigation for keyboard, gamepad, remote control, and accessibility tools](https://learn.microsoft.com/windows/apps/design/input/focus-navigation)

## Build & run

```powershell
dotnet build -c Debug -p:Platform=x64
dotnet run -c Debug -p:Platform=x64
```

## Migration notes

The UWP `Windows.UI.Xaml` namespaces were updated to `Microsoft.UI.Xaml`, and scenario backgrounds were removed so the template Mica backdrop shows through. The sample keeps the SDKTemplate code-behind structure and uses the WinUI 3 NavigationView shell.

## Known differences / limitations

No API gaps were found. The focus visual APIs used by the UWP sample are available in WinUI 3.
