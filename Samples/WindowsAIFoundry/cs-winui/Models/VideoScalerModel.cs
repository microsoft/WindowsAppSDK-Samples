// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using Microsoft.Graphics.Imaging;
using Microsoft.Windows.AI;
using Microsoft.Windows.AI.MachineLearning;
using Microsoft.Windows.AI.Video;
using Microsoft.Windows.Management.Deployment;
using System;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Threading;
using System.Threading.Tasks;
using Windows.Graphics.Imaging;
using Windows.Media;
using Windows.Storage.Streams;
using WindowsAISample.Models.Contracts;
using WindowsAISample.Util;

namespace WindowsAISample.Models;

internal class VideoScalerModel : IModelManager
{
    private VideoScaler? _session;

    private VideoScaler Session => _session ?? throw new InvalidOperationException("Video Scaler session was not created yet");

    public async Task CreateModelSessionWithProgress(IProgress<double> progress, CancellationToken cancellationToken = default)
    {
        var catalog = ExecutionProviderCatalog.GetDefault();
        await catalog.EnsureAndRegisterCertifiedAsync();

        progress.Report(0.5);

        var readyState = VideoScaler.GetReadyState();
        if (readyState == AIFeatureReadyState.NotSupportedOnCurrentSystem)
        {
            throw new InvalidOperationException("VideoScaler not supported on current system (hardware requirements not met)");
        }

        if (readyState == AIFeatureReadyState.NotReady)
        {
            var videoScalerDeploymentOperation = VideoScaler.EnsureReadyAsync();
            videoScalerDeploymentOperation.Progress = (_, modelDeploymentProgress) =>
            {
                progress.Report(0.5 + (modelDeploymentProgress * 0.25) % 0.25);  // all progress is within 50% and 75%
            };
            using var _ = cancellationToken.Register(() => videoScalerDeploymentOperation.Cancel());
            await videoScalerDeploymentOperation;
        }
        else
        {
            progress.Report(0.75);
        }
        _session = await VideoScaler.CreateAsync();
        progress.Report(1.0); // 100% progress
    }

    public SoftwareBitmap ScaleVideoFrame(SoftwareBitmap inputFrame)
    {
        ImageBuffer inputImageBuffer = inputFrame.ConvertToBgr8ImageBuffer();
        var size = (uint)(inputFrame.PixelWidth * inputFrame.PixelHeight * 3);
        IBuffer outputBuffer = new global::Windows.Storage.Streams.Buffer(size);
        outputBuffer.Length = size;
        ImageBuffer outputImageBuffer = ImageBuffer.CreateForBuffer(
            outputBuffer,
            ImageBufferPixelFormat.Bgr8,
            inputFrame.PixelWidth,
            inputFrame.PixelHeight,
            inputFrame.PixelWidth * 3);
        var result = Session.ScaleFrame(inputImageBuffer, outputImageBuffer, new VideoScalerOptions());
        if (result.Status != ScaleFrameStatus.Success)
        {
            throw new Exception($"Failed to scale video frame: {result.Status}");
        }

        return outputImageBuffer.ConvertBgr8ImageBufferToBgra8SoftwareBitmap();
    }
}