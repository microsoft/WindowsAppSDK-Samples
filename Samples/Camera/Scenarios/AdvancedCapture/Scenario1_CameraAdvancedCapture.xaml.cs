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
using Microsoft.UI.Xaml.Navigation;
using Windows.Devices.Enumeration;
using Windows.Devices.Sensors;
using Windows.Foundation;
using Windows.Graphics.Imaging;
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

namespace SDKTemplate
{
    public sealed partial class Scenario1_CameraAdvancedCapture : Page
    {
        private readonly DisplayRequest _displayRequest = new();

        private StorageFolder? _captureFolder;
        private MediaCapture? _mediaCapture;
        private MediaPlayer? _mediaPlayer;
        private AdvancedPhotoCapture? _advancedCapture;
        private SceneAnalysisEffect? _sceneAnalysisEffect;

        private bool _isInitialized;
        private bool _isPreviewing;
        private bool _isActivePage;
        private bool _isWindowVisible = true;
        private bool _isDisplayRequestActive;
        private bool _mirroringPreview;
        private bool _externalCamera;
        private int _advancedCaptureMode = -1;
        private Task _setupTask = Task.CompletedTask;

        private const double CertaintyCap = 0.7;

        public Scenario1_CameraAdvancedCapture()
        {
            InitializeComponent();
            NavigationCacheMode = NavigationCacheMode.Disabled;
            HdrImpactBar.Maximum = CertaintyCap;
        }

        public class AdvancedCaptureContext
        {
            public string CaptureFileName { get; set; } = string.Empty;
            public PhotoOrientation CaptureOrientation { get; set; }
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

        private void CycleModeButton_Click(object sender, RoutedEventArgs e)
        {
            CycleAdvancedCaptureMode();
        }

        private async void PhotoButton_Click(object sender, RoutedEventArgs e)
        {
            await TakeAdvancedCapturePhotoAsync();
        }

        private async void MediaCapture_Failed(MediaCapture sender, MediaCaptureFailedEventArgs errorEventArgs)
        {
            Debug.WriteLine("MediaCapture_Failed: (0x{0:X}) {1}", errorEventArgs.Code, errorEventArgs.Message);
            MainPage.Current.NotifyUser($"Camera capture failed: {errorEventArgs.Message}", NotifyType.ErrorMessage);

            await CleanupCameraAsync();
            App.MainDispatcherQueue.TryEnqueue(UpdateUi);
        }

        private async void SceneAnalysisEffect_SceneAnalyzed(SceneAnalysisEffect sender, SceneAnalyzedEventArgs args)
        {
            await App.MainDispatcherQueue.EnqueueAsync(() =>
            {
                HdrImpactBar.Value = Math.Min(CertaintyCap, args.ResultFrame.HighDynamicRange.Certainty);
                SceneTypeTextBlock.Text = "Scene: " + args.ResultFrame.AnalysisRecommendation;
            });
        }

        private async void AdvancedCapture_OptionalReferencePhotoCaptured(AdvancedPhotoCapture sender, OptionalReferencePhotoCapturedEventArgs args)
        {
            if (_captureFolder == null || args.Context is not AdvancedCaptureContext context)
            {
                return;
            }

            var referenceName = context.CaptureFileName.Replace(".jpg", "_Reference.jpg", StringComparison.OrdinalIgnoreCase);

            using (var frame = args.Frame)
            {
                var file = await _captureFolder.CreateFileAsync(referenceName, CreationCollisionOption.GenerateUniqueName);
                Debug.WriteLine("AdvancedCapture_OptionalReferencePhotoCaptured for " + context.CaptureFileName + ". Saving to " + file.Path);

                await ReencodeAndSavePhotoAsync(frame, file, context.CaptureOrientation);
            }
        }

        private void AdvancedCapture_AllPhotosCaptured(AdvancedPhotoCapture sender, object args)
        {
            Debug.WriteLine("AdvancedCapture_AllPhotosCaptured");
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
                ShowPreviewMessage("No camera device was found. Connect a camera to use preview and advanced photo capture.");
                MainPage.Current.NotifyUser("No camera device was found.", NotifyType.ErrorMessage);
                return;
            }

            _mediaCapture = new MediaCapture();
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

            _externalCamera = CameraRotationHelper.IsEnclosureLocationExternal(cameraDevice.EnclosureLocation);
            _mirroringPreview = cameraDevice.EnclosureLocation?.Panel == Windows.Devices.Enumeration.Panel.Front;

            await StartPreviewAsync();

            if (_isPreviewing)
            {
                await CreateSceneAnalysisEffectAsync();
                await EnableAdvancedCaptureAsync();
            }

