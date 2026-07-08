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
using System.Globalization;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Navigation;
using Windows.Devices.Enumeration;
using Windows.Media.Capture;

namespace SDKTemplate
{
    /// <summary>
    /// ConcurrentProfile page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario2_ConcurrentProfile : Page
    {
        private MainPage? rootPage;

        public Scenario2_ConcurrentProfile()
        {
            InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
        }

        private async void CheckConcurrentProfileBtn_Click(object sender, RoutedEventArgs e)
        {
            bool concurrencySupported = false;

            using MediaCapture mediaCaptureFront = new MediaCapture();
            using MediaCapture mediaCaptureBack = new MediaCapture();

            await LogStatusToOutputBox("Looking for all video capture devices on front panel");
            string frontVideoDeviceId = await GetVideoProfileSupportedDeviceIdAsync(Windows.Devices.Enumeration.Panel.Front);
            await LogStatusToOutputBox("Looking for all video capture devices on back panel");
            string backVideoDeviceId = await GetVideoProfileSupportedDeviceIdAsync(Windows.Devices.Enumeration.Panel.Back);

            if (string.IsNullOrEmpty(frontVideoDeviceId) || string.IsNullOrEmpty(backVideoDeviceId))
            {
                await LogStatus("ERROR: A front/back capture device pair with Video Profile support was not found.", NotifyType.ErrorMessage);
                return;
            }

            await LogStatusToOutputBox("Video Profiles are supported on both cameras");

            MediaCaptureInitializationSettings mediaInitSettingsFront = new MediaCaptureInitializationSettings
            {
                VideoDeviceId = frontVideoDeviceId
            };
            MediaCaptureInitializationSettings mediaInitSettingsBack = new MediaCaptureInitializationSettings
            {
                VideoDeviceId = backVideoDeviceId
            };

            await LogStatusToOutputBox(string.Format(CultureInfo.InvariantCulture, "Front Device Id located: {0}", frontVideoDeviceId));
            await LogStatusToOutputBox(string.Format(CultureInfo.InvariantCulture, "Back Device Id located: {0}", backVideoDeviceId));
            await LogStatusToOutputBox("Querying for concurrent profile");

            var concurrentProfile = (from frontProfile in MediaCapture.FindConcurrentProfiles(frontVideoDeviceId)
                                     from backProfile in MediaCapture.FindConcurrentProfiles(backVideoDeviceId)
                                     where frontProfile.Id == backProfile.Id
                                     select new { frontProfile, backProfile }).FirstOrDefault();

            if (concurrentProfile != null)
            {
                mediaInitSettingsFront.VideoProfile = concurrentProfile.frontProfile;
                mediaInitSettingsBack.VideoProfile = concurrentProfile.backProfile;
                concurrencySupported = true;
                await LogStatus("Concurrent profile located, device supports concurrency", NotifyType.StatusMessage);
            }
            else
            {
                mediaInitSettingsFront.VideoProfile = null;
                mediaInitSettingsBack.VideoProfile = null;
                await LogStatus("No concurrent profiles located, device does not support concurrency", NotifyType.ErrorMessage);
            }

            await LogStatusToOutputBox("Initializing Front camera settings");
            if (!await TryInitializeMediaCaptureAsync(mediaCaptureFront, mediaInitSettingsFront, "Front camera settings initialized"))
            {
                return;
            }

            if (concurrencySupported)
            {
                await LogStatusToOutputBox("Device supports concurrency, initializing Back camera settings");
                await TryInitializeMediaCaptureAsync(mediaCaptureBack, mediaInitSettingsBack, "Back camera settings initialized");
            }
        }

        public async Task<string> GetVideoProfileSupportedDeviceIdAsync(Windows.Devices.Enumeration.Panel panel)
        {
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