// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Media.Imaging;
using Windows.Storage;
using Microsoft.Windows.Media.Capture;
using Microsoft.UI;
using System;

namespace cswinui
{
    public partial class Scenario1_CapturePhoto : Page
    {
        private MainPage rootPage = MainPage.Current;

        public Scenario1_CapturePhoto()
        {
            this.InitializeComponent();
        }

        private async void CapturePhoto_Click(object sender, RoutedEventArgs e)
        {
            // We have exposed a public variable called MainWindow in App.xaml.cs to fetch the window.
            var windowId = ((App)Application.Current).MainWindow.AppWindow.Id;
            // Initialize CameraCaptureUI with a window 
            CameraCaptureUI cameraUI = new CameraCaptureUI(windowId);

            // Optionally, set photo settings such as format, aspect ratio, etc.
            cameraUI.PhotoSettings.Format = CameraCaptureUIPhotoFormat.Jpeg;

            // -- Optional parameters for photo capture --
            //cameraUI.PhotoSettings.MaxResolution = CameraCaptureUIMaxPhotoResolution.HighestAvailable;
            //cameraUI.PhotoSettings.CroppedAspectRatio = new Windows.Foundation.Size(16, 9);
            //cameraUI.PhotoSettings.CroppedSizeInPixels = new Windows.Foundation.Size(200, 200);

            // -- Notes --
            //By default, AllowTrimming is true.
            //MaxDurationInSeconds must be a valid value(i.e., the duration should be in the range of 0 to UINT32_MAX).Specifying an invalid value will result in an invalid argument exception.
            //MaxDurationInSeconds cannot be set if AllowTrimming is false.Attempting to do so will result in an invalid argument exception.

            // Capture the photo asynchronously
            var file = await cameraUI.CaptureFileAsync(CameraCaptureUIMode.Photo);

            // Check if a file was captured successfully
            if (file != null)
            {
                rootPage.NotifyUser("Photo successfully captured!", InfoBarSeverity.Success);

                // Get the file path of the captured image
                string filePath = file.Path;

                // Create a Uri from the file path
                Uri fileUri = new Uri(filePath);

                // Open the file and display it in the Image control
                var fileStream = await file.OpenAsync(FileAccessMode.Read);

                // Create a BitmapImage and set its source to the captured file's stream
                BitmapImage bitmapImage = new BitmapImage();
                bitmapImage.UriSource = fileUri;
                CapturedImage.Source = bitmapImage;
            }
            else
            {
                // If the capture failed, notify the user
                rootPage.NotifyUser("Photo Capture Failed", InfoBarSeverity.Error);
            }
        }

        private void ClearMessage_Click(object sender, RoutedEventArgs e)
        {
            rootPage.NotifyUser("", InfoBarSeverity.Informational, false);
        }
    }
}
