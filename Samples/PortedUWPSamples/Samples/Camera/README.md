# Camera

A single WinUI 3 / Windows App SDK sample that consolidates nine UWP camera samples from the
[Windows-universal-samples](https://github.com/microsoft/Windows-universal-samples) repo into one
app. Each feature lives in its own folder under `Scenarios\` so the code for a given scenario is
easy to find.

## What it shows

The NavigationView groups scenarios by camera feature:

- **Starter kit** - camera preview, photo, and video capture (`Scenarios\StarterKit`).
- **Manual controls** - exposure, flash, focus, ISO, shutter, white balance, and zoom (`Scenarios\ManualControls`).
- **Resolution** - change preview settings, preview + photo settings, and match aspect ratios (`Scenarios\Resolution`).
- **Profile** - locate record-specific profiles, query for concurrency, query for HDR support (`Scenarios\Profile`).
- **Advanced capture** - HDR / low-light advanced photo capture (`Scenarios\AdvancedCapture`).
- **Get preview frame** - grab a single preview frame as a `SoftwareBitmap` (`Scenarios\GetPreviewFrame`).
- **Face detection** - camera preview with the face-detection effect (`Scenarios\FaceDetection`).
- **Frames** - color/depth/IR frame sources and source-group enumeration with `MediaFrameReader` (`Scenarios\Frames`).
- **Video stabilization** - the video-stabilization effect (`Scenarios\VideoStabilization`).

## Project layout

```
Camera/
  App.xaml(.cs), MainWindow.xaml(.cs), MainPage.xaml(.cs)   <- shared shell
  SampleConfiguration.cs                                    <- grouped scenario list
  Common/                                                   <- shared helpers
    CameraRotationHelper.cs
    IMemoryBufferByteAccess.cs
  Scenarios/
    StarterKit/ ManualControls/ Resolution/ Profile/
    AdvancedCapture/ GetPreviewFrame/ FaceDetection/
    Frames/ VideoStabilization/
```

## APIs featured

- `Windows.Media.Capture.MediaCapture` and `Windows.Media.Capture.Frames.MediaFrameReader`
- `Windows.Media.Devices` controls (advanced photo, manual controls), `Windows.Media.Core` effects (face detection, video stabilization)
- `Windows.Media.Capture.MediaCaptureVideoProfile`, `Windows.Media.MediaProperties.VideoEncodingProperties`
- Preview via `Microsoft.UI.Xaml.Controls.MediaPlayerElement` + `MediaSource.CreateFromMediaFrameSource`

## Learn docs this serves

- [Camera overview](https://learn.microsoft.com/windows/uwp/audio-video-camera/)
- [Process media frames with MediaFrameReader](https://learn.microsoft.com/windows/uwp/audio-video-camera/process-media-frames-with-mediaframereader)
- [Camera-independent flip / capture device profiles](https://learn.microsoft.com/windows/uwp/audio-video-camera/camera-profiles)

## Build & run

```powershell
dotnet build -c Debug -p:Platform=x64
winapp run
```

(`dotnet run -c Debug -p:Platform=x64` also works.) The app declares the `webcam` and `microphone`
capabilities, so Windows prompts for camera/microphone access on first run.

## Migration notes

- **Preview rendering:** UWP `CaptureElement` is not available in WinUI 3. Preview uses
  `MediaPlayerElement` with `MediaSource.CreateFromMediaFrameSource(...)` instead.
- **Namespaces:** `Windows.UI.Xaml.*` -> `Microsoft.UI.Xaml.*`; `CoreDispatcher` -> `DispatcherQueue`;
  `Window.Current` -> `App.MainWindow`.
- **Consolidation:** the nine source samples each had an identical shell. This app keeps one shell and
  one copy of each shared helper (`CameraRotationHelper`, `IMemoryBufferByteAccess`,
  `MediaCapturePreviewer`); feature-specific helpers live with their scenario.
- **Shell:** standard NavigationView + InfoBar shell with grouped, expandable feature headers; Mica
  backdrop, app icon in the title bar, no page backgrounds.
