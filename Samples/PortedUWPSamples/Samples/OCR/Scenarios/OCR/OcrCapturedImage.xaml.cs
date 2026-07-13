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

using SDKTemplate;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Windows.Devices.Enumeration;
using Windows.Globalization;
using Windows.Graphics.Imaging;
using Windows.Media;
using Windows.Media.Capture;
using Windows.Media.Capture.Frames;
using Windows.Media.Core;
using Windows.Media.MediaProperties;
using Windows.Media.Ocr;
using Windows.Media.Playback;
using Windows.System.Display;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Media;
using Microsoft.UI.Xaml.Media.Imaging;
using Microsoft.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class OcrCapturedImage : Page
    {
        // A pointer back to the main page.
        // This is needed if you want to call methods in MainPage such as NotifyUser()
        private MainPage rootPage = MainPage.Current;

        // Language for OCR.
        private Language ocrLanguage = new Language("en");

        // Recognized words overlay boxes.
        private List<WordOverlay> wordBoxes = new List<WordOverlay>();

        // Rotation metadata to apply to the preview stream (MF_MT_VIDEO_ROTATION).
        // Reference: http://msdn.microsoft.com/en-us/library/windows/apps/xaml/hh868174.aspx
        private static readonly Guid RotationKey = new Guid("C380465D-2271-428C-9B83-ECEA3B4A85C1");

        // Prevent the screen from sleeping while the camera is running.
        private readonly DisplayRequest displayRequest = new DisplayRequest();

        // MediaCapture and its state variables.
        private MediaCapture mediaCapture;
        private MediaPlayer previewPlayer;
        private bool isInitialized = false;
        private bool isPreviewing = false;

        // Information about the camera device.
        private bool mirroringPreview = false;
        private bool externalCamera = false;

        public OcrCapturedImage()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// Ckecks if English language is available for OCR on device and starts camera preview..
        /// </summary>
        /// <param name="e"></param>
        protected override async void OnNavigatedTo(NavigationEventArgs e)
        {
            if (!OcrEngine.IsLanguageSupported(ocrLanguage))
            {
                rootPage.NotifyUser(ocrLanguage.DisplayName + " is not supported.", NotifyType.ErrorMessage);

                return;
            }

            await StartCameraAsync();

            // Update buttons visibility.
            ExtractButton.Visibility = isInitialized ? Visibility.Visible : Visibility.Collapsed;
            CameraButton.Visibility = isInitialized ? Visibility.Collapsed : Visibility.Visible;
        }

        /// <summary>
        /// Invoked immediately before the Page is unloaded and is no longer the current source of a parent Frame.
        /// Stops camera if initialized.
        /// </summary>
        /// <param name="e"></param>
        protected override async void OnNavigatedFrom(NavigationEventArgs e)
        {
            await CleanupCameraAsync();
        }

        /// <summary>
        /// This is event handler for 'Extract' button.
        /// Captures image from camera ,recognizes text and displays it.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void ExtractButton_Click(object sender, Microsoft.UI.Xaml.RoutedEventArgs e)
        {
            //Get information about the preview.
            var previewProperties = mediaCapture.VideoDeviceController.GetMediaStreamProperties(MediaStreamType.VideoPreview) as VideoEncodingProperties;
            int videoFrameWidth = (int)previewProperties.Width;
            int videoFrameHeight = (int)previewProperties.Height;

            // Create the video frame to request a SoftwareBitmap preview frame.
            var videoFrame = new VideoFrame(BitmapPixelFormat.Bgra8, videoFrameWidth, videoFrameHeight);

            // Capture the preview frame.
            using (var currentFrame = await mediaCapture.GetPreviewFrameAsync(videoFrame))
            {
                // Collect the resulting frame.
                SoftwareBitmap bitmap = currentFrame.SoftwareBitmap;

                OcrEngine ocrEngine = OcrEngine.TryCreateFromLanguage(ocrLanguage);

                if (ocrEngine == null)
                {
                    rootPage.NotifyUser(ocrLanguage.DisplayName + " is not supported.", NotifyType.ErrorMessage);

                    return;
                }

                var imgSource = new WriteableBitmap(bitmap.PixelWidth, bitmap.PixelHeight);
                bitmap.CopyToBuffer(imgSource.PixelBuffer);
                PreviewImage.Source = imgSource;

                var ocrResult = await ocrEngine.RecognizeAsync(bitmap);

                // Used for text overlay.
                // Prepare scale transform for words since image is not displayed in original format.
                var scaleTrasform = new ScaleTransform
                {
                    CenterX = 0,
                    CenterY = 0,
                    ScaleX = PreviewControl.ActualWidth / bitmap.PixelWidth,
                    ScaleY = PreviewControl.ActualHeight / bitmap.PixelHeight
                };

                if (ocrResult.TextAngle != null)
                {
                    // If text is detected under some angle in this sample scenario we want to
                    // overlay word boxes over original image, so we rotate overlay boxes.
                    TextOverlay.RenderTransform = new RotateTransform
                    {
                        Angle = (double)ocrResult.TextAngle,
                        CenterX = PreviewImage.ActualWidth / 2,
                        CenterY = PreviewImage.ActualHeight / 2
                    };
                }

                // Iterate over recognized lines of text.
                foreach (var line in ocrResult.Lines)
                {
                    // Iterate over words in line.
                    foreach (var word in line.Words)
                    {
                        WordOverlay wordBoxOverlay = new WordOverlay(word);

                        // Keep references to word boxes.
                        wordBoxes.Add(wordBoxOverlay);

                        // Create a box with the word inside it.
                        var textBlock = new TextBlock()
                        {
                            Text = word.Text,
                            Style = ExtractedWordTextStyle
                        };
                        TextOverlay.Children.Add(wordBoxOverlay.CreateBorder(HighlightedWordBoxHorizontalLineStyle, textBlock));
                    }
                }

                rootPage.NotifyUser("Image processed using " + ocrEngine.RecognizerLanguage.DisplayName + " language.", NotifyType.StatusMessage);
            }

            UpdateWordBoxTransform();

            PreviewControl.Visibility = Visibility.Collapsed;
            Image.Visibility = Visibility.Visible;
            ExtractButton.Visibility = Visibility.Collapsed;
            CameraButton.Visibility = Visibility.Visible;
        }

        /// <summary>
        /// This is event handler for 'Camera' button.
        /// Clears previous OCR results and starts camera.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void CameraButton_Click(object sender, Microsoft.UI.Xaml.RoutedEventArgs e)
        {
            await StartCameraAsync();
        }

        /// <summary>
        ///  Update word box transform to match current UI size.
        /// </summary>
        private void UpdateWordBoxTransform()
        {
            WriteableBitmap bitmap = PreviewImage.Source as WriteableBitmap;

            if (bitmap != null)
            {
                // Used for text overlay.
                // Prepare scale transform for words since image is not displayed in original size.
                ScaleTransform scaleTransform = new ScaleTransform
                {
                    CenterX = 0,
                    CenterY = 0,
                    ScaleX = PreviewImage.ActualWidth / bitmap.PixelWidth,
                    ScaleY = PreviewImage.ActualHeight / bitmap.PixelHeight
                };

                foreach (var item in wordBoxes)
                {
                    item.Transform(scaleTransform);
                }
            }
        }

        /// <summary>
        /// Occurs when displayed image size changes.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void PreviewImage_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            UpdateWordBoxTransform();

            // Update image rotation center.
            var rotate = TextOverlay.RenderTransform as RotateTransform;
            if (rotate != null)
            {
                rotate.CenterX = PreviewImage.ActualWidth / 2;
                rotate.CenterY = PreviewImage.ActualHeight / 2;
            }
        }

        #region MediaCapture methods

        /// <summary>
        /// Starts the camera. Initializes resources and starts preview.
        /// </summary>
        private async Task StartCameraAsync()
        {
            if (!isInitialized)
            {
                await InitializeCameraAsync();
            }

            if (isInitialized)
            {
                TextOverlay.Children.Clear();
                wordBoxes.Clear();

                PreviewImage.Source = null;

                PreviewControl.Visibility = Visibility.Visible;
                Image.Visibility = Visibility.Collapsed;

                ExtractButton.Visibility = Visibility.Visible;
                CameraButton.Visibility = Visibility.Collapsed;

                rootPage.NotifyUser("Camera started.", NotifyType.StatusMessage);
            }
        }

        /// <summary>
        /// Initializes the MediaCapture, registers events, gets camera device information for mirroring and rotating, and starts preview.
        /// </summary>
        private async Task InitializeCameraAsync()
        {
            if (mediaCapture == null)
            {
                // Attempt to get the back camera if one is available, but use any camera device if not.
                var cameraDevice = await FindCameraDeviceByPanelAsync(Windows.Devices.Enumeration.Panel.Back);

                if (cameraDevice == null)
                {
                    rootPage.NotifyUser("No camera device!", NotifyType.ErrorMessage);

                    return;
                }

                // Create MediaCapture and its settings.
                mediaCapture = new MediaCapture();

                // Register for a notification when something goes wrong
                mediaCapture.Failed += MediaCapture_Failed;

                var settings = new MediaCaptureInitializationSettings { VideoDeviceId = cameraDevice.Id };

                // Initialize MediaCapture
                try
                {
                    await mediaCapture.InitializeAsync(settings);
                    isInitialized = true;
                }
                catch (UnauthorizedAccessException)
                {
                    rootPage.NotifyUser("Denied access to the camera.", NotifyType.ErrorMessage);
                }
                catch (Exception ex)
                {
                    rootPage.NotifyUser("Exception when init MediaCapture. " + ex.Message, NotifyType.ErrorMessage);
                }

                // If initialization succeeded, start the preview.
                if (isInitialized)
                {
                    // Figure out where the camera is located
                    if (cameraDevice.EnclosureLocation == null || cameraDevice.EnclosureLocation.Panel == Windows.Devices.Enumeration.Panel.Unknown)
                    {
                        // No information on the location of the camera, assume it's an external camera, not integrated on the device.
                        externalCamera = true;
                    }
                    else
                    {
                        // Camera is fixed on the device.
                        externalCamera = false;

                        // Only mirror the preview if the camera is on the front panel.
                        mirroringPreview = (cameraDevice.EnclosureLocation.Panel == Windows.Devices.Enumeration.Panel.Front);
                    }

                    await StartPreviewAsync();
                }
            }
        }

        /// <summary>
        /// Starts the preview and adjusts it for for rotation and mirroring after making a request to keep the screen on and unlocks the UI.
        /// </summary>
        private async Task StartPreviewAsync()
        {
            // Prevent the device from sleeping while the preview is running.
            displayRequest.RequestActive();

            // Set the preview source in the UI and mirror it if necessary.
            // WinUI 3 has no CaptureElement; route the camera's color preview frame source
            // through a MediaPlayer hosted in a MediaPlayerElement.
            var previewFrameSource = FindPreviewFrameSource(mediaCapture);
            if (previewFrameSource != null)
            {
                previewPlayer = new MediaPlayer { AutoPlay = true };
                previewPlayer.Source = MediaSource.CreateFromMediaFrameSource(previewFrameSource);
                PreviewControl.SetMediaPlayer(previewPlayer);
                previewPlayer.Play();
            }
            PreviewControl.FlowDirection = mirroringPreview ? FlowDirection.RightToLeft : FlowDirection.LeftToRight;

            // Start the preview.
            try
            {
                await mediaCapture.StartPreviewAsync();
                isPreviewing = true;
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser("Exception starting preview." + ex.Message, NotifyType.ErrorMessage);
            }

            // Initialize the preview to the current orientation.
            if (isPreviewing)
            {
                await SetPreviewRotationAsync();
            }
        }

        private static MediaFrameSource FindPreviewFrameSource(MediaCapture mediaCapture)
        {
            return mediaCapture.FrameSources.Values.FirstOrDefault(source =>
                       source.Info.SourceKind == MediaFrameSourceKind.Color &&
                       source.Info.MediaStreamType == MediaStreamType.VideoPreview)
                   ?? mediaCapture.FrameSources.Values.FirstOrDefault(source =>
                       source.Info.SourceKind == MediaFrameSourceKind.Color);
        }

        private async Task SetPreviewRotationAsync()
        {
            if (mediaCapture == null)
            {
                return;
            }

            const int rotationDegrees = 0;
            try
            {
                var props = mediaCapture.VideoDeviceController.GetMediaStreamProperties(MediaStreamType.VideoPreview);
                if (props.Properties.ContainsKey(RotationKey))
                {
                    props.Properties[RotationKey] = rotationDegrees;
                }
                else
                {
                    props.Properties.Add(RotationKey, rotationDegrees);
                }
                await mediaCapture.SetEncodingPropertiesAsync(MediaStreamType.VideoPreview, props, null);
            }
            catch (Exception ex)
            {
                System.Diagnostics.Debug.WriteLine("SetPreviewRotationAsync skipped: " + ex.Message);
            }
        }

        /// <summary>
        /// Stops the preview and deactivates a display request, to allow the screen to go into power saving modes, and locks the UI
        /// </summary>
        /// <returns></returns>
        private async Task StopPreviewAsync()
        {
            try
            {
                isPreviewing = false;
                await mediaCapture.StopPreviewAsync();
            }
            catch (Exception ex)
            {
                // Use the dispatcher because this method is sometimes called from non-UI threads.
                DispatcherQueue.TryEnqueue(() =>
                {
                    rootPage.NotifyUser("Exception stopping preview. " + ex.Message, NotifyType.ErrorMessage);
                });
            }

            // Use the dispatcher because this method is sometimes called from non-UI threads.
            DispatcherQueue.TryEnqueue(() =>
            {
                PreviewControl.SetMediaPlayer(null);
                previewPlayer?.Dispose();
                previewPlayer = null;

                // Allow the device to sleep now that the preview is stopped.
                displayRequest.RequestRelease();
            });
        }

        /// <summary>
        /// Cleans up the camera resources (after stopping the preview if necessary) and unregisters from MediaCapture events.
        /// </summary>
        private async Task CleanupCameraAsync()
        {
            if (isInitialized)
            {
                if (isPreviewing)
                {
                    // The call to stop the preview is included here for completeness, but can be
                    // safely removed if a call to MediaCapture.Dispose() is being made later,
                    // as the preview will be automatically stopped at that point
                    await StopPreviewAsync();
                }

                isInitialized = false;
            }

            if (mediaCapture != null)
            {
                mediaCapture.Failed -= MediaCapture_Failed;
                mediaCapture.Dispose();
                mediaCapture = null;
            }
        }

        /// <summary>
        /// Queries the available video capture devices to try and find one mounted on the desired panel.
        /// </summary>
        /// <param name="desiredPanel">The panel on the device that the desired camera is mounted on.</param>
        /// <returns>A DeviceInformation instance with a reference to the camera mounted on the desired panel if available,
        ///          any other camera if not, or null if no camera is available.</returns>
        private static async Task<DeviceInformation> FindCameraDeviceByPanelAsync(Windows.Devices.Enumeration.Panel desiredPanel)
        {
            // Get available devices for capturing pictures.
            var allVideoDevices = await DeviceInformation.FindAllAsync(DeviceClass.VideoCapture);

            // Get the desired camera by panel.
            DeviceInformation desiredDevice = allVideoDevices.FirstOrDefault(x => x.EnclosureLocation != null && x.EnclosureLocation.Panel == desiredPanel);

            // If there is no device mounted on the desired panel, return the first device found.
            return desiredDevice ?? allVideoDevices.FirstOrDefault();
        }

        /// <summary>
        /// Handles MediaCapture failures. Cleans up the camera resources.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="errorEventArgs"></param>
        private async void MediaCapture_Failed(MediaCapture sender, MediaCaptureFailedEventArgs errorEventArgs)
        {
            await CleanupCameraAsync();

            DispatcherQueue.TryEnqueue(() =>
            {
                ExtractButton.Visibility = Visibility.Collapsed;
                CameraButton.Visibility = Visibility.Visible;

                rootPage.NotifyUser("MediaCapture Failed. " + errorEventArgs.Message, NotifyType.ErrorMessage);
            });
        }

        #endregion MediaCapture methods
    }
}
