//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using SDKTemplate;
using System;
using Windows.Media;
using Windows.Media.Core;
using Windows.Media.Playback;

namespace SDKTemplate.Controls
{
    /// <summary>
    /// A playback control that hosts a MediaPlayer and demonstrates a particular audio category.
    ///
    /// Migration notes (UWP -> WinUI 3):
    ///  * MediaElement -> MediaPlayerElement + MediaPlayer. The audio category lives on the
    ///    MediaPlayer (MediaPlayerAudioCategory) instead of MediaElement.AudioCategory.
    ///  * SystemMediaTransportControls.GetForCurrentView() is CoreWindow-bound and unavailable on
    ///    desktop. It is replaced with SystemMediaTransportControlsInterop.GetForWindow(hwnd).
    ///  * Window.Current.CoreWindow.Dispatcher.RunAsync(...) -> this.DispatcherQueue.TryEnqueue(...).
    /// </summary>
    public sealed partial class PlaybackControl : UserControl
    {
        // A pointer back to the main page. This is needed if you want to call methods in MainPage
        // such as NotifyUser().
        MainPage rootPage = MainPage.Current;

        bool IsPlaying = false;

        MediaPlayer mediaPlayer;
        SystemMediaTransportControls systemMediaControls = null;

        public PlaybackControl()
        {
            this.InitializeComponent();

            mediaPlayer = new MediaPlayer();
            mediaPlayer.AutoPlay = false;
            // We drive the SMTC manually below, so disable the MediaPlayer's automatic command
            // manager to avoid a competing SMTC integration.
            mediaPlayer.CommandManager.IsEnabled = false;
            OutputMedia.SetMediaPlayer(mediaPlayer);

            // GetForCurrentView() is unavailable on desktop; use the HWND interop instead.
            IntPtr hwnd = WinRT.Interop.WindowNative.GetWindowHandle(App.MainWindow);
            systemMediaControls = SystemMediaTransportControlsInterop.GetForWindow(hwnd);
            systemMediaControls.PropertyChanged += SystemMediaControls_PropertyChanged;
            systemMediaControls.ButtonPressed += SystemMediaControls_ButtonPressed;
            systemMediaControls.IsPlayEnabled = true;
            systemMediaControls.IsPauseEnabled = true;
            systemMediaControls.IsStopEnabled = true;
            systemMediaControls.PlaybackStatus = MediaPlaybackStatus.Closed;
        }

        ~PlaybackControl()
        {
            if (systemMediaControls != null)
            {
                systemMediaControls.PropertyChanged -= SystemMediaControls_PropertyChanged;
                systemMediaControls.ButtonPressed -= SystemMediaControls_ButtonPressed;
            }
        }

        public void Play()
        {
            DispatcherQueue.TryEnqueue(() =>
            {
                mediaPlayer.Play();
                systemMediaControls.PlaybackStatus = MediaPlaybackStatus.Playing;
            });
        }

        public void Pause()
        {
            DispatcherQueue.TryEnqueue(() =>
            {
                mediaPlayer.Pause();
                systemMediaControls.PlaybackStatus = MediaPlaybackStatus.Paused;
            });
        }

        public void Stop()
        {
            DispatcherQueue.TryEnqueue(() =>
            {
                // MediaPlayer has no Stop(); pause and rewind to emulate a stop.
                mediaPlayer.Pause();
                if (mediaPlayer.PlaybackSession != null)
                {
                    mediaPlayer.PlaybackSession.Position = TimeSpan.Zero;
                }
                systemMediaControls.PlaybackStatus = MediaPlaybackStatus.Stopped;
            });
        }

        public void SetAudioCategory(MediaPlayerAudioCategory category)
        {
            mediaPlayer.AudioCategory = category;
        }

        public void SetAudioDeviceType(MediaPlayerAudioDeviceType devicetype)
        {
            mediaPlayer.AudioDeviceType = devicetype;
        }

