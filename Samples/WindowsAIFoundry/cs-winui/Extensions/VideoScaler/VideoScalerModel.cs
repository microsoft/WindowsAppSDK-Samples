// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.Graphics.Imaging;
using Microsoft.Windows.AI;
using Microsoft.Windows.AI.MachineLearning;
using Microsoft.Windows.Management.Deployment;
using Windows.Graphics.Imaging;
using Windows.Storage.Streams;
using WindowsAISample.Models.Contracts;
using WindowsAISample.Util;

// The enclosing namespace 'VideoScaler' would shadow the SDK type; alias it to keep call sites readable.
using VideoScalerApi = Microsoft.Windows.AI.Video.VideoScaler;
using VideoScalerStatusApi = Microsoft.Windows.AI.Video.VideoScalerStatus;

namespace WindowsAISample.Ext.VideoScaler;

internal class VideoScalerModel : IModelManager
{
    private VideoScalerApi? _session;

    private VideoScalerApi Session => _session ?? throw new InvalidOperationException("Video Scaler session was not created yet");

    public async Task CreateModelSessionWithProgress(IProgress<double> progress, CancellationToken cancellationToken = default)
    {
        var catalog = ExecutionProviderCatalog.GetDefault();
        await catalog.EnsureAndRegisterCertifiedAsync();

        progress.Report(0.5);

        var readyState = VideoScalerApi.GetReadyState();
        if (readyState == AIFeatureReadyState.NotSupportedOnCurrentSystem)
        {
            throw new InvalidOperationException("VideoScaler not supported on current system (hardware requirements not met)");
        }

        if (readyState == AIFeatureReadyState.NotReady)
        {
            var videoScalerDeploymentOperation = VideoScalerApi.EnsureReadyAsync();
            videoScalerDeploymentOperation.Progress = (_, modelDeploymentProgress) =>
            {
                progress.Report(0.5 + (modelDeploymentProgress * 0.25) % 0.25);
            };
            using var _ = cancellationToken.Register(() => videoScalerDeploymentOperation.Cancel());
            await videoScalerDeploymentOperation;
        }
        else
        {
            progress.Report(0.75);
        }

        _session = await VideoScalerApi.CreateAsync();
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
        var result = Session.ScaleImageBuffer(inputImageBuffer, outputImageBuffer, null);
        if (result.Status != VideoScalerStatusApi.Success)
        {
            throw new Exception($"Failed to scale video frame: {result.Status}");
        }

        return outputImageBuffer.ConvertBgr8ImageBufferToBgra8SoftwareBitmap();
    }
}
