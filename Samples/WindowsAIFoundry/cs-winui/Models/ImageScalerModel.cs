// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using WindowsAISample.Models.Contracts;
using Microsoft.Graphics.Imaging;
using Microsoft.Windows.Management.Deployment;
using System;
using System.Threading;
using System.Threading.Tasks;
using Windows.Graphics.Imaging;
using Microsoft.Windows.AI;
using Microsoft.Windows.AI.Imaging;

namespace WindowsAISample.Models;

internal class ImageScalerModel : IModelManager
{
    private ImageScaler? _session;

    private ImageScaler Session => _session ?? throw new InvalidOperationException("Image Scaler session was not created yet");

    public async Task CreateModelSessionWithProgress(IProgress<double> progress, CancellationToken cancellationToken = default)
    {
        if (ImageScaler.GetReadyState() == AIFeatureReadyState.NotReady)
        {
            var imageScalerDeploymentOperation = ImageScaler.EnsureReadyAsync();
            imageScalerDeploymentOperation.Progress = (_, modelDeploymentProgress) =>
            {
                progress.Report(modelDeploymentProgress % 0.75);  // all progress is within 75%
            };
            using var _ = cancellationToken.Register(() => imageScalerDeploymentOperation.Cancel());
            await imageScalerDeploymentOperation;
        }
        else
        {
            progress.Report(0.75);
        }
        _session = await ImageScaler.CreateAsync();
        progress.Report(1.0); // 100% progress
    }

    public SoftwareBitmap ScaleSoftwareBitmap(SoftwareBitmap inputImage, int width, int height)
    {
        return Session.ScaleSoftwareBitmap(inputImage, width, height);
    }
}