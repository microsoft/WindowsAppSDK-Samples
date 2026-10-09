---
page_type: sample
languages:
- csharp
products:
- windows
- windows-app-sdk
name: "Input samples"
urlFragment: Input
description: "Shows input, pointer, device, and manipulation APIs in WinUI 3."
extendedZipContent:
- path: LICENSE
  target: LICENSE
---

# Input samples

This WinUI 3 sample demonstrates input APIs from the Windows App SDK, XAML,
and Windows platform APIs.

## Scenarios

- **Gesture recognizer** recognizes pointer gestures.
- **Gesture recognizer manipulations** applies recognized manipulations to
  XAML content.
- **Cursor** demonstrates custom input cursors.
- **Keyboard Shortcut Manager** demonstrates app-wide shortcut registration.
- **Pointer Tracking** tracks multiple pointers, highlights the primary
  pointer, and reports pointer lifecycle events.
- **Pointer Point Properties** displays common and device-specific properties
  for mouse, pen, and touch input.
- **Device Capabilities** reports available keyboard, mouse, and touch
  capabilities.
- **XAML Manipulations** demonstrates translation, rotation, and inertia using
  XAML manipulation events.

## Prerequisites

- Windows 10, version 1809 (build 17763), or later.
- Visual Studio with the .NET desktop development workload.

## Build and run the sample

1. Open `Input.sln` in Visual Studio.
2. Select an x86, x64, or ARM64 configuration.
3. Build and run the `Input` project.

Pointer Tracking and Pointer Point Properties are best explored with multiple
input devices. Touch-specific behavior requires a touch-capable display.

## Related links

- [Handle pointer input][pointer-input]
- [Identify input devices][input-devices]
- [Touch interactions][touch-interactions]
- [Touchpad interactions][touchpad-interactions]
- [Windows App SDK][windows-app-sdk]

[input-devices]: https://learn.microsoft.com/windows/apps/develop/input/identify-input-devices
[pointer-input]: https://learn.microsoft.com/windows/apps/develop/input/handle-pointer-input
[touch-interactions]: https://learn.microsoft.com/windows/apps/develop/input/touch-interactions
[touchpad-interactions]: https://learn.microsoft.com/windows/apps/develop/input/touchpad-interactions
[windows-app-sdk]: https://learn.microsoft.com/windows/apps/windows-app-sdk/
