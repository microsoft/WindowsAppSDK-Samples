# ApplicationData

Ported to WinUI 3 / Windows App SDK from the UWP
[ApplicationData](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/ApplicationData) sample.

## What it shows

This sample shows how to store and retrieve per-user app data using the Windows Runtime application data APIs. It demonstrates local, local cache, temporary, and roaming app-data folders, local settings and setting containers, composite settings, the `ms-appdata://` URI scheme, clearing app data, and app data versioning.

## APIs featured

- `Windows.Storage.ApplicationData`
- `Windows.Storage.ApplicationDataContainer`
- `Windows.Storage.ApplicationDataCompositeValue`
- `Windows.Storage.StorageFolder`, `StorageFile`, and `FileIO`
- `StorageFile.GetFileFromApplicationUriAsync` and `ms-appdata://` URIs
- `Microsoft.UI.Xaml.Controls.Image` with `Microsoft.UI.Xaml.Media.Imaging.BitmapImage`

## Learn docs this serves

- [Store and retrieve settings and other app data](https://learn.microsoft.com/windows/apps/develop/data/store-and-retrieve-app-data)
- [Windows.Storage.ApplicationData](https://learn.microsoft.com/uwp/api/windows.storage.applicationdata)

## Build & run

```powershell
dotnet build -c Debug -p:Platform=x64
dotnet run -c Debug -p:Platform=x64
```

## Migration notes

This is a faithful WinUI 3 / Windows App SDK port that keeps the original `SDKTemplate` namespace and code-behind scenario structure while using the shared NavigationView/InfoBar shell from `templates\ShellTemplate`. XAML namespaces were moved from UWP XAML to WinUI 3 (`Microsoft.UI.Xaml` in code-behind), and scenario pages do not set explicit page backgrounds so the shell Mica backdrop shows through.

Known differences / limitations:

- The source UWP sample uses `ApplicationData.RoamingFolder` for the `ms-appdata:///roaming/...` image example. The API is still available to packaged apps, but Microsoft Learn states that roaming app data and settings are no longer supported as of Windows 11 and recommends using a service such as Azure App Service for cross-device synchronization. This port keeps the roaming folder scenario for API compatibility, but it behaves as local package data and does not demonstrate real cross-device roaming on Windows 11.
- The current upstream UWP sample snapshot only contains shared XAML for scenarios 6 and 7. The XAML for scenarios 1 through 5 was recreated to match the original code-behind control names and event handlers.