            UpdateUi();
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
                ShowPreviewMessage("This camera does not expose a color preview frame source. Advanced capture may still be available.");
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

                await SetPreviewRotationAsync();
            }
            catch (Exception ex)
            {
                ShowPreviewMessage("Unable to start camera preview. Advanced capture may still be available.");
                MainPage.Current.NotifyUser($"Unable to start camera preview: {ex.Message}", NotifyType.ErrorMessage);
                Debug.WriteLine("MediaPlayerElement preview failed: " + ex);
            }
        }

        private Task SetPreviewRotationAsync()
        {
            // WinUI 3 has no CoreWindow-backed DisplayInformation.GetForCurrentView().
            // The port keeps capture orientation metadata neutral and mirrors front cameras only.
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

        private async Task CreateSceneAnalysisEffectAsync()
        {
            if (_mediaCapture == null || _sceneAnalysisEffect != null)
            {
                return;
            }

            try
            {
                var definition = new SceneAnalysisEffectDefinition();
                _sceneAnalysisEffect = (SceneAnalysisEffect)await _mediaCapture.AddVideoEffectAsync(definition, MediaStreamType.VideoPreview);
                _sceneAnalysisEffect.SceneAnalyzed += SceneAnalysisEffect_SceneAnalyzed;
                _sceneAnalysisEffect.HighDynamicRangeAnalyzer.Enabled = true;
                Debug.WriteLine("Scene analysis effect added to pipeline");
            }
            catch (Exception ex)
            {
                _sceneAnalysisEffect = null;
                SceneTypeTextBlock.Text = "Scene analysis unavailable";
                MainPage.Current.NotifyUser($"Scene analysis is not available on this camera: {ex.Message}", NotifyType.ErrorMessage);
                Debug.WriteLine("Unable to add scene analysis effect: " + ex);
            }
        }

        private async Task CleanSceneAnalysisEffectAsync()
        {
            if (_mediaCapture == null || _sceneAnalysisEffect == null)
            {
                return;
            }

            try
            {
                _sceneAnalysisEffect.HighDynamicRangeAnalyzer.Enabled = false;
                _sceneAnalysisEffect.SceneAnalyzed -= SceneAnalysisEffect_SceneAnalyzed;
                await _mediaCapture.RemoveEffectAsync(_sceneAnalysisEffect);
                Debug.WriteLine("Scene analysis effect removed from pipeline");
            }
            catch (Exception ex)
            {
                Debug.WriteLine("Unable to remove scene analysis effect: " + ex);
            }
            finally
            {
                _sceneAnalysisEffect = null;
                HdrImpactBar.Value = 0;
                SceneTypeTextBlock.Text = string.Empty;
            }
        }

        private async Task EnableAdvancedCaptureAsync()
        {
            if (_mediaCapture == null || _advancedCapture != null)
            {
                return;
            }

            var control = _mediaCapture.VideoDeviceController.AdvancedPhotoControl;
            if (!control.Supported || control.SupportedModes.Count == 0)
            {
                ModeTextBlock.Text = "Unsupported";
                MainPage.Current.NotifyUser("This camera does not support AdvancedPhotoCapture modes.", NotifyType.ErrorMessage);
                return;
            }

            try
            {
                CycleAdvancedCaptureMode();

                _advancedCapture = await _mediaCapture.PrepareAdvancedPhotoCaptureAsync(ImageEncodingProperties.CreateJpeg());
                _advancedCapture.AllPhotosCaptured += AdvancedCapture_AllPhotosCaptured;
                _advancedCapture.OptionalReferencePhotoCaptured += AdvancedCapture_OptionalReferencePhotoCaptured;
                MainPage.Current.NotifyUser("Advanced photo capture is ready.", NotifyType.StatusMessage);
                Debug.WriteLine("Enabled Advanced Capture");
            }
            catch (Exception ex)
            {
                _advancedCapture = null;
                ModeTextBlock.Text = "Unavailable";
                MainPage.Current.NotifyUser($"Unable to enable AdvancedPhotoCapture: {ex.Message}", NotifyType.ErrorMessage);
                Debug.WriteLine("Unable to enable AdvancedPhotoCapture: " + ex);
            }
        }

        private void CycleAdvancedCaptureMode()
        {
            if (_mediaCapture == null)
            {
                return;
            }

            var control = _mediaCapture.VideoDeviceController.AdvancedPhotoControl;
            if (!control.Supported || control.SupportedModes.Count == 0)
            {
                ModeTextBlock.Text = "Unsupported";
                return;
            }

            _advancedCaptureMode = (_advancedCaptureMode + 1) % control.SupportedModes.Count;

            var settings = new AdvancedPhotoCaptureSettings
            {
                Mode = control.SupportedModes[_advancedCaptureMode]
            };

            control.Configure(settings);
            ModeTextBlock.Text = control.Mode.ToString();
        }

        private async Task DisableAdvancedCaptureAsync()
        {
            if (_advancedCapture == null)
            {
                return;
            }

            _advancedCapture.AllPhotosCaptured -= AdvancedCapture_AllPhotosCaptured;
            _advancedCapture.OptionalReferencePhotoCaptured -= AdvancedCapture_OptionalReferencePhotoCaptured;

            try
            {
                await _advancedCapture.FinishAsync();
            }
            catch (Exception ex)
            {
                Debug.WriteLine("Unable to finish AdvancedPhotoCapture: " + ex);
            }
            finally
            {
                _advancedCapture = null;
                _advancedCaptureMode = -1;
                ModeTextBlock.Text = "Standard";
            }
        }

        private async Task TakeAdvancedCapturePhotoAsync()
        {
            if (_advancedCapture == null || _captureFolder == null)
            {
                return;
            }

            PhotoButton.IsEnabled = false;
            CycleModeButton.IsEnabled = false;

            try
            {
                Debug.WriteLine("Taking Advanced Capture photo...");
                var photoOrientation = ConvertOrientationToPhotoOrientation(GetCameraOrientation());
                var fileName = $"AdvancedCapturePhoto_{DateTime.Now:HHmmss}.jpg";
                var context = new AdvancedCaptureContext { CaptureFileName = fileName, CaptureOrientation = photoOrientation };

                var capture = await _advancedCapture.CaptureAsync(context);

                using (var frame = capture.Frame)
                {
                    var file = await _captureFolder.CreateFileAsync(fileName, CreationCollisionOption.GenerateUniqueName);
                    Debug.WriteLine("Advanced Capture photo taken! Saving to " + file.Path);

                    await ReencodeAndSavePhotoAsync(frame, file, photoOrientation);
                    MainPage.Current.NotifyUser($"Advanced capture photo saved to {file.Path}", NotifyType.StatusMessage);
                }
            }
            catch (Exception ex)
            {
                MainPage.Current.NotifyUser($"Exception when taking an Advanced Capture photo: {ex.Message}", NotifyType.ErrorMessage);
                Debug.WriteLine("Exception when taking an Advanced Capture photo: " + ex);
            }
            finally
            {
                UpdateUi();
            }
        }

        private async Task CleanupCameraAsync()
        {
            Debug.WriteLine("CleanupCameraAsync");

            if (_isInitialized)
            {
                if (_advancedCapture != null)
                {
                    await DisableAdvancedCaptureAsync();
                }

                if (_sceneAnalysisEffect != null)
                {
                    await CleanSceneAnalysisEffectAsync();
                }

                if (_isPreviewing)
                {
                    await StopPreviewAsync();
                }

                _isInitialized = false;
            }

            if (_mediaCapture != null)
            {
                _mediaCapture.Failed -= MediaCapture_Failed;
                _mediaCapture.Dispose();
                _mediaCapture = null;
            }

            UpdateUi();
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
                }
            };

