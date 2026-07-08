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

using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Media.Imaging;
using Microsoft.UI.Xaml.Navigation;
using System;
using System.Diagnostics;
using System.Linq;
using System.Runtime.InteropServices;
using System.Threading;
using System.Threading.Tasks;
using Windows.Devices.Enumeration;
using Windows.Graphics.Imaging;
using Windows.Media;
using Windows.Media.Capture;
using Windows.Media.Capture.Frames;
using Windows.Media.Core;
using Windows.Media.MediaProperties;
using Windows.Media.Playback;
using Windows.Storage;
using Windows.System.Display;

namespace SDKTemplate
{
    public sealed partial class Scenario1_GetPreviewFrame : Page
    {
        private static readonly Guid RotationKey = new Guid("C380465D-2271-428C-9B83-ECEA3B4A85C1");

        private StorageFolder _captureFolder = ApplicationData.Current.LocalFolder;
        private readonly DisplayRequest _displayRequest = new DisplayRequest();
        private bool _displayRequestActive;

        private MediaCapture? _mediaCapture;
        private MediaPlayer? _previewPlayer;
        private bool _isInitialized;
        private bool _isPreviewing;
        private static readonly SemaphoreSlim _mediaCaptureLifeLock = new SemaphoreSlim(1);

        private bool _mirroringPreview;
        private bool _externalCamera;

        public Scenario1_GetPreviewFrame()
        {
            InitializeComponent();
            NavigationCacheMode = NavigationCacheMode.Required;
        }

        protected override async void OnNavigatedTo(NavigationEventArgs e)
        {
            await InitializeCameraAsync();
        }

        protected override async void OnNavigatingFrom(NavigatingCancelEventArgs e)
        {
            await CleanupCameraAsync();
        }

        private async void GetPreviewFrameButton_Click(object sender, RoutedEventArgs e)
        {
            if (!_isPreviewing || _mediaCapture == null)
            {
                return;
            }

            try
            {
                if ((ShowFrameCheckBox.IsChecked == true) || (SaveFrameCheckBox.IsChecked == true))
                {
                    await GetPreviewFrameAsSoftwareBitmapAsync();
                }
                else
                {
                    await GetPreviewFrameAsD3DSurfaceAsync();
                }
            }
            catch (Exception ex)
            {
                FrameInfoTextBlock.Text = string.Empty;
                MainPage.Current.NotifyUser($"GetPreviewFrameAsync failed: {ex.Message}", NotifyType.ErrorMessage);
            }
        }

        private async void MediaCapture_Failed(MediaCapture sender, MediaCaptureFailedEventArgs errorEventArgs)
        {
            Debug.WriteLine("MediaCapture_Failed: (0x{0:X}) {1}", errorEventArgs.Code, errorEventArgs.Message);

            await CleanupCameraAsync();

            App.MainDispatcherQueue.TryEnqueue(() =>
            {
                GetPreviewFrameButton.IsEnabled = _isPreviewing;
                PreviewPlaceholderTextBlock.Text = "The camera preview stopped.";
                PreviewPlaceholderTextBlock.Visibility = Visibility.Visible;
            });
        }

