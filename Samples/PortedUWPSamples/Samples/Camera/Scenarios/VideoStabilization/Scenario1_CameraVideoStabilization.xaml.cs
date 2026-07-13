//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#define USE_VS_RECOMMENDATION

using System;
using System.Diagnostics;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Media;
using Microsoft.UI.Xaml.Navigation;
using Windows.Devices.Enumeration;
using Windows.Devices.Sensors;
using Windows.Foundation;
using Windows.Media.Capture;
using Windows.Media.Capture.Frames;
using Windows.Media.Core;
using Windows.Media.MediaProperties;
using Windows.Media.Playback;
using Windows.Storage;
using Windows.System.Display;

namespace SDKTemplate
{
    public sealed partial class Scenario1_CameraVideoStabilization : Page
    {
        private static readonly Guid RotationKey = new("C380465D-2271-428C-9B83-ECEA3B4A85C1");

        private readonly DisplayRequest _displayRequest = new();

        private StorageFolder? _captureFolder;
        private MediaCapture? _mediaCapture;
        private MediaPlayer? _mediaPlayer;
        private MediaEncodingProfile? _encodingProfile;
        private VideoStabilizationEffect? _videoStabilizationEffect;
        private VideoEncodingProperties? _inputPropertiesBackup;
        private VideoEncodingProperties? _outputPropertiesBackup;

        private bool _isInitialized;
        private bool _isPreviewing;
        private bool _isRecording;
        private bool _isActivePage;
        private bool _isWindowVisible = true;
        private bool _isDisplayRequestActive;
        private bool _mirroringPreview;
        private bool _externalCamera = true;
        private Task _setupTask = Task.CompletedTask;

        public Scenario1_CameraVideoStabilization()
        {
            InitializeComponent();
            NavigationCacheMode = NavigationCacheMode.Disabled;
        }

        protected override async void OnNavigatedTo(NavigationEventArgs e)
        {
            App.MainWindow.VisibilityChanged += Window_VisibilityChanged;
            _isActivePage = true;
            await SetUpBasedOnStateAsync();
        }

        protected override async void OnNavigatingFrom(NavigatingCancelEventArgs e)
        {
            App.MainWindow.VisibilityChanged -= Window_VisibilityChanged;
            _isActivePage = false;
            await SetUpBasedOnStateAsync();
        }

        private async void Window_VisibilityChanged(object sender, WindowVisibilityChangedEventArgs args)
        {
            _isWindowVisible = args.Visible;
            await SetUpBasedOnStateAsync();
        }

        private async void VsToggleButton_Click(object sender, RoutedEventArgs e)
        {
            if (_mediaCapture == null)
            {
                return;
            }

            if (!_isRecording)
            {
                if (_videoStabilizationEffect == null)
                {
                    await CreateVideoStabilizationEffectAsync();
                }
                else if (_videoStabilizationEffect.Enabled)
                {
                    await CleanUpVideoStabilizationEffectAsync();
                }
            }
            else if (_videoStabilizationEffect != null && !_videoStabilizationEffect.Enabled)
            {
                _videoStabilizationEffect.Enabled = true;
            }

            UpdateCaptureControls();
        }

        private async void VideoButton_Click(object sender, RoutedEventArgs e)
        {
            if (!_isRecording)
            {
                await StartRecordingAsync();
            }
            else
            {
                await StopRecordingAsync();
            }

            UpdateCaptureControls();
        }

        private async void VideoStabilizationEffect_EnabledChanged(VideoStabilizationEffect sender, VideoStabilizationEffectEnabledChangedEventArgs args)
        {
            App.MainDispatcherQueue.TryEnqueue(() =>
            {
                Debug.WriteLine("VS Enabled: " + sender.Enabled + ". Reason: " + args.Reason);
                UpdateCaptureControls();
            });
            await Task.CompletedTask;
        }

        private async void MediaCapture_RecordLimitationExceeded(MediaCapture sender)
        {
            await StopRecordingAsync();
            App.MainDispatcherQueue.TryEnqueue(UpdateCaptureControls);
        }

