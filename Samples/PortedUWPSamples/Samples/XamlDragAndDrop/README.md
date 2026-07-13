# XamlDragAndDrop

Ported to WinUI 3 from the UWP [XamlDragAndDrop](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/XamlDragAndDrop) sample.

## What it shows

Three scenarios demonstrating XAML drag and drop with `Windows.ApplicationModel.DataTransfer`:

1. **ListView Drag and Drop and Reorder**: drag items from an "All Items" list into a "Selection" list (copy), reorder items within the Selection list, and drag items onto a trash glyph to remove them. Uses `CanDragItems`, `CanReorderItems`, `AllowDrop`, `DragItemsStarting`, `DragOver`, `Drop`, and `DragItemsCompleted`, with deferrals for asynchronous drop handling.
2. **Drag UI Customization**: customize the drag visual on both the source and the target. The source can show the default dragged element, the standard `DataPackage` icon, or a custom bitmap rendered from the dragged `TextBox`. The target can keep the default drag UI, hide the glyph and content, or supply a custom image via `DragUIOverride.SetContentFromBitmapImage`.
3. **StartDragAsync**: a small timed game that uses `UIElement.StartDragAsync` so the app owns the drag operation and can cancel it (here, when a timer expires). Drag the emoji shape onto its matching tile before time runs out.

## APIs featured

- `Windows.ApplicationModel.DataTransfer` (`DataPackage`, `DataPackageOperation`, `StandardDataFormats`)
- Drag and drop events on `UIElement` and `ListView` (`DragStarting`, `DragOver`, `DragEnter`, `DragLeave`, `Drop`, `DragItemsStarting`, `DragItemsCompleted`)
- `DragUI.SetContentFromSoftwareBitmap` / `DragUIOverride.SetContentFromBitmapImage`
- `UIElement.StartDragAsync`
- `RenderTargetBitmap`, `SoftwareBitmap`

## Build & run

```powershell
dotnet build -c Debug -p:Platform=x64
dotnet run -c Debug -p:Platform=x64
```

## Migration notes

Standard WinUI 3 transforms apply: `Windows.UI.Xaml` namespaces updated to `Microsoft.UI.Xaml`, namespaces unified to `SDKTemplate`, page backgrounds removed so the template Mica backdrop shows through, and the UWP `SampleHeaderTextStyle` / `ScenarioDescriptionTextStyle` replaced with a "Description:" header (`SubtitleTextBlockStyle`) plus `BodyTextBlockStyle`. The drag and drop APIs port directly with no functional changes.

The `dropcursor.png` (custom target drag image) and `Symbols.txt` (emoji list for scenario 3) assets are declared as `Content` in the project so they are packaged and reachable via `ms-appx:///`.

## Known differences / limitations

None. The Windows App SDK exposes the same drag and drop surface as UWP.