        private async Task InitializeCameraAsync()
        {
            Debug.WriteLine("InitializeCameraAsync");

            await _mediaCaptureLifeLock.WaitAsync();

            try
            {
                if (_mediaCapture != null)
                {
                    return;
                }

                var cameraDevice = await FindCameraDeviceByPanelAsync(Windows.Devices.Enumeration.Panel.Back);

                if (cameraDevice == null)
                {
                    Debug.WriteLine("No camera device found!");
                    PreviewPlaceholderTextBlock.Text = "No camera device was found. Connect a camera to use this sample.";
                    PreviewPlaceholderTextBlock.Visibility = Visibility.Visible;
                    MainPage.Current.NotifyUser("No camera device was found. The sample is running without a preview.", NotifyType.ErrorMessage);
                    return;
                }

                _mediaCapture = new MediaCapture();
                _mediaCapture.Failed += MediaCapture_Failed;

                var settings = new MediaCaptureInitializationSettings
                {
                    VideoDeviceId = cameraDevice.Id,
                    StreamingCaptureMode = StreamingCaptureMode.Video,
                    MemoryPreference = MediaCaptureMemoryPreference.Auto
                };

                try
                {
                    await _mediaCapture.InitializeAsync(settings);
                    _isInitialized = true;
                }
                catch (UnauthorizedAccessException)
                {
                    PreviewPlaceholderTextBlock.Text = "Camera access was denied. Enable webcam access in Windows Settings.";
                    PreviewPlaceholderTextBlock.Visibility = Visibility.Visible;
                    MainPage.Current.NotifyUser("The app was denied access to the camera.", NotifyType.ErrorMessage);
                }
                catch (Exception ex)
                {
                    PreviewPlaceholderTextBlock.Text = "The camera could not be initialized.";
                    PreviewPlaceholderTextBlock.Visibility = Visibility.Visible;
                    MainPage.Current.NotifyUser($"The camera could not be initialized: {ex.Message}", NotifyType.ErrorMessage);
                }

                if (_isInitialized)
                {
                    if (cameraDevice.EnclosureLocation == null || cameraDevice.EnclosureLocation.Panel == Windows.Devices.Enumeration.Panel.Unknown)
                    {
                        _externalCamera = true;
                    }
                    else
                    {
                        _externalCamera = false;
                        _mirroringPreview = (cameraDevice.EnclosureLocation.Panel == Windows.Devices.Enumeration.Panel.Front);
                    }

                    await StartPreviewAsync();

                    try
                    {
                        var picturesLibrary = await StorageLibrary.GetLibraryAsync(KnownLibraryId.Pictures);
                        _captureFolder = picturesLibrary.SaveFolder ?? ApplicationData.Current.LocalFolder;
                    }
                    catch (Exception ex)
                    {
                        Debug.WriteLine("Pictures library unavailable; using local app storage. " + ex.Message);
                        _captureFolder = ApplicationData.Current.LocalFolder;
                    }
                }
            }
            finally
            {
                _mediaCaptureLifeLock.Release();
            }
        }

        private async Task StartPreviewAsync()
        {
            Debug.WriteLine("StartPreviewAsync");

            if (_mediaCapture == null)
            {
                return;
            }

            if (!_displayRequestActive)
            {
                _displayRequest.RequestActive();
                _displayRequestActive = true;
            }

            PreviewControl.FlowDirection = _mirroringPreview ? FlowDirection.RightToLeft : FlowDirection.LeftToRight;

            var previewFrameSource = FindPreviewFrameSource(_mediaCapture);
            if (previewFrameSource != null)
            {
                _previewPlayer = new MediaPlayer { AutoPlay = true };
                _previewPlayer.Source = MediaSource.CreateFromMediaFrameSource(previewFrameSource);
                PreviewControl.SetMediaPlayer(_previewPlayer);
                _previewPlayer.Play();
                PreviewPlaceholderTextBlock.Visibility = Visibility.Collapsed;
            }
            else
            {
                PreviewPlaceholderTextBlock.Text = "No color preview frame source was found for this camera.";
                PreviewPlaceholderTextBlock.Visibility = Visibility.Visible;
                MainPage.Current.NotifyUser("No color preview frame source was found for this camera.", NotifyType.ErrorMessage);
            }

            try
            {
                await _mediaCapture.StartPreviewAsync();
                _isPreviewing = true;
                await SetPreviewRotationAsync();
            }
            catch (Exception ex)
            {
                Debug.WriteLine("StartPreviewAsync failed: " + ex.Message);
                _isPreviewing = previewFrameSource != null;
                MainPage.Current.NotifyUser("Live preview is using MediaPlayerElement. GetPreviewFrameAsync may require a camera that supports MediaCapture preview frames.", NotifyType.StatusMessage);
            }

            GetPreviewFrameButton.IsEnabled = _isPreviewing;
        }

