//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the Microsoft Public License.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using System;
using System.Collections.Generic;
using System.Globalization;
using System.Threading.Tasks;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Navigation;
using Windows.Devices.Enumeration;
using Windows.Media.Capture;
using Windows.Media.Devices;

namespace SDKTemplate
{
    /// <summary>
    /// EnableHDRProfile page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario3_EnableHdrProfile : Page
    {
        private MainPage? rootPage;

        public Scenario3_EnableHdrProfile()
        {
            InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
        }

        private async void CheckHdrSupportBtn_Click(object sender, RoutedEventArgs e)
        {
            bool hdrVideoSupported = false;
            using MediaCapture mediaCapture = new MediaCapture();
            MediaCaptureInitializationSettings mediaCaptureInitSetttings = new MediaCaptureInitializationSettings();

            await LogStatusToOutputBox("Querying for video capture device on back of the device that supports Video Profile");
            string videoDeviceId = await GetVideoProfileSupportedDeviceIdAsync(Windows.Devices.Enumeration.Panel.Back);

            if (string.IsNullOrEmpty(videoDeviceId))
            {
                await LogStatus("ERROR: No Video Device Id found. Verify your device has a camera that supports video profiles.", NotifyType.ErrorMessage);
                return;
            }

            await LogStatusToOutputBox(string.Format(CultureInfo.InvariantCulture,
                "Found device that supports Video Profile, Device Id:\r\n {0}", videoDeviceId));
            IReadOnlyList<MediaCaptureVideoProfile> profiles = MediaCapture.FindKnownVideoProfiles(videoDeviceId, KnownVideoProfile.VideoRecording);
            await LogStatusToOutputBox(string.Format(CultureInfo.InvariantCulture, "Known video recording profiles found: {0}", profiles.Count));

            foreach (MediaCaptureVideoProfile profile in profiles)
            {
                foreach (MediaCaptureVideoProfileMediaDescription videoProfileMediaDescription in profile.SupportedRecordMediaDescription)
                {
                    if (videoProfileMediaDescription.IsHdrVideoSupported)
                    {
                        mediaCaptureInitSetttings.VideoDeviceId = videoDeviceId;
                        mediaCaptureInitSetttings.VideoProfile = profile;
                        mediaCaptureInitSetttings.RecordMediaDescription = videoProfileMediaDescription;
                        hdrVideoSupported = true;
                        await LogStatus("HDR supported video profile found, set video profile to current HDR supported profile", NotifyType.StatusMessage);
                        break;
                    }
                }

                if (hdrVideoSupported)
                {
                    break;
                }
            }

            if (!hdrVideoSupported)
            {
                await LogStatus("No HDR video profile located. This camera does not report HDR video profile support.", NotifyType.ErrorMessage);
                return;
            }

            await LogStatusToOutputBox("Initializing Media settings to HDR supported video profile");
            if (await TryInitializeMediaCaptureAsync(mediaCapture, mediaCaptureInitSetttings, "Media settings initialized to HDR supported video profile"))
            {
                await LogStatusToOutputBox("Initializing HDR Video Mode to Auto");
                mediaCapture.VideoDeviceController.HdrVideoControl.Mode = HdrVideoMode.Auto;
            }
        }

        public async Task<string> GetVideoProfileSupportedDeviceIdAsync(Windows.Devices.Enumeration.Panel panel)
        {
            await LogStatusToOutputBox("Looking for all video capture devices");
            DeviceInformationCollection devices = await DeviceInformation.FindAllAsync(DeviceClass.VideoCapture);
            await LogStatusToOutputBox(string.Format(CultureInfo.InvariantCulture, "Number of video capture devices found: {0}", devices.Count.ToString(CultureInfo.InvariantCulture)));

            foreach (var device in devices)
            {
                if (MediaCapture.IsVideoProfileSupported(device.Id) && device.EnclosureLocation?.Panel == panel)
                {
                    return device.Id;
                }
            }

            return string.Empty;
        }

        private async Task<bool> TryInitializeMediaCaptureAsync(MediaCapture mediaCapture, MediaCaptureInitializationSettings settings, string successMessage)
        {
            try
            {
                await mediaCapture.InitializeAsync(settings);
                await LogStatusToOutputBox(successMessage);
                return true;
            }
            catch (UnauthorizedAccessException)
            {
                await LogStatus("ERROR: The app was denied access to the camera or microphone.", NotifyType.ErrorMessage);
            }
            catch (Exception ex)
            {
                await LogStatus($"ERROR: MediaCapture initialization failed: {ex.Message}", NotifyType.ErrorMessage);
            }

            return false;
        }

        private Task LogStatusToOutputBox(string message)
        {
            return RunOnUiThreadAsync(() =>
            {
                outputBox.Text += message + "\r\n";
                outputScrollViewer.ChangeView(0, outputBox.ActualHeight, 1);
            });
        }

        private async Task LogStatus(string message, NotifyType type)
        {
            await RunOnUiThreadAsync(() => rootPage?.NotifyUser(message, type));
            await LogStatusToOutputBox(message);
        }

        private Task RunOnUiThreadAsync(Action action)
        {
            if (DispatcherQueue.HasThreadAccess)
            {
                action();
                return Task.CompletedTask;
            }

            TaskCompletionSource<object?> tcs = new TaskCompletionSource<object?>();
            if (!DispatcherQueue.TryEnqueue(() =>
            {
                try
                {
                    action();
                    tcs.SetResult(null);
                }
                catch (Exception ex)
                {
                    tcs.SetException(ex);
                }
            }))
            {
                tcs.SetException(new InvalidOperationException("Unable to enqueue UI update."));
            }

            return tcs.Task;
        }
    }
}