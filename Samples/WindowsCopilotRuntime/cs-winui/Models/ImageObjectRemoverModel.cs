// Copyright (c) Microsoft Corporation.
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

internal class ImageObjectRemoverModel : IModelManager
{
    private ImageObjectRemover? _session;

    private ImageObjectRemover Session => _session ?? throw new InvalidOperationException("Image Object Remover session was not created yet");

    public async Task CreateModelSessionWithProgress(IProgress<double> progress, CancellationToken cancellationToken = default)
    {
        if (ImageObjectRemover.GetReadyState() == AIFeatureReadyState.EnsureNeeded)
        {
            var objectRemoveDeploymentOperation = ImageObjectRemover.EnsureReadyAsync();
            objectRemoveDeploymentOperation.Progress = (_, packageDeploymentProgress) =>
            {
                progress.Report(packageDeploymentProgress);
            };
            using var _ = cancellationToken.Register(() => objectRemoveDeploymentOperation.Cancel());
            await objectRemoveDeploymentOperation;
        }
        else
        {
            progress.Report(100.0);
        }
        _session = await ImageObjectRemover.CreateAsync();
    }

    public SoftwareBitmap RemoveFromSoftwareBitmap(SoftwareBitmap inputImage, SoftwareBitmap inputMask)
    {
        return Session.RemoveFromSoftwareBitmap(inputImage, inputMask);
    }
}