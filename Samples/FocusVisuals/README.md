---
page_type: sample
languages:
- csharp
products:
- windows
- windows-app-sdk
name: "Focus visuals"
urlFragment: FocusVisuals
description: "Shows how to customize focus visuals in a WinUI 3 app."
extendedZipContent:
- path: LICENSE
  target: LICENSE
---

# Focus visuals sample

This sample demonstrates how to customize keyboard focus visuals in a WinUI 3
app that uses the Windows App SDK.

## Features

- Customize the focus visual for an in-box control by defining focus states in
  its control template.
- Apply the system focus visual to a custom control.
- Select a template element as the focus target by using
  `Control.IsTemplateFocusTarget`.
- Run the sample as either a packaged or unpackaged app.

The full control template in this sample is intended to illustrate how focus
states work. Applications should prefer the built-in system focus visual unless
their design requires a custom visual.

## Prerequisites

- Windows 10, version 1809 (build 17763), or later.
- Visual Studio with the .NET desktop development workload.

## Build and run the sample

1. Open `cs-winui\FocusVisualsSample.sln` in Visual Studio.
2. Select an x86, x64, or ARM64 configuration.
3. Select the `FocusVisualsSample (Package)` profile for packaged deployment or
   the `FocusVisualsSample (Unpackaged)` profile for unpackaged deployment.
4. Build and run the solution.

## Related links

- [Keyboard accessibility][keyboard-accessibility]
- [Focus navigation for keyboard, gamepad, remote control, and accessibility
  tools][focus-navigation]
- [Original UWP XAML focus visuals sample][uwp-sample]

[focus-navigation]: https://learn.microsoft.com/windows/apps/design/input/focus-navigation
[keyboard-accessibility]: https://learn.microsoft.com/windows/apps/design/accessibility/keyboard-accessibility
[uwp-sample]: https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/XamlFocusVisuals
