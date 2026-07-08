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
    /// SetRecordProfile page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario1_SetRecordProfile : Page
    {
        private MainPage? rootPage;

        public Scenario1_SetRecordProfile()
        {
            InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
        }

        private async void InitRecordProfileBtn_Click(object sender, RoutedEventArgs e)
        {
            using MediaCapture mediaCapture = new MediaCapture();
            MediaCaptureInitializationSettings mediaInitSettings = new MediaCaptureInitializationSettings();

            string videoDeviceId = await GetVideoProfileSupportedDeviceIdAsync(Windows.Devices.Enumeration.Panel.Back);
            if (string.IsNullOrEmpty(videoDeviceId))
            {
                await LogStatus("ERROR: No Video Device Id found. Verify your device has a camera that supports video profiles.", NotifyType.ErrorMessage);
                return;
            }

            await LogStatusToOutputBox(string.Format(CultureInfo.InvariantCulture, "Found video capture device that supports Video Profile, Device Id:\r\n {0}", videoDeviceId));
            await LogStatusToOutputBox("Retrieving all Video Profiles");
            IReadOnlyList<MediaCaptureVideoProfile> profiles = MediaCapture.FindAllVideoProfiles(videoDeviceId);
            await LogStatusToOutputBox(string.Format(CultureInfo.InvariantCulture, "Number of Video Profiles found: {0}", profiles.Count));

            if (profiles.Count == 0)
            {
                await LogStatus("ERROR: The selected video capture device reported profile support but returned no profiles.", NotifyType.ErrorMessage);
                return;
            }

            foreach (var profile in profiles)
            {
                foreach (var desc in profile.SupportedRecordMediaDescription)
                {
                    await LogStatusToOutputBox(string.Format(CultureInfo.InvariantCulture,
                        "Video Profile Found: Profile Id: {0}\r\n Width: {1} Height: {2} FrameRate: {3}", profile.Id, desc.Width, desc.Height, Math.Round(desc.FrameRate).ToString(CultureInfo.InvariantCulture)));
                }
            }

            await LogStatusToOutputBox("Querying for matching WVGA 30FPS Profile");
            var match = (from profile in profiles
                         from desc in profile.SupportedRecordMediaDescription
                         where desc.Width == 640 && desc.Height == 480 && Math.Round(desc.FrameRate) == 30
                         select new { profile, desc }).FirstOrDefault();

            mediaInitSettings.VideoDeviceId = videoDeviceId;
            if (match != null)
            {
                mediaInitSettings.VideoProfile = match.profile;
                mediaInitSettings.RecordMediaDescription = match.desc;
                await LogStatus(string.Format(CultureInfo.InvariantCulture,
                        "Matching WVGA 30FPS Video Profile found: \r\n Profile Id: {0}\r\n Width: {1} Height: {2} FrameRate: {3}",
                        mediaInitSettings.VideoProfile.Id, mediaInitSettings.RecordMediaDescription.Width,
                        mediaInitSettings.RecordMediaDescription.Height, Math.Round(mediaInitSettings.RecordMediaDescription.FrameRate).ToString(CultureInfo.InvariantCulture)), NotifyType.StatusMessage);
            }
            else
            {
                mediaInitSettings.VideoProfile = profiles[0];
                await LogStatus("Could not locate a matching profile, setting to default recording profile", NotifyType.ErrorMessage);
            }

            await TryInitializeMediaCaptureAsync(mediaCapture, mediaInitSettings, "Media Capture settings initialized to selected profile");
        }

        private async void InitCustomProfileBtn_Click(object sender, RoutedEventArgs e)
        {
            using MediaCapture mediaCapture = new MediaCapture();
            MediaCaptureInitializationSettings mediaInitSettings = new MediaCaptureInitializationSettings();
            string customProfileId = "{A0E517E8-8F8C-4F6F-9A57-46FC2F647EC0},0";

            string videoDeviceId = await GetVideoProfileSupportedDeviceIdAsync(Windows.Devices.Enumeration.Panel.Back);
            if (string.IsNullOrEmpty(videoDeviceId))
            {
                await LogStatus("ERROR: No Video Device Id found. Verify your device has a camera that supports video profiles.", NotifyType.ErrorMessage);
                return;
            }

            await LogStatusToOutputBox(string.Format(CultureInfo.InvariantCulture, "Found video capture device that supports Video Profile, Device Id:\r\n {0}", videoDeviceId));
            await LogStatusToOutputBox("Querying device for custom profile support");

            IReadOnlyList<MediaCaptureVideoProfile> profiles = MediaCapture.FindAllVideoProfiles(videoDeviceId);
            if (profiles.Count == 0)
            {
                await LogStatus("ERROR: The selected video capture device reported profile support but returned no profiles.", NotifyType.ErrorMessage);
                return;
            }

            mediaInitSettings.VideoDeviceId = videoDeviceId;
            mediaInitSettings.VideoProfile = profiles.FirstOrDefault(profile => profile.Id == customProfileId);

            if (mediaInitSettings.VideoProfile == null)
            {
                await LogStatus("Could not locate a matching custom profile, setting to default recording profile", NotifyType.ErrorMessage);
                mediaInitSettings.VideoProfile = profiles.FirstOrDefault();
            }
            else
            {
                await LogStatus(string.Format(CultureInfo.InvariantCulture, "Custom recording profile located: {0}", customProfileId), NotifyType.StatusMessage);
            }

            await TryInitializeMediaCaptureAsync(mediaCapture, mediaInitSettings, "Media Capture settings initialized to selected profile");
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

        private async Task TryInitializeMediaCaptureAsync(MediaCapture mediaCapture, MediaCaptureInitializationSettings settings, string successMessage)
        {
            try
            {
                await mediaCapture.InitializeAsync(settings);
                await LogStatusToOutputBox(successMessage);
            }
            catch (UnauthorizedAccessException)
            {
                await LogStatus("ERROR: The app was denied access to the camera or microphone.", NotifyType.ErrorMessage);
            }
            catch (Exception ex)
            {
                await LogStatus($"ERROR: MediaCapture initialization failed: {ex.Message}", NotifyType.ErrorMessage);
            }
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