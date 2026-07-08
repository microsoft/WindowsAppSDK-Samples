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
using Microsoft.UI.Xaml.Media;
using Microsoft.UI.Xaml.Media.Imaging;
using Microsoft.UI.Xaml.Navigation;
using Windows.Devices.Enumeration;
using Windows.Foundation;
using Windows.Graphics.Imaging;
using Windows.Media;
using Windows.Media.Capture;
using Windows.Media.Capture.Frames;
using Windows.Media.Core;
using Windows.Media.MediaProperties;
using Windows.Media.Playback;
using Windows.Storage;
using Windows.Storage.FileProperties;
using Windows.Storage.Streams;
using Windows.System.Display;

namespace SDKTemplate
{
    public sealed partial class Scenario1_CameraStarterKit : Page
    {
        private static readonly Guid RotationKey = new("C380465D-2271-428C-9B83-ECEA3B4A85C1");

        private readonly DisplayRequest _displayRequest = new();
        private readonly SoftwareBitmapSource _previewBitmapSource = new();

        private StorageFolder? _captureFolder;
        private MediaCapture? _mediaCapture;
        private MediaPlayer? _mediaPlayer;
        private MediaFrameReader? _mediaFrameReader;
        private CameraRotationHelper? _rotationHelper;
        private bool _isInitialized;
        private bool _isPreviewing;
        private bool _isRecording;
        private bool _isActivePage;
        private bool _isWindowVisible = true;
        private bool _isDisplayRequestActive;
        private bool _isProcessingFrame;
        private bool _mirroringPreview;
        private Task _setupTask = Task.CompletedTask;

        public Scenario1_CameraStarterKit()
        {
            InitializeComponent();
            NavigationCacheMode = NavigationCacheMode.Disabled;
            PreviewImage.Source = _previewBitmapSource;
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
                ShowPreviewMessage("No camera device was found. Connect a camera to use preview, photo, and video capture.");
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

            _rotationHelper = new CameraRotationHelper(cameraDevice.EnclosureLocation);
            _mirroringPreview = _rotationHelper.ShouldMirrorPreview();

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
            PreviewImage.FlowDirection = PreviewControl.FlowDirection;

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
                PreviewImage.Visibility = Visibility.Collapsed;
                PreviewPlaceholder.Visibility = Visibility.Collapsed;
                _isPreviewing = true;
            }
            catch (Exception ex)
            {
                Debug.WriteLine("MediaPlayerElement preview failed; falling back to MediaFrameReader: " + ex);
                await StartFrameReaderPreviewAsync(previewSource);
            }
        }

        private async Task StartFrameReaderPreviewAsync(MediaFrameSource previewSource)
        {
            if (_mediaCapture == null)
            {
                return;
            }

            _mediaFrameReader = await _mediaCapture.CreateFrameReaderAsync(previewSource, MediaEncodingSubtypes.Bgra8);
            _mediaFrameReader.FrameArrived += MediaFrameReader_FrameArrived;

            var status = await _mediaFrameReader.StartAsync();
            if (status == MediaFrameReaderStartStatus.Success)
            {
                PreviewControl.Visibility = Visibility.Collapsed;
                PreviewImage.Visibility = Visibility.Visible;
                PreviewPlaceholder.Visibility = Visibility.Collapsed;
                _isPreviewing = true;
                MainPage.Current.NotifyUser("Preview is rendered with MediaFrameReader fallback.", NotifyType.StatusMessage);
            }
            else
            {
                _mediaFrameReader.FrameArrived -= MediaFrameReader_FrameArrived;
                _mediaFrameReader.Dispose();
                _mediaFrameReader = null;
                ShowPreviewMessage($"Unable to start camera preview ({status}). Photo/video capture may still work.");
                MainPage.Current.NotifyUser($"Unable to start camera preview ({status}).", NotifyType.ErrorMessage);
            }
        }

        private void MediaFrameReader_FrameArrived(MediaFrameReader sender, MediaFrameArrivedEventArgs args)
        {
            if (_isProcessingFrame)
            {
                return;
            }

            _isProcessingFrame = true;
            SoftwareBitmap? bitmap = null;

            using (var frame = sender.TryAcquireLatestFrame())
            {
                var sourceBitmap = frame?.VideoMediaFrame?.SoftwareBitmap;
                if (sourceBitmap != null)
                {
                    bitmap = SoftwareBitmap.Convert(sourceBitmap, BitmapPixelFormat.Bgra8, BitmapAlphaMode.Premultiplied);
                }
            }

            if (bitmap == null)
            {
                _isProcessingFrame = false;
                return;
            }

            App.MainDispatcherQueue.TryEnqueue(async () =>
            {
                try
                {
                    await _previewBitmapSource.SetBitmapAsync(bitmap);
                }
                finally
                {
                    bitmap.Dispose();
                    _isProcessingFrame = false;
                }
            });
        }

        private async Task StopPreviewAsync()
        {
            _isPreviewing = false;

            if (_mediaFrameReader != null)
            {
                _mediaFrameReader.FrameArrived -= MediaFrameReader_FrameArrived;
                await _mediaFrameReader.StopAsync();
                _mediaFrameReader.Dispose();
                _mediaFrameReader = null;
            }

            if (_mediaPlayer != null)
            {
                _mediaPlayer.Pause();
                PreviewControl.SetMediaPlayer(null);
                _mediaPlayer.Dispose();
                _mediaPlayer = null;
            }

            PreviewImage.Source = null;
            PreviewImage.Source = _previewBitmapSource;
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
            if (_mediaCapture == null || _captureFolder == null || _rotationHelper == null)
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
                var photoOrientation = CameraRotationHelper.ConvertSimpleOrientationToPhotoOrientation(_rotationHelper.GetCameraCaptureOrientation());

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
            if (_mediaCapture == null || _captureFolder == null || _rotationHelper == null)
            {
                return;
            }

            try
            {
                var videoFile = await _captureFolder.CreateFileAsync("SimpleVideo.mp4", CreationCollisionOption.GenerateUniqueName);
                var encodingProfile = MediaEncodingProfile.CreateMp4(VideoEncodingQuality.Auto);

                var rotationAngle = CameraRotationHelper.ConvertSimpleOrientationToClockwiseDegrees(_rotationHelper.GetCameraCaptureOrientation());
                encodingProfile.Video.Properties.Add(RotationKey, PropertyValue.CreateInt32(rotationAngle));

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

            _rotationHelper = null;
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
            PhotoButton.IsEnabled = _isPreviewing;
            VideoButton.IsEnabled = _isPreviewing;

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

        private static async Task ReencodeAndSavePhotoAsync(IRandomAccessStream stream, StorageFile file, PhotoOrientation photoOrientation)
        {
            var decoder = await BitmapDecoder.CreateAsync(stream);

            using var outputStream = await file.OpenAsync(FileAccessMode.ReadWrite);
            var encoder = await BitmapEncoder.CreateForTranscodingAsync(outputStream, decoder);
            var properties = new BitmapPropertySet
            {
                { "System.Photo.Orientation", new BitmapTypedValue(photoOrientation, PropertyType.UInt16) }
            };

            await encoder.BitmapProperties.SetPropertiesAsync(properties);
            await encoder.FlushAsync();
        }

        private void ShowPreviewMessage(string message)
        {
            PreviewMessage.Text = message;
            PreviewPlaceholder.Visibility = Visibility.Visible;
            PreviewControl.Visibility = Visibility.Collapsed;
            PreviewImage.Visibility = Visibility.Collapsed;
        }
    }
}