            _setupTask = setupAsync();
            await _setupTask;
        }

        private async Task SetupUiAsync()
        {
            _captureFolder ??= await GetCaptureFolderAsync();
        }

        private void UpdateUi()
        {
            PhotoButton.IsEnabled = _isPreviewing && _advancedCapture != null;
            CycleModeButton.IsEnabled = _isPreviewing && _mediaCapture?.VideoDeviceController.AdvancedPhotoControl.Supported == true;
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
            if (_externalCamera)
            {
                return SimpleOrientation.NotRotated;
            }

            // DisplayInformation.GetForCurrentView() requires a UWP CoreWindow. The WinUI 3 port
            // keeps a neutral orientation for saved photos and relies on camera apps/viewers to
            // display the JPEG without additional rotation metadata.
            return SimpleOrientation.NotRotated;
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

    internal static class DispatcherQueueExtensions
    {
        public static Task EnqueueAsync(this Microsoft.UI.Dispatching.DispatcherQueue dispatcherQueue, Action callback)
        {
            var tcs = new TaskCompletionSource();
            if (!dispatcherQueue.TryEnqueue(() =>
            {
                try
                {
                    callback();
                    tcs.SetResult();
                }
                catch (Exception ex)
                {
                    tcs.SetException(ex);
                }
            }))
            {
                tcs.SetException(new InvalidOperationException("Unable to enqueue work on the UI thread."));
            }

            return tcs.Task;
        }
    }
}
