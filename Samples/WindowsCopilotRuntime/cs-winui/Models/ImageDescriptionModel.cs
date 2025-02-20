﻿// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using WindowsCopilotRuntimeSample.Models.Contracts;
using Microsoft.Graphics.Imaging;
using Microsoft.Windows.Management.Deployment;
using System;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.Windows.AI.Generative;
using Microsoft.Windows.AI.ContentModeration;

namespace WindowsCopilotRuntimeSample.Models;

class ImageDescriptionModel  : IModelManager
{
    private ImageDescriptionGenerator? _session;
    private ImageDescriptionGenerator Session => _session ?? throw new InvalidOperationException("Image Description generator session was not created yet");

    public async Task CreateModelSessionWithProgress(IProgress<PackageDeploymentProgress> progress, CancellationToken cancellationToken = default)
    {
        if (!ImageDescriptionGenerator.IsAvailable())
        {
            var imageDescriptionDeploymentOperation = ImageDescriptionGenerator.MakeAvailableAsync();
            imageDescriptionDeploymentOperation.Progress = (_, packageDeploymentProgress) =>
            {
                progress.Report(packageDeploymentProgress);
            };
            using var _ = cancellationToken.Register(() => imageDescriptionDeploymentOperation.Cancel());
            await imageDescriptionDeploymentOperation;
        }
        else
        {
            progress.Report(new PackageDeploymentProgress(PackageDeploymentProgressStatus.CompletedSuccess, 100.0));
        }
        _session = await ImageDescriptionGenerator.CreateAsync();

    }
    public async Task<string> DescribeImage(ImageBuffer inputImage, CancellationToken cancellationToken = default)
    {
        ContentFilterOptions filterOptions = new();
        filterOptions.PromptMinSeverityLevelToBlock.ViolentContentSeverity = SeverityLevel.Medium;
        filterOptions.ResponseMinSeverityLevelToBlock.ViolentContentSeverity = SeverityLevel.Medium;
        var modelResponse = await Session.DescribeAsync(inputImage, ImageDescriptionScenario.Caption, filterOptions);
        return modelResponse.Response;
    }
}