        private async void MediaCapture_Failed(MediaCapture sender, MediaCaptureFailedEventArgs errorEventArgs)
        {
            Debug.WriteLine("MediaCapture_Failed: (0x{0:X}) {1}", errorEventArgs.Code, errorEventArgs.Message);
            MainPage.Current.NotifyUser($"Camera capture failed: {errorEventArgs.Message}", NotifyType.ErrorMessage);

            await CleanupCameraAsync();
            App.MainDispatcherQueue.TryEnqueue(UpdateCaptureControls);
        }

        private async Task InitializeCameraAsync()
        {
            Debug.WriteLine("InitializeCameraAsync");

            if (_mediaCapture != null)
            {
                return;
            }

            var cameraDevice = await FindCameraDeviceByPanelAsync(Windows.Devices.Enumeration.Panel.Back);
            if (cameraDevice == null)
            {
                ShowPreviewMessage("No camera device was found. Connect a camera to use preview, recording, and video stabilization.");
                MainPage.Current.NotifyUser("No camera device was found.", NotifyType.ErrorMessage);
                return;
            }

            _mediaCapture = new MediaCapture();
            _mediaCapture.RecordLimitationExceeded += MediaCapture_RecordLimitationExceeded;
            _mediaCapture.Failed += MediaCapture_Failed;

            var settings = new MediaCaptureInitializationSettings
            {
                VideoDeviceId = cameraDevice.Id,
                StreamingCaptureMode = StreamingCaptureMode.AudioAndVideo,
                SharingMode = MediaCaptureSharingMode.ExclusiveControl,
                MemoryPreference = MediaCaptureMemoryPreference.Auto
            };

            try
            {
                await _mediaCapture.InitializeAsync(settings);
                _isInitialized = true;
            }
            catch (UnauthorizedAccessException)
            {
                MainPage.Current.NotifyUser("The app was denied access to the camera or microphone.", NotifyType.ErrorMessage);
            }
            catch (Exception ex)
            {
                MainPage.Current.NotifyUser($"Unable to initialize the camera: {ex.Message}", NotifyType.ErrorMessage);
                Debug.WriteLine("Exception when initializing camera: " + ex);
            }

            if (!_isInitialized)
            {
                await CleanupCameraAsync();
                return;
            }

            _encodingProfile = MediaEncodingProfile.CreateMp4(VideoEncodingQuality.Auto);

            _externalCamera = cameraDevice.EnclosureLocation == null || cameraDevice.EnclosureLocation.Panel == Windows.Devices.Enumeration.Panel.Unknown;
            _mirroringPreview = cameraDevice.EnclosureLocation?.Panel == Windows.Devices.Enumeration.Panel.Front;

            await StartPreviewAsync();
            UpdateCaptureControls();
        }

        private async Task StartPreviewAsync()
        {
            if (_mediaCapture == null)
            {
                return;
            }

            try
            {
                _displayRequest.RequestActive();
                _isDisplayRequestActive = true;
            }
            catch (Exception ex)
            {
                Debug.WriteLine("DisplayRequest.RequestActive failed: " + ex.Message);
            }

            var previewSource = FindPreviewFrameSource(_mediaCapture);
            if (previewSource == null)
            {
                ShowPreviewMessage("This camera does not expose a color preview frame source.");
                MainPage.Current.NotifyUser("No color preview frame source was found.", NotifyType.ErrorMessage);
                return;
            }

            PreviewControl.FlowDirection = _mirroringPreview ? FlowDirection.RightToLeft : FlowDirection.LeftToRight;

            try
            {
                var mediaSource = MediaSource.CreateFromMediaFrameSource(previewSource);
                _mediaPlayer = new MediaPlayer
                {
                    AutoPlay = true,
                    RealTimePlayback = true,
                    Source = mediaSource
                };

                PreviewControl.SetMediaPlayer(_mediaPlayer);
                _mediaPlayer.Play();
                PreviewControl.Visibility = Visibility.Visible;
                PreviewPlaceholder.Visibility = Visibility.Collapsed;
                _isPreviewing = true;
            }
            catch (Exception ex)
            {
                ShowPreviewMessage("Unable to start camera preview. Recording may still be unavailable on this device.");
                MainPage.Current.NotifyUser($"Unable to start camera preview: {ex.Message}", NotifyType.ErrorMessage);
                Debug.WriteLine("MediaPlayerElement preview failed: " + ex);
            }

            await SetPreviewRotationAsync();
        }