        public async void SelectFile()
        {
            Windows.Storage.Pickers.FileOpenPicker picker = new Windows.Storage.Pickers.FileOpenPicker();
            picker.SuggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.MusicLibrary;
            picker.FileTypeFilter.Add(".mp3");
            picker.FileTypeFilter.Add(".mp4");
            picker.FileTypeFilter.Add(".m4a");
            picker.FileTypeFilter.Add(".wma");
            picker.FileTypeFilter.Add(".wav");

            // Pickers require HWND association on desktop.
            IntPtr hwnd = WinRT.Interop.WindowNative.GetWindowHandle(App.MainWindow);
            WinRT.Interop.InitializeWithWindow.Initialize(picker, hwnd);

            Windows.Storage.StorageFile file = await picker.PickSingleFileAsync();
            if (file != null)
            {
                var stream = await file.OpenAsync(Windows.Storage.FileAccessMode.Read);
                mediaPlayer.AutoPlay = false;
                DispatcherQueue.TryEnqueue(() =>
                {
                    mediaPlayer.Source = MediaSource.CreateFromStream(stream, file.ContentType);
                });
            }
        }

        void DisplayStatus(string text)
        {
            DispatcherQueue.TryEnqueue(() =>
            {
                Status.Text += text + "\n";
            });
        }

        string GetTimeStampedMessage(string eventText)
        {
            return eventText + "   " + DateTime.Now.Hour.ToString() + ":" + DateTime.Now.Minute.ToString() + ":" + DateTime.Now.Second.ToString();
        }

        //If your app is playing media you feel that a user should not miss if a VOIP call comes in, you may
        //want to consider pausing playback when your app receives a SoundLevel(Low) notification.
        //A SoundLevel(Low) means your app volume has been attenuated by the system (likely for a VOIP call).

        string SoundLevelToString(SoundLevel level)
        {
            string LevelString;

            switch (level)
            {
                case SoundLevel.Muted:
                    LevelString = "Muted";
                    break;
                case SoundLevel.Low:
                    LevelString = "Low";
                    break;
                case SoundLevel.Full:
                    LevelString = "Full";
                    break;
                default:
                    LevelString = "Unknown";
                    break;
            }
            return LevelString;
        }

        void AppMuted()
        {
            DispatcherQueue.TryEnqueue(() =>
            {
                if (mediaPlayer.PlaybackSession != null && mediaPlayer.PlaybackSession.PlaybackState != MediaPlaybackState.Paused)
                {
                    IsPlaying = true;
                    Pause();
                }
                else
                {
                    IsPlaying = false;
                }
            });
            DisplayStatus(GetTimeStampedMessage("Muted"));
        }

        void AppUnmuted()
        {
            DispatcherQueue.TryEnqueue(() =>
            {
                if (IsPlaying)
                {
                    Play();
                }
            });
            DisplayStatus(GetTimeStampedMessage("Unmuted"));
        }

        void SystemMediaControls_PropertyChanged(SystemMediaTransportControls sender, SystemMediaTransportControlsPropertyChangedEventArgs e)
        {
            if (e.Property == SystemMediaTransportControlsProperty.SoundLevel)
            {
                var soundLevelState = sender.SoundLevel;

                DisplayStatus(GetTimeStampedMessage("App sound level is [" + SoundLevelToString(soundLevelState) + "]"));
                if (soundLevelState == SoundLevel.Muted)
                {
                    AppMuted();
                }
                else
                {
                    AppUnmuted();
                }
            }
        }

        void SystemMediaControls_ButtonPressed(SystemMediaTransportControls sender, SystemMediaTransportControlsButtonPressedEventArgs e)
        {
            switch (e.Button)
            {
                case SystemMediaTransportControlsButton.Play:
                    Play();
                    DisplayStatus(GetTimeStampedMessage("Play Pressed"));
                    break;

                case SystemMediaTransportControlsButton.Pause:
                    Pause();
                    DisplayStatus(GetTimeStampedMessage("Pause Pressed"));
                    break;

                case SystemMediaTransportControlsButton.Stop:
                    Stop();
                    DisplayStatus(GetTimeStampedMessage("Stop Pressed"));
                    break;

                default:
                    break;
            }
        }

        private void Button_Play_Click(object sender, RoutedEventArgs e)
        {
            Play();
        }

        private void Button_Pause_Click(object sender, RoutedEventArgs e)
        {
            Pause();
        }

        private void Button_Stop_Click(object sender, RoutedEventArgs e)
        {
            Stop();
        }
    }
}