        private async Task SetPreviewRotationAsync()
        {
            if (_mediaCapture == null || _externalCamera)
            {
                return;
            }

            const int rotationDegrees = 0;

            try
            {
                var props = _mediaCapture.VideoDeviceController.GetMediaStreamProperties(MediaStreamType.VideoPreview);
                if (props.Properties.ContainsKey(RotationKey))
                {
                    props.Properties[RotationKey] = rotationDegrees;
                }
                else
                {
                    props.Properties.Add(RotationKey, rotationDegrees);
                }

                await _mediaCapture.SetEncodingPropertiesAsync(MediaStreamType.VideoPreview, props, null);
            }
            catch (Exception ex)
            {
                Debug.WriteLine("SetPreviewRotationAsync skipped: " + ex.Message);
            }
        }

        private async Task StopPreviewAsync()
        {
            _isPreviewing = false;

            if (_mediaCapture != null)
            {
                try
                {
                    await _mediaCapture.StopPreviewAsync();
                }
                catch (Exception ex)
                {
                    Debug.WriteLine("StopPreviewAsync skipped: " + ex.Message);
                }
            }

            await EnqueueOnUiThreadAsync(() =>
            {
                PreviewControl.SetMediaPlayer(null);
                _previewPlayer?.Dispose();
                _previewPlayer = null;

                if (_displayRequestActive)
                {
                    _displayRequest.RequestRelease();
                    _displayRequestActive = false;
                }

                GetPreviewFrameButton.IsEnabled = _isPreviewing;
                PreviewPlaceholderTextBlock.Visibility = Visibility.Visible;
            });
        }

        private async Task GetPreviewFrameAsSoftwareBitmapAsync()
        {
            if (_mediaCapture == null)
            {
                return;
            }

            var previewProperties = _mediaCapture.VideoDeviceController.GetMediaStreamProperties(MediaStreamType.VideoPreview) as VideoEncodingProperties;
            if (previewProperties == null)
            {
                MainPage.Current.NotifyUser("The camera did not report video preview properties.", NotifyType.ErrorMessage);
                return;
            }

            var videoFrame = new VideoFrame(BitmapPixelFormat.Bgra8, (int)previewProperties.Width, (int)previewProperties.Height);

            using (var currentFrame = await _mediaCapture.GetPreviewFrameAsync(videoFrame))
            {
                SoftwareBitmap? previewFrame = currentFrame.SoftwareBitmap;
                if (previewFrame == null)
                {
                    MainPage.Current.NotifyUser("The preview frame did not contain a SoftwareBitmap.", NotifyType.ErrorMessage);
                    return;
                }

                FrameInfoTextBlock.Text = string.Format("{0}x{1} {2}", previewFrame.PixelWidth, previewFrame.PixelHeight, previewFrame.BitmapPixelFormat);

                if (GreenEffectCheckBox.IsChecked == true)
                {
                    ApplyGreenFilter(previewFrame);
                }

                if (ShowFrameCheckBox.IsChecked == true)
                {
                    var displayBitmap = SoftwareBitmap.Convert(previewFrame, BitmapPixelFormat.Bgra8, BitmapAlphaMode.Premultiplied);
                    var sbSource = new SoftwareBitmapSource();
                    await sbSource.SetBitmapAsync(displayBitmap);
                    PreviewFrameImage.Source = sbSource;
                }

                if (SaveFrameCheckBox.IsChecked == true)
                {
                    var file = await _captureFolder.CreateFileAsync("PreviewFrame.jpg", CreationCollisionOption.GenerateUniqueName);
                    Debug.WriteLine("Saving preview frame to " + file.Path);
                    await SaveSoftwareBitmapAsync(previewFrame, file);
                }
            }
        }

