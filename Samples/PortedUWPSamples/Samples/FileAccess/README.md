# FileAccess

Ported to WinUI 3 / Windows App SDK from the UWP [FileAccess](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/FileAccess) sample.

## What it shows

Basic file operations on a single `sample.dat` file created in the Pictures library, across 11 scenarios:

1. **Creating a file** (`StorageFolder.CreateFileAsync`)
2. **Getting a file's parent folder** (`StorageFile.GetParentAsync`)
3. **Writing and reading text** (`FileIO.WriteTextAsync` / `ReadTextAsync`)
4. **Writing and reading bytes** (`FileIO.WriteBufferAsync` / `ReadBufferAsync`)
5. **Writing and reading using a stream** (`StorageFile.OpenAsync` + `DataReader` / `DataWriter`)
6. **Displaying file properties** (`GetBasicPropertiesAsync`, `StorageFile.Properties`)
7. **Persisting access to a storage item for future use** (`StorageApplicationPermissions.FutureAccessList` / `MostRecentlyUsedList`)
8. **Copying a file** (`StorageFile.CopyAsync`)
9. **Comparing two files to see if they are the same file** (`StorageFile.IsEqual`)
10. **Deleting a file** (`StorageFile.DeleteAsync`)
11. **Attempting to get a file with no error on failure** (`StorageFolder.TryGetItemAsync`)

Run scenario 1 first to create `sample.dat`; the other scenarios operate on it.

## APIs featured

- `Windows.Storage` (`StorageFile`, `StorageFolder`, `FileIO`, `KnownFolders`)
- `Windows.Storage.AccessCache.StorageApplicationPermissions`
- `Microsoft.Windows.Storage.Pickers.FileOpenPicker` (Windows App SDK picker)

## Build & run

```powershell
dotnet build -c Debug -p:Platform=x64
dotnet run -c Debug -p:Platform=x64
```

## Migration notes

The scenarios port directly with the standard WinUI 3 transforms: `Windows.UI.Xaml` namespaces updated to `Microsoft.UI.Xaml`, the page background removed so the template Mica backdrop shows through, and the UWP `SampleHeaderTextStyle` / `ScenarioDescriptionTextStyle` / `BaseMessageStyle` replaced with a "Description:" header (`SubtitleTextBlockStyle`) plus `BodyTextBlockStyle` body text.

The one meaningful change is the file picker in **Scenario 9 (compare)**. The UWP original uses `Windows.Storage.Pickers.FileOpenPicker`, which in WinUI 3 desktop would need HWND interop (`IInitializeWithWindow`). Instead it now uses the Windows App SDK picker `Microsoft.Windows.Storage.Pickers.FileOpenPicker`, constructed with the window's `AppWindowId` (`this.XamlRoot.ContentIslandEnvironment.AppWindowId`). That picker returns a path rather than a `StorageFile`, so the result is reopened with `StorageFile.GetFileFromPathAsync` before calling `StorageFile.IsEqual`.

The sample stores `sample.dat` in the Pictures library via `KnownFolders.GetFolderForUserAsync(..., KnownFolderId.PicturesLibrary)`, so the `picturesLibrary` capability is declared in the manifest.

## Known differences / limitations

None block the sample. The only behavioral change is that the file-compare picker uses the Windows App SDK picker API instead of the legacy `Windows.Storage.Pickers` picker.
