// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using WindowsAISample.Models.Contracts;
using System;
using System.Threading;
using System.Threading.Tasks;
using Windows.Graphics.Imaging;
using Microsoft.Windows.AI;
using Microsoft.Windows.AI.Imaging;

namespace WindowsAISample.Models;

class ImageForegroundExtractorModel : IModelManager
{
    public async Task CreateModelSessionWithProgress(IProgress<double> progress, CancellationToken cancellationToken = default)
    {
        if (ImageForegroundExtractor.GetReadyState() == AIFeatureReadyState.NotReady)
        {
            var foregroundExtractorDeploymentOperation = ImageForegroundExtractor.EnsureReadyAsync();
            foregroundExtractorDeploymentOperation.Progress = (_, modelDeploymentProgress) =>
            {
                progress.Report(modelDeploymentProgress % 0.75);  // all progress is within 75%
            };
            using var _ = cancellationToken.Register(() => foregroundExtractorDeploymentOperation.Cancel());
            await foregroundExtractorDeploymentOperation;
        }
        else
        {
            progress.Report(0.75);
        }
        progress.Report(1.0); // 100% progress
    }

    public async Task<SoftwareBitmap> ExtractForegroundMaskAsync(SoftwareBitmap inputImage, CancellationToken cancellationToken = default)
    {
        using var extractor = await ImageForegroundExtractor.CreateAsync();
        return extractor.GetMaskFromSoftwareBitmap(inputImage);
    }
}
