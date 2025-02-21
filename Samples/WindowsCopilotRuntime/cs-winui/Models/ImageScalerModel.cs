// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using WindowsCopilotRuntimeSample.Models.Contracts;
using Microsoft.Graphics.Imaging;
using Microsoft.Windows.Management.Deployment;
using System;
using System.Threading;
using System.Threading.Tasks;
using Windows.Graphics.Imaging;
using System.Diagnostics;

namespace WindowsCopilotRuntimeSample.Models;

internal class ImageScalerModel : IModelManager
{
    private ImageScaler? _session;

    private ImageScaler Session => _session ?? throw new InvalidOperationException("Image Scaler session was not created yet");

    public async Task CreateModelSessionWithProgress(IProgress<PackageDeploymentProgress> progress, CancellationToken cancellationToken = default)
    {
        if (!ImageScaler.IsAvailable())
        {
            var imageScalerDeploymentOperation = ImageScaler.MakeAvailableAsync();
            imageScalerDeploymentOperation.Progress = (_, packageDeploymentProgress) =>
            {
                progress.Report(packageDeploymentProgress);
            };
            using var _ = cancellationToken.Register(() => imageScalerDeploymentOperation.Cancel());
            await imageScalerDeploymentOperation;
        }
        else
        {
            progress.Report(new PackageDeploymentProgress(PackageDeploymentProgressStatus.CompletedSuccess, 100.0));
        }
        _session = await ImageScaler.CreateAsync();
    }

    public SoftwareBitmap ScaleSoftwareBitmap(SoftwareBitmap inputImage, int width, int height)
    {
        Stopwatch stopwatch = Stopwatch.StartNew();
        SoftwareBitmap bitmap = Session.ScaleSoftwareBitmap(inputImage, width, height);
        stopwatch.Stop();
        (App.Current as App).timerMap.Add("ImageScaler", stopwatch.ElapsedMilliseconds);

        return bitmap;
    }
}