        private Task SetPreviewRotationAsync()
        {
            // WinUI 3 desktop does not expose the CoreWindow-backed DisplayInformation.GetForCurrentView used by the UWP sample.
            // The port keeps preview mirroring and writes neutral rotation metadata; this difference is documented in the README.
            return Task.CompletedTask;
        }

        private async Task StopPreviewAsync()
        {
            _isPreviewing = false;

            if (_mediaPlayer != null)
            {
                _mediaPlayer.Pause();
                PreviewControl.SetMediaPlayer(null);
                _mediaPlayer.Dispose();
                _mediaPlayer = null;
            }

            ShowPreviewMessage("Camera preview is not available.");

            if (_isDisplayRequestActive)
            {
                try
                {
                    _displayRequest.RequestRelease();
                }
                catch (Exception ex)
                {
                    Debug.WriteLine("DisplayRequest.RequestRelease failed: " + ex.Message);
                }

                _isDisplayRequestActive = false;
            }

            await Task.CompletedTask;
        }

        private async Task CreateVideoStabilizationEffectAsync()
        {
            if (_mediaCapture == null || _encodingProfile == null || _videoStabilizationEffect != null)
            {
                return;
            }

            try
            {
                var definition = new VideoStabilizationEffectDefinition();
                _videoStabilizationEffect = (VideoStabilizationEffect)await _mediaCapture.AddVideoEffectAsync(definition, MediaStreamType.VideoRecord);
                Debug.WriteLine("VS effect added to pipeline");

                _videoStabilizationEffect.EnabledChanged += VideoStabilizationEffect_EnabledChanged;
                _videoStabilizationEffect.Enabled = true;

#if USE_VS_RECOMMENDATION
                await SetUpVideoStabilizationRecommendationAsync();
#else
                Debug.WriteLine("Not setting up VS recommendation");
#endif

                MainPage.Current.NotifyUser("Video stabilization is enabled.", NotifyType.StatusMessage);
            }
            catch (Exception ex)
            {
                MainPage.Current.NotifyUser($"Unable to enable video stabilization: {ex.Message}", NotifyType.ErrorMessage);
                Debug.WriteLine("Exception when adding VS effect: " + ex);
                if (_videoStabilizationEffect != null)
                {
                    _videoStabilizationEffect.EnabledChanged -= VideoStabilizationEffect_EnabledChanged;
                    _videoStabilizationEffect = null;
                }
            }
        }

        private async Task SetUpVideoStabilizationRecommendationAsync()
        {
            if (_mediaCapture == null || _encodingProfile == null || _videoStabilizationEffect == null)
            {
                return;
            }

            Debug.WriteLine("Setting up VS recommendation...");

            var recommendation = _videoStabilizationEffect.GetRecommendedStreamConfiguration(_mediaCapture.VideoDeviceController, _encodingProfile.Video);

            if (recommendation.InputProperties != null)
            {
                _inputPropertiesBackup = _mediaCapture.VideoDeviceController.GetMediaStreamProperties(MediaStreamType.VideoRecord) as VideoEncodingProperties;
                await _mediaCapture.VideoDeviceController.SetMediaStreamPropertiesAsync(MediaStreamType.VideoRecord, recommendation.InputProperties);
                Debug.WriteLine("VS recommendation for the MediaStreamProperties (input) has been applied");
            }

            if (recommendation.OutputProperties != null)
            {
                _outputPropertiesBackup = _encodingProfile.Video;
                _encodingProfile.Video = recommendation.OutputProperties;
                Debug.WriteLine("VS recommendation for the MediaEncodingProfile (output) has been applied");
            }
        }

