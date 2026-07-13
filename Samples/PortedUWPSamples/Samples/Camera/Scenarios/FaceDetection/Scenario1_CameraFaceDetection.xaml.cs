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
using Microsoft.UI;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Media;
using Microsoft.UI.Xaml.Navigation;
using Microsoft.UI.Xaml.Shapes;
using Windows.Devices.Enumeration;
using Windows.Foundation;
using Windows.Graphics.Imaging;
using Windows.Media;
using Windows.Media.Capture;
using Windows.Media.Capture.Frames;
using Windows.Media.Core;
using Windows.Media.FaceAnalysis;
using Windows.Media.MediaProperties;
using Windows.Media.Playback;
using Windows.Storage;
using Windows.Storage.FileProperties;
using Windows.Storage.Streams;
using Windows.System.Display;

namespace SDKTemplate
{
    public sealed partial class Scenario1_CameraFaceDetection : Page
    {
        private static readonly Guid RotationKey = new("C380465D-2271-428C-9B83-ECEA3B4A85C1");

        private readonly DisplayRequest _displayRequest = new();

        private StorageFolder? _captureFolder;
        private MediaCapture? _mediaCapture;
        private MediaPlayer? _mediaPlayer;
        private FaceDetectionEffect? _faceDetectionEffect;
        private double _previewStreamWidth;
        private double _previewStreamHeight;
        private bool _isInitialized;
        private bool _isPreviewing;
        private bool _isRecording;
        private bool _isActivePage;
        private bool _isWindowVisible = true;
        private bool _isDisplayRequestActive;
        private bool _mirroringPreview;
        private Task _setupTask = Task.CompletedTask;

        public Scenario1_CameraFaceDetection()
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

        private async void FaceDetectionButton_Click(object sender, RoutedEventArgs e)
        {
            if (_faceDetectionEffect == null || !_faceDetectionEffect.Enabled)
            {
                FacesCanvas.Children.Clear();
                await CreateFaceDetectionEffectAsync();
            }
            else
            {
                await CleanUpFaceDetectionEffectAsync();
            }

            UpdateCaptureControls();
        }

        private void PageRoot_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            SetFacesCanvasBounds();
        }

        private async void MediaCapture_RecordLimitationExceeded(MediaCapture sender)
        {
            await StopRecordingAsync();
            App.MainDispatcherQueue.TryEnqueue(UpdateCaptureControls);
        }

        private void MediaCapture_Failed(MediaCapture sender, MediaCaptureFailedEventArgs errorEventArgs)
        {
            Debug.WriteLine("MediaCapture_Failed: (0x{0:X}) {1}", errorEventArgs.Code, errorEventArgs.Message);
            MainPage.Current.NotifyUser($"Camera capture failed: {errorEventArgs.Message}", NotifyType.ErrorMessage);

            App.MainDispatcherQueue.TryEnqueue(async () =>
            {
                try
                {
                    await CleanupCameraAsync();
                    UpdateCaptureControls();
                }
                catch (Exception ex)
                {
                    Debug.WriteLine("Exception when handling MediaCapture failure: " + ex);
                }
            });
        }

        private void FaceDetectionEffect_FaceDetected(FaceDetectionEffect sender, FaceDetectedEventArgs args)
        {
            var faces = args.ResultFrame.DetectedFaces.ToList();
            App.MainDispatcherQueue.TryEnqueue(() => HighlightDetectedFaces(faces));
        }

        private async Task InitializeCameraAsync()
        {
            Debug.WriteLine("InitializeCameraAsync");

            if (_mediaCapture != null)
            {
                return;
            }

            var cameraDevice = await FindCameraDeviceByPanelAsync(Windows.Devices.Enumeration.Panel.Front);
            if (cameraDevice == null)
            {
                ShowPreviewMessage("No camera device was found. Connect a camera to use the preview, capture, recording, and face detection features.");
                MainPage.Current.NotifyUser("No camera device was found.", NotifyType.ErrorMessage);
                return;
            }

            _mirroringPreview = cameraDevice.EnclosureLocation?.Panel == Windows.Devices.Enumeration.Panel.Front;

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

            await StartPreviewAsync();
            UpdateCaptureControls();
        }

