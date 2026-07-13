# BackgroundMediaPlayback

Ported to WinUI 3 / Windows App SDK from the UWP [BackgroundMediaPlayback](https://github.com/microsoft/Windows-universal-samples/tree/main/Samples/BackgroundMediaPlayback) sample.

## What it shows

This sample demonstrates playing a playlist with `MediaPlayer`, `MediaPlaybackList`, `MediaPlaybackItem` display properties, `MediaPlayerElement`, and custom transport controls.

## APIs featured

- `Windows.Media.Playback.MediaPlayer`
- `Windows.Media.Playback.MediaPlaybackList`
- `Windows.Media.Playback.MediaPlaybackItem`
- `Windows.Media.Core.MediaSource`
- `Microsoft.UI.Xaml.Controls.MediaPlayerElement`

## Learn docs this serves

- [Media playback](https://learn.microsoft.com/windows/uwp/audio-video-camera/media-playback)
- [Media items, playlists, and tracks](https://learn.microsoft.com/windows/uwp/audio-video-camera/media-playback-with-mediasource)
- [Migrate from UWP to the Windows App SDK](https://learn.microsoft.com/windows/apps/windows-app-sdk/migrate-to-windows-app-sdk/overview)

## Build & run

```powershell
dotnet build -c Debug -p:Platform=x64
dotnet run -c Debug -p:Platform=x64
```

## Migration notes

The playback code ports directly because `MediaPlayer`, `MediaPlaybackList`, and related media APIs remain in the `Windows.Media.*` namespaces. XAML and page code were updated from `Windows.UI.Xaml` to `Microsoft.UI.Xaml`, and UWP `CoreDispatcher` usage was replaced with WinUI `DispatcherQueue`.

The UWP sample relied on the `backgroundMediaPlayback` manifest capability and app lifecycle/background notifications to keep audio running after the app left the foreground. WinUI 3 desktop apps are not suspended like UWP apps while the process is running, so media continues to play as long as the app remains open and no UWP background-audio declaration is used here. UWP-only lifecycle toast/background-task plumbing was omitted.

## Known differences / limitations

- Closing the desktop app ends playback because there is no UWP background task or suspended-app background audio mode in this port.
- UWP memory-limit and EnteredBackground/LeavingBackground guidance is documented in the original sample but is not implemented for the WinUI desktop app model.
