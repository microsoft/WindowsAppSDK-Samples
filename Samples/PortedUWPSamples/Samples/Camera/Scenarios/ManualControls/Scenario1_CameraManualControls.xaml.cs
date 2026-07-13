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

using System;
using System.Diagnostics;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Data;
using Microsoft.UI.Xaml.Input;
using Microsoft.UI.Xaml.Media;
using Microsoft.UI.Xaml.Navigation;
using Windows.Devices.Enumeration;
using Windows.Devices.Sensors;
using Windows.Foundation;
using Windows.Graphics.Display;
using Windows.Graphics.Imaging;
using Windows.Media;
using Windows.Media.Capture;
using Windows.Media.Capture.Frames;
using Windows.Media.Core;
using Windows.Media.Devices;
using Windows.Media.MediaProperties;
using Windows.Media.Playback;
using Windows.Storage;
using Windows.Storage.FileProperties;
using Windows.Storage.Streams;
using Windows.System.Display;
using Windows.UI;

namespace SDKTemplate
{
    public sealed partial class Scenario1_CameraManualControls : Page
    {
        private SimpleOrientation _deviceOrientation = SimpleOrientation.NotRotated;
        private DisplayOrientations _displayOrientation = DisplayOrientations.Landscape;

        // Rotation metadata to apply to recorded videos (MF_MT_VIDEO_ROTATION).
        private static readonly Guid RotationKey = new("C380465D-2271-428C-9B83-ECEA3B4A85C1");

        private StorageFolder? _captureFolder;
        private readonly DisplayRequest _displayRequest = new();
        private MediaCapture? _mediaCapture;
        private MediaPlayer? _mediaPlayer;
        private bool _isInitialized;
        private bool _isPreviewing;
        private bool _isRecording;
        private bool _isFocused;
        private bool _mirroringPreview;
        private bool _externalCamera;
        private bool _singleControlMode;
        private bool _settingUpUi;
        private bool _isActivePage;
        private bool _isWindowVisible = true;
        private bool _isDisplayRequestActive;
        private Task _setupTask = Task.CompletedTask;

        public Scenario1_CameraManualControls()
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

        private async void PhotoButton_Click(object sender, RoutedEventArgs e)
        {
            await TakePhotoAsync();
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

        private async void PreviewControl_Tapped(object sender, TappedRoutedEventArgs e)
        {
            if (!_isPreviewing || TapFocusRadioButton.IsChecked != true || _mediaCapture == null)
            {
                return;
            }

            if (!_isFocused && _mediaCapture.VideoDeviceController.FocusControl.FocusState != MediaCaptureFocusState.Searching)
            {
                var content = App.MainWindow.Content as FrameworkElement;
                var smallEdge = Math.Min(content?.ActualWidth ?? PreviewControl.ActualWidth, content?.ActualHeight ?? PreviewControl.ActualHeight);
                var size = new Size(smallEdge / 4, smallEdge / 4);
                var position = e.GetPosition(sender as UIElement);
                await TapToFocus(position, size);
            }
            else
            {
                await TapUnfocus();
            }
        }

        private void PreviewControl_ManipulationDelta(object sender, ManipulationDeltaRoutedEventArgs e)
        {
            if (!_isPreviewing)
            {
                return;
            }

            ZoomSlider.Value *= e.Delta.Scale;
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
                Debug.WriteLine("No camera device found!");
                MainPage.Current.NotifyUser("No camera device was found. Connect a camera to use the manual controls.", NotifyType.ErrorMessage);
                ShowPreviewMessage("No camera device was found. Connect a camera to use preview, photo, video, and manual controls.");
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

            if (cameraDevice.EnclosureLocation == null || cameraDevice.EnclosureLocation.Panel == Windows.Devices.Enumeration.Panel.Unknown)
            {
                _externalCamera = true;
            }
            else
            {
                _externalCamera = false;
                _mirroringPreview = cameraDevice.EnclosureLocation.Panel == Windows.Devices.Enumeration.Panel.Front;
            }

            await StartPreviewAsync();
            UpdateCaptureControls();
            UpdateManualControlCapabilities();
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
            FocusCanvas.FlowDirection = PreviewControl.FlowDirection;

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
                _isPreviewing = true;
                PreviewPlaceholder.Visibility = Visibility.Collapsed;

                await SetPreviewRotationAsync();
            }
            catch (Exception ex)
            {
                Debug.WriteLine("MediaPlayerElement preview failed: " + ex);
                ShowPreviewMessage("Unable to start camera preview. Manual controls remain unavailable until preview starts.");
                MainPage.Current.NotifyUser($"Unable to start camera preview: {ex.Message}", NotifyType.ErrorMessage);
            }
        }

        private Task SetPreviewRotationAsync()
        {
            // WinUI 3 desktop has no CoreWindow-backed DisplayInformation.GetForCurrentView().
            // The port keeps captures upright for the desktop/laptop case and documents this simplification in the README.
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
        }