        private async Task CleanUpVideoStabilizationEffectAsync()
        {
            if (_mediaCapture == null || _encodingProfile == null || _videoStabilizationEffect == null)
            {
                return;
            }

            try
            {
                _videoStabilizationEffect.Enabled = false;
                _videoStabilizationEffect.EnabledChanged -= VideoStabilizationEffect_EnabledChanged;

                await _mediaCapture.RemoveEffectAsync(_videoStabilizationEffect);
                Debug.WriteLine("VS effect removed from pipeline");

                if (_inputPropertiesBackup != null)
                {
                    await _mediaCapture.VideoDeviceController.SetMediaStreamPropertiesAsync(MediaStreamType.VideoRecord, _inputPropertiesBackup);
                    _inputPropertiesBackup = null;
                }

                if (_outputPropertiesBackup != null)
                {
                    _encodingProfile.Video = _outputPropertiesBackup;
                    _outputPropertiesBackup = null;
                }
            }
            catch (Exception ex)
            {
                Debug.WriteLine("Exception when cleaning up VS effect: " + ex);
            }
            finally
            {
                _videoStabilizationEffect = null;
            }
        }

        private async Task StartRecordingAsync()
        {
            if (_mediaCapture == null || _encodingProfile == null || _captureFolder == null)
            {
                return;
            }

            try
            {
                var videoFile = await _captureFolder.CreateFileAsync("SimpleVideo.mp4", CreationCollisionOption.GenerateUniqueName);
                var rotationAngle = 360 - ConvertDeviceOrientationToDegrees(GetCameraOrientation());
                _encodingProfile.Video.Properties[RotationKey] = PropertyValue.CreateInt32(rotationAngle);

                await _mediaCapture.StartRecordToStorageFileAsync(_encodingProfile, videoFile);
                _isRecording = true;

                MainPage.Current.NotifyUser($"Recording video to {videoFile.Path}", NotifyType.StatusMessage);
                Debug.WriteLine("Started recording to: " + videoFile.Path);
            }
            catch (Exception ex)
            {
                MainPage.Current.NotifyUser($"Exception when starting video recording: {ex.Message}", NotifyType.ErrorMessage);
                Debug.WriteLine("Exception when starting video recording: " + ex);
            }
        }

        private async Task StopRecordingAsync()
        {
            if (_mediaCapture == null || !_isRecording)
            {
                return;
            }

            try
            {
                _isRecording = false;
                await _mediaCapture.StopRecordAsync();

                if (_videoStabilizationEffect != null && !_videoStabilizationEffect.Enabled)
                {
                    await CleanUpVideoStabilizationEffectAsync();
                }

                MainPage.Current.NotifyUser("Stopped recording video.", NotifyType.StatusMessage);
            }
            catch (Exception ex)
            {
                MainPage.Current.NotifyUser($"Exception when stopping video recording: {ex.Message}", NotifyType.ErrorMessage);
                Debug.WriteLine("Exception when stopping video recording: " + ex);
            }
        }

        private async Task CleanupCameraAsync()
        {
            Debug.WriteLine("CleanupCameraAsync");

            if (_isInitialized)
            {
                if (_isRecording)
                {
                    await StopRecordingAsync();
                }

                if (_videoStabilizationEffect != null)
                {
                    await CleanUpVideoStabilizationEffectAsync();
                }

                if (_isPreviewing || _isDisplayRequestActive || _mediaPlayer != null)
                {
                    await StopPreviewAsync();
                }

                _isInitialized = false;
            }

            if (_mediaCapture != null)
            {
                _mediaCapture.RecordLimitationExceeded -= MediaCapture_RecordLimitationExceeded;
                _mediaCapture.Failed -= MediaCapture_Failed;
                _mediaCapture.Dispose();
                _mediaCapture = null;
            }

            _encodingProfile = null;
            UpdateCaptureControls();
        }

