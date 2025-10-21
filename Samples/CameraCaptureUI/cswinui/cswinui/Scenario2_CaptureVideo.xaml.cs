// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Media.Imaging;
using Windows.Storage;
using Windows.Storage.Streams;
using Microsoft.Windows.Media.Capture;
using Windows.Media.Core;
using Windows.Media.Playback;
using Windows.Foundation;
using WinRT.Interop;
using System;

namespace cswinui
{
    public partial class Scenario2_CaptureVideo : Page
    {
        private MainPage rootPage;
        public Scenario2_CaptureVideo()
        {
            this.InitializeComponent();
            rootPage = MainPage.Current;
        }
        private async void CaptureVideo_Click(object sender, RoutedEventArgs e)
        {
            // We have exposed a public variable called MainWindow in App.xaml.cs to fetch the window.
            var windowId = ((App)Application.Current).MainWindow.AppWindow.Id;
            // Initialize CameraCaptureUI with a window handle
            CameraCaptureUI cameraUI = new CameraCaptureUI(windowId);

            // Optionally set video settings such as format, resolution, etc.
            cameraUI.VideoSettings.Format = CameraCaptureUIVideoFormat.Mp4;

            // -- Optional parameters for video capture --
            cameraUI.VideoSettings.MaxDurationInSeconds = 30;
            cameraUI.VideoSettings.MaxResolution = CameraCaptureUIMaxVideoResolution.HighestAvailable;
            cameraUI.VideoSettings.AllowTrimming = true;

            // -- Notes --
            //By default, AllowTrimming is true.
            //MaxDurationInSeconds must be a valid value(i.e., the duration should be in the range of 0 to UINT32_MAX).Specifying an invalid value will result in an invalid argument exception.
            //MaxDurationInSeconds cannot be set if AllowTrimming is false.Attempting to do so will result in an invalid argument exception.

            //Capture the video asynchronously
            var file = await cameraUI.CaptureFileAsync(CameraCaptureUIMode.Video);

            // Check if the capture was successful
            if (file != null)
            {
                rootPage.NotifyUser("Video successfully captured!", InfoBarSeverity.Success);

                // Get the file path of the captured video
                string filePath = file.Path;

                // Open the file as a stream
                var fileStream = await file.OpenAsync(FileAccessMode.Read);

                // Create a MediaSource from the file stream
                var mediaSource = MediaSource.CreateFromStream(fileStream, file.ContentType);

                // Create a MediaPlayer to play the video
                var mediaPlayer = new MediaPlayer();

                // Set the media source for the MediaPlayer
                mediaPlayer.Source = mediaSource;

                // Set the MediaPlayer to the MediaPlayerElement control in the UI
                MediaPlayElement.SetMediaPlayer(mediaPlayer);

                // Play the video
                mediaPlayer.Play();
            }
            else
            {
                // If the capture failed, notify the user
                rootPage.NotifyUser("Video Capture Failed", InfoBarSeverity.Error);
            }
            return;
        }
        private void ClearMessage_Click(object sender, RoutedEventArgs e)
        {
            rootPage.NotifyUser("", InfoBarSeverity.Informational, false);
        }
    }
}
