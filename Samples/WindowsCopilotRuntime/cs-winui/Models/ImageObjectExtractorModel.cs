﻿// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using WindowsCopilotRuntimeSample.Models.Contracts;
using Microsoft.Graphics.Imaging;
using Microsoft.Windows.Management.Deployment;
using System;
using System.Threading;
using System.Threading.Tasks;
using Windows.Graphics.Imaging;
using Microsoft.Windows.AI;

namespace WindowsCopilotRuntimeSample.Models;

class ImageObjectExtractorModel : IModelManager
{
    public async Task CreateModelSessionWithProgress(IProgress<double> progress, CancellationToken cancellationToken = default)
    {
        if (ImageObjectExtractor.GetReadyState() == AIFeatureReadyState.EnsureNeeded)
        {
            var objectExtractorDeploymentOperation = ImageObjectExtractor.EnsureReadyAsync();
            objectExtractorDeploymentOperation.Progress = (_, modelDeploymentProgress) =>
            {
                progress.Report(modelDeploymentProgress);
            };
            using var _ = cancellationToken.Register(() => objectExtractorDeploymentOperation.Cancel());
            await objectExtractorDeploymentOperation;
        }
        else
        {
            progress.Report(100.0);
        }
    }

    public async Task<SoftwareBitmap> ApplyImageObjectExtractorAsync(SoftwareBitmap inputImage, ImageObjectExtractorHint hint, CancellationToken cancellationToken = default)
    {
        var model = await ImageObjectExtractor.CreateWithSoftwareBitmapAsync(inputImage);
        return model.GetSoftwareBitmapObjectMask(hint);
    }
}
