# XamlMasterDetail

Ported to WinUI 3 / Windows App SDK from the UWP [XamlMasterDetail](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/XamlMasterDetail) sample.

## What it shows

This sample demonstrates the list/details (master/detail) pattern with an adaptive layout. When the window is wide the list of items and the details of the selected item appear side by side. When the window is narrow the layout collapses to a single pane: tapping an item drills in to its details, and an in-page back button returns to the list.

## APIs featured

- `Microsoft.UI.Xaml.Controls.ListView` (`IsItemClickEnabled`, `ItemClick`)
- `Microsoft.UI.Xaml.Controls.Grid` adaptive column sizing (`GridLength`, `GridUnitType.Star`)
- `Microsoft.UI.Xaml.FrameworkElement.SizeChanged`
- `Windows.Globalization.DateTimeFormatting.DateTimeFormatter`

## Learn docs this serves

- [List/details](https://learn.microsoft.com/windows/apps/develop/ui/controls/list-details)

## Build & run

```powershell
dotnet build -c Debug -p:Platform=x64
dotnet run -c Debug -p:Platform=x64
```

## Migration notes

The UWP original is a standalone `MasterDetailApp` (App → MasterDetailPage plus a separate DetailPage) rather than an `SDKTemplate` sample. It was rebuilt as a single scenario page inside the repo's SDKTemplate NavigationView shell.

The narrow-mode drill-in in the UWP sample navigated to a separate `DetailPage` and used `SystemNavigationManager.GetForCurrentView()` for the back button plus `Window.Current.SizeChanged` / `Window.Current.Bounds.Width` for the adaptive breakpoint. None of those are available in WinUI 3 desktop, so the master/detail is implemented as one in-page adaptive layout driven by the page's own `SizeChanged` (narrow when the layout width is under 720 pixels), with an in-page back button instead of the system back button. `Windows.UI.Xaml` namespaces were updated to `Microsoft.UI.Xaml`, and the page background was removed so the template Mica backdrop shows through.

## Known differences / limitations

No API gaps block the pattern itself. The only change from the UWP original is that narrow-mode navigation and the back button are handled in-page rather than through `SystemNavigationManager`, because that API and `Window.Current` are not available in WinUI 3 desktop.