        private async Task GetPreviewFrameAsD3DSurfaceAsync()
        {
            if (_mediaCapture == null)
            {
                return;
            }

            using (var currentFrame = await _mediaCapture.GetPreviewFrameAsync())
            {
                if (currentFrame.Direct3DSurface != null)
                {
                    var surface = currentFrame.Direct3DSurface;
                    FrameInfoTextBlock.Text = string.Format("{0}x{1} {2}", surface.Description.Width, surface.Description.Height, surface.Description.Format);
                }
                else if (currentFrame.SoftwareBitmap != null)
                {
                    SoftwareBitmap previewFrame = currentFrame.SoftwareBitmap;
                    FrameInfoTextBlock.Text = string.Format("{0}x{1} {2}", previewFrame.PixelWidth, previewFrame.PixelHeight, previewFrame.BitmapPixelFormat);
                }
                else
                {
                    FrameInfoTextBlock.Text = "The preview frame did not contain a D3D surface or SoftwareBitmap.";
                }

                PreviewFrameImage.Source = null;
            }
        }

        private async Task CleanupCameraAsync()
        {
            await _mediaCaptureLifeLock.WaitAsync();

            try
            {
                if (_isInitialized)
                {
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
            }
            finally
            {
                _mediaCaptureLifeLock.Release();
            }
        }

        private static MediaFrameSource? FindPreviewFrameSource(MediaCapture mediaCapture)
        {
            return mediaCapture.FrameSources.Values.FirstOrDefault(source =>
                       source.Info.SourceKind == MediaFrameSourceKind.Color &&
                       source.Info.MediaStreamType == MediaStreamType.VideoPreview)
                   ?? mediaCapture.FrameSources.Values.FirstOrDefault(source =>
                       source.Info.SourceKind == MediaFrameSourceKind.Color);
        }

        private static async Task<DeviceInformation?> FindCameraDeviceByPanelAsync(Windows.Devices.Enumeration.Panel desiredPanel)
        {
            var allVideoDevices = await DeviceInformation.FindAllAsync(DeviceClass.VideoCapture);
            DeviceInformation? desiredDevice = allVideoDevices.FirstOrDefault(x => x.EnclosureLocation != null && x.EnclosureLocation.Panel == desiredPanel);
            return desiredDevice ?? allVideoDevices.FirstOrDefault();
        }

        private static async Task SaveSoftwareBitmapAsync(SoftwareBitmap bitmap, StorageFile file)
        {
            using (var outputStream = await file.OpenAsync(FileAccessMode.ReadWrite))
            {
                var encoder = await BitmapEncoder.CreateAsync(BitmapEncoder.JpegEncoderId, outputStream);
                encoder.SetSoftwareBitmap(bitmap);
                await encoder.FlushAsync();
            }
        }

        private unsafe void ApplyGreenFilter(SoftwareBitmap bitmap)
        {
            if (bitmap.BitmapPixelFormat == BitmapPixelFormat.Bgra8)
            {
                const int BYTES_PER_PIXEL = 4;

                using (var buffer = bitmap.LockBuffer(BitmapBufferAccessMode.ReadWrite))
                using (var reference = buffer.CreateReference())
                {
                    if (reference is IMemoryBufferByteAccess byteAccess)
                    {
                        byte* data;
                        uint capacity;
                        byteAccess.GetBuffer(out data, out capacity);

                        var desc = buffer.GetPlaneDescription(0);

                        for (uint row = 0; row < desc.Height; row++)
                        {
                            for (uint col = 0; col < desc.Width; col++)
                            {
                                var currPixel = desc.StartIndex + desc.Stride * row + BYTES_PER_PIXEL * col;
                                var b = data[currPixel + 0];
                                var g = data[currPixel + 1];
                                var r = data[currPixel + 2];

                                data[currPixel + 0] = b;
                                data[currPixel + 1] = (byte)Math.Min(g + 80, 255);
                                data[currPixel + 2] = r;
                            }
                        }
                    }
                }
            }
        }

        private static Task EnqueueOnUiThreadAsync(Action action)
        {
            if (App.MainDispatcherQueue.HasThreadAccess)
            {
                action();
                return Task.CompletedTask;
            }

            var tcs = new TaskCompletionSource<object?>();
            App.MainDispatcherQueue.TryEnqueue(() =>
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
            });
            return tcs.Task;
        }
    }
}