        private async Task TakePhotoAsync()
        {
            if (_mediaCapture == null || _captureFolder == null)
            {
                return;
            }

            VideoButton.IsEnabled = _mediaCapture.MediaCaptureSettings.ConcurrentRecordAndPhotoSupported;
            VideoButton.Opacity = VideoButton.IsEnabled ? 1 : 0;

            using var stream = new InMemoryRandomAccessStream();

            try
            {
                Debug.WriteLine("Taking photo...");
                await _mediaCapture.CapturePhotoToStreamAsync(ImageEncodingProperties.CreateJpeg(), stream);
                var file = await _captureFolder.CreateFileAsync("SimplePhoto.jpg", CreationCollisionOption.GenerateUniqueName);
                var photoOrientation = ConvertOrientationToPhotoOrientation(GetCameraOrientation());
                await ReencodeAndSavePhotoAsync(stream, file, photoOrientation);
                MainPage.Current.NotifyUser($"Photo saved to {file.Path}", NotifyType.StatusMessage);
            }
            catch (Exception ex)
            {
                MainPage.Current.NotifyUser($"Exception when taking a photo: {ex.Message}", NotifyType.ErrorMessage);
                Debug.WriteLine("Exception when taking a photo: " + ex);
            }

            VideoButton.IsEnabled = _isPreviewing;
            VideoButton.Opacity = 1;
        }

        private async Task StartRecordingAsync()
        {
            if (_mediaCapture == null || _captureFolder == null)
            {
                return;
            }

            try
            {
                var videoFile = await _captureFolder.CreateFileAsync("SimpleVideo.mp4", CreationCollisionOption.GenerateUniqueName);
                var encodingProfile = MediaEncodingProfile.CreateMp4(VideoEncodingQuality.Auto);
                var rotationAngle = 360 - ConvertDeviceOrientationToDegrees(GetCameraOrientation());
                encodingProfile.Video.Properties.Add(RotationKey, PropertyValue.CreateInt32(rotationAngle));

                Debug.WriteLine("Starting recording to " + videoFile.Path);
                await _mediaCapture.StartRecordToStorageFileAsync(encodingProfile, videoFile);
                _isRecording = true;
                MainPage.Current.NotifyUser($"Recording video to {videoFile.Path}", NotifyType.StatusMessage);
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

                if (_isPreviewing)
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
                    await CleanupUiAsync();
                }
            };

            _setupTask = setupAsync();
            await _setupTask;
        }

        private async Task SetupUiAsync()
        {
            RegisterEventHandlers();
            _captureFolder ??= await GetCaptureFolderAsync();
        }

        private Task CleanupUiAsync()
        {
            UnregisterEventHandlers();
            return Task.CompletedTask;
        }

        private void UpdateCaptureControls()
        {
            PhotoButton.IsEnabled = _isPreviewing;
            VideoButton.IsEnabled = _isPreviewing;
            CameraControlsGrid.Visibility = _isPreviewing ? Visibility.Visible : Visibility.Collapsed;

            StartRecordingIcon.Visibility = _isRecording ? Visibility.Collapsed : Visibility.Visible;
            StopRecordingIcon.Visibility = _isRecording ? Visibility.Visible : Visibility.Collapsed;

            if (_isInitialized && _mediaCapture != null && !_mediaCapture.MediaCaptureSettings.ConcurrentRecordAndPhotoSupported)
            {
                PhotoButton.IsEnabled = !_isRecording && _isPreviewing;
                PhotoButton.Opacity = PhotoButton.IsEnabled ? 1 : 0;
            }
            else
            {
                PhotoButton.Opacity = 1;
            }
        }

        private void RegisterEventHandlers()
        {
            UpdateButtonOrientation();
        }

        private void UnregisterEventHandlers()
        {
        }

        private static async Task<DeviceInformation?> FindCameraDeviceByPanelAsync(Windows.Devices.Enumeration.Panel desiredPanel)
        {
            var allVideoDevices = await DeviceInformation.FindAllAsync(DeviceClass.VideoCapture);
            var desiredDevice = allVideoDevices.FirstOrDefault(x => x.EnclosureLocation != null && x.EnclosureLocation.Panel == desiredPanel);
            return desiredDevice ?? allVideoDevices.FirstOrDefault();
        }