        private async Task SetUpBasedOnStateAsync()
        {
            while (!_setupTask.IsCompleted)
            {
                await _setupTask;
            }

            bool wantUIActive = _isActivePage && _isWindowVisible;

            Func<Task> setupAsync = async () =>
            {
                if (wantUIActive)
                {
                    await SetupUiAsync();
                    await InitializeCameraAsync();
                }
                else
                {
                    await CleanupCameraAsync();
                    CleanupUi();
                }
            };

            _setupTask = setupAsync();
            await _setupTask;
        }

        private async Task SetupUiAsync()
        {
            _captureFolder ??= await GetCaptureFolderAsync();
        }

        private void CleanupUi()
        {
            UpdateCaptureControls();
        }

        private void UpdateCaptureControls()
        {
            VideoButton.IsEnabled = _isPreviewing;
            VsToggleButton.IsEnabled = (_isPreviewing && !_isRecording) || (_videoStabilizationEffect != null && !_videoStabilizationEffect.Enabled);

            StartRecordingIcon.Visibility = _isRecording ? Visibility.Collapsed : Visibility.Visible;
            StopRecordingIcon.Visibility = _isRecording ? Visibility.Visible : Visibility.Collapsed;

            bool isVsEnabled = _videoStabilizationEffect != null && _videoStabilizationEffect.Enabled;
            VsOnIcon.Visibility = isVsEnabled ? Visibility.Visible : Visibility.Collapsed;
            VsOffIcon.Visibility = isVsEnabled ? Visibility.Collapsed : Visibility.Visible;
        }

        private static MediaFrameSource? FindPreviewFrameSource(MediaCapture mediaCapture)
        {
            return mediaCapture.FrameSources.Values.FirstOrDefault(source =>
                    source.Info.MediaStreamType == MediaStreamType.VideoPreview &&
                    source.Info.SourceKind == MediaFrameSourceKind.Color)
                ?? mediaCapture.FrameSources.Values.FirstOrDefault(source =>
                    source.Info.SourceKind == MediaFrameSourceKind.Color);
        }

        private static async Task<DeviceInformation?> FindCameraDeviceByPanelAsync(Windows.Devices.Enumeration.Panel desiredPanel)
        {
            var allVideoDevices = await DeviceInformation.FindAllAsync(DeviceClass.VideoCapture);
            var desiredDevice = allVideoDevices.FirstOrDefault(x => x.EnclosureLocation != null && x.EnclosureLocation.Panel == desiredPanel);
            return desiredDevice ?? allVideoDevices.FirstOrDefault();
        }

        private static async Task<StorageFolder> GetCaptureFolderAsync()
        {
            try
            {
                var picturesLibrary = await StorageLibrary.GetLibraryAsync(KnownLibraryId.Pictures);
                return picturesLibrary.SaveFolder ?? ApplicationData.Current.LocalFolder;
            }
            catch (Exception ex)
            {
                Debug.WriteLine("Pictures library unavailable; using local folder: " + ex.Message);
                return ApplicationData.Current.LocalFolder;
            }
        }

        private SimpleOrientation GetCameraOrientation()
        {
            return _externalCamera ? SimpleOrientation.NotRotated : SimpleOrientation.NotRotated;
        }

        private static int ConvertDeviceOrientationToDegrees(SimpleOrientation orientation)
        {
            return orientation switch
            {
                SimpleOrientation.Rotated90DegreesCounterclockwise => 90,
                SimpleOrientation.Rotated180DegreesCounterclockwise => 180,
                SimpleOrientation.Rotated270DegreesCounterclockwise => 270,
                _ => 0,
            };
        }

        private void ShowPreviewMessage(string message)
        {
            PreviewMessage.Text = message;
            PreviewPlaceholder.Visibility = Visibility.Visible;
            PreviewControl.Visibility = Visibility.Collapsed;
        }
    }
}