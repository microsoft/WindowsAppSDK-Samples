// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using System;
using System.Threading;
using System.Threading.Tasks;
using Windows.Graphics.Imaging;
using Microsoft.Windows.AI;
using AIImaging = Microsoft.Windows.AI.Imaging;

namespace WindowsAISample.Ext.ImageForegroundExtractor.Models;

public class ImageForegroundExtractorModel : IModelManager
{
    public async Task CreateModelSessionWithProgress(IProgress<double> progress, CancellationToken cancellationToken = default)
    {
        if (AIImaging.ImageForegroundExtractor.GetReadyState() == AIFeatureReadyState.NotReady)
        {
            var foregroundExtractorDeploymentOperation = AIImaging.ImageForegroundExtractor.EnsureReadyAsync();
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
        using var extractor = await AIImaging.ImageForegroundExtractor.CreateAsync();
        return extractor.GetMaskFromSoftwareBitmap(inputImage);
    }
}
