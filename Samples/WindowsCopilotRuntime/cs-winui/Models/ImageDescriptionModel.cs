// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using WindowsCopilotRuntimeSample.Models.Contracts;
using Microsoft.Graphics.Imaging;
using Microsoft.Windows.Management.Deployment;
using System;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.Windows.AI.Text;
using Microsoft.Windows.AI.ContentSafety;
using Microsoft.Windows.AI;
using Microsoft.Windows.AI.Imaging;

namespace WindowsCopilotRuntimeSample.Models;

class ImageDescriptionModel  : IModelManager
{
    private ImageDescriptionGenerator? _session;
    private ImageDescriptionGenerator Session => _session ?? throw new InvalidOperationException("Image Description generator session was not created yet");

    public async Task CreateModelSessionWithProgress(IProgress<double> progress, CancellationToken cancellationToken = default)
    {
        if (ImageDescriptionGenerator.GetReadyState() == AIFeatureReadyState.NotReady)
        {
            var imageDescriptionDeploymentOperation = ImageDescriptionGenerator.EnsureReadyAsync();
            imageDescriptionDeploymentOperation.Progress = (_, modelDeploymentProgress) =>
            {
                progress.Report(modelDeploymentProgress % 0.75);  // all progress is within 75%
            };
            using var _ = cancellationToken.Register(() => imageDescriptionDeploymentOperation.Cancel());
            await imageDescriptionDeploymentOperation;
        }
        else
        {
            progress.Report(0.75);
        }
        _session = await ImageDescriptionGenerator.CreateAsync();
        progress.Report(1.0); // 100% progress

    }
    public async Task<string> DescribeImage(ImageBuffer inputImage, CancellationToken cancellationToken = default)
    {
        ContentFilterOptions filterOptions = new();
        var modelResponse = await Session.DescribeAsync(inputImage, ImageDescriptionKind.BriefDescription, filterOptions);
        if (modelResponse.Status != ImageDescriptionResultStatus.Complete)
        {
            return $"Image description failed with status: {modelResponse.Status}";
        }
        return modelResponse.Description;
    }
}
