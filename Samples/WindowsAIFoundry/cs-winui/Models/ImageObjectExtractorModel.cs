// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using WindowsAISampleSample.Models.Contracts;
using System;
using System.Threading;
using System.Threading.Tasks;
using Windows.Graphics.Imaging;
using Microsoft.Windows.AI;
using Microsoft.Windows.AI.Imaging;

namespace WindowsAISampleSample.Models;

class ImageObjectExtractorModel : IModelManager
{
    public async Task CreateModelSessionWithProgress(IProgress<double> progress, CancellationToken cancellationToken = default)
    {
        if (ImageObjectExtractor.GetReadyState() == AIFeatureReadyState.NotReady)
        {
            var objectExtractorDeploymentOperation = ImageObjectExtractor.EnsureReadyAsync();
            objectExtractorDeploymentOperation.Progress = (_, modelDeploymentProgress) =>
            {
                progress.Report(modelDeploymentProgress % 0.75);  // all progress is within 75%
            };
            using var _ = cancellationToken.Register(() => objectExtractorDeploymentOperation.Cancel());
            await objectExtractorDeploymentOperation;
        }
        else
        {
            progress.Report(0.75);
        }
        progress.Report(1.0); // 100% progress
    }

    public async Task<SoftwareBitmap> ApplyImageObjectExtractorAsync(SoftwareBitmap inputImage, ImageObjectExtractorHint hint, CancellationToken cancellationToken = default)
    {
        var model = await ImageObjectExtractor.CreateWithSoftwareBitmapAsync(inputImage);
        return model.GetSoftwareBitmapObjectMask(hint);
    }
}