        private static MediaFrameSource? FindPreviewFrameSource(MediaCapture mediaCapture)
        {
            return mediaCapture.FrameSources.Values.FirstOrDefault(source =>
                    source.Info.MediaStreamType == MediaStreamType.VideoPreview &&
                    source.Info.SourceKind == MediaFrameSourceKind.Color)
                ?? mediaCapture.FrameSources.Values.FirstOrDefault(source =>
                    source.Info.SourceKind == MediaFrameSourceKind.Color);
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

        private static async Task ReencodeAndSavePhotoAsync(IRandomAccessStream stream, StorageFile file, PhotoOrientation photoOrientation)
        {
            var decoder = await BitmapDecoder.CreateAsync(stream);
            using var outputStream = await file.OpenAsync(FileAccessMode.ReadWrite);
            var encoder = await BitmapEncoder.CreateForTranscodingAsync(outputStream, decoder);
            var properties = new BitmapPropertySet { { "System.Photo.Orientation", new BitmapTypedValue(photoOrientation, PropertyType.UInt16) } };
            await encoder.BitmapProperties.SetPropertiesAsync(properties);
            await encoder.FlushAsync();
        }

        public static Rect GetPreviewStreamRectInControl(VideoEncodingProperties? previewResolution, MediaPlayerElement previewControl, DisplayOrientations displayOrientation)
        {
            var result = new Rect();

            if (previewControl == null || previewControl.ActualHeight < 1 || previewControl.ActualWidth < 1 ||
                previewResolution == null || previewResolution.Height == 0 || previewResolution.Width == 0)
            {
                return result;
            }

            var streamWidth = previewResolution.Width;
            var streamHeight = previewResolution.Height;

            if (displayOrientation == DisplayOrientations.Portrait || displayOrientation == DisplayOrientations.PortraitFlipped)
            {
                streamWidth = previewResolution.Height;
                streamHeight = previewResolution.Width;
            }

            result.Width = previewControl.ActualWidth;
            result.Height = previewControl.ActualHeight;

            if (previewControl.ActualWidth / previewControl.ActualHeight > streamWidth / (double)streamHeight)
            {
                var scale = previewControl.ActualHeight / streamHeight;
                var scaledWidth = streamWidth * scale;
                result.X = (previewControl.ActualWidth - scaledWidth) / 2.0;
                result.Width = scaledWidth;
            }
            else
            {
                var scale = previewControl.ActualWidth / streamWidth;
                var scaledHeight = streamHeight * scale;
                result.Y = (previewControl.ActualHeight - scaledHeight) / 2.0;
                result.Height = scaledHeight;
            }

            return result;
        }

        private SimpleOrientation GetCameraOrientation()
        {
            if (_externalCamera)
            {
                return SimpleOrientation.NotRotated;
            }

            var result = _deviceOrientation;

            if (_mirroringPreview)
            {
                switch (result)
                {
                    case SimpleOrientation.Rotated90DegreesCounterclockwise:
                        return SimpleOrientation.Rotated270DegreesCounterclockwise;
                    case SimpleOrientation.Rotated270DegreesCounterclockwise:
                        return SimpleOrientation.Rotated90DegreesCounterclockwise;
                }
            }

            return result;
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

        private static PhotoOrientation ConvertOrientationToPhotoOrientation(SimpleOrientation orientation)
        {
            return orientation switch
            {
                SimpleOrientation.Rotated90DegreesCounterclockwise => PhotoOrientation.Rotate90,
                SimpleOrientation.Rotated180DegreesCounterclockwise => PhotoOrientation.Rotate180,
                SimpleOrientation.Rotated270DegreesCounterclockwise => PhotoOrientation.Rotate270,
                _ => PhotoOrientation.Normal,
            };
        }

        private void UpdateButtonOrientation()
        {
            var transform = new RotateTransform { Angle = 0 };
            PhotoButton.RenderTransform = transform;
            VideoButton.RenderTransform = transform;
        }

        private void SetSingleControl(object? activeButton)
        {
            _singleControlMode = activeButton != null;

            foreach (var button in ScenarioControlStackPanel.Children.OfType<Button>())
            {
                if (button != activeButton)
                {
                    button.Visibility = _singleControlMode ? Visibility.Collapsed : (Visibility)button.Tag;
                }
            }

            ManualControlsGrid.Visibility = _singleControlMode ? Visibility.Visible : Visibility.Collapsed;
        }

        private void ManualControlButton_Click(object sender, RoutedEventArgs e)
        {
            SetSingleControl(_singleControlMode ? null : sender);
        }

        private void UpdateManualControlCapabilities()
        {
            if (_mediaCapture == null)
            {
                return;
            }

            _settingUpUi = true;
            UpdateFlashControlCapabilities();
            UpdateZoomControlCapabilities();
            UpdateFocusControlCapabilities();
            UpdateWbControlCapabilities();
            UpdateIsoControlCapabilities();
            UpdateExposureControlCapabilities();
            UpdateEvControlCapabilities();
            _settingUpUi = false;
        }

        private void ShowPreviewMessage(string message)
        {
            PreviewMessage.Text = message;
            PreviewPlaceholder.Visibility = Visibility.Visible;
        }
    }

    public class InvertBoolConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, string language)
        {
            return !(bool)value;
        }

        public object ConvertBack(object value, Type targetType, object parameter, string language)
        {
            return !(bool)value;
        }
    }

    public class RoundingConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, string language)
        {
            return Math.Round((double)value, 2);
        }

        public object ConvertBack(object value, Type targetType, object parameter, string language)
        {
            return value;
        }
    }
}
