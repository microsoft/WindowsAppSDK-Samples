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
using System.Linq;
using System.Threading.Tasks;
using Microsoft.UI.Dispatching;
using Microsoft.UI.Xaml.Controls;
using Windows.Media.Capture;
using Windows.Media.Capture.Frames;
using Windows.Media.Core;
using Windows.Media.MediaProperties;
using Windows.Media.Playback;

namespace SDKTemplate
{
    public class MediaCapturePreviewer
    {
        private readonly DispatcherQueue _dispatcherQueue;
        private readonly MediaPlayerElement _previewControl;
        private MediaPlayer _mediaPlayer;
        private MediaFrameSource _frameSource;

        public MediaCapturePreviewer(MediaPlayerElement previewControl, DispatcherQueue dispatcherQueue)
        {
            _previewControl = previewControl;
            _dispatcherQueue = dispatcherQueue;
        }

        public bool IsPreviewing { get; private set; }
        public bool IsRecording { get; set; }
        public MediaCapture MediaCapture { get; private set; }

        /// <summary>
        /// Sets encoding properties on a camera stream and rebuilds the MediaPlayerElement preview source.
        /// </summary>
        public async Task SetMediaStreamPropertiesAsync(MediaStreamType streamType, IMediaEncodingProperties encodingProperties)
        {
            StopPreviewPlayback();

            await MediaCapture.VideoDeviceController.SetMediaStreamPropertiesAsync(streamType, encodingProperties);

            StartPreviewPlayback();
        }

        /// <summary>
        /// Initializes MediaCapture and starts preview using MediaPlayerElement.
        /// </summary>
        public async Task InitializeCameraAsync()
        {
            await CleanupCameraAsync();

            MediaCapture = new MediaCapture();
            MediaCapture.Failed += MediaCapture_Failed;

            try
            {
                await MediaCapture.InitializeAsync();
                _frameSource = FindPreviewFrameSource();

                if (_frameSource == null)
                {
                    MainPage.Current.NotifyUser("No video preview or record stream was found for this camera.", NotifyType.ErrorMessage);
                    await CleanupCameraAsync();
                    return;
                }

                StartPreviewPlayback();
            }
            catch (UnauthorizedAccessException)
            {
                MainPage.Current.NotifyUser("The app was denied access to the camera", NotifyType.ErrorMessage);
                await CleanupCameraAsync();
            }
            catch (Exception ex)
            {
                MainPage.Current.NotifyUser("Unable to initialize the camera: " + ex.Message, NotifyType.ErrorMessage);
                await CleanupCameraAsync();
            }
        }

        public async Task CleanupCameraAsync()
        {
            if (MediaCapture != null)
            {
                if (IsRecording)
                {
                    try
                    {
                        await MediaCapture.StopRecordAsync();
                    }
                    catch (Exception ex)
                    {
                        MainPage.Current.NotifyUser("Exception when stopping video recording: " + ex.Message, NotifyType.ErrorMessage);
                    }
                    IsRecording = false;
                }

                MediaCapture.Failed -= MediaCapture_Failed;
            }

            StopPreviewPlayback();

            if (MediaCapture != null)
            {
                MediaCapture.Dispose();
                MediaCapture = null;
            }

            _frameSource = null;
        }

        private MediaFrameSource FindPreviewFrameSource()
        {
            return MediaCapture.FrameSources.FirstOrDefault(source =>
                source.Value.Info.MediaStreamType == MediaStreamType.VideoPreview &&
                source.Value.Info.SourceKind == MediaFrameSourceKind.Color).Value
                ?? MediaCapture.FrameSources.FirstOrDefault(source =>
                    source.Value.Info.MediaStreamType == MediaStreamType.VideoRecord &&
                    source.Value.Info.SourceKind == MediaFrameSourceKind.Color).Value
                ?? MediaCapture.FrameSources.FirstOrDefault(source =>
                    source.Value.Info.SourceKind == MediaFrameSourceKind.Color).Value;
        }

        private void StartPreviewPlayback()
        {
            if (_frameSource == null)
            {
                return;
            }

            _mediaPlayer = new MediaPlayer
            {
                AutoPlay = false,
                RealTimePlayback = true,
                Source = MediaSource.CreateFromMediaFrameSource(_frameSource)
            };
            _mediaPlayer.MediaFailed += MediaPlayer_MediaFailed;
            _previewControl.SetMediaPlayer(_mediaPlayer);
            _mediaPlayer.Play();
            IsPreviewing = true;
        }

        private void StopPreviewPlayback()
        {
            if (_mediaPlayer != null)
            {
                _mediaPlayer.MediaFailed -= MediaPlayer_MediaFailed;
                _mediaPlayer.Pause();
                _previewControl.SetMediaPlayer(null);
                _mediaPlayer.Dispose();
                _mediaPlayer = null;
            }

            IsPreviewing = false;
        }

        private void MediaCapture_Failed(MediaCapture sender, MediaCaptureFailedEventArgs e)
        {
            _dispatcherQueue.TryEnqueue(async () =>
            {
                MainPage.Current.NotifyUser("Preview stopped: " + e.Message, NotifyType.ErrorMessage);
                await CleanupCameraAsync();
            });
        }

        private void MediaPlayer_MediaFailed(MediaPlayer sender, MediaPlayerFailedEventArgs args)
        {
            _dispatcherQueue.TryEnqueue(() =>
            {
                MainPage.Current.NotifyUser("Preview playback failed: " + args.ErrorMessage, NotifyType.ErrorMessage);
                StopPreviewPlayback();
            });
        }
    }
}