        private async Task StartPreviewAsync()
        {
            if (_mediaCapture == null)
            {
                return;
            }

            var previewSource = FindPreviewFrameSource(_mediaCapture);
            if (previewSource == null)
            {
                ShowPreviewMessage("This camera does not expose a color preview frame source.");
                MainPage.Current.NotifyUser("No color preview frame source was found.", NotifyType.ErrorMessage);
                return;
            }

            _previewStreamWidth = previewSource.CurrentFormat.VideoFormat.Width;
            _previewStreamHeight = previewSource.CurrentFormat.VideoFormat.Height;
            if (_previewStreamWidth <= 0 || _previewStreamHeight <= 0)
            {
                var previewProperties = _mediaCapture.VideoDeviceController.GetMediaStreamProperties(MediaStreamType.VideoPreview) as VideoEncodingProperties;
                _previewStreamWidth = previewProperties?.Width ?? 0;
                _previewStreamHeight = previewProperties?.Height ?? 0;
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

            try
            {
                PreviewControl.FlowDirection = _mirroringPreview ? FlowDirection.RightToLeft : FlowDirection.LeftToRight;

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
                SetFacesCanvasBounds();
            }
            catch (Exception ex)
            {
                ShowPreviewMessage($"Unable to start camera preview: {ex.Message}");
                MainPage.Current.NotifyUser($"Unable to start camera preview: {ex.Message}", NotifyType.ErrorMessage);
                Debug.WriteLine("MediaPlayerElement preview failed: " + ex);
            }
        }

        private async Task StopPreviewAsync()
        {
            _isPreviewing = false;
            _previewStreamWidth = 0;
            _previewStreamHeight = 0;
            FacesCanvas.Children.Clear();
            FacesCanvas.Visibility = Visibility.Collapsed;

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

        private async Task CreateFaceDetectionEffectAsync()
        {
            if (_mediaCapture == null || !_isPreviewing)
            {
                return;
            }

            try
            {
                var definition = new FaceDetectionEffectDefinition
                {
                    SynchronousDetectionEnabled = false,
                    DetectionMode = FaceDetectionMode.HighPerformance
                };

                _faceDetectionEffect = (FaceDetectionEffect)await _mediaCapture.AddVideoEffectAsync(definition, MediaStreamType.VideoPreview);
                _faceDetectionEffect.FaceDetected += FaceDetectionEffect_FaceDetected;
                _faceDetectionEffect.DesiredDetectionInterval = TimeSpan.FromMilliseconds(33);
                _faceDetectionEffect.Enabled = true;
                FacesCanvas.Visibility = Visibility.Visible;
            }
            catch (Exception ex)
            {
                _faceDetectionEffect = null;
                FacesCanvas.Visibility = Visibility.Collapsed;
                MainPage.Current.NotifyUser($"Unable to enable face detection: {ex.Message}", NotifyType.ErrorMessage);
                Debug.WriteLine("Exception when enabling face detection: " + ex);
            }
        }

        private async Task CleanUpFaceDetectionEffectAsync()
        {
            if (_mediaCapture == null || _faceDetectionEffect == null)
            {
                return;
            }

            try
            {
                _faceDetectionEffect.Enabled = false;
                _faceDetectionEffect.FaceDetected -= FaceDetectionEffect_FaceDetected;
                await _mediaCapture.RemoveEffectAsync(_faceDetectionEffect);
            }
            catch (Exception ex)
            {
                Debug.WriteLine("Exception when disabling face detection: " + ex);
            }
            finally
            {
                _faceDetectionEffect = null;
                FacesCanvas.Children.Clear();
                FacesCanvas.Visibility = Visibility.Collapsed;
            }
        }

        private async Task TakePhotoAsync()
        {
            if (_mediaCapture == null || _captureFolder == null || !_isPreviewing)
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
                await ReencodeAndSavePhotoAsync(stream, file, PhotoOrientation.Normal);
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
            if (_mediaCapture == null || _captureFolder == null || !_isPreviewing)
            {
                return;
            }

            try
            {
                var videoFile = await _captureFolder.CreateFileAsync("SimpleVideo.mp4", CreationCollisionOption.GenerateUniqueName);
                var encodingProfile = MediaEncodingProfile.CreateMp4(VideoEncodingQuality.Auto);
                encodingProfile.Video.Properties.Add(RotationKey, Windows.Foundation.PropertyValue.CreateInt32(0));

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

                if (_faceDetectionEffect != null)
                {
                    await CleanUpFaceDetectionEffectAsync();
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
            FaceDetectionButton.IsEnabled = _isPreviewing;

            FaceDetectionDisabledIcon.Visibility = (_faceDetectionEffect == null || !_faceDetectionEffect.Enabled) ? Visibility.Visible : Visibility.Collapsed;
            FaceDetectionEnabledIcon.Visibility = (_faceDetectionEffect != null && _faceDetectionEffect.Enabled) ? Visibility.Visible : Visibility.Collapsed;
            FacesCanvas.Visibility = (_faceDetectionEffect != null && _faceDetectionEffect.Enabled) ? Visibility.Visible : Visibility.Collapsed;

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

        private void HighlightDetectedFaces(System.Collections.Generic.IReadOnlyList<DetectedFace> faces)
        {
            FacesCanvas.Children.Clear();

            for (int i = 0; i < faces.Count; i++)
            {
                var faceBoundingBox = ConvertPreviewToUiRectangle(faces[i].FaceBox);
                faceBoundingBox.StrokeThickness = 2;
                faceBoundingBox.Stroke = new SolidColorBrush(i == 0 ? Colors.Blue : Colors.DeepSkyBlue);
                FacesCanvas.Children.Add(faceBoundingBox);
            }

            SetFacesCanvasBounds();
        }

        private Rectangle ConvertPreviewToUiRectangle(BitmapBounds faceBoxInPreviewCoordinates)
        {
            var result = new Rectangle();
            if (_previewStreamWidth <= 0 || _previewStreamHeight <= 0)
            {
                return result;
            }

            var previewInUI = GetPreviewStreamRectInControl(_previewStreamWidth, _previewStreamHeight, PreviewControl);
            if (previewInUI.Width <= 0 || previewInUI.Height <= 0)
            {
                return result;
            }

            result.Width = (faceBoxInPreviewCoordinates.Width / _previewStreamWidth) * previewInUI.Width;
            result.Height = (faceBoxInPreviewCoordinates.Height / _previewStreamHeight) * previewInUI.Height;

            Canvas.SetLeft(result, (faceBoxInPreviewCoordinates.X / _previewStreamWidth) * previewInUI.Width);
            Canvas.SetTop(result, (faceBoxInPreviewCoordinates.Y / _previewStreamHeight) * previewInUI.Height);

            return result;
        }

        private void SetFacesCanvasBounds()
        {
            var previewArea = GetPreviewStreamRectInControl(_previewStreamWidth, _previewStreamHeight, PreviewControl);
            FacesCanvas.Width = previewArea.Width;
            FacesCanvas.Height = previewArea.Height;
            Canvas.SetLeft(FacesCanvas, previewArea.X);
            Canvas.SetTop(FacesCanvas, previewArea.Y);
            FacesCanvas.FlowDirection = _mirroringPreview ? FlowDirection.RightToLeft : FlowDirection.LeftToRight;
        }

        private static Rect GetPreviewStreamRectInControl(double streamWidth, double streamHeight, FrameworkElement previewControl)
        {
            var result = new Rect();
            if (previewControl == null || previewControl.ActualHeight < 1 || previewControl.ActualWidth < 1 || streamWidth <= 0 || streamHeight <= 0)
            {
                return result;
            }

            result.Width = previewControl.ActualWidth;
            result.Height = previewControl.ActualHeight;

            if (previewControl.ActualWidth / previewControl.ActualHeight > streamWidth / streamHeight)
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
        }
    }
}
